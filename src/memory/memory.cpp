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

#include "memory.h"

#include "../cpu/cpu.h"
#include "../cartridge/cartridge.h"
#include "../ppu/ppu.h"

using namespace nes;


auto memory::read(word address) const -> byte
{
    if (address < 0x2000) return _cpu.read(address);
    if (address < 0x4000) return _ppu.read(address);
    // TODO: Special case OAMDMA, joysticks, etc.
    if (address < 0x4020) return _registers.read(address);
    else return _cartridge.read(address);
}


void memory::write(word address, byte data)
{
    if (address < 0x2000) return _cpu.write(address, data);
    if (address < 0x4000) return _ppu.write(address, data);
    // TODO: Special case OAMDMA, joysticks, etc.
    if (address < 0x4020) return _registers.write(address, data);
    else return _cartridge.write(address, data);
}



auto memory::ram() -> segment_view
{
    return _ram.view();
}