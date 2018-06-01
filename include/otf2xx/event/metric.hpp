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

#ifndef INCLUDE_OTF2XX_EVENT_METRIC_HPP
#define INCLUDE_OTF2XX_EVENT_METRIC_HPP

#include <otf2xx/definition/fwd.hpp>
#include <otf2xx/definition/metric_member.hpp>

#include <otf2xx/event/base.hpp>

#include <otf2xx/chrono/chrono.hpp>

#include <otf2/OTF2_Events.h>

#include <otf2xx/exception.hpp>

#include <otf2xx/definition/detail/weak_ref.hpp>
#include <otf2xx/writer/fwd.hpp>

#include <cmath>
#include <complex>
#include <cstdint>
#include <vector>

namespace otf2
{
namespace event
{

    class metric : public base<metric>
    {
    public:
        class value_container
        {
            template <typename T>
            T scale(T x) const
            {
                int base = 2;
                if (metric->value_base() ==
                    otf2::definition::metric_member::value_base_type::decimal)
                    base = 10;
                return x * std::pow(base, metric->value_exponent());
            }

        public:
            value_container(
                otf2::definition::detail::weak_ref<otf2::definition::metric_member> metric,
                OTF2_MetricValue value)
            : metric(metric), value(value)
            {
            }

            otf2::definition::detail::weak_ref<otf2::definition::metric_member> metric;
            OTF2_MetricValue value;

            double as_double() const
            {
                typedef otf2::definition::metric_member::value_type_type value_type;

                switch (metric->value_type())
                {
                case value_type::Double:
                    return scale<double>(value.floating_point);
                case value_type::int64:
                    return scale<double>(static_cast<double>(value.signed_int));
                case value_type::uint64:
                    return scale<double>(static_cast<double>(value.unsigned_int));
                default:
                    make_exception("Unexpected type given in metric member");
                }

                return 0;
            }

            std::int64_t as_int64() const
            {
                typedef otf2::definition::metric_member::value_type_type value_type;

                switch (metric->value_type())
                {
                case value_type::Double:
                    return scale<std::int64_t>(static_cast<std::int64_t>(value.floating_point));
                case value_type::int64:
                    return scale<std::int64_t>(value.signed_int);
                case value_type::uint64:
                    return scale<std::int64_t>(static_cast<std::int64_t>(value.unsigned_int));
                default:
                    make_exception("Unexpected type given in metric member");
                }

                return 0;
            }

            std::uint64_t as_uint64() const
            {
                typedef otf2::definition::metric_member::value_type_type value_type;

                switch (metric->value_type())
                {
                case value_type::Double:
                    return scale<std::uint64_t>(static_cast<std::uint64_t>(value.floating_point));
                case value_type::int64:
                    return scale<std::uint64_t>(static_cast<std::uint64_t>(value.signed_int));
                case value_type::uint64:
                    return scale<std::uint64_t>(value.unsigned_int);
                default:
                    make_exception("Unexpected type given in metric member");
                }

                return 0;
            }

            template <typename T>
            void set(T x)
            {
                typedef otf2::definition::metric_member::value_type_type value_type;

                switch (metric->value_type())
                {
                case value_type::Double:
                    value.floating_point = static_cast<double>(x);
                    break;
                case value_type::int64:
                    value.signed_int = static_cast<std::int64_t>(x);
                    break;
                case value_type::uint64:
                    value.unsigned_int = static_cast<std::uint64_t>(x);
                    break;
                default:
                    make_exception("Unexpected type given in metric member");
                }
            }

            template <typename T>
            value_container& operator=(T x)
            {
                set(x);

                return *this;
            }
        };

        class metric_values
        {
        public:
            template <typename Definition>
            using weak_ref = otf2::definition::detail::weak_ref<Definition>;

            metric_values(const OTF2_Type* type_ids, const OTF2_MetricValue* values,
                          std::size_t num_events)
            : type_ids_(type_ids, type_ids + num_events), values_(values, values + num_events)
            {
            }

            std::size_t size() const
            {
                assert(type_ids_.size() == values_.size());
                return type_ids_.size();
            }

            const std::vector<OTF2_Type>& type_ids() const
            {
                return type_ids_;
            }

            const std::vector<OTF2_MetricValue>& values() const
            {
                return values_;
            }

        private:
            std::vector<OTF2_Type> type_ids_;
            std::vector<OTF2_MetricValue> values_;
        };

        metric() = default;

        // construct with values
        metric(otf2::chrono::time_point timestamp, const otf2::definition::metric_class& metric_c,
               metric_values&& values)
        : base<metric>(timestamp), metric_class_(metric_c), metric_instance_(),
          values_(std::move(values))
        {
        }

        metric(OTF2_AttributeList* al, otf2::chrono::time_point timestamp,
               const otf2::definition::metric_class& metric_c, metric_values&& values)
        : base<metric>(al, timestamp), metric_class_(metric_c), metric_instance_(),
          values_(std::move(values))
        {
        }

        // construct with values
        metric(otf2::chrono::time_point timestamp,
               const otf2::definition::metric_instance& metric_c, metric_values&& values)
        : base<metric>(timestamp), metric_class_(), metric_instance_(metric_c),
          values_(std::move(values))
        {
        }

        metric(OTF2_AttributeList* al, otf2::chrono::time_point timestamp,
               const otf2::definition::metric_instance& metric_c, metric_values&& values)
        : base<metric>(al, timestamp), metric_class_(), metric_instance_(metric_c),
          values_(std::move(values))
        {
        }

        // copy constructor with new timestamp
        metric(const otf2::event::metric& other, otf2::chrono::time_point timestamp)
        : base<metric>(other, timestamp), metric_class_(other.metric_class()),
          metric_instance_(other.metric_instance()), values_(other.values())
        {
        }

        metric_values& values()
        {
            return values_;
        }

        const metric_values& values() const
        {
            return values_;
        }

        value_container get_value_for(const otf2::definition::metric_member& member) const
        {
            const otf2::definition::metric_class& metric_class = this->resolve_metric_class();

            // Look up index of member inside of metric class, use this index to
            // construct a value container from the right OT2_MetricValue.
            std::size_t index = 0;
            for (const auto& class_member : metric_class)
            {
                if (class_member == member)
                {
                    OTF2_MetricValue metric_value = values_.values()[index];

                    return value_container{ member, metric_value };
                }

                index++;
            }

            throw std::out_of_range("Failed to look up metric_member inside metric_class");
        }

        bool has_metric_class() const
        {
            return static_cast<bool>(metric_class_);
        }

        template <typename Definition>
        using weak_ref = otf2::definition::detail::weak_ref<Definition>;

        weak_ref<otf2::definition::metric_class> metric_class() const
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

        weak_ref<otf2::definition::metric_instance> metric_instance() const
        {
            return metric_instance_;
        }

        void metric_instance(const otf2::definition::metric_instance& mi)
        {
            metric_class_ = nullptr;
            metric_instance_ = mi;
        }

        weak_ref<otf2::definition::metric_class> resolve_metric_class() const
        {
            assert(((void)"Malformed metric event: does reference neither metric class nor metric "
                          "instance!",
                    has_metric_class() || has_metric_instance()));

            if (has_metric_class())
            {
                return metric_class_;
            }
            else
            {
                return metric_instance_->metric_class();
            }
        }

        friend class otf2::writer::local;

    private:
        otf2::definition::detail::weak_ref<otf2::definition::metric_class> metric_class_;
        otf2::definition::detail::weak_ref<otf2::definition::metric_instance> metric_instance_;
        metric_values values_;
    };
} // namespace event
} // namespace otf2

#endif // INCLUDE_OTF2XX_EVENT_ENTER_HPP
