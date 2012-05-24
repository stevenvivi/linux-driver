target := 
app = $(patsubst %, %_test, $(target))
appsrc = $(patsubst %, %_test.c, $(target))

ifneq ($(arch),)
CC = arm-none-linux-gnueabi-gcc
else
CC = gcc
endif

ifneq ($(KERNELRELEASE),)
	obj-m := $(patsubst %, %.o, $(target))
else
KERNELDIR :=/lib/modules/$(shell uname -r)/build  
PWD := $(shell pwd)
endif

modules:
	@echo "\033[32;47m Now Let's begin Compile !\033[0m"
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

test:
	@echo "\033[34;47m Now Let's Compile the App !\033[0m"
	$(CC) -o $(app) $(appsrc)

.PHONY : clean test


clean:
	@echo "\033[34;47m Clean all the temp file !\033[0m"
	-rm -rf *.o *.ko *.mod.c *.symvers *.order  .$(target)* .tmp_versions 
