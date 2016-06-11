FMT=bin

OPT=-Ox

all: main.asm
	nasm main.asm -f $(FMT) $(OPT) -o kq.bin


show: all
	@cat kq.bin|hexdump -C
	@echo "bytes:" `cat kq.bin|wc -c`
	@ndisasm kq.bin

