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
 */
class status {
public:
    template<typename Integer>
    explicit constexpr status(Integer flags) :
        _flags{from_byte(static_cast<byte>(flags))}
    {}

    constexpr auto operator=(byte flags) -> status&
    {
        _flags = from_byte(flags);
        return *this;
    }

    constexpr auto as_byte() const -> byte
    {
        return static_cast<byte>(parse_bitset(_flags));
    }

    /**
     *  Processor flag getters
     */
    constexpr auto carry()              const noexcept -> bool { return _flags[0]; }
    constexpr auto zero()               const noexcept -> bool { return _flags[1]; }
    constexpr auto interrupt_disable()  const noexcept -> bool { return _flags[2]; }
    constexpr auto decimal()            const noexcept -> bool { return _flags[3]; }
    constexpr auto break_flag()         const noexcept -> bool { return _flags[4]; }
    constexpr auto unused()             const noexcept -> bool { return _flags[5]; }
    constexpr auto overflow()           const noexcept -> bool { return _flags[6]; }
    constexpr auto negative()           const noexcept -> bool { return _flags[7]; }


    /**
     *  Boolean processor flag setters
     */
    constexpr auto carry(bool c)                noexcept -> bool { return _flags[0] = c; }
    constexpr auto zero(bool z)                 noexcept -> bool { return _flags[1] = z; }
    constexpr auto interrupt_disable(bool i)    noexcept -> bool { return _flags[2] = i; }
    constexpr auto decimal(bool d)              noexcept -> bool { return _flags[3] = d; }
    constexpr auto break_flag(bool b)           noexcept -> bool { return _flags[4] = b; }
    constexpr auto unused(bool u)               noexcept -> bool { return _flags[5] = u; }
    constexpr auto overflow(bool v)             noexcept -> bool { return _flags[6] = v; }
    constexpr auto negative(bool n)             noexcept -> bool { return _flags[7] = n; }


    /**
     *  Some flags can also be set according to a predicate.
     */
    constexpr auto carry(int result)            noexcept -> bool { return _flags[0] = (result < -128) || (result > 127); }
    constexpr auto zero(int result)             noexcept -> bool { return _flags[1] = result; }
    //TODO: Implement predicate for overflow
    constexpr auto negative(int result)         noexcept -> bool { return _flags[7] = (result < 0); }
    
private:
    constexpr auto from_byte(byte flags) const -> std::array<bool, 8> {
        auto result = std::array<bool, 8>{};
        for (int i = 0, I = 8; i < I; ++i) result[i] = flags.bit(i);
        return result;
    }

    std::array<bool, 8> _flags;
};
}