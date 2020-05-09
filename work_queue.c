#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>


#define irq_num 7

struct work_struct test_wq;

void test_do_work(struct work_struct *work);

void test_do_work(struct work_struct *work)
{

}


irqreturn_t work_queue_interrupt(int irq, void *dev_id)
{
	schedule_work(&test_wq);
	return IRQ_HANDLED;
}

static int __init work_queue_init(void)
{
	int result = request_irq(irq_num,work_queue_interrupt,0,"work_queue_test",NULL);

	INIT_WORK(&test_wq,test_do_work);
	return 0;
}

static void __exit work_queue_exit(void)
{
	free_irq(irq_num,NULL);
}

module_init(work_queue_init);
module_exit(work_queue_exit);

MODULE_LICENSE("GPL");
