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

#ifndef INCLUDE_OTF2XX_CHRONO_CONVERT_HPP
#define INCLUDE_OTF2XX_CHRONO_CONVERT_HPP

#include <otf2xx/chrono/clock.hpp>
#include <otf2xx/chrono/ticks.hpp>
#include <otf2xx/chrono/time_point.hpp>

#include <cassert>
#include <limits>

namespace otf2
{
namespace chrono
{

    /**
     * \brief class to convert between ticks and time points
     *
     * This class can convert between ticks and time points.
     * For this, it needs the number of ticks per second.
     *
     * \note The time epoch is assumed to be equal between the time point and
     *       time point represented with the number ticks given.
     */
    class convert
    {
        const uint64_t ticks_per_second;

        static_assert(clock::period::num == 1, "Don't mess around with chrono!");

    public:
        /**
         * \param[in] ticks_per_second Number of ticks per second
         */
        convert(uint64_t ticks_per_second) : ticks_per_second(ticks_per_second)
        {
        }

        /**
         * \param[in] ticks Number of ticks per second
         */
        convert(otf2::chrono::ticks ticks) : ticks_per_second(ticks.count())
        {
        }

        /**
         * \brief converts from ticks to time point
         *
         * \param[in] ticks ticks since epoch
         * \return time_point with a duration equal to the passed time
         *         since the epoch.
         */
        otf2::chrono::time_point operator()(otf2::chrono::ticks ticks) const
        {
            // WARNING: Be careful, when changing clock::period::den.
            // You will have to think about every calculations twice, as there
            // might be narrowing and rounding anywhere.
            // We also assumed here, thatwe have picoseconds resolution and the
            // input resolution is about nanoseconds or a few hundred
            // picoseconds.
            // These assumptions have to be double checked!
            static_assert(clock::period::den == 1e12, "Assumed, that we calculate in picoseconds");

            double factor = static_cast<double>(clock::period::den) / ticks_per_second;

            assert(ticks_per_second <= clock::period::den);

            assert(ticks.count() <
                   static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) / factor);

            return time_point(otf2::chrono::duration(static_cast<int64_t>(ticks.count() * factor)));
        }

        /**
         * \brief converts from time points to ticks
         *
         * \param[in] t a time point
         * \return number ticks equal to passed time of the duration of the time
         *         point
         */
        otf2::chrono::ticks operator()(time_point t) const
        {
            assert(ticks_per_second == clock::period::den);
            return ticks(
                otf2::chrono::duration_cast<otf2::chrono::duration>(t.time_since_epoch()).count());
        }
    };

    /**
     * \brief converts from std::chrono::timepoint to otf2::chrono::time_point
     *
     * \param[in] tp the std::chrono time point
     * \return the same time point as otf2::chrono::time_point
     */
    template <typename Clock, typename Duration>
    otf2::chrono::time_point convert_time_point(std::chrono::time_point<Clock, Duration> tp)
    {
        return otf2::chrono::time_point(
            std::chrono::duration_cast<otf2::chrono::clock::duration>(tp.time_since_epoch()));
    }
}
} // namespace otf2::chrono

#endif // INCLUDE_OTF2XX_CHRONO_CONVERT_HPP
