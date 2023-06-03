#ifndef UTIL_BIT_OPERATIONS_HPP
#define UTIL_BIT_OPERATIONS_HPP

#include <limits>

template <typename T, uint8_t start, uint8_t count>
constexpr T get_mask()
{
    return (static_cast<T>(~(std::numeric_limits<T>::max() << count)) << start);
}

// TODO: Check if this is optimal at all
template <typename T, uint8_t start, uint8_t count>
void set_bits(T& num, T value)
{
    constexpr T mask = get_mask<T, start, count>();

    num &= ~mask;

    num |= ((value << start) & mask);
}

template <typename T, uint8_t start, uint8_t count>
T get_bits(T num)
{
    constexpr T mask = get_mask<T, start, count>();

    return (num & mask) >> start;
}

#endif // UTIL_BIT_OPERATIONS_HPP
