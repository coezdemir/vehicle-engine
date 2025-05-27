// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>

#ifndef  _TYPESAFE_ID_TYPE_NAME
static_assert(false, "You must define _TYPESAFE_ID_TYPE_NAME when including typesafe_id.h - See usage example in comments.");
#endif // ! _TYPESAFE_ID_TYPE_NAME

#include <ostream>

struct _TYPESAFE_ID_TYPE_NAME
{
    using value_t = unsigned long;
    value_t v;

    static const value_t invalid_value = std::numeric_limits<value_t>::max();

    constexpr bool operator<(const _TYPESAFE_ID_TYPE_NAME &b) const { return v < b.v; }

    constexpr bool operator==(const _TYPESAFE_ID_TYPE_NAME &b) const { return v == b.v; }

    constexpr bool operator!=(const _TYPESAFE_ID_TYPE_NAME &b) const { return v != b.v; }

    _TYPESAFE_ID_TYPE_NAME &operator++() {
        ++v;
        return *this;
    }

    _TYPESAFE_ID_TYPE_NAME operator++(int) {
        _TYPESAFE_ID_TYPE_NAME val{v};
        v++;
        return val;
    }

};
