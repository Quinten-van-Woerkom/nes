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

namespace nes {
/**
 *  Implementation of the processor status register.
 *  Stores 
 */
class status : public byte {
public:
    using byte::byte;

    template<typename Integer>
    explicit constexpr status(Integer other) :
        byte{other.set(5).clear(4)}
    {}

    constexpr auto with_break() const -> byte
    {
        return byte{*this}.set(4);
    }

    /**
     *  Processor flag getters
     */
    constexpr auto carry()              const -> bool { return bit(0); }
    constexpr auto zero()               const -> bool { return bit(1); }
    constexpr auto interrupt_disable()  const -> bool { return bit(2); }
    constexpr auto decimal()            const -> bool { return bit(3); }
    constexpr auto overflow()           const -> bool { return bit(6); }
    constexpr auto negative()           const -> bool { return bit(7); }


    /**
     *  Boolean processor flag setters
     */
    constexpr auto carry(bool c)                -> bool { set(0, c); return bit(0); }
    constexpr auto zero(bool z)                 -> bool { set(1, z); return bit(1); }
    constexpr auto interrupt_disable(bool i)    -> bool { set(2, i); return bit(2); }
    constexpr auto decimal(bool d)              -> bool { set(3, d); return bit(3); }
    constexpr auto overflow(bool v)             -> bool { set(6, v); return bit(6); }
    constexpr auto negative(bool n)             -> bool { set(7, n); return bit(7); }


    /**
     *  Some flags can also be set according to a predicate.
     */
    constexpr auto carry(int result)            -> bool { return carry(result < 0 || result > 0xff); } // Note: SBC should set the carry when 0 <= result <= 0xff
    constexpr auto zero(int result)             -> bool { return zero(result == 0); }
    constexpr auto negative(int result)         -> bool { return negative(byte{result}.bit(7)); }
    constexpr auto overflow(int result)         -> bool { return overflow(result < 0x8f || result > 0x17f); } // Assumes unsigned byte addition
};
}