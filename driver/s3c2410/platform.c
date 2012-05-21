#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>

struct resource s3c_led_resources[] = {
	[0] = {
	.start = 0x56000050,
	.end = 0x56000054,
	.flags = IORESOURCE_MEM,
	}
};

struct platform_device s3c_device_led = {
	.name = "s3c2410-led",
	.ld = -1,
	.num_resource = ARRAY_SIZE();
	.resource = s3c_led_resource,
};

static int __init platform_dev_init(void)
{
	int ret = 0;
	ret = platform_device_register(&s3c_device_led);
	if(ret)
		printk("platform_device_register failed! \n");
	return ret;
}

static void __exit platform_dev_exit(void)
{
	platform_device_unregister(&s3c_device_led);
}

module_init(platform_dev_init);
module_exit(platform_dev_exit);

MODULE_LICENSE("GPL");





static void *led_base;

static int my_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct resource *led_mem;
	struct device *led_dev;

	unsigned int gpfcon;
	unsigned int gpfdata;

	int ret;
	int size;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL){
		printk("no memory resource specified\n");
		return -ENOENT;
	}

	size = resource_size(res);

	led_mem = request_mem_region(res->sstart, size, pdev->name);
	if(led_mem == NULL){
		printk("failed to get memory region\n");
		return -EBUSY;
	}

	led_base = ioremap(res->start, size);
	if (led_base == NULL){
		printk("failed to ioremap() region\n");
		ret = -EINVAL;
		goto err_req;
	}

	gpfcon = readl(led_base);
	gpfcon = (gpfcon & (~(0xff << 8)) | (0x55 << 8));
	writel(gpfcon, led_base);

err_req:
	release_resource(led_mem);
	kfree(led_mem);

	return 0;
}



static int my_remove(struct platform_device *dev)
{
	printk("Driver found device upluged !\n");
	return 0;
}


static struct platform_drvier my_driver = {
	.probe = my_probe,
	.remove = my_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "s3c2410-led",
	},
};


static int __init my_driver_init(void)
{
	return platform_driver_register(&my_driver);
}

static void __exit my_driver_exit(void)
{
	platform_driver_unregister(&my_driver);
}

module_init(my_driver_init);
module_exit(my_driver_exit);

