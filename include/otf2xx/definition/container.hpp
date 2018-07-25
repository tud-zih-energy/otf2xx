/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2016, Technische Universität Dresden, Germany
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef INCLUDE_OTF2XX_DEFINITIONS_CONTAINER_HPP
#define INCLUDE_OTF2XX_DEFINITIONS_CONTAINER_HPP

#include <otf2xx/fwd.hpp>
#include <otf2xx/reference.hpp>

#include <otf2xx/definition/io_pre_created_handle_state.hpp>

#include <otf2xx/traits/definition.hpp>

#include <algorithm>
#include <cassert>
#include <map>
#include <stdexcept>
#include <vector>

namespace otf2
{
namespace definition
{

    template <typename Definition>
    class container
    {
    public:
        static_assert(otf2::traits::is_definition<Definition>::value,
                      "The type Definition has to be an otf2::definition");

        typedef Definition value_type;

    private:
        typedef typename otf2::reference<Definition>::ref_type key_type;
        typedef std::map<key_type, value_type> map_type;

        typedef container self;

        class iterator
        {
        public:
            iterator(typename map_type::const_iterator it, typename map_type::const_iterator end)
            : it(it), end(end)
            {
            }

            iterator& operator++()
            {
                assert(it != end);

                it++;
                return *this;
            }

            iterator operator++(int) // postfix ++
            {
                return iterator(it++, end);
            }

            value_type operator*() const
            {
                assert(it != end);

                return it->second;
            }

            const value_type* operator->() const
            {
                assert(it != end);

                return &(it->second);
            }

            bool operator!=(const iterator& other) const
            {
                return it != other.it;
            }

            bool operator==(const iterator& other) const
            {
                return it == other.it;
            }

            operator bool() const
            {
                return it != end;
            }

        private:
            typename map_type::const_iterator it;
            typename map_type::const_iterator end;
        };

    public:
        container(const self&) = default;
        self& operator=(const self&) = default;

        container() = default;

        container(self&&) = default;
        self& operator=(self&&) = default;

    public:
        const value_type& operator[](key_type key) const
        {
            if (key == otf2::reference<Definition>::undefined())
                return value_type::undefined();

            return data.at(key);
        }

        template <typename... Args>
        const value_type& emplace(key_type ref, Args&&... args)
        {
            return data
                .emplace(std::piecewise_construct, std::forward_as_tuple(ref),
                         std::forward_as_tuple(ref, args...))
                .first->second;
        }

        void add_definition(Definition&& def)
        {
            auto ref = def.ref();
            data.emplace(ref, std::move(def));
        }

        void add_definition(const Definition& def)
        {
            data.emplace(def.ref(), def);
        }

        std::size_t count(key_type key) const
        {
            return data.count(key);
        }

        std::size_t size() const
        {
            return data.size();
        }

        iterator find(key_type key) const
        {
            return iterator(data.find(key), data.end());
        }

    public:
        iterator begin() const
        {
            return iterator(data.begin(), data.end());
        }

        iterator end() const
        {
            return iterator(data.end(), data.end());
        }

    private:
        map_type data;
    };

    /**
     * Specialization for supplement definition, as they don't have a reference
     */
    template <typename SupplementDefinition>
    class supplement_container
    {
    public:
        static_assert(otf2::traits::is_definition<SupplementDefinition>::value,
                      "The type SupplementDefinition has to be an otf2::definition");

        typedef SupplementDefinition value_type;

    private:
        typedef std::vector<value_type> map_type;

        typedef supplement_container self;
        typedef typename map_type::const_iterator iterator;

    public:
        supplement_container(const self&) = default;
        self& operator=(const self&) = default;

        supplement_container() = default;

        supplement_container(self&&) = default;
        self& operator=(self&&) = default;

    public:
        template <typename... Args>
        const value_type& emplace(Args&&... args)
        {
            data.emplace_back(std::forward<Args>(args)...);

            return data.back();
        }

        const value_type& add_definition(const value_type& def)
        {
            data.push_back(def);

            return data.back();
        }

        const value_type& add_definition(value_type&& def)
        {
            data.emplace_back(std::move(def));

            return data.back();
        }

        std::size_t size() const
        {
            return data.size();
        }

    public:
        iterator begin() const
        {
            return data.cbegin();
        }

        iterator end() const
        {
            return data.cend();
        }

    private:
        map_type data;
    };

    template <>
    class container<otf2::definition::io_pre_created_handle_state>
    : public supplement_container<otf2::definition::io_pre_created_handle_state>
    {
    };

    template <typename Definition>
    class container<otf2::definition::property<Definition>>
    : public supplement_container<otf2::definition::property<Definition>>
    {
    public:
        static_assert(otf2::traits::is_definition<Definition>::value,
                      "The type Definition has to be an otf2::definition");
    };
} // namespace definition
} // namespace otf2

#endif // INCLUDE_OTF2XX_DEFINITIONS_CONTAINER_HPP
