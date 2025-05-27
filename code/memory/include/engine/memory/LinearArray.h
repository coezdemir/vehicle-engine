// GameLab 3 Tutorial 2019/20
// Thanks to Andreas Knote <andreas.knote@uni-wuerzburg.de>

#pragma once

#include <array>
#include <stdexcept>

namespace engine::memory
{
    /***
     * Array-like fixed-size container that only allows adding elements.
     * @tparam T Type of objects stored in the array.
     * @tparam Size Maximum number of objects in the array.
     */
    template <typename T, size_t Size>
    struct LinearArray
    {
        using data_t = typename std::array<T, Size>;
        using iterator_t = typename data_t::iterator;

        LinearArray()
        {
        };

        virtual ~LinearArray()
        {
        };

        LinearArray(const LinearArray&) = delete;

        LinearArray(LinearArray&&) = delete;

        // Actual number of explicitly added elements
        size_t count() const
        {
            return _count;
        }

        // Maximum number of elements
        size_t capacity() const
        {
            return data.size();
        }

        // Range-checked move-insert at end
        void add(T&& value)
        {
            if (!(_count < capacity()))
                throw std::out_of_range("Access out of range.");

            data[_count] = std::move(value);
            _count++;
        }

        // Range-checked copy-insert at end
        void add(const T& value)
        {
            if (!(_count < capacity()))
                throw std::out_of_range("Access out of range.");

            data[_count] = value;
            _count++;
        }

        bool remove(const T& value)
        {
            _count--;

            for(int i = 0; i <= _count; i++)
            {
                if(data[i] == value)
                {
                    // data[_count] will get overridden eventually
                    std::swap(data[i], data[_count]);
                    return true;
                }
            }

            _count++;
            return false;
        }

        void pop_back()
        {
            if(!(_count < 0))
                throw std::out_of_range("Array is already empty.");

            data[_count] = 0;
            _count--;
        }

        bool contains(const T& value)
        {
            for(int i = 0; i < _count; i++)
            {
                if(data[i] == value)
                    return true;
            }

            return false;
        }

        // Range-checked
        constexpr T& at(size_t pos)
        {
            if (!pos < count())
                throw std::out_of_range("Access out of range.");
            return data[pos];
        }

        // Not range-checked, no modification
        constexpr const T& operator[](size_t pos) const
        {
            return data[pos];
        };

    private:
        size_t _count = 0;
        data_t data;

    public:
        //
        // Iterators for Range-based for loops, e.g. for(auto & s : systems) { }
        //
        constexpr typename data_t::const_iterator cbegin() const noexcept
        {
            return data.cbegin();
        }

        constexpr typename data_t::const_iterator begin() const noexcept
        {
            return data.begin();
        }

        constexpr typename data_t::iterator begin() noexcept
        {
            return data.begin();
        }

        typename data_t::const_iterator cend() const noexcept
        {
            return data.cbegin() + count();
        }

        typename data_t::const_iterator end() const noexcept
        {
            return data.begin() + count();
        }

        typename data_t::iterator end() noexcept
        {
            return data.begin() + count();
        }
    };
}
