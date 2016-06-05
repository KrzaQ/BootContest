#include "common.hpp"

#include "video_buffer.hpp"

//template<typename T>
//inline T& addr(u32 addr){
//	auto ptr = reinterpret_cast<void*>(addr);
//	return *static_cast<T*>(ptr);
//}

static kq::VideoBuffer screen{kq::NoSegmentChange{}};
static kq::SegmentedAddress<0x9000, kq::SegmentRegister::fs, kq::IndexRegister::si> data{kq::NoSegmentChange{}};

struct Keyboard
{
	static inline bool keyAvaiable() noexcept {
		u8 ret = 1;
		asm(
			"movb 1, %%ah; \n\t"
			"int $16h; \n\t"
			"jnz 1f; \n\t"
			"movb $0h, %0; \n\t"
			"1:"
			: "=g" (ret)
			:
			: "ax"
		);
		return ret;
	}

	static inline u8 getKey() noexcept {
		u8 k;
		asm(
			"xor %%ax, %%ax; \n\t"
			"int $22; \n\t"
			"movb %%ah, %0; \n\t"
			: "=g" (k)
			:
			: "ax"
		);
		return k;
	}

};

static inline void sleep(u32 us) noexcept {
	us *= 1000;
	asm(
		"movb 86, %%ah; \n\t"
		"movw %0, %%cx; \n\t"
		"movw %1, %%dx; \n\t"
		"int $21;"
		:
		: "g" (us >> 16), "g" (us % 0xFFFF)
		: "ax", "cx", "dx"
	);
}

Keyboard keyboard;

struct Brick
{
	u8 x;
	u8 y;
	bool valid;
};

class Game
{
public:
	struct Entity{
		char character;
		kq::VideoBuffer::Colour foreground;
		kq::VideoBuffer::Colour background;
	};

	using Colour = kq::VideoBuffer::Colour;

	constexpr static Entity Food = { '+', Colour::Brown, Colour::LightGreen };
	constexpr static Entity Grass = { ' ', Colour::LightGreen, Colour::LightGreen };
	constexpr static Entity Snake = { 'O', Colour::Black, Colour::LightGreen };


	Game() noexcept {
		screen.set_addr(0);
//		for(u16 i = 400; i --> 0;){
//			auto it = screen.iter();
//			*it = Food.character;
//			++it;
//			*it = screen.makeCharColour(Food.foreground, Food.background);
//			++it;
//		}
		for(u16 i = 1920; i --> 0;){
			auto it = screen.iter();
			*it = Grass.character;
			++it;
			*it = screen.makeCharColour(Grass.foreground, Grass.background);
			++it;
		}
	}

	void loop() noexcept {
		bool active = false;
		while(!active){
			if(keyboard.getKey() == 0x39){
				active = true;
			}
		}

		while(active){
			sleep(16);

//			processBall();
//			screen.writeChar('0' + n, 3840);
//			n++;
//			n = n & 0x7;
//			if(keyboard.keyAvaiable()){
//				u8 k = keyboard.getKey();

//				screen.writeChar(toHex((k>>4)&0xF), 3840);
//				screen.writeChar(toHex(k&0xF), 3842);
//			}
		}
	}

	char elementAt(u8 x, u8 y) noexcept {
		u16 addr = 160 * y + 2 * x;
		return screen.raw_read<8>(addr);
	}

	void processSnake() noexcept {
//		u8 x = b.x;
//		u8 y = b.y;
//		b.x += b.speed_horizontal;
//		b.y += b.speed_vertical;
//		screen.writeChar(Grass.character, 160 * y + 2 * x, Grass.foreground, Grass.background);
//		x = b.x;
//		y = b.y;
//		screen.writeChar(Ball.character, 160 * y + 2 * x, Ball.foreground, Ball.background);
	}

	void drawBall() noexcept {

	}
};

static inline void foo()
{
	asm("mov 9000, %bp");
	asm("mov %bp, %sp");


	Game g;

	g.loop();

//	screen.writeLine("Hello, World!", 0);
//	screen.raw_write_n("Hello, World!", 14, 0);

//	while(true){
//		asm("cli");
//		addr<u8>(base + cnt) = 0x0041;
//		buf.raw_write<16>(0x941, cnt);
//		cnt = (cnt + 2) % 4000;
//		buf
//		asm("mov $0B000h, %ax");
//		asm("movw %ax, %ds");
//		asm("movb $41h, %ds:8000h");
//		asm("sti");
//	}
}

#ifdef YES_MAIN

int main()
{
}

#else // YES_MAIN

void _start()
{
	kq::VideoBuffer{};
	//kq::SegmentedAddress<0x9000, kq::SegmentRegister::fs, kq::IndexRegister::si>{};
	foo();
}

#endif // YES_MAIN

