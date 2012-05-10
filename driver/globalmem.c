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

#define GLOBALMEM_SIZE 0x1000
#define MEM_CLEAR 0x1
#define GLOBALMEM_MAJOR 250

static int globalmem_major = GLOBALMEM_MAJOR;

struct globalmem_dev{
	struct cdev cdev;
	unsigned char mem[GLOBALMEM_SIZE];	
	struct semaphore sem;
};

struct globalmem_dev *globalmem_devp;   /*全局变量*/

int globalmem_open(struct inode *inode, struct file *filp)
{
	filp->private_data = globalmem_devp;
	return 0;
	/*
	struct globalmem_dev *dev;
	dev = container_of(inode->i_cdev, struct globalmem_dev , cdev)
	filp->private_data = dev;
	return 0;
	*/
}

int globalmem_release(struct inode *inode, struct file *filp)
{
	return 0;
}


static long globalmem_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct globalmem_dev *dev = filp->private_data;

	switch(cmd)
	{
		case MEM_CLEAR:
			/*
			if(down_interruptible(&dev->sem))
			{
				return - ERESTARTSYS;
			}
			*/
			memset(dev->mem, 0, GLOBALMEM_SIZE);
			/*up(&dev->sem);*/
			printk(KERN_INFO "glboalmem is set to zero\n");
			break;

		default:
			return - EINVAL;
	}
	return 0;
}

static ssize_t globalmem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev = filp->private_data;  /*获得设备结构体指针*/

	if(p >=	GLOBALMEM_SIZE)
		return count ? -ENXIO : 0;
	if(count > GLOBALMEM_SIZE - p)
		count = GLOBALMEM_SIZE - p;
	/*
	if(down_interruptible(&dev->sem))
	{
		return - ERESTARTSYS;
	}
	*/

	if(copy_to_user(buf, (void *)(dev->mem + p), count))
	{
		ret = - EFAULT;
	}
	else{
		*ppos += count;
		ret = count;
		PRINTK(KERN_INFO "read %d bytes from %d\n",count, p);
	}

	/*up(&dev->sem);*/

	return ret;
}

static ssize_t globalmem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev = filp->private_data;
	
	if (p >= GLOBALMEM_SIZE)
		return count ? - ENXIO : 0;
	if(count > GLOBALMEM_SIZE - p)
		count = GLOBALMEM_SIZE - p;

	/*
	if(down_interruptible(&dev->sem))
	{
		return -ERESTARTSYS;
	}
	*/

	if(copy_from_user(dev->mem + p, buf, count))
		ret = - EFAULT;
	else{
		*ppos += count;
		ret = count;

		printk(KERN_INFO "written %d bytes from %d\n", count, p);
	}

	/*up(&dev->sem)*/
	return ret;
}

static loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig)
{
	loff_t ret = 0;
	switch(orig)
	{
		case 0:
			if(offset < 0)
			{
				ret = - EINVAL;
				break;
			}
			if((unsigned int)offset > GLOBALMEM_SIZE)
			{
				ret = - EINVAL;
				break;
			}
			filp->f_pos = (unsigned int)offset;
			ret = filp->f_pos;
			break;
		case 1:
			if((filp->f_pos + offset) > GLOBALMEM_SIZE)
			{
				ret = - EINVAL;
				break;
			}
			if((filp->f_pos + offset)< 0)
			{
				ret = - EINVAL;
				break;
			}
			
			filp->f_pos += offset;
			ret = filp->f_pos;
			break;
		default:
			ret = - EINVAL;
	}
	return ret;
}
static const struct file_operations globalmem_fops = 
{
	.owner  = THIS_MODULE,
	.llseek = globalmem_llseek,
	.read   = globalmem_read,
	.write	= globalmem_write,
	.unlocked_ioctl	= globalmem_ioctl,
	.open	= globalmem_open,
	.release= globalmem_release,
};

static void globalmem_setup_cdev(struct globalmem_dev *dev, int index)
{
	int err, devno = MKDEV(globalmem_major, index);
	cdev_init(&dev->cdev, &globalmem_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops	= &globalmem_fops;
	err	= cdev_add(&dev->cdev, devno, 1);
	if(err)
		printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}

int globalmem_init(void)
{
	int result;
	dev_t devno = MKDEV(globalmem_major, 0);

	if(globalmem_major)
		result = register_chrdev_region(devno, 1, "globalmem");
	else{
		result = alloc_chrdev_region(&devno, 0, 1, "globalmem");
		globalmem_major = MAJOR(devno);
	}

	if(result < 0)
		return result;

	globalmem_devp = kmalloc(sizeof (struct globalmem_dev), GFP_KERNEL);
	/*globalmem_devp = kmalloc(2*sizeof(struct globalmem_dev), GFP_KERNEL);*/
	if(!globalmem_devp)
	{
		result = - ENOMEM;
		goto fail_malloc;
	}
	memset(globalmem_devp, 0, sizeof(struct globalmem_dev));
	/*memset(globalmem_devp, 0, 2*sizeof(struct globalmem_dev));*/

	globalmem_setup_cdev(globalmem_devp, 0);
	/*
	globalmem_setup_cdev(globalmem_devp[0], 0);
	globalmem_setup_cdev(&globalmem_devp[1], 1);*
	*/
	
	/*init_MUTEX(&globalfifo_devp->sem); 增加并发控制*/
	return 0;

fail_malloc: unregister_chrdev_region(devno, 1);
			
	return result;
}

void globalmem_exit(void)
{
	cdev_del(&globalmem_devp->cdev); /*注销cdev*/
	kfree(globalmem_devp);
	unregister_chrdev_region(MKDEV(globalmem_major, 0), 1); /*释放设备号*/

	/*
	cdev_del(&globalmem_devp[0].cdev);
	cdev_del(&globalmem_devp[1].cdev);
	kfree(globalmem_devp);
	unregister_chrdev_region(MKDEV(globalmem_major, 0), 2); 
	*/
}

MODULE_AUTHOR("Chang ShaoRan");
MODULE_LICENSE("Dual BSD/GPL");
module_param(globalmem_major, int, S_IRUGO);
module_init(globalmem_init);
module_exit(globalmem_exit);
