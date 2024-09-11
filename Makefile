KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(CURDIR)
KO := todo.ko

all: clean $(KO)

$(KO):
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install: $(KO)
	cp $(KO) $(MYLINUX)/initramfs

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
