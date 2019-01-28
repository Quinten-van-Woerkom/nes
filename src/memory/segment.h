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

#pragma once

#include <array>

#include "../byte.h"
#include "span.h"

namespace nes {
/**
 *  Address wrapping implementation of a memory span.
 *  Used as view into a memory segment.
 */
class segment_view {
public:
	explicit constexpr segment_view(span<byte> segment, word begin, word size) :
        _segment{segment}, _begin{begin}, _size{size} {}

    /**
     *  Accessors using global address
     */
    constexpr auto read(word address) const -> byte
    {
        const auto index = compute_index(address);
        return _segment[index];
    }

    constexpr void write(word address, byte data)
    {
        const auto index = compute_index(address);
        _segment[index] = data;
    }

    /**
     *  Accessors using local address
     */
    constexpr auto operator[](std::ptrdiff_t index) const -> byte
    {
        return _segment[index];
    }

    constexpr auto operator[](std::ptrdiff_t index) -> byte&
    {
        return _segment[index];
    }


    /**
     *  Returns a subspan of the segment view.
     */
    constexpr auto subspan(word begin, word size) -> segment_view
    {
        return segment_view{_segment.subspan(begin - _begin, size), begin, size};
    }


    /**
     *  Returns whether or not the memory segment's address space contains
     *  the address given.
     */
    constexpr bool contains(word address) const noexcept
    {
        return address >= _begin && address - _begin < _size;
    }

private:
    constexpr auto compute_index(word address) const noexcept -> std::ptrdiff_t
    {
        return (address - _begin) % _segment.size();
    }

    span<byte> _segment;
    word _begin, _size;
};



/**
 *  A memory segment is an array which is aware of its global position
 *  in memory. This allows accessing using the global indices.
 */
template<auto size, auto begin, auto end>
class segment {
public:
    /**
     *  Returns a view into a memory segment.
     */
    constexpr auto view() -> segment_view
    {
        return segment_view{span<byte>{_storage}, word{begin}, word{end}};
    }


    /**
     *  Accessors using global address
     */
    constexpr auto read(word address) const -> byte
    {
        const auto index = compute_index(address);
        return _storage[index];
    }

    constexpr void write(word address, byte data)
    {
        const auto index = compute_index(address);
        _storage[index] = data;
    }


    /**
     *  Returns whether or not the memory segment's address space contains
     *  the address given.
     */
    static constexpr bool contains(word address) noexcept
    {
        return address >= begin && address < end;
    }


private:
    /**
     *  Converts the global address into the index for array access.
     */
    static constexpr auto compute_index(word address) noexcept -> std::ptrdiff_t
    {
        return (address - begin) % size;
    }

    std::array<byte, size> _storage;
};
}