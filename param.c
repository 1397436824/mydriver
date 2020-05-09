#include <linux/init.h>
#include <linux/module.h>




static char *book_name = "dissecting Linux Device Driver";
module_param(book_name,charp,S_IRUGO);

static int book_num = 4000;
module_param(book_num,int,S_IRUGO);


static int __init para_init(void)
{
	printk(KERN_INFO "book name:%s\n",book_name);
	printk(KERN_INFO "book num:%d\n",book_num);
	printk(KERN_INFO "S_IRUGO:%o\n",S_IRUGO);
	return 0;
}

static void __exit para_exit(void)
{
	printk(KERN_INFO"book module exit\n");
}



module_init(para_init);
module_exit(para_exit);

MODULE_LICENSE("GPL v2");
