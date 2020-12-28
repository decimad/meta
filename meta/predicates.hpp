//          Copyright Michael Steinberg 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef META_PREDICATES_HPP___
#define META_PREDICATES_HPP___

namespace meta::predicates {

    template<template<typename> typename Predicate>
    struct negation {
        template<typename T>
        struct type {
            static constexpr bool value = !Predicate<T>::value;
        };
    };

    template<template <typename A, typename B> typename T, typename S>
    struct bind_1st {
        template<typename U>
        using type = T<S, U>;
    };

    template<template <typename A, typename B> typename T, typename S>
    struct bind_2nd {
        template<typename U>
        using type = T<U, S>;
    };

    template<template <typename T> typename... Predicates>
    struct disjunction {
        template<typename U>
        struct type {
            static constexpr bool value = (Predicates<U>::value && ...);
        };
    };

    template<template <typename T> typename... Predicates>
    struct conjunction {
        template<typename U>
        struct type {
            static constexpr bool value = (Predicates<U>::value || ...);
        };
    };

}

#endif
