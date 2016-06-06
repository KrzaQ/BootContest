#ifndef VIDEO_BUFFER_HPP
#define VIDEO_BUFFER_HPP

#include "segmented_address.hpp"

namespace kq
{

struct VideoBuffer : SegmentedAddress<0xB800, SegmentRegister::gs>
{
	enum class Colour : u8 {
		Black,
		Blue,
		Green,
		Cyan,
		Red,
		Magenta,
		Brown,
		LightGray,
		DarkGray,
		LightBlue,
		LightGreen,
		LightCyan,
		LightRed,
		LightMagenta,
		Yellow,
		White,
	};

	using SegmentedAddress<0xB800, SegmentRegister::gs>::SegmentedAddress;

	inline static void writeLine(char const* text, u8 line, Colour fg = Colour::LightGray, Colour bg = Colour::Black) noexcept {
		u16 offset = 160 * line;
		for(u8 i{}; i < 80 && text[i]; ++i){
			writeChar(text[i], offset + 2*i, fg, bg);
		}
	}

	inline static void writeChar(char c, u16 addr, Colour fg = Colour::LightGray, Colour bg = Colour::Black) noexcept {
		raw_write<16>(c | (static_cast<u8>(fg) << 8) | (static_cast<u8>(bg) << 12), addr);
	}

	inline static u8 makeCharColour(Colour fg, Colour bg = Colour::Black) noexcept {
		return static_cast<u8>(fg) | (static_cast<u8>(bg) << 4);
	}
};

}

#endif // VIDEO_BUFFER_HPP
