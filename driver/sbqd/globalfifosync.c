#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/poll.h>
#include <linux/semaphore.h>

#define GLOBALFIFO_SIZE 0x1000
#define FIFO_CLEAR 0x1
#define GLOBALFIFO_MAJOR 250

static int globalfifo_major = GLOBALFIFO_MAJOR;

struct globalfifo_dev{
	struct cdev cdev;
	unsigned int current_len;   /* FIFO有效数据长度   */
	unsigned char mem[GLOBALFIFO_SIZE];	
	struct semaphore sem;        /*并发用的控制信号量   */
	wait_queue_head_t r_wait; /*阻塞读用读等待队列头*/
	wait_queue_head_t w_wait; /*阻塞写用的等待队列头*/
	struct fasync_struct *async_queue; /*异步结构体指针*/
};

struct globalfifo_dev *globalfifo_devp;   /*设备结构体指针*/

int globalfifo_open(struct inode *inode, struct file *filp)
{
	filp->private_data = globalfifo_devp;
	return 0;
	/*
	struct globalfifo_dev *dev;
	dev = container_of(inode->i_cdev, struct globalfifo_dev , cdev)
	filp->private_data = dev;
	return 0;
	*/
}

static int globalfifo_fasync(int fd, struct file *filp, int mode)
{
	struct globalfifo_dev *dev = filp->private_data;
	return fasync_helper(fd, filp, mode, &dev->async_queue);
}

int globalfifo_release(struct inode *inode, struct file *filp)
{
	globalfifo_fasync(-1, filp, 0);
	return 0;
}


static long globalfifo_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct globalfifo_dev *dev = filp->private_data;

	switch(cmd)
	{
		case FIFO_CLEAR:
			down(&dev->sem);  /*注意不能被信号打断*/
			dev->current_len = 0;
			memset(dev->mem, 0, GLOBALFIFO_SIZE);
			up(&dev->sem);
			printk(KERN_INFO "glboalmem is set to zero\n");
			break;

		default:
			return - EINVAL;
	}
	return 0;
}


static unsigned int globalfifo_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;
	struct globalfifo_dev *dev = filp->private_data;
	
	down(&dev->sem);
	poll_wait(filp, &dev->r_wait, wait);
	poll_wait(filp, &dev->w_wait, wait);

	if(dev->current_len != 0)
	{
		mask |= POLLIN | POLLRDNORM; /*表示数据可获得*/
	}
	if(dev->current_len != GLOBALFIFO_SIZE)
	{
		mask |= POLLOUT | POLLWRNORM;
	}
	up(&dev->sem);
	return mask;
}

static ssize_t globalfifo_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	int ret;
	struct globalfifo_dev *dev = filp->private_data;  /*获得设备结构体指针*/
	
	DECLARE_WAITQUEUE(wait, current);	//current未声明
	down(&dev->sem);
	add_wait_queue(&dev->r_wait, &wait);     //进入读等待队列
	

	if(dev->current_len == 0)
	{
		if(filp->f_flags &O_NONBLOCK)
		{
			ret = - EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);
		up(&dev->sem);
		schedule();
		if(signal_pending(current))
		{
			ret = - ERESTARTSYS;
			goto out2;
		}
		down(&dev->sem);
	}
	if(count >	dev->current_len)
		count = dev->current_len;
	if(copy_to_user(buf, dev->mem, count))
	{
		ret = - EFAULT;
		goto out;
	}
	else{
		memcpy(dev->mem, dev->mem + count, dev->current_len - count); /*FIFO数据前移*/
		dev->current_len -= count;
		printk(KERN_INFO "read %d bytes, current_len: %d\n", count, dev->current_len);
		wake_up_interruptible(&dev->w_wait);  /*唤醒写等待队列*/
		ret = count;	
	}
	out: up(&dev->sem);
	out2: remove_wait_queue(&dev->w_wait, &wait); /*从等待队列中移除*/
	set_current_state(TASK_RUNNING);
	return ret;
}

static ssize_t globalfifo_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	int ret = 0;
	struct globalfifo_dev *dev = filp->private_data;
	DECLARE_WAITQUEUE(wait, current);

	down(&dev->sem);
	add_wait_queue(&dev->w_wait, &wait);                /*进入写等待队列头*/
	
	if(dev->current_len == GLOBALFIFO_SIZE)
	{
		if(filp->f_flags & O_NONBLOCK)                    /*如果是非阻塞访问*/
		{
			ret = - EAGAIN;
			goto out;
		}
		__set_current_state(TASK_INTERRUPTIBLE);  /*改变进程状态为睡眠*/
		up(&dev->sem);
		schedule();
		if(signal_pending(current))                                /*如果是因为信号唤醒*/
		{
			ret = -ERESTARTSYS;
			goto out2;
		}
		down(&dev->sem);
	}

	/*从用户空间拷贝到内核空间*/
	if(count > GLOBALFIFO_SIZE - dev->current_len)
		count = GLOBALFIFO_SIZE - dev->current_len;

	if(copy_from_user(dev->mem + dev->current_len, buf, count))
	{
		ret = - EFAULT;
		goto out;
	}
	else{
		dev->current_len += count;		
		printk(KERN_INFO "written %d bytes from %d\n", count, dev->current_len);
		wake_up_interruptible(&dev->r_wait);    /*唤醒读等待队列*/
		/////////////////////////////////
		if(dev->async_queue)
			kill_fasync(&dev->async_queue, SIGIO, POLL_IN);

		ret = count;
	}

	out: up(&dev->sem);
	out2: remove_wait_queue(&dev->w_wait, &wait); /*从附属的等待队列头移除*/
	set_current_state(TASK_RUNNING);
	return ret;
}


static const struct file_operations globalfifo_fops = 
{
	.owner   =	 	THIS_MODULE,
	.read   	 = 		globalfifo_read,
	.write	 =	    globalfifo_write,
	.unlocked_ioctl	= globalfifo_ioctl,
	.poll		 = 		globalfifo_poll,
	.open		 =	    globalfifo_open,
	.release =	    globalfifo_release,
	.fasync  =     globalfifo_fasync,
};

static void globalfifo_setup_cdev(struct globalfifo_dev *dev, int index)
{
	int err, devno = MKDEV(globalfifo_major, index);
	cdev_init(&dev->cdev, &globalfifo_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops	= &globalfifo_fops;
	err	= cdev_add(&dev->cdev, devno, 1);
	if(err)
		printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}

int globalfifo_init(void)
{
	int ret;
	dev_t devno = MKDEV(globalfifo_major, 0);

	if(globalfifo_major)
		ret = register_chrdev_region(devno, 1, "globalfifo");
	else{
		ret = alloc_chrdev_region(&devno, 0, 1, "globalfifo");
		globalfifo_major = MAJOR(devno);
	}

	if(ret < 0)
		return ret;

	globalfifo_devp = kmalloc(sizeof (struct globalfifo_dev), GFP_KERNEL);
	/*globalfifo_devp = kmalloc(2*sizeof(struct globalfifo_dev), GFP_KERNEL);*/
	if(!globalfifo_devp)
	{
		ret = - ENOMEM;
		goto fail_malloc;
	}
	memset(globalfifo_devp, 0, sizeof(struct globalfifo_dev));
	/*memset(globalfifo_devp, 0, 2*sizeof(struct globalfifo_dev));*/

	globalfifo_setup_cdev(globalfifo_devp, 0);
	/*
	globalfifo_setup_cdev(globalfifo_devp[0], 0);
	globalfifo_setup_cdev(&globalfifo_devp[1], 1);*
	*/
	
	/*init_MUTEX(&globalfifo_devp->sem); 初始化信号量*/
	sema_init(&globalfifo_devp->sem, 1);
	init_waitqueue_head(&globalfifo_devp->r_wait);
	init_waitqueue_head(&globalfifo_devp->w_wait);
	return 0;

	fail_malloc: unregister_chrdev_region(devno, 1);
			
	return ret;
}

void globalfifo_exit(void)
{
	cdev_del(&globalfifo_devp->cdev); /*注销cdev*/
	kfree(globalfifo_devp);
	unregister_chrdev_region(MKDEV(globalfifo_major, 0), 1); /*释放设备号*/

	/*
	cdev_del(&globalfifo_devp[0].cdev);
	cdev_del(&globalfifo_devp[1].cdev);
	kfree(globalfifo_devp);
	unregister_chrdev_region(MKDEV(globalfifo_major, 0), 2); 
	*/
}

MODULE_AUTHOR("Chang ShaoRan");
MODULE_LICENSE("Dual BSD/GPL");
module_param(globalfifo_major, int, S_IRUGO);
module_init(globalfifo_init);
module_exit(globalfifo_exit);
