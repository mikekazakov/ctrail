#include <catch2/catch.hpp>
#include <ctrail/RegistryImpl.h>
#include <ctrail/RegistrySnapshot.h>

#define PREFIX "RegistryImpl "

TEST_CASE(PREFIX"implements every counter")
{
    ctrail::RegistryImpl registry;
    SECTION("int32")
    {
        std::int32_t cnt;
        registry.add("1", cnt);
        const auto snapshot = registry.bake();
        CHECK(snapshot.int32s.at(0).second == &cnt);
    }
    SECTION("uint32")
    {
        std::uint32_t cnt;
        registry.add("1", cnt);
        const auto snapshot = registry.bake();
        CHECK(snapshot.uint32s.at(0).second == &cnt);
    }
    SECTION("int64")
    {
        std::int64_t cnt;
        registry.add("1", cnt);
        const auto snapshot = registry.bake();
        CHECK(snapshot.int64s.at(0).second == &cnt);
    }
    SECTION("uint64")
    {
        std::uint64_t cnt;
        registry.add("1", cnt);
        const auto snapshot = registry.bake();
        CHECK(snapshot.uint64s.at(0).second == &cnt);
    }
    SECTION("atomic_int32")
    {
        std::atomic_int32_t cnt;
        registry.add("1", cnt);
        const auto snapshot = registry.bake();
        CHECK(snapshot.atomic_int32s.at(0).second == &cnt);
    }
    SECTION("atomic_uint32")
    {
        std::atomic_uint32_t cnt;
        registry.add("1", cnt);
        const auto snapshot = registry.bake();
        CHECK(snapshot.atomic_uint32s.at(0).second == &cnt);
    }
    SECTION("atomic_int64")
    {
        std::atomic_int64_t cnt;
        registry.add("1", cnt);
        const auto snapshot = registry.bake();
        CHECK(snapshot.atomic_int64s.at(0).second == &cnt);
    }
    SECTION("atomic_uint64")
    {
        std::atomic_uint64_t cnt;
        registry.add("1", cnt);
        const auto snapshot = registry.bake();
        CHECK(snapshot.atomic_uint64s.at(0).second == &cnt);
    }
    SECTION("puller")
    {
        static const auto func = []() -> std::int64_t { return 12345; };
        std::function<std::int64_t()> puller{func};
        registry.add("1", puller);
        const auto snapshot = registry.bake();
        CHECK(snapshot.pullers.at(0).second() == func());
    }
}

TEST_CASE(PREFIX"throws on registering an empty name")
{
    ctrail::RegistryImpl registry;
    std::int32_t cnt;
    CHECK_THROWS( registry.add(std::string_view{}, cnt) ); 
    CHECK_THROWS( registry.add("", cnt) ); 
}

TEST_CASE(PREFIX"throws on registering the same name twice")
{
    ctrail::RegistryImpl registry;
    std::int32_t cnt1;
    std::int32_t cnt2;
    CHECK_NOTHROW( registry.add("1", cnt1) );
    CHECK_THROWS( registry.add("1", cnt2) ); 
}

TEST_CASE(PREFIX "throws on registering an empty puller")
{
    ctrail::RegistryImpl registry;
    std::function<std::int64_t()> puller;
    CHECK_THROWS(registry.add("1", puller));
}

TEST_CASE(PREFIX"sorts counters names")
{
    ctrail::RegistryImpl registry;
    std::int32_t cnt1;
    std::int32_t cnt2;
    registry.add("b", cnt1);
    registry.add("a", cnt2);
    
    const auto snapshot = registry.bake();

    CHECK(snapshot.names == std::vector<std::string>{ "a", "b" } );
    CHECK(snapshot.int32s.at(0).first == 1 );
    CHECK(snapshot.int32s.at(0).second == &cnt1 );
    CHECK(snapshot.int32s.at(1).first == 0 );
    CHECK(snapshot.int32s.at(1).second == &cnt2 );     
}
