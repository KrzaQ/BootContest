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
		asm volatile(
			"movb $1, %%ah; \n\t"
			"int $22; \n\t"
			"jnz 1f; \n\t"
			"movb $0, %0; \n\t"
			"1:"
			: "=q" (ret)
			:
			: "ax"
		);
		return !ret;
	}

	static inline u8 getKey() noexcept {
		u8 k;
		asm volatile(
			"xor %%ax, %%ax; \n\t"
			"int $22; \n\t"
			"movb %%ah, %0; \n\t"
			: "=q" (k)
			:
			: "ax"
		);
		return k;
	}

};

static inline void sleep(u32 us) noexcept {
	us *= 1000;
	asm volatile(
		"movb 86, %%ah; \n\t"
		"movw %0, %%cx; \n\t"
		"movw %1, %%dx; \n\t"
		"int $21;"
		:
		: "Ri" (us >> 16), "Ri" (us % 0xFFFF)
		: "ax", "cx", "dx"
	);
}

static inline u16 ticks() noexcept {
	u16 ret;
	asm volatile(
		"xor %%ax, %%ax; \n\t"
		"int $26; \n\t"
		"movw %%cx, %0; \n\t"
		: "=q" (ret)
		:
		: "ax", "cx", "dx"
	);
	return ret;
}

Keyboard keyboard;

struct Brick
{
	u8 x;
	u8 y;
	bool valid;
};

//struct array_tag{};
//struct single_tag{};

template<u16 Addr, u8 Size, u16 Elements = 1>
struct MemoryEntity{
	constexpr static u16 addr = Addr;
	constexpr static u8 size = Size;
	constexpr static u16 elements = Elements;

	template<typename T>
	static inline void set(T val, u16 n) noexcept {
		data.raw_write<Size>(val, Addr + n * Size);
	}

	static inline auto get(u16 n) noexcept {
		return data.raw_read<Size>(Addr + n * Size);
	}
};

template<u16 Addr, u8 Size>
struct MemoryEntity<Addr, Size, 1>{
	constexpr static u16 addr = Addr;
	constexpr static u8 size = Size;

	template<typename T>
	static inline void set(T val) noexcept {
		data.raw_write<Size>(val, addr);
	}

	static inline auto get() noexcept {
		return data.raw_read<Size>(Addr);
	}
};

//template<typename T, typename... Ts>

//template<typename T>
//auto dbGet(T t){
//	return data.raw_read<T::size>(T::addr);
//}

//template<typename T, typename U>
//void dbSet(T t, U val){
//	data.raw_write<T::size>(val, T::addr);
//}

static inline u8 toHex(u8 x) noexcept {
	return x < 10 ? x + '0' : x - 10 + 'A';
}

class Game
{
public:
	struct Entity{
		char character;
		kq::VideoBuffer::Colour foreground;
		kq::VideoBuffer::Colour background;
	};

	enum Direction : u8{
		Up = 0x48,
		Down = 0x50,
		Left = 0x4B,
		Right = 0x4D,
	};

	enum State {
		Alive,
		Dead,
	};

	using Colour = kq::VideoBuffer::Colour;

	constexpr static Entity Food = { '+', Colour::Brown, Colour::LightGreen };
	constexpr static Entity Grass = { ' ', Colour::LightGreen, Colour::LightGreen };
	constexpr static Entity Snake = { 'O', Colour::Black, Colour::LightBlue };

	constexpr static auto Size = MemoryEntity<0x100, 16>{};
	constexpr static auto Head = MemoryEntity<0x102, 16>{};
	constexpr static auto Tail = MemoryEntity<0x104, 16>{};
	constexpr static auto Blocks = MemoryEntity<0x200, 16, 256>{};



	Game() noexcept {
		screen.set_addr(0);

		for(u16 i = 1920; i --> 0;){
			auto it = screen.iter();
			*it = Grass.character;
			++it;
			*it = screen.makeCharColour(Grass.foreground, Grass.background);
			++it;
		}

		Size.set(1);
		Head.set(0);
		Tail.set(0);
		Blocks.set((40 << 8) | 12, 0);
		writeSnakeElement(40, 12);
		spawnFood();
	}

	void loop() noexcept {
//		while(true){
//			screen.writeLine("Press space to start", 24);
			bool active = false;
			while(!active){
				if(keyboard.getKey() == 0x39){
					active = true;
				}
			}
			Direction d = Direction::Right;
//			u8 n;
			while(active){
				sleep(500);

				//			processBall();
				//			screen.writeChar('0' + n, 3840);
//				n++;
//				n = n & 0x7;
				if(keyboard.keyAvaiable()){
					d = static_cast<Direction>(keyboard.getKey());
//					u8 k = keyboard.getKey();
//					screen.writeChar(toHex((k>>4)&0xF), 3840);
//					screen.writeChar(toHex(k&0xF), 3842);
				}
//				processSnake(d);
//				asm("testw %ax, %ax;");
				u16 idx = Head.get();
//				asm("testw %ax, %ax;");
				u16 xy = Blocks.get(idx);
				u8 x = xy >> 8;
				u8 y = xy & 0xF;

				u8 ox, oy;
				switch(d){
				case Direction::Up:
					ox = x;
					oy = y-1;
					break;
				case Direction::Down:
					ox = x;
					oy = y+1;
					break;
				case Direction::Left:
					ox = x-1;
					oy = y;
					break;
				case Direction::Right:
					ox = x+1;
					oy = y;
					break;
				default:
					continue;
				}

				if(ox > 79 || oy > 24){
					active = false;
					break;
				}

				u8 next = elementAt(ox, oy);

				if(next == Snake.character){
					active = false;
					break;
				}

				writeSnakeElement(ox, oy);
				idx = (idx+1) % Blocks.elements;
				Head.set(idx);
				Blocks.set((ox << 8) | oy, idx);

				if(next != Food.character){
//					Tail.set(Tail.get()+1);
					idx = Tail.get();
					xy = Blocks.get(idx);
					Tail.set(idx % Blocks.elements);
					clearSnakeElement(xy >> 8, xy & 0xF);
				}
			}

			while(true);
//		}
	}

	void spawnFood() noexcept {
		for(u16 t = ticks();; t++){
			char c = elementAt(t >> 8, t & 0xF);
			if(c == Grass.character){
				writeFoodElement(t >> 8, t & 0xF);
				return;
			}
		}
	}

	char elementAt(u8 x, u8 y) noexcept {
		u16 addr = 160 * y + 2 * x;
		return screen.raw_read<8>(addr);
	}


//	State processSnake(Direction d) noexcept {
//		u16 idx = Head.get();
//		u16 xy = Blocks.get(idx);
//		u8 x = xy >> 8;
//		u8 y = xy & 0xF;

//		u8 ox, oy;
//		switch(d){
//		case Direction::Up:
//			ox = x;
//			oy = y-1;
//			break;
//		case Direction::Down:
//			ox = x;
//			oy = y+1;
//			break;
//		case Direction::Left:
//			ox = x-1;
//			oy = y;
//			break;
//		case Direction::Right:
//			ox = x+1;
//			oy = y;
//			break;
//		default:
//			return State::Alive;
//		}


//		if(ox > 80 || oy > 24){
//			return State::Dead;
//		}

//		u8 next = elementAt(ox, oy);

//		if(next == Snake.character){
//			return State::Dead;
//		}

//		writeSnakeElement(ox, oy);
//		idx = (idx+1) % Blocks.elements;
//		Head.set(idx);
//		Blocks.set((ox << 8) | oy, idx);

//		if(next == Food.character){

//		}else{
//			Tail.set(Tail.get()+1);
//			clearSnakeElement(ox, oy);
//		}

//		return State::Alive;
//	}

	void writeSnakeElement(u8 x, u8 y) noexcept {
		u16 addr = 160 * y + 2 * x;
		screen.writeChar(Snake.character, addr, Snake.foreground, Snake.background);
	}

	void clearSnakeElement(u8 x, u8 y) noexcept {
		u16 addr = 160 * y + 2 * x;
		screen.writeChar(Grass.character, addr, Grass.foreground, Grass.background);
	}

	void writeFoodElement(u8 x, u8 y) noexcept {
		u16 addr = 160 * y + 2 * x;
		screen.writeChar(Food.character, addr, Food.foreground, Food.background);
	}

	void drawBall() noexcept {

	}
};

static inline void foo()
{
//	asm("mov 9000, %bp");
//	asm("mov %bp, %sp");


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
	kq::SegmentedAddress<0x9000, kq::SegmentRegister::fs, kq::IndexRegister::si>{};
	foo();
}

#endif // YES_MAIN

