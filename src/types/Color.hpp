#ifndef TYPES_COLOR_HPP
#define TYPES_COLOR_HPP

enum class Color : uint8_t
{
    White = 0,
    Black = 1,
    Empty
};

inline Color get_other_color(Color color)
{
    return static_cast<Color>(!static_cast<uint8_t>(color));
}

#endif // TYPES_COLOR_HPP
