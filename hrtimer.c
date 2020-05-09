#include <linux/module.h>
#include <linux/init.h>
#include <linux/hrtimer.h>

struct hrtimer hrtimer_test;
int i = 0;

enum hrtimer_restart hrtimer_test_fn(struct hrtimer *ht)
{
	printk(KERN_DEBUG "hrtimer up:%d\n",i++);
	hrtimer_forward_now(ht, ns_to_ktime(1000000000));
	return HRTIMER_RESTART;
}


static int __init hrtimer_test_init(void)
{
	hrtimer_init(&hrtimer_test,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
	hrtimer_test.function = hrtimer_test_fn;
	hrtimer_start(&hrtimer_test,ns_to_ktime(1000000000),HRTIMER_MODE_REL);

	return 0;
}

static void __exit hrtimer_test_exit(void)
{
	hrtimer_cancel(&hrtimer_test);
}

module_init(hrtimer_test_init);
module_exit(hrtimer_test_exit);

MODULE_LICENSE("GPL v2");
