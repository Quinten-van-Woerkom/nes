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
#include "segment.h"

namespace nes {
/*
 *  Generalised memory management class.
 *  Checks all member devices upon memory access.
 */
template<typename... Devices>
class memory {
public:
    constexpr memory(Devices&... devices) :
        _devices{std::forward_as_tuple(devices...)} {}

    constexpr auto read(word address) const -> byte {
        return read_helper<0>(address);
    }

    constexpr void write(word address, byte data) {
        write_helper<0>(address, data);
    }

    constexpr auto access(word access) const -> reference {
        return reference{*this, address};
    }

    /**
     *  Due to memory mapping and bank switching, normal references cannot be used.
     *  Instead, a simple wrapper class is used to allow for references.
     */
    class reference {
    public:
        constexpr reference(memory& host, word address) :
            _host{host}, _address{address}
        {}

        constexpr operator byte() const {
            return _host.read(_address);
        }

        constexpr operator word() const {
            return word{
                _host.read(_address),
                _host.read(word{_address + 1})
            };
        }

        constexpr auto operator=(byte data) -> reference& {
            _host.write(_address, data);
            return *this;
        }

        constexpr auto operator=(word data) -> reference& {
            _host.write(_address, data.low());
            _host.write(word{_address + 1}, data.high());
            return *this;
        }

        constexpr auto pointer() const -> pointer {
            return pointer{_host, _address};
        }

    private:
        memory& _host;
        word _address;
    };


    /**
     *  Pointer implementation.
     */
    class pointer {
    public:
        constexpr pointer(memory& host, word address) :
            _host{host}, _address{address}
        {}

        constexpr operator word() const {
            return _address;
        }

        constexpr auto operator*() -> reference {
            return reference{_host, _address};
        }

        constexpr auto operator++() -> pointer& {
            ++_address;
            return *this;
        }

        constexpr auto operator++(int) -> pointer {
            const auto temp = *this;
            ++(*this);
            return temp;
        }

    private:
        memory& _host;
        word _address;
    };
    
private:
    using Tuple = std::tuple<std::reference_wrapper<Devices>...>;
    static constexpr auto device_count = std::tuple_size_v<Tuple>;

    template<auto depth>
    constexpr auto read_helper(word address) const -> byte {
        if constexpr (depth == device_count) {
            throw std::runtime_error{"Unhandled memory read at address: " + address};
            return byte{0x00};
        } else {
            if (std::get<depth>(_devices).get().contains(address)) {
                return std::get<depth>(_devices).get().read(address);
            } else {
                return read_helper<depth + 1>(address);
            }
        }
    }

    template<auto depth>
    constexpr void write_helper(word address, byte data) {
        if constexpr (depth == device_count) {
            throw std::runtime_error{"Unhandled memory write at addres: " + address};
            return;
        }
        else {
            if (std::get<depth>(_devices).get().contains(address)) {
                return std::get<depth>(_devices).get().write(address, data);
            } else {
                return write_helper<depth + 1>(address, data);
            }
        }
    }

    Tuple _devices;
};
}