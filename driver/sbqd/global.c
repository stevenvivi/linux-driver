#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#define  GLOBALMEM_SIZE 0x1000  /* */
#define  MEM_CLEAR      0x1     /* */
#define  GLOBALMEM_MAJOR 254

static int globalmem_major = GLOBALMEM_MAJOR;

struct globalmem_dev{
	struct cdev cdev;   /*Cdev结构体*/
	unsigned char mem[GLOBALMEM_SIZE]; /* 全局内存*/ 
};

strut globalmem_dev *globalmem_devp;  /*设备结构体指针*/ 

int globalmem_open(struct inode *inode, struct file *filp)
{
	/* 将设备结构体指针赋值给文件私有数据 */
	filp->private_data = globalmem_devp;
	return 0;
}

int globalmem_release(struct inode *inode , struct file *filp)
{
	return 0;
}

static int globalmem_ioctl(struct inode *inodep, struct file *filp，
	unsigned int cmd, unsigned long arg）
{
	struct globalmem_dev *dev = filp->private_date;
	switch(cmd)
	{
		case MEM_CLEAR:
			memset(dev->mem,0,GLOBALMEM_SIZE);
			printk(KERN_INFO "globalmem is set to zero\n");
			break;
		default:
			return - EINVAL;
	}
	return 0;
}

static ssize_t globalmem_read(struct file *filp, char  __user *buf,
	size_t size,loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev = filp->private_data;/*获得设备结构体指针*/
	/*分析和获取有效的长度*/ 
	if(p >= GLOBALMEM_SIZE)
		return count ? -ENXIO: 0;
	if(count > GLOBALMEM_SIZE - p)
		count = GLOBALMEM_SIZE - p ;
	/*内核空间-> 用户空间*/ 
	if(copy_to_user(buf, (void*)(dev->mem + p), count))
	{
		ret = - EFAULT;
	}
	else
	{
		*ppos += count;
		ret = count;
		printk(KERN_INFO "read %d bytes(s) from %d\n", count, p);
	}
	return ret;	
}

static ssize_t globalmem_write(struct file *filp,const char __user *buf,
	size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret =0;
	struct globalmem_dev *dev = filp->private_data;
	
	if(p >= GLOBALMEM_SIZE)
		return count ? -ENXIO: 0;
	if(count > GLOBALMEM_SIZE - p)
		count = GLOBALMEM_SIZE -p;
		
	if(copy_from_user(dev->mem + p, buf, count))
		ret = - EFAULT;
	else
	{
		*ppos += count;
		ret =count;
		printk(KERN_INFO "Write %d bytes(s) from %d\n",count,p)
	}
	return ret;
}

static const struct file_operations globalmem_fops = {
	.owner = THIS_MODULE,
	.llseek = globalmem_llseek,
	.read = globalmem_read,
	.write = globalmem_write,
	.ioctl = globalmem_ioctl,
	.open = globalmem_open,
	.release = globalmem_release,
};

static void globalmem_setup_cdev(struct globalmem_dev *dev,
	int index)
{
	int err, devno =MKDEV(globalmem_major, index);
	
	cdev_init(&dev->cdev, &globalmem_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &globalmem_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if(err)
		printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}

int globalmem_init(void)
{
	int result;
	dev_t devno = MKDEV(globalmem_major, 0);
	
	if(globalmem_major)
		result = register_chrdev_region(devno, 1, "globalmem");
	else
	{
		result = alloc_chrdev_region(&devno, 0, 1, "globalmem");
		globalmem_major = MAJOR(devno);		
	}
	if(result < 0)
	return result;
	
	/* 动态申请设备结构体的内存*/
	globalmem_devp = kmalloc(sizeof(struct globalmem_dev), GFP_KERNEL);
	if(!globalmem)
	{
		result = - ENOMEM;
		goto fail_malloc;
	}
	memset(globalmem_devp, 0, sizeof(struct globalmem_dev));
	globalmem_setup_cdev(globalmem_devp, 0);
	return 0;
	
	fail_malloc: unregister_chrdev_region(devno, 1);
	return result;
}

void globalmem_exit(void)
{
	cdev_del(&globalmem_devp->cdev)	;
	kfree(global_devp);
	unregister_chrdev_region(MKDEV(globalmem_major, 0),1);
}

MODULE_AUTHOR("CHANG SHAORAN");
MODULE_LICENSE("Dual BSD/GPL");

module_parm(globalmem_major, int, S_IRUGO);
module_init(globalmem_init);
module_exit(globalmem_exit);
