#ifndef SEGMENTED_ADDRESS_HPP
#define SEGMENTED_ADDRESS_HPP

#include "common.hpp"

namespace kq{

enum class SegmentRegister : u8 {
	ds,fs,gs,es
};

enum class IndexRegister : u8 {
	si,di
};

struct NoSegmentChange{};

template<u16 Segment, SegmentRegister = SegmentRegister::ds, IndexRegister = IndexRegister::di>
struct SegmentedAddress;

// :(
#define MAKE_SEGMENTED_ADDRESS(segment_register, index_register) \
template<u16 Segment> \
struct SegmentedAddress<Segment, SegmentRegister::segment_register, IndexRegister::index_register> \
{ \
	struct DataProxy; \
	struct AlmostIterator{ \
\
		DataProxy operator*() noexcept { \
			return {}; \
		} \
\
		u8 operator*() const noexcept { \
			return static_cast<u8>(DataProxy{}); \
		} \
\
		AlmostIterator operator++() const noexcept { \
			inc_addr(); \
			return {}; \
		} \
\
		AlmostIterator operator--() const noexcept { \
			dec_addr(); \
			return {}; \
		} \
\
		bool operator==(AlmostIterator) = delete; \
	}; \
\
	struct DataProxy : AlmostIterator{ \
\
		void operator=(u8 value) noexcept { \
			raw_write_impl(kq::integral_constant<8>{}, value); \
		} \
\
		operator u8() const noexcept { \
			return raw_read_impl(kq::integral_constant<8>{}); \
		} \
	}; \
\
	SegmentedAddress() noexcept { \
		asm volatile("mov %0, %%ax;" \
			"mov %%ax, %%" #segment_register \
			: \
			: "Ri" (Segment) \
		); \
	} \
\
	SegmentedAddress(NoSegmentChange){} \
\
	inline static AlmostIterator iter() noexcept { \
		return {}; \
	} \
\
	template<u32 Size, typename T> \
	inline static void raw_write(T value, u16 addr = 0) noexcept { \
		set_addr(addr); \
		raw_write_impl(kq::integral_constant<Size>{}, value); \
	} \
\
	template<u32 Size> \
	inline static auto raw_read(u16 addr = 0) noexcept { \
		set_addr(addr); \
		return raw_read_impl(kq::integral_constant<Size>{}); \
	} \
\
\
	template<typename It> \
	inline static void raw_write_n(It o, u8 n, u16 addr) noexcept { \
		set_addr(addr); \
		auto it = iter(); \
		for(;n --> 0; ++o, ++it){ \
			*it = *o; \
		} \
	} \
\
	inline static void set_addr(u16 addr) noexcept { \
		asm volatile("movw %0, %%" #index_register : : "Ri" (addr) : #index_register); \
	}\
	\
	inline static void inc_addr() noexcept { \
		asm volatile("inc %%" #index_register : : : #index_register); \
	} \
\
	inline static void dec_addr() noexcept { \
		asm volatile("dec %%" #index_register : : : #index_register); \
	} \
\
\
private: \
	inline static void raw_write_impl(kq::integral_constant<8>, u8 val) noexcept { \
		asm volatile("movb %0, %%" #segment_register ":0(%%" #index_register ")" \
			: \
			: "Ri" (val) \
			: #index_register \
		); \
	} \
 \
	inline static void raw_write_impl(kq::integral_constant<16>, u16 val) noexcept { \
		asm volatile("movw %0, %%" #segment_register ":0(%%" #index_register ")" \
			: \
			: "Ri" (val) \
			: #index_register \
		); \
	} \
\
	inline static void raw_write_impl(kq::integral_constant<32>, u32 val) noexcept { \
		asm volatile("movl %0, %%" #segment_register " :0(%%" #index_register ")" \
			: \
			: "Ri" (val) \
			: #index_register \
		); \
	} \
\
	inline static u8 raw_read_impl(kq::integral_constant<8>) noexcept { \
		u8 ret; \
		asm volatile("movb %%" #segment_register " :0(%%" #index_register "), %0" : "=q" (ret)); \
		return ret; \
	} \
\
	inline static u16 raw_read_impl(kq::integral_constant<16>) noexcept { \
		u16 ret; \
		asm  volatile("movw %%" #segment_register " :0(%%" #index_register "), %0" : "=q" (ret)); \
		return ret; \
	} \
\
	inline static u32 raw_read_impl(kq::integral_constant<32>) noexcept { \
		u32 ret; \
		asm volatile("movl %%" #segment_register " :0(%%" #index_register "), %0" : "=q" (ret)); \
		return ret; \
	} \
};

MAKE_SEGMENTED_ADDRESS(ds,di);
MAKE_SEGMENTED_ADDRESS(fs,di);
MAKE_SEGMENTED_ADDRESS(gs,di);
MAKE_SEGMENTED_ADDRESS(es,di);

MAKE_SEGMENTED_ADDRESS(ds,si);
MAKE_SEGMENTED_ADDRESS(fs,si);
MAKE_SEGMENTED_ADDRESS(gs,si);
MAKE_SEGMENTED_ADDRESS(es,si);

}

#endif // SEGMENTED_ADDRESS_HPP
