OUT=kq.bin

CXX=clang++

ENTRY_POINT=-Wl,-e,0x7c00
#BINARY=-Wl,--oformat=binary -nostdlib -fomit-frame-pointer -fno-builtin -nostartfiles -nodefaultlibs -m16

OPT=-Os

CFLAGS= $(OPT) -std=c++1z -m16
LFLAGS=-Wl,--oformat=binary -nostdlib -fomit-frame-pointer -fno-builtin -nostartfiles -nodefaultlibs $(ENTRY_POINT)


all: main.cpp
	$(CXX) $(LFLAGS) $(CFLAGS) main.cpp -o $(OUT)

asm: main.cpp
	$(CXX) $(CFLAGS) -g -S main.cpp -o $(OUT).asm

show: all
	@ndisasm -b 16 $(OUT)


