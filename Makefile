obj-m := iowait_stuck.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
insmod:
	insmod iowait_stuck.ko
rmmod:
	rmmod iowait_stuck.ko
