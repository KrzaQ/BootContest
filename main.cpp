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
		return ret;
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
		"movb $134, %%ah; \n\t"
		"movw %0, %%cx; \n\t"
		"movw %1, %%dx; \n\t"
		"int $21;"
		:
		: "Ri" (us >> 16), "Ri" (us & 0xFFFF)
		: "ax", "cx", "dx"
	);
}

static inline u16 ticks() noexcept {
	u16 ret;
	asm volatile(
		"xor %%ax, %%ax; \n\t"
		"int $26; \n\t"
		"movw %%dx, %0; \n\t"
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

template<typename T, typename U>
T& nasty_cast(U&& u){
	return *static_cast<T*>(static_cast<void*>(&u));
}

template<u16 Addr, u8 Size, typename Elem = kq::sized_type<Size>, u16 Elements = 1>
struct MemoryEntity{
	constexpr static u16 addr = Addr;
	constexpr static u8 size = Size;
	constexpr static u16 elements = Elements;
	using type = Elem;
	using storage_type = kq::sized_type<Size>;

	template<typename T>
	static inline void set(T&& val, u16 n) noexcept {
		data.raw_write<Size>(nasty_cast<storage_type>(val), Addr + n * Size);
	}

	static inline type get(u16 n) noexcept {
		return nasty_cast<type>(data.raw_read<Size>(Addr + n * Size));
	}
};

template<u16 Addr, u8 Size, typename Elem>
struct MemoryEntity<Addr, Size, Elem, 1>{
	constexpr static u16 addr = Addr;
	constexpr static u8 size = Size;
	using type = Elem;
	using storage_type = kq::sized_type<Size>;

	template<typename T>
	static inline void set(T&& val) noexcept {
		data.raw_write<Size>(nasty_cast<storage_type>(val), addr);
	}

	static inline type get() noexcept {
		return nasty_cast<type>(data.raw_read<Size>(Addr));
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

//static inline u8 toHex(u8 x) noexcept {
//	return x < 10 ? x + '0' : x - 10 + 'A';
//}

struct Point2D
{
	u8 x;
	u8 y;
};

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
	constexpr static auto Blocks = MemoryEntity<0x200, 16, Point2D, 256>{};



	Game() noexcept {
		screen.set_addr(0);

		auto it = screen.iter<16>();
		for(u16 i = 1920; i --> 0;){
//			*it = Grass.character;
//			*it = screen.makeCharColour(Grass.foreground, Grass.background);
			*it = Grass.character | (screen.makeCharColour(Grass.foreground, Grass.background) << 8);
			++it;
			++it;
//			screen.writeChar(Grass.character, i - 2, Grass.foreground, Grass.background);
		}

		Size.set(1);
		Head.set(0);
		Tail.set(0);
		Blocks.set(Point2D{40,12}, 0);
		writeSnakeElement({40, 12});
		spawnFood();
	}

	static inline void loop() noexcept {
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
				sleep(100);

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
				auto p = Blocks.get(idx);

				Point2D other = p;
//				u8 ox, oy;
				switch(d){
				case Direction::Up:
//					other.x = p.x;
					other.y = p.y-1;
					break;
				case Direction::Down:
//					other.x = p.x;
					other.y = p.y+1;
					break;
				case Direction::Left:
					other.x = p.x-1;
//					other.y = p.y;
					break;
				case Direction::Right:
					other.x = p.x+1;
//					other.y = p.y;
					break;
				default:
					continue;
				}

				if(other.x > 79 || other.y > 24){
					active = false;
					break;
				}

				u8 next = elementAt(other);

				if(next == Snake.character){
					active = false;
					break;
				}

				writeSnakeElement(other);
				idx = (idx+1) % Blocks.elements;
				Head.set(idx);
				Blocks.set(other, idx);

				if(next != Food.character){
//					Tail.set(Tail.get()+1);
					idx = Tail.get();
					p = Blocks.get(idx);
					Tail.set((idx + 1) % Blocks.elements);
					clearSnakeElement(p);
				}else{
					spawnFood();
				}
			}

			while(true);
//		}
	}

	static inline void spawnFood() noexcept {
		for(u16 t = ticks();; t++){
			char c = elementAt(Point2D{static_cast<u8>(t >> 8), static_cast<u8>(t & 0xF)});
			if(c == Grass.character){
				writeFoodElement(Point2D{static_cast<u8>(t >> 8), static_cast<u8>(t & 0xF)});
				return;
			}
		}
	}

	static inline char elementAt(Point2D p) noexcept {
		u16 addr = 160 * p.y + 2 * p.x;
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

	static inline void writeSnakeElement(Point2D p) noexcept {
		u16 addr = 160 * p.y + 2 * p.x;
		screen.writeChar(Snake.character, addr, Snake.foreground, Snake.background);
	}

	static inline void clearSnakeElement(Point2D p) noexcept {
		u16 addr = 160 * p.y + 2 * p.x;
		screen.writeChar(Grass.character, addr, Grass.foreground, Grass.background);
	}

	static inline void writeFoodElement(Point2D p) noexcept {
		u16 addr = 160 * p.y + 2 * p.x;
		screen.writeChar(Food.character, addr, Food.foreground, Food.background);
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

