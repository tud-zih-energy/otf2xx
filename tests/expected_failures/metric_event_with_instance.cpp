#include <otf2xx/otf2.hpp>

int main(){
    namespace def = otf2::definition;
    def::container<def::string> strings;
    strings.add_definition({ 0, "42" });

    def::system_tree_node root_node(0, strings[0], strings[0]);

    def::location_group lg(0, strings[0], def::location_group::location_group_type::process, root_node);

    def::location location(0, strings[0], lg, def::location::location_type::cpu_thread);
    
    def::metric_class mClass(0, otf2::common::metric_occurence::strict, otf2::common::recorder_kind::abstract);
    def::metric_instance metric(0, mClass, location, location);

#ifdef OTF2XX_MAKE_FAIL
    // event::metric expects a metric_class not a metric_instance
    otf2::event::metric ev(otf2::chrono::genesis(), metric);
#endif
}