#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/stat.h>


void obj_test_release(struct kobject *);
ssize_t kobj_test_show(struct kobject *, struct attribute *, char *);
ssize_t kobj_test_store(struct kobject *, struct attribute *, const char *, size_t);


static struct kobject kobj;

static struct attribute test_attr = {
	.name = "kobj_config",
	.mode = S_IRWXUGO,
};

static struct attribute *def_attrs[] ={

	&test_attr,
	NULL,
};

static struct sysfs_ops obj_test_sysops = {
	.show = kobj_test_show,
	.store = kobj_test_store,
};


static struct kobj_type ktype = {
	.release = obj_test_release,
	.sysfs_ops = &obj_test_sysops,
	.default_attrs = def_attrs,
};

void obj_test_release(struct kobject *kobject)  
{
	  
	printk("eric_test: release!\n");  
}


ssize_t kobj_test_show(struct kobject *kobject,struct attribute *attr,char *buf)  
{
	  
	printk("Have show -->\n");  
	printk("attrname: %s.\n",attr->name);  
	sprintf("buf,%s\n",attr->name);  
	return strlen(attr->name) + 2;
}


ssize_t kobj_test_store(struct kobject *kobject,struct attribute *attr,  
		            const char *buf,size_t size)  
{
	  
	printk("Have store -->\n");  
	printk("write: %s.\n",buf);  
	return size;  
}


static int __init kobj_test_init(void) 
{
	  
	printk("Kobject test init.\n");
	kobject_init_and_add(&kobj,&ktype,NULL,"kobject_test");
	return 0;
}  
  
static void __exit kobj_test_exit(void)
{
	  
	printk("Kobject test exit.\n");  
	kobject_del(&kobj);  
}


MODULE_AUTHOR("Chang ShaoRan");  
MODULE_LICENSE("GPL"); 
module_init(kobj_test_init);  
module_exit(kobj_test_exit);
