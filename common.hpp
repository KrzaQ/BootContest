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

}


#endif // COMMON_HPP
