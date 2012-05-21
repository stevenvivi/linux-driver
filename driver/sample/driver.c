#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>

MODULE_AUTHOR("Chang ShaoRan");
MODULE_LICENSE("GPL");

extern struct bus_type my_bus_type;

/* 驱动结构的 probe 和 remove 成员方法，分别会在驱动
 * 匹配上总线上的对应设备和一处驱动时调用 */
static int my_probe(struct device *dev)
{
	printk("[Driver found device which my driver can handle!]\n");
	return 0;
}
static int my_remove(struct device *dev)
{
	printk("[Driver found device unpluged!]\n");
	return 0;
}

struct device_driver my_driver =
{
	.name = "my_dev",
	.bus = &my_bus_type,
	.probe = my_probe,
	.remove = my_remove,
};

/* 属性文件的读操作 */
static ssize_t my_drv_show(struct device_driver *driver,char *buf)
{
	return sprintf(buf,"%s\n","This is a driver!");
}
/* 创建属性文件结构 */
static DRIVER_ATTR(drv,S_IRUGO,my_drv_show,NULL);

static int __init my_driver_init(void)
{
	int ret = 0;
	
	printk("[Call my_driver_init!]\n");
	
 	/* 注册驱动 */
	if(ret = driver_register(&my_driver))
		return ret;
	/* 创建属性文件 */
	ret = driver_create_file(&my_driver,&driver_attr_drv);

	return ret;
}

static void __exit my_driver_exit(void)
{
	printk("[Call my_driver_exit!]\n");
	
	driver_unregister(&my_driver);
}

module_init(my_driver_init);
module_exit(my_driver_exit);

