#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>


struct timer_list timer_test;

void timer_up(struct timer_list *time_list);

void timer_up(struct timer_list *time_list)
{
	static int number = 0;
	printk(KERN_DEBUG"timer is up!:%d\n",number++);
	timer_test.expires = jiffies + 5 * HZ;
	add_timer(&timer_test);
}

static int __init timer_init(void)
{
	timer_test.entry.next = NULL;
	timer_test.entry.pprev = NULL;
	timer_test.expires = jiffies + 5 * HZ;
	timer_test.function = timer_up;
	add_timer(&timer_test);
	return 0;
}

static void __exit timer_exit(void)
{
	del_timer(&timer_test);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL v2");
