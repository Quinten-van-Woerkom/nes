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
#include "segment.h"

namespace nes {
class cpu;
class ppu;
class cartridge;

/**
 *  NES memory is very much interlinked, though it is owned by multiple
 *  devices. This class implements the memory bus linking all memory resources.
 */
class memory {
public:
    using random_access_memory = segment<0x800, 0x0000, 0x4000>;
    using registers = segment<0x20, 0x4000, 0x4020>; // TODO: Implement memory mapped registers

    constexpr auto read(word address) const -> byte;
    constexpr void write(word address, byte data);

    constexpr auto ram() -> segment_view;

private:
    random_access_memory _ram;
    ppu& _ppu;
    cartridge& _cartridge;
    registers _registers;
};



/**
 *  Due to memory mapping and bank switching, normal references cannot be used.
 *  Instead, a simple wrapper class is used to allow for references.
 */
class reference {
public:
    constexpr reference(memory& host, word address) :
        _host{host}, _address{address}
    {}

    constexpr operator byte() const
    {
        return _host.read(_address);
    }

    constexpr operator word() const
    {
        return word{
            _host.read(_address),
            _host.read(word{_address + 1})
        };
    }

    constexpr auto operator=(byte data) -> reference&
    {
        _host.write(_address, data);
        return *this;
    }

    constexpr auto operator=(word data) -> reference&
    {
        _host.write(_address, data.low());
        _host.write(word{_address + 1}, data.high());
        return *this;
    }

private:
    memory& _host;
    word _address;
};


/**
 *  Pointer implementation.
 */
class pointer {
public:
    constexpr pointer(memory& host, word address) :
        _host{host}, _address{address}
    {}

    constexpr operator word() const
    {
        return _address;
    }

    constexpr auto operator*() -> reference
    {
        return reference{_host, _address};
    }

    constexpr auto operator++() -> pointer&
    {
        ++_address;
        return *this;
    }

    constexpr auto operator++(int) -> pointer
    {
        const auto temp = *this;
        ++(*this);
        return temp;
    }

private:
    memory& _host;
    word _address;
};
}