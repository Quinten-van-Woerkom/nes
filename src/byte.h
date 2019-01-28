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

#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <limits>

namespace nes {
/**
 *  Wrapper for values of integral type to extend them with bitwise functionality.
 *  Mostly a convenience wrapper, but allows for easy adaptation into register types and
 *  is useful for print output formatting.
 */
template<typename Derived, typename T>
class bitwise_wrapper {
public:
    using value_type = T;
    static constexpr std::size_t bit_count = sizeof(value_type) * 8;

    explicit constexpr bitwise_wrapper() = default;

    template<typename Integer>
    explicit constexpr bitwise_wrapper(Integer other) :
        _value{static_cast<T>(other)}
    {}

    constexpr operator value_type() const { return _value; }
    constexpr operator value_type&() { return _value; }

    constexpr auto bit(std::size_t index) const -> bool
    {
        return (_value >> index) & 1;
    }

    constexpr auto high_bit() const -> bool
    {
        return bit(bit_count - 1);
    }

    constexpr auto low_bit() const -> bool
    {
        return bit(0);
    }

    constexpr auto set(std::size_t index, bool value = true) -> Derived&
    {
        _value = _value & ~(1 << index) | (value << index);
        return static_cast<Derived&>(*this);
    }

    constexpr auto clear(std::size_t index) -> Derived&
    {
        _value &= ~(1ul << index);
        return static_cast<Derived&>(*this);
    }

    constexpr auto toggle(std::size_t index) -> Derived&
    {
        _value ^= (1ul << index);
        return static_cast<Derived&>(*this);
    }

    constexpr auto as_signed() const
    {
        return bitwise_wrapper<std::make_signed_t<T>>{static_cast<std::make_signed_t<T>>(_value)};
    }

    constexpr auto as_unsigned() const
    {
        return bitwise_wrapper<std::make_unsigned_t<T>>{static_cast<std::make_unsigned_t<T>>(_value)};
    }

    constexpr auto increment(int step = 1) -> Derived&
    {
        _value += step;
        return static_cast<Derived&>(*this);
    }

    constexpr auto decrement(int step = 1) -> Derived&
    {
        _value -= step;
        return static_cast<Derived&>(*this);
    }

    constexpr auto shift_left(bool carry = false) -> bool
    {
        const auto new_carry = high_bit();
        _value <<= 1;
        set(0, carry);
        return new_carry;
    }

    constexpr auto shift_right(bool carry = false) -> bool
    {
        const auto new_carry = low_bit();
        _value >>= 1;
        set(bit_count - 1, carry);
        return new_carry;
    }

    constexpr auto rotate_left(bool& carry) -> Derived&
    {
        carry = shift_left(carry);
        return static_cast<Derived&>(*this);
    }

    constexpr auto rotate_right(bool& carry) -> Derived&
    {
        carry = shift_right(carry);
        return static_cast<Derived&>(*this);
    }

    /**
     *  A byte's sign is determined from its two's complement interpretation:
     *  if bit 7 is set, the byte is negative, otherwise it is positive.
     */
    constexpr auto sign() const -> bool
    {
        return bit(7);
    }

    /**
     *  Bitwise integers are printed in hexadecimal with a width equal to the
     *  number of digits required to represent the maximum possible value, padded
     *  to the left with zeroes.
     */
    friend auto operator<<(std::ostream& os, bitwise_wrapper value) -> std::ostream&
    {
        if constexpr (std::is_unsigned_v<T>)
        {
            auto old_state = std::ios{nullptr};
            old_state.copyfmt(os);

            os << std::hex << std::setw(2*sizeof(value_type)) << std::right << std::setfill('0');
            os << static_cast<int>(value._value);

            os.copyfmt(old_state);
            return os;
        }
        else {
            return os << static_cast<int>(value._value);
        }
    }

protected:
    value_type _value = 0;
};


class byte : public bitwise_wrapper<byte, std::uint8_t> {
public:
    using bitwise_wrapper::bitwise_wrapper;
};

class word : public bitwise_wrapper<word, std::uint16_t> {
public:
    using bitwise_wrapper::bitwise_wrapper;

    explicit constexpr word(byte high, byte low) :
        bitwise_wrapper(high << 8 | low)
    {}

    constexpr auto high() const -> byte
    {
        return byte{_value >> 8};
    }

    constexpr auto low() const -> byte
    {
        return byte{_value & 0xff};
    }
};


/**
 *  Determines if addition would result in signed overflow.
 */
constexpr auto overflows(byte left, byte right) -> bool
{
    const bool carry_in = left.bit(6) && right.bit(6);
    const bool carry_out = (left.bit(7) && right.bit(7)) || (left.bit(7) && carry_in) || (right.bit(7) && carry_in);
    return carry_in ^ carry_out;
}


/**
 *  Small tests checking byte and word behaviour.
 */
static_assert(byte{0x12} +0x34 == 0x46);
static_assert(byte{0x1a} +word{0x2b00} == 0x2b1a);
static_assert(byte{0xff} << 8 == word{0xff00});
static_assert(word{0xabcd}.high() == byte{0xab});
static_assert(word{0xabcd}.low() == byte{0xcd});

static_assert(overflows(byte{0x50}, byte{0x50}));
static_assert(overflows(byte{0xd0}, byte{0x90}));
static_assert(!overflows(byte{0x50}, byte{0x10}));
static_assert(!overflows(byte{0xd0}, byte{0xd0}));
}