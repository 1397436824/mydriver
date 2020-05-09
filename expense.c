#include <linux/init.h>
#include <linux/module.h>

#if 0 
	#define print(fmt,arg...) printk(fmt,##arg) 
#else
	#define print(fmt,arg...) 
#endif

int add_integar(int ,int);
int sub_integar(int ,int);

static int __init expense_init(void)
{
	__attribute__((unused)) int a=10,b=20;
	print(KERN_INFO"add=%d\n",add_integar(a,b));
	print(KERN_INFO"sub=%d\n",sub_integar(b,a));
	return 0;
}

static void __exit expense_exit(void)
{
	print(KERN_INFO"expense module exit\n");
}


module_init(expense_init);
module_exit(expense_exit);



MODULE_LICENSE("GPL v2");
