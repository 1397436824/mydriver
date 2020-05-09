#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>


#define irq_num 7



irqreturn_t threaded_irq_handler(int irq, void *dev_id)
{
	return IRQ_WAKE_THREAD;
}

irqreturn_t threaded_irq_fn(int irq, void *dev_id)
{
	
	return 0;
}





static int __init threaded_init(void)
{
	request_threaded_irq(irq_num,threaded_irq_handler,
			 threaded_irq_fn, IRQF_ONESHOT,
			 "threaded_irq", NULL);
	return 0;
}

static void __exit threaded_exit(void)
{
	free_irq(irq_num,NULL);
}



module_init(threaded_init);
module_exit(threaded_exit);
MODULE_LICENSE("GPL v2");
