#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#define test_irq 7

void test_do_tasklet(unsigned long data);
DECLARE_TASKLET(test_tasklet, test_do_tasklet, 0);

void test_do_tasklet(unsigned long data)
{
	printk(KERN_DEBUG"%s",__func__);
}



irqreturn_t test_interrupt(int irq, void *dev_id)
{
	printk(KERN_DEBUG"%s",__func__);
	tasklet_schedule(&test_tasklet);
	return IRQ_HANDLED;
}

static void __exit test_exit(void)
{
	free_irq(test_irq,NULL);
}

static int __init test_init(void)
{
	int result = request_irq(test_irq,test_interrupt,0,"xxx",NULL);

	return result;
}


module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
