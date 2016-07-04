
# sudo apt-get install g++ binutils libc6-dev-i386
# sudo apt-get install VirtualBox grub-legacy xorriso

GCCPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
ASPARAMS = --32
LDPARAMS = -melf_i386

<<<<<<< HEAD
objects = loader.o gdt.o port.o kernel.o
=======
objects = loader.o gdt.o port.o interruptstubs.o interrupts.o keyboard.o kernel.o
>>>>>>> a2f1cad... Files for part 6, https://www.youtube.com/watch?v=RTKj4jdo_2s



%.o: %.cpp
	gcc $(GCCPARAMS) -c -o $@ $<

%.o: %.s
	as $(ASPARAMS) -o $@ $<

uranium.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

test.iso: uranium.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp uranium.bin iso/boot/uranium.bin
	echo 'set timeout=0'					  > iso/boot/grub/grub.cfg
	echo 'set default=0'					 >> iso/boot/grub/grub.cfg
	echo '' 								 >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/uranium.bin'	>> iso/boot/grub/grub.cfg
	echo '	boot'							 >> iso/boot/grub/grub.cfg
	echo '}'								 >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=test.iso iso
	rm -rf iso

vbox: test.iso
	(killall VirtualBox && sleep 1) || true
	VirtualBox --startvm 'My Operating System' &

test: test.iso
	(killall qemu-system-i386 && sleep 1) || true
		qemu-system-i386 -cdrom test.iso -m 128M &

qemu: test.iso
		(killall qemu-system-i386 && sleep 1) || true
		qemu-system-i386 -cdrom test.iso -m 128M &

install: uranium.bin
	sudo cp $< /boot/uranium.bin

.PHONY: clean
clean:
	rm -f $(objects) uranium.bin test.iso
