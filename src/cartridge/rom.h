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
#include <cstdint>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <variant>
#include <vector>

namespace nes {
namespace fs = std::experimental::filesystem;

/**
 *  iNES format is used.
 *  For documentation of the header and file format, see https://wiki.nesdev.com/w/index.php/INES
 */
using prg_rom_unit = std::array<byte, 0x4000>;
using chr_rom_unit = std::array<byte, 0x2000>;

struct rom_file {
    std::int16_t mapper;            // Currently, only 0 is supported

    // Flags 6
    bool vertical_mirroring;
    bool persistent_memory;
    bool trainer_present;
    bool four_screen_vram;

    // Flags 7
    bool vs_unisystem;
    bool playchoice;
    
    std::vector<byte> trainer;  // 0 or 512 bytes
    std::vector<prg_rom_unit> prg_rom;  // In 16 KB units
    std::vector<chr_rom_unit> chr_rom;  // In 8 KB units
    std::vector<byte> playchoice_data; // 0 or 8 KB
};



/**
 *  Small convenience wrappers for file reading.
 */
template<typename Container>
void read(std::ifstream& file, Container& destination, std::ptrdiff_t count)
{
    using value_type = typename Container::value_type;
    std::copy_n(std::istream_iterator<value_type>{file}, count, destination.begin());
}

template<typename Type>
void read(std::ifstream& file, std::vector<Type>& destination, std::ptrdiff_t count)
{
    destination.reserve(count);
    std::copy_n(std::istream_iterator<Type>{file}, count, destination.begin());
}


template<typename Container>
void read(std::ifstream& file, Container& destination)
{
    using istream_iterator = std::istream_iterator<typename Container::value_type>;
    std::copy(istream_iterator{file}, istream_iterator{}, destination.begin());
}


/**
 *  iNES headers should start with the byte combination $4e $45 $53 $1a,
 *  which is NES followed by an EOF character.
 */
constexpr bool valid_header(const std::array<byte, 16>& header)
{
    return (header[0] == 0x4E && header[1] == 0x45 && header[2] == 0x53 && header[3] == 0x1a);
}


/**
 *  Reads the iNES file header into the given ROM object.
 */
void read_header(std::ifstream& file, rom_file& result)
{
    std::array<byte, 16> header;
    read(file, header, 16);

    if (!valid_header(header)) throw std::runtime_error("Invalid file format or corrupted file.");

    result.vertical_mirroring = header[6].bit(0);
    result.persistent_memory = header[6].bit(1);
    result.trainer_present = header[6].bit(2);
    result.four_screen_vram = header[6].bit(3);
    result.vs_unisystem = header[7].bit(0);
    result.playchoice = header[7].bit(1);

    result.mapper = (header[6] >> 4) | (header[7] & 0xf0);

    result.prg_rom.resize(header[4]);
    result.chr_rom.resize(header[5]);
}


/**
 *  Reads from the file path given.
 */
auto read_rom(const fs::path& path) -> rom_file
{
    if (!fs::exists(path)) throw std::invalid_argument("Non-existent file.");
    auto file = std::ifstream{path, std::ios::binary};
    if (!file.is_open()) throw std::invalid_argument("Unable to open file.");

    rom_file result;
    read_header(file, result);

    if (result.trainer_present) read(file, result.trainer, 0x200);
    for (auto& bank : result.prg_rom) read(file, bank, 0x4000);
    for (auto& bank : result.chr_rom) read(file, bank, 0x2000);
    if (result.playchoice) read(file, result.playchoice_data, 0x2000);

    return result;
}
}