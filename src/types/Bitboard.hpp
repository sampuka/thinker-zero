#ifndef TYPES_BITBOARD_HPP
#define TYPES_BITBOARD_HPP

#include "types/Square.hpp"

#include <cstdint>

class Bitboard
{
public:
	constexpr Bitboard() = default;

	constexpr Bitboard(uint64_t data) : m_board(data)
	{
	}

	constexpr uint64_t get_data() const
	{
		return m_board;
	}

	constexpr bool read_by_square(Square square) const
	{
		return m_board & (uint64_t{1} << square.get_data());
	}

	constexpr uint8_t read_bitcount() const
	{
		// Implement with fancy inline ASM thingy
		return 0;
	}

	constexpr void set_by_square(Square square)
	{
		m_board |= (uint64_t{1} << square.get_data());
	}

	constexpr void clear_by_square(Square square)
	{
		m_board &= ~(uint64_t{1} << square.get_data());
	}

	uint8_t scan_forward() const
	{
		uint64_t res = 0;  // Must be 64 bit for the asm instruction

		asm("bsf %1,%0\n" : "=r"(res) : "b"(m_board));

		return res;
	}

	uint8_t scan_backward() const
	{
		uint64_t res = 0;

		asm("bsr %1,%0\n" : "=r"(res) : "b"(m_board));

		return res;
	}

	bool empty() const
	{
		return m_board == 0;
	}

	constexpr Bitboard operator|(const Bitboard& rhs)
	{
		return Bitboard(m_board | rhs.get_data());
	}

	constexpr Bitboard operator&(const Bitboard& rhs)
	{
		return Bitboard(m_board & rhs.get_data());
	}

	constexpr Bitboard operator~()
	{
		return Bitboard(~m_board);
	}

private:
	uint64_t m_board = 0;
};

#endif  // TYPES_BITBOARD_HPP
