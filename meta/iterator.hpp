#ifndef META_ITERATOR_HPP__
#define META_ITERATOR_HPP__

#include <type_traits>

namespace meta {

    using std::size_t;

    template<typename Iterator>
    struct dereference;

    template<typename Iterator>
    using dereference_t = typename dereference<Iterator>::type;

    template<typename Iterator>
    struct advance;

    template<typename Iterator, size_t N>
    struct advance_n;

    template<typename Iterator>
    using advance_t = typename advance<Iterator>::type;

    template<typename Iterator, size_t N>
    using advance_n_t = typename advance_n<Iterator, N>::type;

    template<typename Iterator, size_t N>
    struct advance_n {
        using type = typename advance_n< advance_t<Iterator>, N-1 >::type;
    };

    template<typename Iterator>
    struct advance_n<Iterator, 0> {
        using type = Iterator;
    };

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
    struct dereference<filter_iterator_struct<Iterator, End, Filter>>
    {
        using type = dereference_t<Iterator>;
    };

    template<typename Iterator, typename End, template<typename> typename Filter>
    struct advance<filter_iterator_struct<Iterator, End, Filter>>
    {
        using type = filter_iterator_struct<advance_t<Iterator>, End, Filter>;
    };


    template<typename Begin, typename End>
    struct iterator_range
    {
        using begin = Begin;
        using end   = End;
    };

    template<typename T>
    struct range_begin;

    template<typename T>
    struct range_begin;

    template<typename IteratorRange, template<typename> typename Filter>
    using filter_range_t = iterator_range<
        filter_iterator<typename IteratorRange::begin, typename IteratorRange::end, Filter>,
        filter_iterator<typename IteratorRange::end,   typename IteratorRange::end, Filter>
    >;

    template<typename Begin, typename End>
    struct advance<iterator_range<Begin, End>>
    {
        using type = iterator_range<advance_t<Begin>, End>;
    };

    template<typename Begin, typename End>
    struct dereference<iterator_range<Begin, End>>
    {
        using type = dereference_t<Begin>;
    };

    template<typename Range>
    concept EmptyRange = std::is_same_v<typename Range::begin, typename Range::end>;

}


#endif