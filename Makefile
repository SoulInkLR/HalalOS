.PHONY: clean all

all:

	$(MAKE) -C user/ all VERBOSE=$(VERBOSE)
	$(MAKE) -C kernel/ kernel.bin VERBOSE=$(VERBOSE)

clean:
	$(MAKE) clean -C kernel/
	$(MAKE) clean -C user/

qemu: all
	qemu-system-i386 --no-kvm -m 256 -kernel kernel/kernel.bin &

qemu_debug: all 
	qemu-system-i386 --no-kvm -m 256 -kernel kernel/kernel.bin -s -S -debugcon stdio &

qemu_terminal: all 
	qemu-system-i386 --no-kvm -m 256 -kernel kernel/kernel.bin -debugcon stdio &	

disk : 
	mkdir -p $@
.PHONY: bochs
bochs: all disk
	sudo mount -t ext2 -o loop,offset=1048576 disk.img disk/
	sudo cp kernel/kernel.bin disk/kernel.bin
	sync
	sudo umount disk/
	bochs  
