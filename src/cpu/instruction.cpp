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

#include "cpu.h"

namespace nes {
/**************************************************************************************************
 *  Storage
 */
void processor::lda(byte operand) { _accumulator = operand; }
void processor::ldx(byte operand) { _x = operand; }
void processor::ldy(byte operand) { _y = operand; }

void processor::sta(reference operand) { operand = _accumulator; }
void processor::stx(reference operand) { operand = _x; }
void processor::sty(reference operand) { operand = _y; }

void processor::transfer(byte& from, byte& to)
{
    to = from;
    _status.logical(to);
}

void processor::tax() { transfer(_accumulator, _x); }
void processor::tay() { transfer(_accumulator, _y); }
void processor::tsx() { transfer(_stack.pointer, _x); }
void processor::txa() { transfer(_x, _accumulator); }
void processor::txs() { transfer(_x, _stack.pointer); }
void processor::tya() { transfer(_y, _accumulator); }


/**************************************************************************************************
 *  Math
 */

/**
 *  Add with carry.
 *  A,Z,C,N = A + M + C
 */
void processor::adc(byte operand)
{
    auto result = _accumulator + operand + _status.carry;
    _status.arithmetic(result);
    _status.overflows(_accumulator, operand, result);
    _accumulator = result;
}

/**
 *  Subtract with carry.
 *  A,Z,C,N = A - M + C
 *  Implemented in terms of ADC
 */
void processor::sbc(byte operand)
{
    adc(~operand);
}

/**
 *  Decrement and increment
 */
auto processor::decrement(byte operand) -> byte
{
    --operand;
    _status.logical(operand);
    return operand;
}

void processor::dec(reference operand) { operand = decrement(operand); }
void processor::dex() { _x = decrement(_x); }
void processor::dey() { _y = decrement(_y); }

auto processor::increment(byte operand) -> byte
{
    ++operand;
    _status.logical(operand);
    return operand;
}

void processor::inc(reference operand) { operand = increment(operand); }
void processor::inx() { _x = increment(_x); }
void processor::iny() { _y = increment(_y); }



/**************************************************************************************************
 *  Bitwise
 */

/**
 *  Logical AND of accumulator and operand.
 *  A,Z,N = A & M
 */
void processor::and_(byte operand)
{
    _accumulator &= operand;
    _status.logical(_accumulator);
}

/**
 *  Arithmetic shift left
 *  M,Z,C,N = M << 1
 */
auto processor::shift_left(byte operand) -> byte
{
    _status.carry = operand.bit(7);
    operand.shift_left();
    _status.arithmetic(operand);
    return operand;
}

void processor::asl() { _accumulator = shift_left(_accumulator); }
void processor::asl(reference operand) { operand = shift_left(operand); }

/**
 *  Logical shift right
 *  M,Z,C,N = M >> 1
 */
auto processor::shift_right(byte operand) -> byte
{
    _status.carry = operand.bit(0);
    operand.shift_right();
    _status.arithmetic(operand);
    return operand;
}

void processor::lsr() { _accumulator = shift_right(_accumulator); }
void processor::lsr(reference operand) { operand = shift_right(operand); }

/**
 *  Rotate left
 *  M,C,Z,N = M << 1, C
 */
auto processor::rotate_left(byte operand) -> byte
{
    operand.rotate_left(_status.carry);
    _status.logical(operand);
}

void processor::rol() { _accumulator = rotate_left(_accumulator); }
void processor::rol(reference operand) { operand = rotate_left(operand); }


/**
 *  Rotate right
 *  M,C,Z,N = M >> 1, C
 */
auto processor::rotate_right(byte operand) -> byte
{
    operand.rotate_right(_status.carry);
    _status.logical(operand);
}

void processor::ror() { _accumulator = rotate_right(_accumulator); }
void processor::ror(reference operand) { operand = rotate_right(operand); }

/**
 *  Bit test
 */
void processor::bit(byte operand)
{
    _status.zero = (_accumulator & operand == 0);
    _status.overflow = operand.bit(6);
    _status.negative = operand.bit(7);
}

/**
 *  Exclusive OR
 *  A,Z,N = A^M
 */
void processor::eor(byte operand)
{
    _accumulator ^= operand;
    _status.logical(_accumulator);
}

/**
 * Logical inclusive OR 
 *  A,Z,N = A|M
 */
void processor::ora(byte operand)
{
    _accumulator |= operand;
    _status.logical(_accumulator);
}


/**************************************************************************************************
 *  Branch
 */
void processor::branch(pointer location)
{
    _program_counter = location;
}

void processor::bcs(pointer location) { if (_status.carry) branch(location); }
void processor::bcc(pointer location) { if (!_status.carry) branch(location); }
void processor::beq(pointer location) { if (_status.zero) branch(location); }
void processor::bne(pointer location) { if (!_status.zero) branch(location); }
void processor::bmi(pointer location) { if (_status.negative) branch(location); }
void processor::bpl(pointer location) { if (!_status.negative) branch(location); }
void processor::bvs(pointer location) { if (_status.overflow) branch(location); }
void processor::bvc(pointer location) { if (!_status.overflow) branch(location); }


/**************************************************************************************************
 *  Jump
 */
void processor::jmp(pointer location)
{
    branch(location);
}

void processor::jsr(pointer location)
{
    _stack.push(_program_counter - 1);
    _program_counter = location;
}

void processor::rti()
{
    _status = _stack.pull();
    _program_counter = _stack.pull_word();
}

void processor::rts()
{
    _program_counter = _stack.pull_word() + 1;
}


/**************************************************************************************************
 *  Registers
 */
void processor::clc() { _status.carry = false; }
void processor::sec() { _status.carry = true; }
void processor::cld() { _status.decimal = false; }
void processor::sed() { _status.decimal = true; }
void processor::cli() { _status.interrupt_disable = false; }
void processor::sei() { _status.interrupt_disable = true; }
void processor::clv() { _status.overflow = false; }

void processor::compare(byte left, byte right)
{
    const auto result = left - right;
    _status.logical(result);
    _status.carry = left > right;
}

void processor::cmp(byte operand) { compare(_accumulator, operand); }
void processor::cpx(byte operand) { compare(_x, operand); }
void processor::cpy(byte operand) { compare(_y, operand); }


/**************************************************************************************************
 *  Stack
 */
void processor::pha() { _stack.push(_accumulator); }
void processor::php() { _stack.push(_status.instruction_value(); }
void processor::pla() { _accumulator = _stack.pull(); }
void processor::plp() { _status = _stack.pull(); }


/**************************************************************************************************
 *  System
 */
void processor::brk(reference vector) // Vector is generally located at 0xfffe, the IRQ vector
{
    _stack.push(_program_counter);
    _stack.push(_status.instruction_value());
    _program_counter = vector;
}
}