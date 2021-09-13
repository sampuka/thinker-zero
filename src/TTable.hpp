#ifndef TTABLE_H
#define TTABLE_H

#include "zobrist.hpp"
#include "Move.hpp"

#include <cmath>
#include <vector>

class TValue
{
public:
    std::uint64_t zobrist_key;
    Move best_move;
    unsigned int depth;
    double score;
};

class TTable
{
public:
    TTable(unsigned int bits = 20)
        : bit_size(bits), mask(std::pow(2, bits)-1)
    {
        table.resize(std::pow(2, bit_size));
    }

    TValue read(std::uint64_t zob) const
    {
        return table[zob & mask];
    }

    void insert(std::uint64_t zob)
    {
        TValue tv;
        tv.zobrist_key = zob;

        table[zob & mask] = tv;
    }

private:
    const unsigned int bit_size;
    const std::uint64_t mask;
    std::vector<TValue> table;
};

#endif
