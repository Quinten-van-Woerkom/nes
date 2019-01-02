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
class memory {
    /**
     *  A memory segment is a span which is aware of its relative position in NES memory.
     *  As a result, it is useful in the implementation of stateful pointers, as
     *  it allows address-based access and knows its size.
     */
    class segment {
    public:
        constexpr segment(span<byte> view, word location) noexcept :
            _view{view}, _location{location} {}

        /**
         *  Returns whether or not an address falls within this segment's memory range.
         */
        constexpr bool contains(word address) const noexcept
        {
            const auto relative_address = address - _location;
            return relative_address >= 0 && relative_address < size();
        }


        /**
         *  Read and access functions.
         */
        constexpr auto read(word address) const -> byte
        {
            const auto index = address - _location;
            return _view[index];
        }

        constexpr void write(word address, byte value)
        {
            const auto index = address - _location;
            _view[index] = value;
        }

        constexpr auto access(word address) const -> const byte&
        {
            const auto index = address - _location;
            return _view[index];
        }

        constexpr auto access(word address) -> byte&
        {
            const auto index = address - _location;
            return _view[index];
        }


    private:
        span<byte> _view;
        word _location;
    };


    /**
     *  Pointers are stateful; they are aware of their location in memory and
     *  the limits of their current segment. As such, they can automatically
     *  map to a new memory segment when exceeding the current segment's bounds.
     *  This is useful in the efficient implementation of a memory map.
     */
    class pointer {
    public:
        constexpr pointer(word address, segment location, memory& storage) :
            _address{address}, _storage{storage} {}



    private:
        word _address;
        segment _location;
        memory& _storage;
    };


    /**
     *  Maps an address to the corresponding memory segment.
     */
    constexpr auto map(word address) -> segment
    {
        if (address < 0x0800) return segment{_ram, 0x0000};
        if (address < 0x1000) return segment{_ram, 0x0800};
        if (address < 0x1800) return segment{_ram, 0x1000};
        if (address < 0x2000) return segment{_ram, 0x1800};
    }

    std::array<byte, 0x4028> _storage;
    span<byte> _ram{_storage.data(), 0x800};
}
}