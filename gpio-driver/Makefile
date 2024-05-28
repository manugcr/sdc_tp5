obj-m += gpio-signal.o
KDIR = /lib/modules/$(shell uname -r)/build

all: kernel_module

kernel_module:
	make -C $(KDIR) M=$(shell pwd) modules

clean:
	make -C $(KDIR) M=$(shell pwd) clean
