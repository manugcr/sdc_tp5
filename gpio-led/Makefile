obj-m += gpio-led.o
KDIR = /lib/modules/$(shell uname -r)/build
USER_PROGRAM = gpio-control

all: $(BUILD_DIR) kernel_module user_program

kernel_module:
	make -C $(KDIR) M=$(shell pwd) modules

user_program: gpio-control.c
	gcc -o $(USER_PROGRAM) gpio-control.c

clean:
	make -C $(KDIR) M=$(shell pwd) clean
	rm -f $(USER_PROGRAM)
