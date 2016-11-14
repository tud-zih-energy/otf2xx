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

#ifndef INCLUDE_OTF2XX_COMMON_HPP
#define INCLUDE_OTF2XX_COMMON_HPP

namespace otf2
{
namespace common
{

    /**
     * an enum for representing possible types within events and definitions
     */
    enum class type
    {
        none,
        uint8,
        uint16,
        uint32,
        uint64,
        int8,
        int16,
        int32,
        int64,
        Float,
        Double,
        string,
        attribute,
        location,
        region,
        group,
        metric,
        comm,
        parameter,
        rma_win,
        source_code_location,
        calling_context,
        interrupt_generator
    };

    enum class system_tree_node_domain
    {
        machine,
        shared_memory,
        numa,
        socket,
        cache,
        core,
        pu

    };

    /**
     * an enum for representing the contants of a group definition
     */
    enum class group_type
    {
        /** a unknown type */
        unknown,
        /** locations */
        locations,
        /** regions (not used) */
        regions,
        /** metrics (not used) */
        metric,
        /**  */
        comm_locations,
        /**  */
        comm_group,
        /** special group, only contains itself */
        comm_self
    };

    enum class group_flag_type
    {
        none,
        global_members
    };

    /**
     * an enum for representing the type of a location
     */
    enum class location_type
    {
        /** unknown */
        unknown,
        /** a cpu thread */
        cpu_thread,
        /** on gpu */
        gpu,
        /** a metric - acts as recorder for metric instances */
        metric
    };

    enum class parameter_type
    {
        string,
        int64,
        uint64
    };

    enum class location_group_type
    {
        unknown,
        process
    };

    enum class role_type
    {
        unknown,
        function,
        wrapper,
        loop,
        code,
        parallel,
        sections,
        section,
        workshare,
        single,
        single_sblock,
        master,
        critical,
        critical_sblock,
        atomic,
        barrier,
        implicit_barrier,
        flush,
        ordered,
        ordered_sblock,
        task,
        task_create,
        task_wait,
        coll_one2all,
        coll_all2one,
        coll_all2all,
        coll_other,
        file_io,
        point2point,
        rma,
        data_transfer,
        artifical,
        thread_create,
        thread_wait,
        task_untied,
        allocate,
        deallocate,
        reallocate
    };

    enum class paradigm_type
    {
        unknown,
        user,
        compiler,
        openmp,
        mpi,
        cuda,
        measurement_system,
        pthread,
        hmpp,
        ompss,
        hardware,
        gaspi,
        upc,
        shmem,
        winthread,
        qtthread,
        acethread,
        tbbthread,
        openacc,
        opencl,
        mtapi,
        sampling,
        none
    };

    enum class paradigm_class_type
    {
        process,
        thread_fork_join,
        thread_create_wait,
        accelerator
    };

    enum class paradigm_property_type
    {
        comm_name_template,
        rma_win_name_template,
        rma_only
    };

    enum class flags_type
    {
        none,
        dynamic,
        phase
    };

    enum class collective_type
    {
        barrier,
        broadcast,
        gather,
        gatherv,
        scatter,
        scatterv,
        all_gather,
        all_gatherv,
        all_to_all,
        all_to_allv,
        all_to_allw,
        all_reduce,
        reduce,
        reduce_scatter,
        scan,
        exscan,
        reduce_scatter_block,
        create_handle,
        destroy_handle,
        allocate,
        deallocate,
        create_handle_and_allocate,
        destroy_handle_and_deallocate
    };

    enum class metric_type
    {
        other,
        papi,
        rusage,
        user
    };

    enum class metric_timing
    {
        start = 0,
        point = 1 << 4,
        last = 2 << 4,
        next = 3 << 4
    };

    /**
     * metric occurence
     */
    enum class metric_occurence
    {
        /** Metric occurs at every region enter and leave. */
        strict,
        /** Metric occurs only at a region enter and leave, but does not need to occur at every
           enter/leave. */
        sync,
        /** Metric can occur at any place i.e. it is not related to region enter and leaves. */
        async
    };

    /**
     * \brief metric scope
     *
     * Specifies the scope the values of a metric class are valid for
     */
    enum class metric_scope
    {
        /** for a location*/
        location,
        /** for a location group */
        location_group,
        /** for a system tree node */
        system_tree_node,
        /** a custom group of locations */
        group
    };

    enum class metric_value_property
    {
        accumulated = 0,
        absolute = 1,
        relative = 2
    };

    enum class base_type
    {
        binary,
        decimal
    };

    enum class metric_mode;

    template <metric_timing timing, metric_value_property property>
    class both
    {
    public:
        static const int value = static_cast<int>(timing) | static_cast<int>(property);
    };

    enum class metric_mode
    {
        accumulated_start = both<metric_timing::start, metric_value_property::accumulated>::value,
        accumulated_point = both<metric_timing::point, metric_value_property::accumulated>::value,
        accumulated_last = both<metric_timing::last, metric_value_property::accumulated>::value,
        accumulated_next = both<metric_timing::next, metric_value_property::accumulated>::value,
        absolute_point = both<metric_timing::point, metric_value_property::absolute>::value,
        absolute_last = both<metric_timing::last, metric_value_property::absolute>::value,
        absolute_next = both<metric_timing::next, metric_value_property::absolute>::value,
        relative_point = both<metric_timing::point, metric_value_property::relative>::value,
        relative_last = both<metric_timing::last, metric_value_property::relative>::value,
        relative_next = both<metric_timing::next, metric_value_property::relative>::value
    };

    enum class recorder_kind
    {
        unknown,
        abstract,
        cpu,
        gpu
    };

    /**
     * enum for representing the type of an event
     *
     * \internal
     * This is used by the \ref otf2::event::buffer to distinguish between events,
     * as they are stored as void pointers.
     */
    enum class event_type
    {
        buffer_flush,
        enter,
        leave,
        measurement,
        metric,
        mpi_collective_begin,
        mpi_collective_end,
        mpi_ireceive,
        mpi_ireceive_request,
        mpi_isend,
        mpi_isend_complete,
        mpi_receive,
        mpi_request_cancelled,
        mpi_request_test,
        mpi_send,
        parameter_int,
        parameter_string,
        parameter_unsigned_int,
        thread_acquire_lock,
        thread_fork,
        thread_join,
        thread_release_lock,
        thread_task_complete,
        thread_task_create,
        thread_task_switch,
        thread_team_begin,
        thread_team_end
    };

    enum class interrupt_generator_mode_type
    {
        time,
        count
    };

    enum class mapping_type_type
    {
        string,
        attribute,
        location,
        region,
        group,
        metric,
        comm,
        parameter,
        rma_win,
        source_code_location,
        calling_context,
        interrupt_generator,
        max
    };

    enum class marker_severity_type
    {
        none,
        low,
        medium,
        high
    };

    enum class marker_scope_type
    {
        global,
        location,
        location_group,
        system_tree_node,
        group,
        comm
    };
}
}

#endif // INCLUDE_OTF2XX_COMMON_HPP