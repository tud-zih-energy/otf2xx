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
        };

        metric() = default;

        // construct with values
        metric(otf2::chrono::time_point timestamp, const otf2::definition::metric_class& metric_c,
               std::vector<value_container> values)
        : base<metric>(timestamp), metric_class_(metric_c), metric_instance_(), values_(values)
        {
        }

        metric(OTF2_AttributeList* al, otf2::chrono::time_point timestamp,
               const otf2::definition::metric_class& metric_c,
               const std::vector<value_container>& values)
        : base<metric>(al, timestamp), metric_class_(metric_c), metric_instance_(), values_(values)
        {
        }

        // construct with values
        metric(otf2::chrono::time_point timestamp,
               const otf2::definition::metric_instance& metric_c,
               const std::vector<value_container>& values)
        : base<metric>(timestamp), metric_class_(), metric_instance_(metric_c), values_(values)
        {
        }

        metric(OTF2_AttributeList* al, otf2::chrono::time_point timestamp,
               const otf2::definition::metric_instance& metric_c,
               const std::vector<value_container>& values)
        : base<metric>(al, timestamp), metric_class_(), metric_instance_(metric_c), values_(values)
        {
        }

        // copy constructor with new timestamp
        metric(const otf2::event::metric& other, otf2::chrono::time_point timestamp)
        : base<metric>(other, timestamp), metric_class_(other.metric_class()),
          metric_instance_(other.metric_instance()), values_(other.values())
        {
        }

        std::vector<value_container>& values()
        {
            return values_;
        }

        const std::vector<value_container>& values() const
        {
            return values_;
        }

        const value_container& get_value_for(const otf2::definition::metric_member& member) const
        {
            std::size_t i = 0;

            auto mc = metric_class_.lock();

            if (metric_instance_)
            {
                mc = metric_instance_->metric_class();
            }

            for (; i < mc.size(); i++)
            {
                if (mc[i] == member)
                    break;
            }

            assert(i < mc.size());

            return values_[i];
        }

        otf2::definition::metric_class metric_class() const
        {
            return metric_class_;
        }

        otf2::definition::metric_instance metric_instance() const
        {
            return metric_instance_;
        }

        friend class otf2::writer::local;

    private:
        otf2::definition::detail::weak_ref<otf2::definition::metric_class> metric_class_;
        otf2::definition::detail::weak_ref<otf2::definition::metric_instance> metric_instance_;
        std::vector<value_container> values_;
    };
}
} // namespace otf2::event

#endif // INCLUDE_OTF2XX_EVENT_ENTER_HPP
