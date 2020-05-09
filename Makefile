obj-m := Hello.o

KDIR := /usr/src/linux-headers-5.3.0-46-generic/

PWD := $(shell pwd)

all:  
	$(MAKE) -C $(KDIR) M=$(PWD) modules  
        
clean:  
	rm *.mod.c *.o *.ko  modules.* Module.* *.mod

