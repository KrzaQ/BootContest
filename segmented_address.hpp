#ifndef SEGMENTED_ADDRESS_HPP
#define SEGMENTED_ADDRESS_HPP

#include "common.hpp"

enum class SegmentRegister : u8 {
	ds,fs,gs,es
};

template<u16 Segment, SegmentRegister = SegmentRegister::ds>
struct SegmentedAddress;

// :(
#define MAKE_SEGMENTED_ADDRESS(segment_register) \
template<u16 Segment> \
struct SegmentedAddress<Segment, SegmentRegister::segment_register> \
{ \
	SegmentedAddress(){ \
		asm("mov %0, %%ax;" \
			"mov %%ax, %%" #segment_register \
			: \
			: "r" (Segment) \
		); \
	} \
\
	template<u32 Size, typename T> \
	inline static void raw_write(T value, u16 addr = 0){ \
		raw_write_impl(kq::integral_constant<Size>{}, addr, value); \
	} \
\
private: \
	inline static void raw_write_impl(kq::integral_constant<8>, u16 addr, u8 val){ \
		asm("movw %1, %%di;\n" \
			"movb %0, %%" #segment_register ":0(%%di)" \
			: \
			: "r" (val), "r" (addr) \
			: "di" \
		); \
	} \
 \
	inline static void raw_write_impl(kq::integral_constant<16>, u16 addr, u16 val){ \
		asm("movw %1, %%di;\n" \
			"movw %0, %%" #segment_register ":0(%%di)" \
			: \
			: "r" (val), "r" (addr) \
			: "di" \
		); \
	} \
\
	inline static void raw_write_impl(kq::integral_constant<32>, u16 addr, u32 val){ \
		asm("movw %1, %%di;\n" \
			"movl %0, %%" #segment_register " :0(%%di)" \
			: \
			: "r" (val), "r" (addr) \
			: "di" \
		); \
	} \
};

MAKE_SEGMENTED_ADDRESS(ds);
MAKE_SEGMENTED_ADDRESS(fs);
MAKE_SEGMENTED_ADDRESS(gs);
MAKE_SEGMENTED_ADDRESS(es);

#endif // SEGMENTED_ADDRESS_HPP
