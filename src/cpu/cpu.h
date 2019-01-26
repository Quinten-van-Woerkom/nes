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

#include "../byte.h"
#include "../memory/memory.h"
#include "../memory/span.h"

namespace nes {
/**
 *  Implementation of the processor status register.
 *  Bit 4 (break flag) is not physically present in the status register, but
 *  is set or cleared when the status register's byte value is pushed on
 *  the stack.
 *  Bit 5 is unused and not physically present, but is always considered set.
 */
struct status {
    bool carry;
    bool zero;
    bool interrupt_disable;
    bool decimal;
    bool overflow;
    bool negative;

    constexpr status(byte other = byte{0x24}) :
        carry{other.bit(0)},
        zero{other.bit(1)},
        interrupt_disable{other.bit(2)},
        decimal{other.bit(3)},
        overflow{other.bit(6)},
        negative{other.bit(7)}
    {}

    constexpr status(int other) : status(byte{other}) {}

    constexpr auto operator=(byte other) -> status&
    {
        carry = other.bit(0);
        zero = other.bit(1);
        interrupt_disable = other.bit(2);
        decimal = other.bit(3);
        overflow = other.bit(6);
        negative = other.bit(7);
        return *this;
    }

    constexpr auto value() const
    {
        return byte{
            carry << 0 | zero << 1 | interrupt_disable << 2 |
            decimal << 3 | overflow << 6 | negative << 7
        };
    }

    constexpr auto instruction_value() const -> byte
    {
        return value().set(5).set(4);
    }

    constexpr auto interrupt_value() const -> byte
    {
        return value().set(5);
    }
};



/**
 *  Implementation of the stack with the corresponding stack pointer register.
 *  The 6502 stack is of the empty, descending kind and the pointer wraps around
 *  when overflow occurs.
 */
class stack {
public:
    constexpr stack(segment_view ram, byte pointer = byte{0xff}) noexcept :
        pointer{pointer},
        _storage{ram.subspan(word{0x100}, word{0x200})} {}

    constexpr void push(byte value)
    {
        _storage[pointer] = value;
        pointer.decrement();
    }

    constexpr auto pull() -> byte
    {
        pointer.increment();
        return _storage[pointer];
    }

    constexpr auto peek() -> byte
    {
        return _storage[pointer + 1];
    }

    byte pointer;

private:
    segment_view _storage;
};



/**
 *  Implementation of the processor registers with instructions and addressing modes.
 */
class processor {
public:
    constexpr processor(segment_view ram) :
        _stack{ram},
        _status{0x24},
        _accumulator{0x00},
        _x{0x00},
        _y{0x00},
        _program_counter{0xfffd}
    {}

private:
    stack _stack;
    status _status;
    byte _accumulator;
    byte _x, _y;
    word _program_counter;
};



/**
 *  
 */
class cpu {
public:
    constexpr cpu(memory& memory) :
        _processor{memory.ram()},
        _memory{memory}
    {}

private:
    processor _processor;
    memory& _memory;
};
}