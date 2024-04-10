# 设置交叉编译器
CC = arm-linux-gnueabi-gcc

obj-m := ramdisk.o

KERNEL_SRC := /home/zyc/code/kdriver/linux-4.0
SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c \
	rm -f Module.markers Module.symvers modules.order \
	rm -rf .tmp_versions Modules.symvers
