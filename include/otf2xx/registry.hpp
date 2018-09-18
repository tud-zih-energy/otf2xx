/*
 * This file is part of otf2xx (https://github.com/tud-zih-energy/otf2xx)
 * otf2xx - A wrapper for the Open Trace Format 2 library
 *
 * Copyright (c) 2013-2018, Technische Universität Dresden, Germany
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

#pragma once

#include <otf2xx/definition/definitions.hpp>
#include <otf2xx/reference.hpp>
#include <otf2xx/reference_generator.hpp>

#include <type_traits>
#include <utility>

namespace otf2
{
class Registry;

template <typename Definition>
class DefinitionHolder
{
    static_assert(otf2::traits::is_referable_definition<Definition>::value, "Whoopsy.");

public:
    DefinitionHolder(otf2::trace_reference_generator& refs) : refs_(refs)
    {
    }

public:
    const Definition& operator[](typename Definition::reference_type ref) const
    {
        return definitions_[ref];
    }

    void operator()(const Definition& def)
    {
        definitions_.add_definition(def);
        refs_.register_definition(def);
    }

    void operator()(otf2::definition::detail::weak_ref<Definition> ref)
    {
        auto def = ref.lock();
        refs_.register_definition(def);
        definitions_.add_definition(std::move(def));
    }

    template <typename... Args>
    const Definition& create(Args&&... args)
    {
        return definitions_.emplace(refs_.next<Definition>(), std::forward<Args>(args)...);
    }

    template <typename RefType, typename... Args>
    std::enable_if_t<std::is_convertible<RefType, typename Definition::reference_type>::value,
                     const Definition&>
    create(RefType ref, Args&&... args)
    {
        // TODO I fucking bet that some day there will be a definition, where this is well-formed in
        // the case you wanted to omit the ref FeelsBadMan
        const auto& def = definitions_.emplace(ref, std::forward<Args>(args)...);
        refs_.register_definition(def);
        return def;
    }

    const otf2::definition::container<Definition>& data() const
    {
        return definitions_;
    }

    auto begin() const
    {
        return definitions_.begin();
    }

    auto end() const
    {
        return definitions_.end();
    }

private:
    otf2::definition::container<Definition> definitions_;
    otf2::trace_reference_generator& refs_;
};

template <typename Property>
class PropertyHolder
{
public:
    PropertyHolder(otf2::trace_reference_generator&)
    {
    }

    void operator()(const Property& def)
    {
        properties_.emplace(def);
    }

    template <typename... Args>
    const Property& create(Args&&... args)
    {
        return properties_.emplace(std::forward<Args>(args)...);
    }

    const otf2::definition::container<Property>& data() const
    {
        return properties_;
    }

    auto begin() const
    {
        return properties_.begin();
    }

    auto end() const
    {
        return properties_.end();
    }

private:
    otf2::definition::container<Property> properties_;
};

template <typename Definition, typename = void>
struct Holder;

template <typename Definition>
struct Holder<Definition,
              typename std::enable_if<!traits::is_referable_definition<Definition>::value>::type>
{
    using type = PropertyHolder<Definition>;
};

template <typename Definition>
struct Holder<Definition,
              typename std::enable_if<traits::is_referable_definition<Definition>::value>::type>
{
    using type = DefinitionHolder<Definition>;
};

class Registry
{
    template <typename Definition>
    struct make_holder
    {
        using type = typename Holder<Definition>::type;
    };

    using holders =
        tmp::apply_t<tmp::transform_t<traits::usable_definitions, make_holder>, std::tuple>;

    template <class Definition>
    auto& get_holder()
    {
        using holder = typename make_holder<Definition>::type;
        static_assert(tmp::contains<holders, holder>(), "Cannot get a holder for this definition!");
        return std::get<holder>(holders_);
    }

    template <class Definition>
    const auto& get_holder() const
    {
        using holder = typename make_holder<Definition>::type;
        static_assert(tmp::contains<holders, holder>(), "Cannot get a holder for this definition!");
        return std::get<holder>(holders_);
    }

    template <typename Holders>
    class construct_holders;

    template <typename... Holders>
    class construct_holders<std::tuple<Holders...>>
    {
    public:
        template <typename... Args>
        auto operator()(Args&&... args)
        {
            return std::make_tuple(Holders{ args... }...);
        }
    };

public:
    Registry() : holders_(construct_holders<holders>()(refs_))
    {
    }

public:
    template <typename Definition>
    const auto& all() const
    {
        return get_holder<Definition>();
    }

    template <typename Definition>
    auto& all()
    {
        return get_holder<Definition>();
    }

    template <typename Definition, typename... Args>
    auto& create(Args&&... args)
    {
        return get_holder<Definition>().create(std::forward<Args>(args)...);
    }

    template <typename Definition, typename Key>
    const auto& get(const Key& key) const
    {
        return get_holder<Definition>()[key];
    }

public:
    template <class Definition>
    void register_definition(const Definition& def)
    {
        get_holder<Definition>()(def);
    }

public:
    const holders& get_holders() const
    {
        return holders_;
    }

private:
    trace_reference_generator refs_;

    holders holders_;
};
} // namespace otf2
