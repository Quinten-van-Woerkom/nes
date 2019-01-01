/**
 *  project: NES Emulator
 *  author: Quinten van Woerkom
 *
 *  Copyright 2018 Quinten van Woerkom
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 *  Fundamental type definitions for the byte types used throughout the project.
 */

#pragma once

#include <cstdint>
#include <iostream>
#include <iomanip>

namespace nes {
/**
 *  Wrapper for values of integral type to extend them with bitwise functionality.
 *  Mostly a convenience wrapper, but allows for easy adaptation into register types and
 *  is useful for print output formatting.
 */
template<typename T>
class bitwise_wrapper {
public:
    using value_type = T;

    template<typename Integer>
    explicit constexpr bitwise_wrapper(Integer other) :
        _value{static_cast<T>(other)}
    {}

    constexpr operator value_type() const { return _value; }

    constexpr auto bit(std::size_t index) const -> bool
    {
        return (_value >> index) & 1;
    }

    constexpr void set(std::size_t index, bool value = true)
    {
        _value ^= (-static_cast<unsigned long>(value) ^ _value) & (1ul << index);
    }

    constexpr void clear(std::size_t index)
    {
        _value &= ~(1ul << index);
    }

    constexpr auto toggle(std::size_t index) -> bool
    {
        return _value ^= (1ul << index);
    }

    friend auto operator<<(std::ostream& os, bitwise_wrapper value) -> std::ostream&
    {
        auto old_state = std::ios{nullptr};
        old_state.copyfmt(os);

        os << std::hex << std::setw(2*sizeof(value_type)) << std::right << std::setfill('0');
        os << static_cast<int>(value._value);

        os.copyfmt(old_state);
        return os;
    }

protected:
    value_type _value;
};


class byte : public bitwise_wrapper<std::uint8_t> {
public:
    using bitwise_wrapper::bitwise_wrapper;
};

class word : public bitwise_wrapper<std::uint16_t> {
public:
    using bitwise_wrapper::bitwise_wrapper;

    explicit constexpr word(byte high, byte low) :
        bitwise_wrapper(high << 8 | low)
    {}

    constexpr auto high() const -> byte
    {
        return static_cast<byte>(_value >> 8);
    }

    constexpr auto low() const -> byte
    {
        return static_cast<byte>(_value);
    }
};


/**
 *  Returns the most significant byte of a word.
 */
constexpr auto high_byte(const word& word) -> byte
{
    return static_cast<byte>(word >> 8);
}


/**
 *  Returns the least significant byte of a word.
 */
constexpr auto low_byte(const word& word) -> byte
{
    return static_cast<byte>(word);
}


/**
 *  Small tests checking byte and word behaviour.
 */
static_assert(byte{0x12} +0x34 == 0x46);
static_assert(byte{0x1a} +word{0x2b00} == 0x2b1a);
static_assert(byte{0xff} << 8 == word{0xff00});


/**
 *  Converts a range of booleans into the unsigned integer it represents.
 */
template<typename Container>
constexpr auto parse_bitset(const Container& bits) -> unsigned long long
{
    unsigned long long result = 0;
    for (int i = 0, I = 8; i < I; ++i) {
        result |= (bits[i] << i);
    }
    return result;
}
}