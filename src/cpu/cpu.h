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


    /**
     *  The numeric value of the processor status pushed to the stack depends
     *  on the operation being executed:
     *      - Instructions (PHP, BRK): bit 4 set
     *      - Interrupts (IRQ, NMI): bit 4 is clear
     *  Bit 5, although not actually physically present, is always pushed as set.
     */
    constexpr auto instruction_value() const -> byte
    {
        return value().set(5).set(4);
    }

    constexpr auto interrupt_value() const -> byte
    {
        return value().set(5);
    }


    /**
     *  Most logical operations affect the zero and negative flags.
     *  Almost always, the zero flag is set if the result of an operation is
     *  zero, and the negative flag in case its bit 7 is set.
     */
    void logical(const unsigned int result)
    {
        zero = byte{result} == 0;
        negative = byte{result}.sign();
    }

    /**
     *  In addition, most arithmetic operations update the carry flag as well
     *  as the logical flags.
     */
    void arithmetic(const unsigned int result)
    {
        logical(result);
        carry = result > 0xff;
    }

    /**
     *  If addition or subtraction is performed, signed overflow can occur.
     *  When this happens, the overflow flag must be set, indicating that the
     *  sign of the result is incorrect with respect to the operand signs.
     */
    void overflows(const byte left, const byte right, const unsigned int result)
    {
        overflow = (left.sign() == right.sign()) && (left.sign() != byte{result}.sign());
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

    constexpr void push(word value)
    {
        push(value.high());
        push(value.low());
    }

    constexpr auto pull() -> byte
    {
        pointer.increment();
        return _storage[pointer];
    }

    constexpr auto pull_word()
    {
        const auto low = pull();
        const auto high = pull();
        return word{high, low};
    }

    constexpr auto peek() -> byte
    {
        return _storage[pointer + 1];
    }

    byte pointer;

private:
    segment_view _storage;
};


class cpu;
class ppu;
class registers;
class cartridge;

/**
 *  Implementation of the processor registers with instructions and addressing modes.
 */
class processor {
public:
    using memory = memory<cpu, ppu, registers, cartridge>;

    constexpr processor(segment_view ram) :
        _stack{ram},
        _status{0x24},
        _accumulator{0x00},
        _x{0x00},
        _y{0x00},
        _program_counter{0xfffd}
    {}

    /**
     *  56 supported instructions.
     *  Four operand types are possible:
     *      - implied: no operand is needed
     *      - byte: byte is passed by value
     *      - reference: reference to byte in memory
     *      - pointer: address in memory
     *  Reference and pointer can only refer to memory, byte can also be a register.
     *  Implementations of these functions are found in instruction.cpp.
     */

    /* Storage */
    void lda(byte);
    void ldx(byte);
    void ldy(byte);
    void sta(memory::reference);
    void stx(memory::reference);
    void sty(memory::reference);
    void tax();
    void tay();
    void tsx();
    void txa();
    void txs();
    void tya();

    /* Math */
    void adc(byte);
    void dec(memory::reference);
    void dex();
    void dey();
    void inc(memory::reference);
    void inx();
    void iny();
    void sbc(byte);

    /* Bitwise */
    void and_(byte);
    void asl();
    void asl(memory::reference);
    void bit(byte);
    void eor(byte);
    void lsr();
    void lsr(memory::reference);
    void ora(byte);
    void rol();
    void rol(memory::reference);
    void ror();
    void ror(memory::reference);

    /* Branch */
    void bcc(memory::pointer);
    void bcs(memory::pointer);
    void beq(memory::pointer);
    void bmi(memory::pointer);
    void bne(memory::pointer);
    void bpl(memory::pointer);
    void bvc(memory::pointer);
    void bvs(memory::pointer);
        
    /* Jump */
    void jmp(memory::pointer);
    void jsr(memory::pointer);
    void rti();
    void rts();

    /* Registers */
    void clc();
    void cld();
    void cli();
    void clv();
    void cmp(byte);
    void cpx(byte);
    void cpy(byte);
    void sec();
    void sed();
    void sei();

    /* Stack */
    void pha();
    void php();
    void pla();
    void plp();

    /* System */
    void nop() {};
    void brk(memory::pointer vector);

private:
    /**
     *  Helper functions implementing often-repeated parts of instructions.
     */
    void transfer(byte& from, byte& to);
    auto decrement(byte operand) -> byte;
    auto increment(byte operand) -> byte;
    void branch(memory::pointer location);
    auto shift_left(byte operand) -> byte;
    auto shift_right(byte operand) -> byte;
    auto rotate_left(byte operand) -> byte;
    auto rotate_right(byte operand) -> byte;
    void compare(byte left, byte right);

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
    using ram = segment<0x800, 0x000, 0x2000>;
    using memory = memory<cpu, ppu, registers, cartridge>;

    constexpr cpu(memory& memory) :
        _processor{_ram.view()},
        _memory{memory}
    {}

private:
    processor _processor;
    ram _ram;
    memory& _memory;
};
}