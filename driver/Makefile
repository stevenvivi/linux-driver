ifneq ($(KERNELRELEASE),)
	obj-m := second.o    
else
KERNELDIR :=/lib/modules/$(shell uname -r)/build  
PWD := $(shell pwd)
all:
	@echo "\033[32;47m Now Let's begin Compile !\033[0m"
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules
endif
.PHONY : clean


clean:
	@echo "\033[34;47m Clean all the temp file !\033[0m"
	rm -f *.o *.ko *.mod.c *.symvers *.order   
