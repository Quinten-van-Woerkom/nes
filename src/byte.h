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

namespace nes {

class byte {
public:
    template<typename Integer>
    explicit constexpr byte(Integer other) :
        _byte{static_cast<std::uint8_t>(other)}
    {}

    constexpr operator std::uint8_t() const { return _byte; }

    constexpr auto bit(std::size_t index) -> bool
    {
        return (_byte >> index) & 1;
    }

private:
    std::uint8_t _byte;
};

class word {
public:
    template<typename Integer>
    explicit constexpr word(Integer other) :
        _word{static_cast<std::uint16_t>(other)}
    {}

    constexpr operator std::uint16_t() const { return _word; }

    constexpr auto bit(std::size_t index) -> bool
    {
        return (_word >> index) & 1;
    }

private:
    std::uint16_t _word;
};


/**
 *  Returns a word constructed from the given high and low bytes.
 */
constexpr auto make_word(const byte& high, const byte& low) -> word
{
    return static_cast<word>(high << 8 | low);
}


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