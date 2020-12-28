#ifndef META_UTIL_HPP___
#define META_UTIL_HPP___

#include <type_traits>

namespace meta {

    struct error_type {};

    //
    // are_same< Ts... >

    template<typename... Ts>
    struct are_same {
        static constexpr bool value = true;
    };

    template<typename T0, typename T1, typename... Ts>
    struct are_same<T0, T1, Ts...>
    {
        static constexpr bool value = std::is_same_v<T0, T1> && (std::is_same_v<T0, Ts> && ...);
    };

    template<typename... Ts>
    static constexpr bool are_same_v = are_same<Ts...>::value;

    //
    // are_equal< Vs... >

    template<auto... Vs>
    struct are_equal {
        static constexpr bool value = true;
    };

    template<auto V0, auto V1, auto... Vs>
    struct are_equal<V0, V1, Vs...> {
        static constexpr bool value = (V0 == V1) && are_equal<V1, Vs...>::value;
    };

    template<auto... Vs>
    static constexpr bool are_equal_v = are_equal<Vs...>::value;
}

#endif