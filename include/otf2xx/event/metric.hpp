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

#ifndef INCLUDE_OTF2XX_EVENT_METRIC_HPP
#define INCLUDE_OTF2XX_EVENT_METRIC_HPP

#include <otf2xx/event/detail/metric_values.hpp>
#include <otf2xx/event/detail/value_proxy.hpp>

#include <otf2xx/definition/fwd.hpp>
#include <otf2xx/definition/metric_member.hpp>

#include <otf2xx/event/base.hpp>

#include <otf2xx/chrono/chrono.hpp>

#include <otf2xx/exception.hpp>

#include <otf2xx/definition/detail/weak_ref.hpp>
#include <otf2xx/writer/fwd.hpp>

namespace otf2
{
namespace event
{
    class metric : public base<metric>
    {
    public:
        using values = detail::metric_values;
        using value_proxy = detail::value_proxy;
        using const_value_proxy = detail::const_value_proxy;

        metric() = default;

        // construct with values
        metric(otf2::chrono::time_point timestamp, const otf2::definition::metric_class& metric_c,
               values&& values)
        : base<metric>(timestamp), metric_class_(metric_c), metric_instance_(),
          values_(std::move(values))
        {
        }

        metric(OTF2_AttributeList* al, otf2::chrono::time_point timestamp,
               const otf2::definition::metric_class& metric_c, values&& values)
        : base<metric>(al, timestamp), metric_class_(metric_c), metric_instance_(),
          values_(std::move(values))
        {
        }

        // construct with values
        metric(otf2::chrono::time_point timestamp,
               const otf2::definition::metric_instance& metric_c, values&& values)
        : base<metric>(timestamp), metric_class_(), metric_instance_(metric_c),
          values_(std::move(values))
        {
        }

        metric(OTF2_AttributeList* al, otf2::chrono::time_point timestamp,
               const otf2::definition::metric_instance& metric_c, values&& values)
        : base<metric>(al, timestamp), metric_class_(), metric_instance_(metric_c),
          values_(std::move(values))
        {
        }

        // construct without values, but reserve memory for them
        metric(otf2::chrono::time_point timestamp, const otf2::definition::metric_class& metric_c)
        : base<metric>(timestamp), metric_class_(metric_c), metric_instance_(), values_(metric_c)
        {
        }

        metric(OTF2_AttributeList* al, otf2::chrono::time_point timestamp,
               const otf2::definition::metric_class& metric_c)
        : base<metric>(al, timestamp), metric_class_(metric_c), metric_instance_(),
          values_(metric_c)
        {
        }

        // copy constructor with new timestamp
        metric(const otf2::event::metric& other, otf2::chrono::time_point timestamp)
        : base<metric>(other, timestamp), metric_class_(other.metric_class()),
          metric_instance_(other.metric_instance()), values_(other.raw_values())
        {
        }

        /// construct without referencing a metric class or a metric instance
        metric(OTF2_AttributeList* al, otf2::chrono::time_point timestamp, values&& values)
        : base<metric>(al, timestamp), metric_class_(), metric_instance_(),
          values_(std::move(values))
        {
        }

        values& raw_values()
        {
            return values_;
        }

        const values& raw_values() const
        {
            return values_;
        }

        value_proxy get_value_at(std::size_t index)
        {
            auto metric_class = resolve_weak_ref_to_metric_class();

            assert(static_cast<bool>(metric_class));

            return value_proxy(values_[index], (*metric_class)[index]);
        }

        const_value_proxy get_value_at(std::size_t index) const
        {
            auto metric_class = resolve_weak_ref_to_metric_class();

            assert(static_cast<bool>(metric_class));

            return const_value_proxy(values_[index], (*metric_class)[index]);
        }

        value_proxy get_value_for(const otf2::definition::metric_member& member)
        {
            auto metric_class = resolve_weak_ref_to_metric_class();

            // TODO: maybe check if metric_class is undefined? This might happen
            // if the event was constructed without a reference to a metric
            // class or metric instance.

            assert(static_cast<bool>(metric_class));

            // Look up the index of a member inside of metric class and use it to
            // construct a value_proxy from the right OTF2_Type and OTF2_MetricValue.

            auto it = std::find(metric_class->begin(), metric_class->end(), member);
            if (it == metric_class->end())
            {
                throw std::out_of_range("Failed to look up metric_member inside metric_class");
            }

            auto index = std::distance(metric_class->begin(), it);
            return value_proxy{ values_[index], member };
        }

        bool has_metric_class() const
        {
            return static_cast<bool>(metric_class_);
        }

        otf2::definition::metric_class metric_class() const
        {
            return metric_class_;
        }

        void metric_class(const otf2::definition::metric_class& mc)
        {
            metric_instance_ = nullptr;
            metric_class_ = mc;
        }

        bool has_metric_instance() const
        {
            return static_cast<bool>(metric_instance_);
        }

        otf2::definition::metric_instance metric_instance() const
        {
            return metric_instance_;
        }

        void metric_instance(const otf2::definition::metric_instance& mi)
        {
            metric_class_ = nullptr;
            metric_instance_ = mi;
        }

        otf2::definition::metric_class resolve_metric_class() const
        {
            return otf2::definition::metric_class{ resolve_weak_ref_to_metric_class() };
        }

        friend class otf2::writer::local;

    private:
        template <typename Definition>
        using weak_ref = otf2::definition::detail::weak_ref<Definition>;

        weak_ref<otf2::definition::metric_class> resolve_weak_ref_to_metric_class() const
        {
            if (has_metric_instance())
            {
                return otf2::definition::make_weak_ref(metric_instance_->metric_class());
            }
            else
            {
                return metric_class_;
            }
        }

    private:
        weak_ref<otf2::definition::metric_class> metric_class_;
        weak_ref<otf2::definition::metric_instance> metric_instance_;
        values values_;
    };
} // namespace event
} // namespace otf2

#endif // INCLUDE_OTF2XX_EVENT_ENTER_HPP
