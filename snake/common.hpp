#ifndef COMMON_HPP
#define COMMON_HPP

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;



namespace kq
{

template<u32 Value>
struct integral_constant
{
	static constexpr u32 value = Value;
};

namespace detail{
template<u32 Size> struct sized_type;

template<> struct sized_type<8>{ using type = u8; };
template<> struct sized_type<16>{ using type = u16; };
template<> struct sized_type<32>{ using type = u32; };

}

template<u32 Size>
using sized_type = typename detail::sized_type<Size>::type;

}


#endif // COMMON_HPP
