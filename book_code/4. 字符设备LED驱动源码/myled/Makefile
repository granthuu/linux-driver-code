
#make file for myled.c

obj-m:=myled.o
KDIR = /home/cao/mini2440/linux-2.6.29

all:
	make -C $(KDIR)  SUBDIRS=$(shell pwd) modules
	arm-linux-gcc -o ledtest ledtest.c
clean:
	rm -rf *.o *.ko

