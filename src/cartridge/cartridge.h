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
 *  Implements the functionality associated with the Nintendo cartridge boards.
 */
class cartridge {
public:
    constexpr auto read(word address) const -> byte
    {
        // To be implemented
        return byte{0};
    }

    constexpr void write(word address, byte data)
    {
        /* Writes to rom are a no-op. */
    }

private:

};
}