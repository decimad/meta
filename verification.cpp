
#include "meta/tree.hpp"
#include "meta/util.hpp"
//#include <meta/detail/list_verification.hpp>
#include <meta/detail/tree_verification.hpp>

/*
using namespace meta;

template<size_t Index>
struct tlist_leaf {};

// Type-List-Type tree for simplicity
template<typename... Ts>
struct tlist_tree : public meta::range_lr<tlist_tree<Ts...>, contexts::tlist>
{
};

using advance_test_tree =
    tlist_tree<
        tlist_leaf<0>,
        tlist_leaf<1>
    >;


static_assert(
    are_same_v<
        dereference_t<advance_t<tree_begin<tlist_tree<tlist_leaf<0>>, contexts::tlist>>>,
        tlist_leaf<0>
    >
);

template<typename T>
concept TypeHasType = requires(T)
{
    T::type;
};

template<size_t Value>
struct test {
    static constexpr size_t value = 1 / Value;
};

template<size_t Value>
struct func {
    using type = test<test<Value>::value>;
};

template<typename T>
struct switch_
{
    using type = void;
};

template<TypeHasType T>
struct switch_<T>
{
    using type = typename T::type;
};


static_assert(std::is_same_v<typename switch_<func<3>>::type, void>, "Yay!");
*/

int main()
{
    return 0;
}

/*

[build] In file included from .././meta/tlist.hpp:12,
[build]                  from ../meta/tree.hpp:4,
[build]                  from ../verification.cpp:2:
[build] .././meta/iterator.hpp: In instantiation of 'struct meta::advance_n<meta::iterator_range<meta::tree_iterator<meta::contexts::tlist, meta::tlist<meta::verification::tlist_leaf<2>, meta::detail::SE<meta::verification::tlist_tree<meta::verification::tlist_tree<meta::verification::tlist_leaf<0>, meta::verification::tlist_leaf<1> >, meta::verification::tlist_leaf<2> >, 1> >, meta::traversals::nlr>, meta::sentinel<meta::tree_iterator<meta::contexts::tlist, meta::tlist<meta::verification::tlist_leaf<2>, meta::detail::SE<meta::verification::tlist_tree<meta::verification::tlist_tree<meta::verification::tlist_leaf<0>, meta::verification::tlist_leaf<1> >, meta::verification::tlist_leaf<2> >, 1> >, meta::traversals::nlr> > >, 1>':
[build] .././meta/iterator.hpp:56:15:   recursively required from 'struct meta::advance_n<meta::iterator_range<meta::tree_iterator<meta::contexts::tlist, meta::tlist<meta::verification::tlist_tree<meta::verification::tlist_leaf<0>, meta::verification::tlist_leaf<1> >, meta::detail::SE<meta::verification::tlist_tree<meta::verification::tlist_tree<meta::verification::tlist_leaf<0>, meta::verification::tlist_leaf<1> >, meta::verification::tlist_leaf<2> >, 0> >, meta::traversals::nlr>, meta::sentinel<meta::tree_iterator<meta::contexts::tlist, meta::tlist<meta::verification::tlist_leaf<2>, meta::detail::SE<meta::verification::tlist_tree<meta::verification::tlist_tree<meta::verification::tlist_leaf<0>, meta::verification::tlist_leaf<1> >, meta::verification::tlist_leaf<2> >, 1> >, meta::traversals::nlr> > >, 4>'
[build] .././meta/iterator.hpp:56:15:   required from '
        struct meta::advance_n<
            meta::iterator_range<
                meta::tree_iterator<
                    meta::contexts::tlist,
                    meta::tlist<
                        meta::verification::tlist_tree<
                            meta::verification::tlist_tree<
                                meta::verification::tlist_leaf<0>,
                                meta::verification::tlist_leaf<1>
                            >,
                            meta::verification::tlist_leaf<2>
                        >
                    >,
                    meta::traversals::nlr
                >,
                meta::sentinel<
                    meta::tree_iterator<
                        meta::contexts::tlist,
                        meta::tlist<
                            meta::verification::tlist_leaf<2>,
                            meta::detail::SE<
                                meta::verification::tlist_tree<
                                    meta::verification::tlist_tree<
                                        meta::verification::tlist_leaf<0>,
                                        meta::verification::tlist_leaf<1>
                                    >,
                                    meta::verification::tlist_leaf<2>
                                >,
                                1
                            >
                        >,
                        meta::traversals::nlr
                    >
                >
            >,
        5>'
[build] .././meta/iterator.hpp:66:11:   required by substitution of '
    template<class Iter, unsigned int N>
    requires  Iterator<Iter>
    using advance_n_t = typename meta::advance_n::type
    [with
        Iter =  meta::iterator_range<
                    meta::tree_iterator<
                        meta::contexts::tlist,
                        meta::tlist<
                            meta::verification::tlist_tree<
                                meta::verification::tlist_tree<
                                    meta::verification::tlist_leaf<0>,
                                    meta::verification::tlist_leaf<1>
                                >,
                                meta::verification::tlist_leaf<2>
                            >
                        >,
                        meta::traversals::nlr
                    >,
                    meta::sentinel<
                        meta::tree_iterator<
                            meta::contexts::tlist,
                            meta::tlist<
                                meta::verification::tlist_leaf<2>,
                                meta::detail::SE<
                                    meta::verification::tlist_tree<
                                        meta::verification::tlist_tree<
                                            meta::verification::tlist_leaf<0>,
                                            meta::verification::tlist_leaf<1>
                                        >,
                                        meta::verification::tlist_leaf<2>
                                    >,
                                    1
                                >
                            >,
                            meta::traversals::nlr
                        >
                    >
                >;
                unsigned int N = 5
    ]'

[build] .././meta/detail/tree_verification.hpp:260:76:   required from here
[build] .././meta/iterator.hpp:56:15: error: template constraint failure for '
    template<class Iter, unsigned int N>
    requires  Iterator<Iter> struct meta::advance_n'
[build]    56 |  using type = typename advance_n<advance_t<Iter>, N-1>::type;
[build]       |               ^~~~
[build] .././meta/iterator.hpp:56:15: note: constraints not satisfied
[build] .././meta/iterator.hpp:39:17:   required for the satisfaction of 'Iterator<Iter>' [with
    Iter = meta::iterator_range<
        meta::sentinel<
            meta::tree_iterator<
                meta::contexts::tlist,
                meta::tlist<
                    meta::verification::tlist_leaf<2>, meta::detail::SE<meta::verification::tlist_tree<meta::verification::tlist_tree<meta::verification::tlist_leaf<0>, meta::verification::tlist_leaf<1> >, meta::verification::tlist_leaf<2> >, 1> >, meta::traversals::nlr> >, meta::sentinel<meta::tree_iterator<meta::contexts::tlist, meta::tlist<meta::verification::tlist_leaf<2>, meta::detail::SE<meta::verification::tlist_tree<meta::verification::tlist_tree<meta::verification::tlist_leaf<0>, meta::verification::tlist_leaf<1> >, meta::verification::tlist_leaf<2> >, 1> >, meta::traversals::nlr> > >]
[build] .././meta/iterator.hpp:39:45: note: no operand of the disjunction is satisfied
[build]    39 |         concept Iterator = ValidIterator<T> || Sentinel<T>;
[build]       |                            ~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~
[build] cc1plus.exe: note: set '-fconcepts-diagnostics-depth=' to at least 2 for more detail
[build] In file included from ../verification.cpp:5:
[build] .././meta/detail/tree_verification.hpp:259:9: error: template argument 1 is invalid
[build]   259 |         are_same_v<
[build]       |         ^~~~~~~~~~~
[build]   260 |             advance_n_t<tree_range_t<advance_test_tree, contexts::tlist>, 5>,
[build]       |             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[build]   261 |             iterator_range<tree_end<advance_test_tree, contexts::tlist>, tree_end<advance_test_tree, contexts::tlist>>
[build]       |             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[build]   262 |         >
[build]       |         ~


*/