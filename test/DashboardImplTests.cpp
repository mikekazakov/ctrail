#include <catch2/catch.hpp>
#include <ctrail/DashboardImpl.h>
#include <ctrail/RegistryImpl.h>

using namespace ctrail; 

#define PREFIX "DashboardImpl "
TEST_CASE(PREFIX"provides the same set of counters names")
{
    std::int64_t cnt;
    RegistryImpl registry;
    registry.add("c", cnt);
    registry.add("b", cnt);
    registry.add("a", cnt);
    DashboardImpl dashboard{ registry.bake() };
    
    CHECK( dashboard.names() == std::vector<std::string>{"a", "b", "c"} );
}

TEST_CASE(PREFIX"fetches values for every counter")
{
    std::int32_t int32_1 = 0, int32_2 = 0;
    std::uint32_t uint32_1 = 0, uint32_2 = 0;     
    std::int64_t int64_1 = 0, int64_2 = 0;
    std::uint64_t uint64_1 = 0, uint64_2 = 0;
    std::atomic_int32_t atomic_int32_1 = 0, atomic_int32_2 = 0;
    std::atomic_uint32_t atomic_uint32_1 = 0, atomic_uint32_2 = 0;
    std::atomic_int64_t atomic_int64_1 = 0, atomic_int64_2 = 0;
    std::atomic_uint64_t atomic_uint64_1 = 0, atomic_uint64_2 = 0;
    std::int64_t external_1 = 0, external_2 = 0;
    
    RegistrySnapshot snapshot;
    snapshot.names.resize(18);
    snapshot.int32s.emplace_back(0, &int32_1);
    snapshot.int32s.emplace_back(1, &int32_2);
    snapshot.uint32s.emplace_back(2, &uint32_1);
    snapshot.uint32s.emplace_back(3, &uint32_2);
    snapshot.int64s.emplace_back(4, &int64_1);
    snapshot.int64s.emplace_back(5, &int64_2);
    snapshot.uint64s.emplace_back(6, &uint64_1);
    snapshot.uint64s.emplace_back(7, &uint64_2);
    snapshot.atomic_int32s.emplace_back(8, &atomic_int32_1);
    snapshot.atomic_int32s.emplace_back(9, &atomic_int32_2);
    snapshot.atomic_uint32s.emplace_back(10, &atomic_uint32_1);
    snapshot.atomic_uint32s.emplace_back(11, &atomic_uint32_2);
    snapshot.atomic_int64s.emplace_back(12, &atomic_int64_1);
    snapshot.atomic_int64s.emplace_back(13, &atomic_int64_2);
    snapshot.atomic_uint64s.emplace_back(14, &atomic_uint64_1);
    snapshot.atomic_uint64s.emplace_back(15, &atomic_uint64_2);
    snapshot.pullers.emplace_back(16, [&]{return external_1;});
    snapshot.pullers.emplace_back(17, [&]{return external_2;});    

    DashboardImpl dashboard{snapshot};
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    });
    
    int32_1++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    });

    int32_2++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    });

    uint32_1++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    });
    
    uint32_2++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    });
    
    int64_1++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    });
    
    int64_2++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    });
    
    uint64_1++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    });
    
    uint64_2++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    });
    
    atomic_int32_1++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
    });
    
    atomic_int32_2++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
    });
    
    atomic_uint32_1++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0
    });
    
    atomic_uint32_2++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0
    });
    
    atomic_int64_1++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
    });

    atomic_int64_2++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0
    });
    
    atomic_uint64_1++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0
    });
    
    atomic_uint64_2++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0
    });

    external_1++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
    });

    external_2++;
    CHECK( dashboard.values() == std::vector<std::int64_t>{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    });    
}
