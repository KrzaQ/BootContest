FMT=bin

OPT=-Ox

all: main.asm
	nasm main.asm -f $(FMT) $(OPT) -o kq.bin


show: all
	@echo "bytes:" `cat kq.bin|wc -c`
	@cat kq.bin|hexdump -C
	@ndisasm kq.bin

