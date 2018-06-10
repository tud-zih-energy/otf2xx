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

    namespace detail
    {
        /**
         * \brief A proxy class that allows type safe access to a metric value
         *
         * For performance reasons, the types and values of an event's metric
         * values are stored in separate locations. This proxy class allows to
         * set and retrieve a value with the correct type.
         */
        class typed_value_proxy
        {
        private:
            using value_type = otf2::common::type;

        public:
            typed_value_proxy(OTF2_Type& type, OTF2_MetricValue& value) : type_(type), value_(value)
            {
            }

            double as_double() const
            {
                switch (type())
                {
                case value_type::Double:
                    return static_cast<double>(value_.floating_point);
                case value_type::int64:
                    return static_cast<double>(value_.signed_int);
                case value_type::uint64:
                    return static_cast<double>(value_.unsigned_int);
                default:
                    make_exception("Unexpected type given in metric member");
                }

                return 0;
            }

            std::int64_t as_int64() const
            {
                switch (type())
                {
                case value_type::Double:
                    return static_cast<std::int64_t>(value_.floating_point);
                case value_type::int64:
                    return static_cast<std::int64_t>(value_.signed_int);
                case value_type::uint64:
                    return static_cast<std::int64_t>(value_.unsigned_int);
                default:
                    make_exception("Unexpected type given in metric member");
                }

                return 0;
            }

            std::uint64_t as_uint64() const
            {
                switch (type())
                {
                case value_type::Double:
                    return static_cast<std::uint64_t>(value_.floating_point);
                case value_type::int64:
                    return static_cast<std::uint64_t>(value_.signed_int);
                case value_type::uint64:
                    return static_cast<std::uint64_t>(value_.unsigned_int);
                default:
                    make_exception("Unexpected type given in metric member");
                }

                return 0;
            }

            template <typename T>
            std::enable_if_t<std::is_arithmetic<T>::value> value(T x)
            {
                switch (type())
                {
                case value_type::Double:
                    value_.floating_point = static_cast<double>(x);
                    break;
                case value_type::int64:
                    value_.signed_int = static_cast<std::int64_t>(x);
                    break;
                case value_type::uint64:
                    value_.unsigned_int = static_cast<std::uint64_t>(x);
                    break;
                default:
                    make_exception("Unexpected type given in metric member");
                }
            }

            template <typename T>
            typed_value_proxy operator=(T x)
            {
                value(x);
                return { *this };
            }

            otf2::common::type type() const
            {
                return static_cast<otf2::common::type>(type_);
            }

            void type(otf2::common::type type_id)
            {
                type_ = static_cast<OTF2_Type>(type_id);
            }

            void type(OTF2_Type type_id)
            {
                type_ = type_id;
            }

        private:
            OTF2_Type& type_;
            OTF2_MetricValue& value_;
        };

    } // namespace detail

    class metric : public base<metric>
    {
    public:
        /**
         * \brief A proxy class allowing type safe, correctly scaled access to a
         *        metric value
         */
        class value_proxy
        {
            template <typename T>
            T scale(T x) const
            {
                using value_base_type = otf2::definition::metric_member::value_base_type;

                int base;
                switch (metric_->value_base())
                {
                case value_base_type::binary:
                    base = 2;
                    break;
                case value_base_type::decimal:
                    base = 10;
                    break;
                }
                return x * std::pow(base, metric_->value_exponent());
            }

        public:
            value_proxy(OTF2_Type& type, OTF2_MetricValue& value,
                        otf2::definition::detail::weak_ref<otf2::definition::metric_member> metric)
            : value_(type, value), metric_(metric)
            {
            }

            value_proxy(const detail::typed_value_proxy& value,
                        otf2::definition::detail::weak_ref<otf2::definition::metric_member> metric)
            : value_(value), metric_(metric)
            {
            }

            value_proxy(detail::typed_value_proxy&& value,
                        otf2::definition::detail::weak_ref<otf2::definition::metric_member> metric)
            : value_(std::move(value)), metric_(metric)
            {
            }

            double as_double() const
            {
                return scale<double>(value_.as_double());
            }

            std::int64_t as_int64() const
            {
                return scale<std::int64_t>(value_.as_int64());
            }

            std::uint64_t as_uint64() const
            {
                return scale<std::uint64_t>(value_.as_uint64());
            }

            template <typename T>
            void set(T x)
            {
                value_.value(x);
            }

            template <typename T>
            value_proxy& operator=(T x)
            {
                value_ = x;
                return *this;
            }

            otf2::common::type type() const
            {
                assert(value_.type() == metric_->value_type());

                return value_.type();
            }

            // void type(otf2::common::type type_id)
            // {
            //     value_.type(type_id);
            //     metric_->value_type(type_id);
            // }

        private:
            detail::typed_value_proxy value_;
            otf2::definition::detail::weak_ref<otf2::definition::metric_member> metric_;
        };

        class metric_values
        {
        public:
            template <typename Definition>
            using weak_ref = otf2::definition::detail::weak_ref<Definition>;

            metric_values(std::vector<OTF2_Type>&& type_ids,
                          std::vector<OTF2_MetricValue>&& metric_values)
            : type_ids_(std::move(type_ids)), values_(std::move(metric_values))
            {
                if (type_ids_.size() != values_.size())
                {
                    make_exception(
                        "attempting to construct metric_values from data of different sizes");
                }
            }

            std::size_t size() const
            {
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

            detail::typed_value_proxy at(std::size_t index)
            {
                if (index >= size())
                {
                    throw std::out_of_range("Out of bounds access in metric_values");
                }

                return { type_ids_[index], values_[index] };
            }

            detail::typed_value_proxy operator[](std::size_t index)
            {
                return { type_ids_[index], values_[index] };
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

        /// construct without referencing a metric class or a metric instance
        metric(OTF2_AttributeList* al, otf2::chrono::time_point timestamp, metric_values&& values)
        : base<metric>(al, timestamp), metric_class_(), metric_instance_(),
          values_(std::move(values))
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

        value_proxy get_value_for(const otf2::definition::metric_member& member)
        {
            const otf2::definition::metric_class& metric_class = this->resolve_metric_class();

            // TODO: maybe check if metric_class is undefined? This might happen
            // if the event was constructed without a reference to a metric
            // class or metric instance.

            assert(metric_class.is_valid());

            // Look up the index of a member inside of metric class and use it to
            // construct a value_proxy from the right OTF2_Type and OTF2_MetricValue.

            auto it = std::find(metric_class.begin(), metric_class.end(), member);
            if (it == metric_class.end())
            {
                throw std::out_of_range("Failed to look up metric_member inside metric_class");
            }

            auto index = std::distance(metric_class.begin(), it);
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
            if (has_metric_instance())
            {
                return metric_instance_->metric_class();
            }
            else
            {
                return metric_class_;
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
