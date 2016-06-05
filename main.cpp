#include "common.hpp"

#include "video_buffer.hpp"

//template<typename T>
//inline T& addr(u32 addr){
//	auto ptr = reinterpret_cast<void*>(addr);
//	return *static_cast<T*>(ptr);
//}



static inline void foo()
{
//	asm("movb 10h, %al;"
//		"movb 0Ah, %ah");
//	asm("movw $0A31h, %ax");
//	asm("movb $1, %cl");
//	asm("int $10h");
//	asm("mov $08000h, %ebp");
//	asm("mov %ebp, %esp");
//	constexpr u32 base = 0xB8000;
//	u16 cnt{};

	VideoBuffer buf;

	buf.writeLine("Hello, World!", 0);

	while(true){
//		asm("cli");
//		addr<u8>(base + cnt) = 0x0041;
//		buf.raw_write<16>(0x941, cnt);
//		cnt = (cnt + 2) % 4000;
//		buf
//		asm("mov $0B000h, %ax");
//		asm("movw %ax, %ds");
//		asm("movb $41h, %ds:8000h");
//		asm("sti");
	}
}

#ifdef YES_MAIN

int main()
{
}

#else // YES_MAIN

void _start()
{
	foo();
}

#endif // YES_MAIN

