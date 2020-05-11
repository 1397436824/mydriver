#include <linux/module.h>
#include <linux/init.h>
#include <linux/workqueue.h>

struct delayed_work my_delayed_work;

void work_fn(struct work_struct *work);

DECLARE_DELAYED_WORK(my_delayed_work,work_fn);

void work_fn(struct work_struct *work)
{
	printk(KERN_DEBUG"delayed_work test\n");
	schedule_delayed_work(&my_delayed_work, msecs_to_jiffies(1000));
}

static int __init delayed_work_init(void)
{
	schedule_delayed_work(&my_delayed_work, msecs_to_jiffies(1000));
	return 0;
}

static void __exit delayed_work_exit(void)
{
	cancel_delayed_work(&my_delayed_work);	
}

module_init(delayed_work_init);
module_exit(delayed_work_exit);

MODULE_LICENSE("GPL");
