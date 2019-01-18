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

#include <cassert>
#include <iterator>
#include <type_traits>

#include "byte.h"

namespace nes {
namespace detail {
constexpr std::ptrdiff_t dynamic_extent = -1;
}

/**
 *  Provides a view into a contiguous area of memory, similar to the proposed std::span.
 *  Basically a pointer-size pair.
 */
template<typename T, std::ptrdiff_t Extent = detail::dynamic_extent>
class span {
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using index_type = std::ptrdiff_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = T* const;
    using reference = T&;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr std::ptrdiff_t extent = Extent;

    static_assert(extent >= 0 || extent == detail::dynamic_extent, "Extent must be either non-negative or dynamic.");

    /**
     *  Constructors
     */
    constexpr span(const span& other) noexcept = default;

    constexpr span() noexcept :
        _data{nullptr}, _size{0} {}

    constexpr span(pointer ptr, index_type count) :
        _data{pointer}, _size{count} {
        assert(size() == extent || size() == 0 || extent == detail::dynamic_extent);
    }

    constexpr span(pointer first, pointer last) :
        _data{first}, _size{std::distance(first, last)} {
        assert(size() == extent || size() == 0 || extent == detail::dynamic_extent);
    }

    template<std::size_t N>
    constexpr span(std::array<value_type, N>& container) noexcept :
        _data{container.data()}, _size{static_cast<index_type>(container.size())} {
        assert(size() == extent || size() == 0 || extent == detail::dynamic_extent);
    }

    template<std::size_t N>
    constexpr span(const std::array<value_type, N>& container) noexcept :
        _data{container.data()}, _size{static_cast<index_type>(container.size())} {
        assert(size() == extent || size() == 0 || extent == detail::dynamic_extent);
    }

    template<class Container>
    constexpr span(Container& container) :
        _data{container.data()}, _size{static_cast<index_type>(container.size())} {
        assert(size() == extent || size() == 0 || extent == detail::dynamic_extent);
    }

    template<class Container>
    constexpr span(const Container& container) :
        _data{container.data()}, _size{static_cast<index_type>(container.size())} {
        assert(size() == extent || size() == 0 || extent == detail::dynamic_extent);
    }

    template<class U, std::ptrdiff_t N>
    constexpr span(const span<U, N>& other) noexcept :
        _data{other.data()}, _size{other.size()} {
        assert(size() == extent || size() == 0 || extent == detail::dynamic_extent);
    }


    /**
     *  Iterator accessors
     */
    constexpr auto begin() const noexcept -> iterator
    {
        return data();
    }

    constexpr auto cbegin() const noexcept -> const_iterator
    {
        return data();
    }

    constexpr auto end() const noexcept -> iterator
    {
        return data() + size();
    }

    constexpr auto cend() const noexcept -> const_iterator
    {
        return data() + size();
    }

    constexpr auto rbegin() const noexcept -> reverse_iterator
    {
        return data() + size();
    }

    constexpr auto crbegin() const noexcept -> const_reverse_iterator
    {
        return data() + size();
    }

    constexpr auto rend() const noexcept -> reverse_iterator
    {
        return data();
    }

    constexpr auto crend() const noexcept -> const_reverse_iterator
    {
        return data();
    }


    /**
     *  Observers
     */
    constexpr auto size() const noexcept -> index_type
    {
        return _size;
    }

    constexpr auto size_bytes() const noexcept -> index_type
    {
        return size() * sizeof(element_type);
    }

    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }


    /**
     *  Subspans
     */
    constexpr auto first(std::ptrdiff_t count) const -> span<element_type, detail::dynamic_extent>
    {
        assert(count <= size());
        return span{data(), count};
    }

    template<std::ptrdiff_t count>
    constexpr auto first() const -> span<element_type, count>
    {
        assert(count <= size());
        return span{data(), count};
    }

    constexpr auto last(std::ptrdiff_t count) const -> span<element_type, detail::dynamic_extent>
    {
        assert(count <= size());
        return span{data() + size() - count, count};
    }

    template<std::ptrdiff_t count>
    constexpr auto last() const -> span<element_type, count>
    {
        assert(count <= size());
        return span{data() + size() - count, count};
    }

    constexpr auto subspan(std::ptrdiff_t offset, std::ptrdiff_t count) const -> span<element_type, detail::dynamic_extent>
    {
        assert(count <= size());
        return span{data() + offset, count};
    }

    template<std::ptrdiff_t count>
    constexpr auto subspan(std::ptrdiff_t offset) const -> span<element_type, count>
    {
        assert(count <= size());
        return span{data() + offset, count};
    }


    /**
     *  Element access
     */
    constexpr reference operator[](index_type index) const
    {
        assert(index < size());
        return *(data() + index);
    }

    constexpr reference operator()(index_type index) const
    {
        return this->operator[](index);
    }

    constexpr pointer data() const noexcept
    {
        return _data;
    }

private:
    pointer _data;
    index_type _size;
};


/**
*   Deduction guides
*/
template<class T, std::size_t N>
span(std::array<T, N>&) -> span<T, N>;

template<class T, std::size_t N>
span(const std::array<T, N>&) -> span<const T, N>;

template<class Container>
span(Container&) -> span<typename Container::value_type>;

template<class Container>
span(const Container&) -> span<const typename Container::value_type>;
}