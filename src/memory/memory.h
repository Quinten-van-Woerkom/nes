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
 *  A memory segment is an array which is aware of its global position
 *  in memory. This allows accessing using the global indices.
 */
template<auto size, auto begin, auto end>
class segment {
public:
    /**
     *  Accessors using global address
     */
    constexpr auto operator()(word address) const -> byte
    {
        const auto index = compute_index(address);
        return _storage[index];
    }

    constexpr auto operator()(word address) -> byte&
    {
        const auto index = compute_index(address);
        return _storage[index];
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
    static constexpr auto compute_index(word address) noexcept ->std::ptrdiff_t
    {
        return (address - begin) % size;
    }

    std::array<byte, size> _storage;
};



class cpu;
class ppu;
class registers;
class cartridge;

/**
 *  NES memory is very much interlinked, though it is owned by multiple
 *  devices. This class implements the memory bus linking all memory resources.
 */
class memory {
public:
    constexpr memory(cpu& cpu, ppu& ppu, cartridge& cartridge) :
        _cpu{cpu}, _ppu{ppu}, _cartridge{cartridge} {}


    /**
     *  Memory contains several registers.
     *  Access might result in additional actions.
     */
    class registers {
    public:

    private:

    };


private:
    cpu& _cpu;
    ppu& _ppu;
    cartridge& _cartridge;
    registers _registers;
};
}