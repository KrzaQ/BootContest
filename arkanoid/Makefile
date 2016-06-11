FMT=bin

OPT=-Ox

all: main.asm
	nasm main.asm -f $(FMT) $(OPT) -o kq.bin


show: all
	@cat kq.bin|hexdump -C
	@ndisasm kq.bin
	@echo "bytes:" `cat kq.bin|wc -c`

