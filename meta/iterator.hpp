#ifndef META_ITERATOR_HPP__
#define META_ITERATOR_HPP__

#include <type_traits>

namespace meta {

    using std::size_t;

    template<typename T = void>
    struct sentinel {};

    template<typename T>
    std::false_type is_sentinel(T);

    template<typename T>
    std::true_type is_sentinel(sentinel<T>);

    namespace concepts {

        template<typename Iter>
        concept ValidIterator = requires(Iter) {
            dereference(std::declval<Iter>());
            advance(std::declval<Iter>());
        };

        template<typename Iter>
        concept Sentinel = decltype(is_sentinel(std::declval<Iter>()))::value;

        template<typename T>
        concept Iterator   = Sentinel<T> || ValidIterator<T>; // last element is range

        template<typename T>
        concept Enumerator = ValidIterator<T> || Sentinel<T>;   // This is basically the same since we already need to know Sentinel<> for the iterator

    }

    //
    // advance_n
    //

    template<concepts::ValidIterator Iter>
    using advance_t = decltype(advance(std::declval<Iter>()));;

    template<concepts::ValidIterator Iter>
    using dereference_t = decltype(dereference(std::declval<Iter>()));

    template<concepts::Iterator Iter, size_t N>
       struct advance_n
    {
        using type = typename advance_n<advance_t<Iter>, N-1>::type;
    };

    template<concepts::Iterator Iter>
    struct advance_n<Iter, 0>
    {
        using type = Iter;
    };

    template<concepts::Iterator Iter, size_t N>
    using advance_n_t = typename advance_n<Iter, N>::type;


    //
    // transform_iterator
    //

    template<concepts::Iterator Iter, template<typename> typename Transform>
    struct transform_iterator
    {};

    template<concepts::Iterator Iter, template<typename> typename Transform>
    auto dereference(transform_iterator<Iter, Transform>) -> typename Transform<dereference_t<Iter>>::type;

    template<concepts::Iterator Iter, template<typename> typename Transform>
    auto advance(transform_iterator<Iter, Transform>) -> typename Transform<advance_t<Iter>>::type;

    //
    // iterator_equal
    // details: It may not always be desirable to compute the true type of the sentinel iterator 'end' when iterating sequences
    //          with a bidirectional iterator. Thus we add a comparison overload to compare two iterators for equality, even if their
    //          types are not equal.
    //

    template<concepts::Iterator Iter1, concepts::Iterator Iter2>
    std::false_type iterator_equal(Iter1, Iter2);

    template<concepts::Iterator Iter>
    std::true_type iterator_equal(Iter, Iter);

    template<concepts::Iterator Iter1, concepts::Iterator Iter2>
    static constexpr bool iterator_equal_v = decltype(iterator_equal(std::declval<Iter1>(), std::declval<Iter2>()))::value;

    //
    // filter_iterator
    //

    namespace detail {

        template<typename Iterator, typename End, template<typename> typename Filter>
        struct apply_filter {
            static constexpr bool value = Filter<Iterator>::value;
        };

        template<typename End, template<typename> typename Filter>
        struct apply_filter<End, End, Filter> {
            static constexpr bool value = false;
        };

        template<typename Iterator, typename End, template <typename> typename Filter, bool Value = apply_filter<Iterator, End, Filter>::value>
        struct advance_until
        {
            using type = typename advance_until<advance_t<Iterator>, End, Filter>::type;
        };

        template<typename Iterator, typename End, template <typename> typename Filter>
        struct advance_until<Iterator, End, Filter, true>
        {
            using type = Iterator;
        };

        template<typename End, template <typename> typename Filter>
        struct advance_until<End, End, Filter, false>
        {
            using type = End;
        };

    }

    template<typename Iterator, typename End, template<typename> typename Filter>
    struct filter_iterator_struct
    {
    };

    template<typename Iterator, typename End, template <typename> typename Filter>
    using filter_iterator = filter_iterator_struct<typename detail::advance_until<Iterator, End, Filter>::type, End, Filter>;

    template<typename Iterator, typename End, template<typename> typename Filter>
    auto dereference(filter_iterator_struct<Iterator, End, Filter>) -> dereference_t<Iterator>;

    template<typename Iterator, typename End, template<typename> typename Filter>
    auto advance(filter_iterator_struct<Iterator, End, Filter>) -> filter_iterator<advance_t<Iterator>, End, Filter>;

}


#endif