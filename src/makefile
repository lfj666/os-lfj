BUILD:=../build
SRC:=.
ENTRYPOINT:=0x10000

CFLAGS:= -m32
CFLAGS+= -fno-builtin # 不需要gcc内置函数
CFLAGS+= -nostdinc # 不需要标准头文件
CFLAGS+= -fno-pic # 不需要位置无关代码
CFLAGS+= -fno-pie # 不需要位置无关可执行程序
CFLAGS+= -nostdlib # 不需要标准库
CFLAGS+= -fno-stack-protector # 不需要桟保护
CFLAGS:= $(strip ${CFLAGS})

DEBUG:=-g
INCLUDE:=-I$(SRC)/include

# boot和loader用
$(BUILD)/boot/%.bin: ./boot/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@

# start用
$(BUILD)/kernel/%.o: ./kernel/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

# c源代码用
$(BUILD)/kernel/%.o: ./kernel/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@


# kernel.bin用
$(BUILD)/kernel.bin: $(BUILD)/kernel/start.o \
					 $(BUILD)/kernel/main.o \
					 $(BUILD)/kernel/io.o \
					 $(BUILD)/kernel/string.o \
					 $(BUILD)/kernel/console.o \
					 $(BUILD)/kernel/printk.o \
					 $(BUILD)/kernel/vsprintf.o \
					 $(BUILD)/kernel/assert.o \
					 $(BUILD)/kernel/debug.o \
					 $(BUILD)/kernel/global.o \
					 $(BUILD)/kernel/task.o \
					 $(BUILD)/kernel/schedule.o \
					 $(BUILD)/kernel/interrupt.o \
					 $(BUILD)/kernel/handler.o \
					 $(BUILD)/kernel/stdlib.o \
					 $(BUILD)/kernel/clock.o \



	$(shell mkdir -p $(dir $@))
	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPOINT)

# system.bin用
$(BUILD)/system.bin: $(BUILD)/kernel.bin
	objcopy -O binary $< $@

# system.map用
$(BUILD)/system.map: $(BUILD)/kernel.bin
	nm $< | sort > $@

# master.img用
$(BUILD)/master.img: $(BUILD)/boot/boot.bin $(BUILD)/boot/loader.bin $(BUILD)/system.bin $(BUILD)/system.map

	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@

	dd if=$(BUILD)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD)/system.bin of=$@ bs=512 count=200 seek=10 conv=notrunc


test: $(BUILD)/master.img


.PHONY:usb
usb:$(BUILD)/boot/boot.bin /dev/sdb
	sudo dd if=/dev/sdb of=tmp.bin bs=512 count=1 conv=notrunc
	cp tmp.bin usb.bin
	sudo rm tmp.bin
	dd if=$(BUILD)/boot/boot.bin of=usb.bin bs=446 count=1 conv=notrunc
	sudo dd if=usb.bin of=/dev/sdb bs=512 conv=notrunc
	rm usb.bin


.PHONY:clean
clean:
	rm -rf $(BUILD)

.PHONY:bochs
bochs:$(BUILD)/master.img
	bochs -q

.PHONY:qemu
qemu:$(BUILD)/master.img
	qemu-system-i386 \
	-m 32M \
	-boot c \
	-drive file=$<,if=ide,index=0,media=disk,format=raw \
	-audiodev pa,id=hda \
	-machine pcspk-audiodev=hda \

.PHONY:qemug
qemug:$(BUILD)/master.img
	qemu-system-i386 -s -S -m 32M -boot c \
	-drive file=$<,if=ide,index=0,media=disk,format=raw \
	-audiodev pa,id=hda \
	-machine pcspk-audiodev=hda \



$(BUILD)/master.vmdk:$(BUILD)/master.img
	qemu-img convert -O vmdk $< $@

.PHONY:vmdk
vmdk:$(BUILD)/master.vmdk
