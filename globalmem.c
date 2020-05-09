#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/signal.h>
#include <linux/sched/signal.h>
#include <linux/poll.h>

#define GLOBALMEM_SIZE 0x1000
#define MEM_CKEAR 0X1
#define GLOBALMEM_MAJOR 230

#define GLOBALMEM_MAGIC 'g'
#define MEM_CLEAR _IO(GLOBALMEM_MAGIC,0)

#define CLASS_NAME "test_class"


MODULE_LICENSE("GPL");

static int globalmem_major = GLOBALMEM_MAJOR;
module_param(globalmem_major,int,S_IRUGO);

struct class *my_class = NULL;

struct globalmem_dev {
	struct cdev cdev;
	unsigned char mem[GLOBALMEM_SIZE];
	struct mutex my_mutex;
	unsigned int current_len;
	struct wait_queue_head read_list;
	struct wait_queue_head write_list;
	struct fasync_struct *async_queue;
};

int  globalmem_fasync(int fd ,struct file *filp,int mode);
https://zhidao.baidu.com/question/511678751.html)
{
	filp->private_data = globalmem_devp;
	return 0;
}

int globalmem_release(struct inode *node, struct file *filp)
{
	printk(KERN_DEBUG"globalmem_release\n");
	if(globalmem_fasync(-1,filp,0)) {
		printk(KERN_WARNING"globalmem_fasync(-1,filp,0) is failed\n");
	}
	return 0;
}

static ssize_t globalmem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev;
	DECLARE_WAITQUEUE(wait,current);
	
	dev = filp->private_data;
	printk(KERN_INFO"read_one");
	mutex_lock(&dev->my_mutex);
	add_wait_queue(&dev->read_list,&wait);
	while(dev->current_len == 0){
		if(filp->f_flags & O_NDELAY){
			ret = -EAGAIN;
			goto out1;
		}
		set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->my_mutex);
		printk(KERN_INFO"read_two");
		schedule();
		set_current_state(TASK_RUNNING);
		if(signal_pending(current)){
			ret = -ERESTARTSYS;
			goto out2;
		}
		printk(KERN_INFO"read_three");
		mutex_lock(&dev->my_mutex);
	}
		printk(KERN_INFO"read_four");
		if (count > dev->current_len)
			count = dev->current_len;
	
		if (copy_to_user(buf,dev->mem,count)){
			ret = -EFAULT;
		}
		else{
			memcpy(dev->mem,dev->mem+count,dev->current_len-count);
			
			dev->current_len -= count;
			printk(KERN_INFO"read %u bytes(s) ,current is  %u\n",count,dev->current_len);
			wake_up_interruptible(&dev->write_list);
			ret = count;
		}
out1:
	printk(KERN_INFO"read_five");
	remove_wait_queue(&dev->read_list,&wait);
	mutex_unlock(&dev->my_mutex);
	return ret;
out2:
	printk(KERN_INFO"read_six");
	remove_wait_queue(&dev->read_list,&wait);
	return ret;
}

static ssize_t globalmem_write(struct file *filp,const char __user *buf, size_t size, loff_t *ppos)
{
	unsigned int count = size;
	int ret = 0;
	struct globalmem_dev *dev;
	DECLARE_WAITQUEUE(wait,current);

	dev = filp->private_data;
	mutex_lock(&dev->my_mutex);
	add_wait_queue(&dev->read_list,&wait);
	while(dev->current_len == GLOBALMEM_SIZE){
		if(filp->f_flags & O_NDELAY){
			ret = -EFAULT;
			goto out1;
		}
		set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->my_mutex);
		schedule();
		set_current_state(TASK_RUNNING);
		if(signal_pending(current)){
			ret = -ERESTARTSYS;
			goto out2;
		}
		mutex_lock(&dev->my_mutex);
	}
	if (count > GLOBALMEM_SIZE - dev->current_len)
			count = GLOBALMEM_SIZE - dev->current_len;

	if(copy_from_user(dev->mem+dev->current_len,buf,count)){
		ret = -EFAULT;
	}
	else{
		dev->current_len += count;

		printk(KERN_INFO"written %u bytes(s),cuurent_len = %u\n",count,dev->current_len);
		wake_up_interruptible(&dev->read_list);

		if(dev->async_queue){
			kill_fasync(&dev->async_queue,SIGIO,POLL_IN);
			printk(KERN_DEBUG "%s kill SIGIO\n",__func__);
		}
		ret = count;
	}
out1:
	remove_wait_queue(&dev->write_list,&wait);
	mutex_unlock(&dev->my_mutex);
	return ret;
out2:
	remove_wait_queue(&dev->write_list,&wait);
	return ret;
}

static loff_t globalmem_llseek(struct file *filp, loff_t offset, int orig)
{
	loff_t ret = 0;
	struct globalmem_dev *dev;
	dev = filp->private_data;
	mutex_lock(&dev->my_mutex);
	switch(orig){
		case 0:
			if (offset < 0 || offset > GLOBALMEM_SIZE){
				ret = -EINVAL;
				break;
			}
			filp->f_pos = offset;
			ret = filp->f_pos;
			break;
		case 1:
			if (filp->f_pos + offset < 0 || filp->f_pos + offset > GLOBALMEM_SIZE){
				ret = -EINVAL;
				break;
			}
			filp->f_pos += offset ;
			ret = filp->f_pos;
			break;
		case 2:
			if(offset>0 || offset < -GLOBALMEM_SIZE){
				ret = -EINVAL;
				break;
			}
			filp->f_pos += offset ;
			ret = filp->f_pos;
			break;
		default:
			ret = -EINVAL;
			break;
	}
	mutex_unlock(&dev->my_mutex);
	return 0;
}

static long globalmem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct globalmem_dev *dev = filp->private_data;
		printk(KERN_DEBUG"cmd = %d\n",cmd);
		switch(cmd)	{
		case MEM_CLEAR:
			mutex_lock(&dev->my_mutex);
			memset(dev->mem,0,GLOBALMEM_SIZE);
			dev->current_len = 0;
			mutex_unlock(&dev->my_mutex);
			printk("globalmem is set to zero\n");
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

unsigned int globalmem_poll(struct file *filp, struct poll_table_struct *wait)
{
		unsigned int mask = 0;
		unsigned int debug = 0;
		struct globalmem_dev *dev = filp->private_data;
		mutex_lock(&dev->my_mutex);
		printk(KERN_DEBUG"wait.key = %d",wait->_key);

		poll_wait(filp,&dev->read_list,wait);
		poll_wait(filp,&dev->write_list,wait);

		printk(KERN_DEBUG"debug = %u\n",debug);
		if (dev->current_len != 0) {
			mask |= POLLIN;
		}
		
		if (dev->current_len != GLOBALMEM_SIZE) {
			mask |= POLLOUT;
		}

		mutex_unlock(&dev->my_mutex);

		return mask;
}
int  globalmem_fasync(int fd ,struct file *filp,int mode)
{
	struct globalmem_dev *dev = filp->private_data;
	return fasync_helper(fd, filp, mode, &dev->async_queue);
}

static const struct file_operations globalmem_fops = {
	.owner = THIS_MODULE,
	.open = globalmem_open,
	.release = globalmem_release,
	.write = globalmem_write,
	.read = globalmem_read,
	.llseek = globalmem_llseek,
	.unlocked_ioctl = globalmem_ioctl,
	.poll = globalmem_poll,
	.fasync = globalmem_fasync,
};


static int globalmem_setup_cdev(struct globalmem_dev *dev,int index)
{
	int err,devno = MKDEV(globalmem_major,index);

	cdev_init(&dev->cdev,&globalmem_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev,devno,1);
	if(err){
		printk(KERN_NOTICE"Error %d adding globlmem%d",err,index);
		return err;
	}
	return 0;
}
	
static int __init globalmem_init(void)
{
	int ret = 0;
	dev_t devno = MKDEV(globalmem_major,0);

	my_class = class_create(THIS_MODULE,CLASS_NAME);
	if( (sizeof(typeof(*my_class)) == sizeof(ret)) ){
		printk(KERN_INFO"class create is failed.\n");
		return ret;
	}
	if(globalmem_major)
		ret = register_chrdev_region(devno,1,"globalmem");
	else{
		ret = alloc_chrdev_region(&devno,0,1,"globalmem");
		globalmem_major = MAJOR(devno);
	}
	if(ret < 0)
		return ret;
	globalmem_devp = kzalloc(sizeof(struct globalmem_dev),GFP_KERNEL);
	if(!globalmem_devp){
		ret = -ENOMEM;
		goto fail_malloc;
	}
	mutex_init(&globalmem_devp->my_mutex);
	init_waitqueue_head(&globalmem_devp->read_list);
	init_waitqueue_head(&globalmem_devp->write_list);

	ret = globalmem_setup_cdev(globalmem_devp,0);
	if(ret < 0){
		goto setup_cdev_failed;
	}
	device_create(my_class,NULL,devno,NULL,"globalmem");
	return 0;

setup_cdev_failed:
	kzfree(globalmem_devp);
fail_malloc:
	unregister_chrdev_region(devno,1);
	return ret;
}
module_init(globalmem_init);

static void __exit globalmem_exit(void)
{
	dev_t devno = MKDEV(globalmem_major,0);
	device_destroy(my_class,devno);
	cdev_del(&globalmem_devp->cdev);
	kzfree(globalmem_devp);
	unregister_chrdev_region(devno,1);
	class_destroy(my_class);
}

module_exit(globalmem_exit);















