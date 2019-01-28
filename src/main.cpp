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

#include <array>
#include <iostream>
#include <string_view>

#include "byte.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "cartridge/rom.h"
#include "cartridge/cartridge.h"

using namespace nes;

int main() {
    auto cartr = cartridge{"../../external/donkey kong.nes"};

    std::cout << "Hello, world!\n";

    std::cout << "Press any key to continue...";
    std::cin.get();
    return 0;
}
