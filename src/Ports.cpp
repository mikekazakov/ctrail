#include <ctrail/Ports.h>

namespace ctrail::internal {

struct tm localtime(time_t _time) noexcept
{
    struct tm tm;
#if _MSC_VER >= 1400
    localtime_s(&tm, &_time);
#else
    localtime_r(&_time, &tm);
#endif
    return tm;
} 

struct tm gmtime(time_t _time) noexcept
{
    struct tm tm;
#if _MSC_VER >= 1400    
    gmtime_s(&tm, &_time);
#else
    gmtime_r(&_time, &tm);    
#endif
    return tm;
}

}
