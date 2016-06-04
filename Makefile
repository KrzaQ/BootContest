OUT=kq.bin

CXX=clang++

ENTRY_POINT=-Wl,-e,0x7c00
BINARY=-Wl,--oformat=binary -nostdlib -fomit-frame-pointer -fno-builtin -nostartfiles -nodefaultlibs -m16

OPT=-O3

CFLAGS=$(BINARY) $(ENTRY_POINT) $(OPT)

all: main.cpp
	$(CXX) $(ENTRY_POINT) $(BINARY) $(OPT) main.cpp -o $(OUT)

asm: main.cpp
	$(CXX) $(BINARY) -S main.cpp -o $(OUT).asm

show: all
	@ndisasm -b 16 $(OUT)


