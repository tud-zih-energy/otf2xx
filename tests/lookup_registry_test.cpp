#include <otf2xx/registry.hpp>

#include <iostream>
#include <string>

struct ByCPU
{
    using key_type = int;

    key_type key;

    ByCPU(key_type key) : key(key)
    {
    }
};

struct ByProcess
{
    using key_type = int;

    key_type key;

    ByProcess(key_type key) : key(key)
    {
    }
};

template <typename Definition>
struct MyHolder
{
    using type = typename otf2::get_default_holder<Definition>::type;
};

template <>
struct MyHolder<otf2::definition::string>
{
    using type = otf2::lookup_definition_holder<otf2::definition::string, ByCPU, ByProcess>;
};

int main()
{
    otf2::registry rego;
    rego.create<otf2::definition::string>(2, std::string("horst"));
    rego.create<otf2::definition::string>(std::string("regenbogen"));

    otf2::lookup_registry<MyHolder> reg;

    reg.create<otf2::definition::string>(ByCPU(7), std::string("cpu 7"));
    reg.create<otf2::definition::string>(ByProcess(7435), std::string("process 7435"));
    reg.create<otf2::definition::string>(ByProcess(745), 17, std::string("process 745"));
    reg.create<otf2::definition::string>(2, std::string("horst"));
    reg.create<otf2::definition::string>(std::string("regenbogen"));

    {
        auto test = reg.get<otf2::definition::string>(ByCPU(7));
        std::cout << test.ref() << " - " << test.str() << std::endl;
    }
    {
        auto test = reg.get<otf2::definition::string>(ByProcess(7435));
        std::cout << test.ref() << " - " << test.str() << std::endl;
    }

    for (auto& def : reg.all<otf2::definition::string>())
    {
        std::cout << def.ref() << " - " << def.str() << std::endl;
    }

    std::cout << reg.has<otf2::definition::string>(6) << " "
              << reg.has<otf2::definition::string>(ByCPU(6)) << " "
              << reg.has<otf2::definition::string>(ByCPU(7)) << " "
              << reg.has<otf2::definition::string>(ByProcess(6)) << std::endl;
}
