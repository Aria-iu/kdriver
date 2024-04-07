# 设置交叉编译器
CC = arm-linux-gnueabi-gcc
 
# 设置其他编译标志
# EXTRA_CFLAGS += -I$(INCLUDEDIR)
 
# 编译目标
all: hello.ko
 
# 驱动模块规则
obj-m += hello.o
 
# 调用内核构建系
hello.ko:
	make -C /home/zyc/code/kdriver/linux-4.0 M=$(PWD) modules
 
# 清理规则
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
