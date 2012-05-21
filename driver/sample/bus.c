#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/string.h>

static char *version = "$Revision: 1.0 $";

static int my_match(struct device *dev, struct device_driver * driver)
{
	return !strncmp(dev->kobj.name, driver->name, strlen(driver->name));
}

struct bus_type my_bus_type =
{
	.name = "my_bus",
	.match = my_match,
};

static ssize_t show_bus_version(struct bus_type *bus,char *buf)  
{  
    return snprintf(buf,PAGE_SIZE,"%s\n",version);  
}  
  
/* 构造总线属性的宏 */  
static BUS_ATTR(version,S_IRUGO,show_bus_version,NULL);  
  
static void my_bus_release(struct device *dev)  
{  
    printk("[Call my_bus_release!]\n");  
}  
  
struct device my_bus_dev =  
{  
    .init_name = "my_bus_dev",  
    .release = my_bus_release,  
};  
  
/* Export */  
EXPORT_SYMBOL(my_bus_dev);  
EXPORT_SYMBOL(my_bus_type);  
  
static int __init my_bus_init(void)  
{  
    int ret;  
  
    printk("[Call my_bus_init!]\n");  
    /* 注册总线 */  
    ret = bus_register(&my_bus_type);  
    if(ret)  
        return ret;  
  
    /* 创建属性文件 */  
    if(bus_create_file(&my_bus_type,&bus_attr_version))  
        printk("Fail to create version attribute!\n");  
  
    /* 注册总线设备 */  
    ret = device_register(&my_bus_dev);  
    if(ret)  
        printk("Fail to register device:my_bus!\n");      
  
    return ret;  
}  
  
static void __exit my_bus_exit(void)  
{  
    printk("[Call my_bus_exit!]\n");  
    device_unregister(&my_bus_dev);  
    bus_unregister(&my_bus_type);  
}  

MODULE_AUTHOR("Chang ShaoRan");  
MODULE_LICENSE("GPL"); 
module_init(my_bus_init);  
module_exit(my_bus_exit); 
