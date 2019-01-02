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
#include "span.h"

namespace nes {
/**
 *  Small class representing a range of value types [begin, end).
 */
template<typename T>
class range {
public:
    explicit constexpr range(T begin, T end) :
        _begin{begin}, _end{end} {}

    constexpr bool contains(T value) const noexcept
    {
        return value >= _begin && value < _end;
    }

    constexpr auto begin() const noexcept -> T
    {
        return _begin;
    }

    constexpr auto end() const noexcept -> T
    {
        return _end;
    }

    constexpr auto size() const noexcept -> T
    {
        return _end - _begin;
    }

private:
    T _begin, _end;
};


/**
 *  NES memory is very much interlinked, though it is 'owned' by multiple
 *  devices. This class effectively implements the memory bus linking all
 *  memory resources.
 */
class memory {
public:
    /**
     *  A memory segment is a span which is aware of its relative position in NES memory.
     *  This allows accessing using the NES indices.
     */
    class segment :
        private span<byte>,
        private range<word> {
    public:
        constexpr segment(span memory, range address_space) :
            span{memory}, range{address_space} {}

        using span::size;
        using span::begin;
        using span::end;
        using range::contains;

        /**
         *  Accessors
         */
        constexpr auto operator()(word address) const -> byte
        {
            const auto index = compute_index(address);
            return this->operator[](index);
        }

        constexpr auto operator()(word address) -> byte&
        {
            const auto index = compute_index(address);
            return this->operator[](index);
        }

        
    private:
        constexpr auto compute_index(word address) const -> index_type
        {
            return (address - range::begin()) % size();
        }
    };

private:

};
}