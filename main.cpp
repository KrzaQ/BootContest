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

struct Ball
{
	float speed_horizontal;
	float speed_vertical;
	float x;
	float y;
};

static inline u8 toHex(u8 c) noexcept {
	return c < 10 ? c + '0' : c + 'A' - 10;
}

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

	constexpr static Entity Brick = { 'X', Colour::Green, Colour::Black };
	constexpr static Entity Empty = { ' ', Colour::LightBlue, Colour::LightBlue };
	constexpr static Entity Desk = { '^', Colour::Green, Colour::Black };
	constexpr static Entity Ball = { '^', Colour::Green, Colour::Black };


	Game() noexcept:
		b{0.1,0.1,40,19},
		points(0),
		active(false)
	{
		screen.set_addr(0);
		for(u16 i = 400; i --> 0;){
			auto it = screen.iter();
			*it = Brick.character;
			++it;
			*it = screen.makeCharColour(Brick.foreground, Brick.background);
			++it;
		}
		for(u16 i = 1520; i --> 0;){
			auto it = screen.iter();
			*it = Empty.character;
			++it;
			*it = screen.makeCharColour(Empty.foreground, Empty.background);
			++it;
		}
	}

	void loop() noexcept {
//		u8 n = 0;
		while(!active){
			if(keyboard.getKey() == 0x39){
				active = true;
			}
		}

		while(true){
			sleep(16);

			processBall();
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

	bool brickAt(u8 x, u8 y) noexcept {
		u16 addr = 160 * y + 2 * x;
		return screen.raw_read<8>(addr) == 'X';
	}

	void processBall() noexcept {
		u8 x = b.x;
		u8 y = b.y;
		b.x += b.speed_horizontal;
		b.y += b.speed_vertical;
		screen.writeChar(Empty.character, 160 * y + 2 * x, Empty.foreground, Empty.background);
		x = b.x;
		y = b.y;
		screen.writeChar(Ball.character, 160 * y + 2 * x, Ball.foreground, Ball.background);
	}

	void drawBall() noexcept {

	}

	::Ball b;
	u16 points;
	bool active;
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

