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
#include <experimental/filesystem>

#include "../byte.h"
#include "rom.h"

namespace nes {
namespace fs = std::experimental::filesystem;
/**
 *  Implements the functionality associated with the Nintendo cartridge boards.
 */
class cartridge {
public:
    cartridge(const fs::path path) :
        cartridge{read_rom(path)}
    {}

    cartridge(rom_file file) :
        _prg_rom(file.prg_rom),
        _chr_rom(file.chr_rom),
        _prg_lower{span<byte>{_prg_rom.data(), 0x4000}, word{0x8000}, word{0x4000}},
        _prg_upper{span<byte>{_prg_rom.data() + 0x4000, 0x4000}, word{0xc000}, word{0x4000}}
    {
        if (file.mapper != 0x00) throw std::runtime_error{"Unsupported mapper type: only mapper 0 is implemented"};
        if (_prg_rom.size() > 0x8000) throw std::runtime_error{"Unsupported PRG ROM size in ROM file: bank switching is not yet supported"};
        if (_chr_rom.size() > 0x2000) throw std::runtime_error{"Unsupported CHR ROM size in ROM file: bank switching is not yet supported"};
    }


    constexpr auto read(word address) const -> byte
    {
        if (address < 0xc000) return _prg_lower.read(address);
        else return _prg_lower.read(address);
    }

    constexpr void write(word address, byte data)
    {
        /* Writes to rom are a no-op. */
    }

private:
    std::vector<byte> _prg_rom;
    std::vector<byte> _chr_rom;
    segment_view _prg_lower;
    segment_view _prg_upper;
    /* TODO: CHR ROM segment*/
};
}