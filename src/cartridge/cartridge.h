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
#include <vector>

#include "../byte.h"
#include "rom.h"

namespace nes {
/**
 *  Implements PRG ROM functionality as required for mapper 0 ROMs.
 */
class prg_rom {
public:
    using bank = std::array<byte, 0x4000>;

    explicit prg_rom(std::vector<bank> banks) :
        _banks{banks}
    {}

private:
    std::vector<bank> _banks;
    bank* _upper;
    bank* _lower;
};



/**
 *  Implements CHR ROM as required for mapper 0 support.
 */
class chr_rom {
public:
    using bank = std::array<byte, 0x2000>;

    explicit chr_rom(std::vector<bank> banks) :
        _banks{banks}
    {}

private:
    std::vector<bank> _banks;
    bank* _current;
};



/**
 *  Implements the functionality associated with the Nintendo cartridge boards.
 *  Currently, only support for mapper 0 is considered.
 */
class cartridge {
public:
    explicit cartridge(rom_file& rom) :
        _chr_banks{rom.chr_rom}, _prg_banks{rom.prg_rom} {}


private:
    std::vector<chr_rom> _chr_banks;
    
};
}