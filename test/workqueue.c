#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL")
MODULE_AUTHOR("Chang")

struct my_data{
	struct work_struct my_work;
	int value;
};

struct workqueue_struct *wq = NULL;
struct work_struct work_queue;

struct my_data* init_data(struct my_data *md)
{
	md = (struct my_data*)kmalloc(sizeof(struct my_data), GFP_KERNEL);
	md->value = 1;
	md->my_work = work_queue;
	return md;
}

static void work_func(struct work_struct *work)s
{
	struct my_data *md = container_of(work, struct my_data, my_work);
	printk("<2>""The value of my data is:%d\n", md->value);
}

static __init int work_init(void)
{
	struct my_data *md=NULL;
	struct my_data *md2=NULL;
	md2=init_data(md2);

	md=init_data(md);     

    md2->value=20;

    md->value=10;
    INIT_WORK(&md->my_work,work_func);

    schedule_work(&md->my_work);
    
    wq=create_workqueue("test");

    INIT_WORK(&md2->my_work,work_func);

    queue_work(wq,&md2->my_work);    

    return 0;
}

static void work_exit(void)

{
         destroy_workqueue(wq);
}

module_init(work_init);

module_exit(work_exit);
