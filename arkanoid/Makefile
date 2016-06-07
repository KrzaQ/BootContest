all: main.asm
	nasm main.asm -o kq.bin


show: all
	@cat kq.bin|hexdump -C
	@ndisasm kq.bin

