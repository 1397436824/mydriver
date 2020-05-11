obj-m += timer_list.o hrtimer.o delayed_work.o

KDIR := /usr/src/linux-headers-$(shell uname -r)/

PWD := $(shell pwd)

all:  
	$(MAKE) -C $(KDIR) M=$(PWD) modules  
        
clean:  
	rm *.mod.c *.o *.ko  modules.* Module.* *.mod

