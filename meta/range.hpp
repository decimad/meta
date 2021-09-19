#ifndef META_RANGE_HPP___
#define META_RANGE_HPP___

#include "meta/iterator.hpp"

namespace meta {

    template<typename T>
    using begin_t = decltype(begin(std::declval<T>()));

    template<typename T>
    using end_t = decltype(end(std::declval<T>()));

    namespace concepts {

        template<typename T>
        concept Range = requires (T) {
            typename begin_t<T>;
            typename end_t<T>;
        };

        template<typename T>
        concept EmptyRange = Range<T> && iterator_equal_v<begin_t<T>, end_t<T>>;

    }

    //
    //  iterator_range (also works as iterator)
    //

    template<concepts::Iterator Begin, concepts::Iterator End>
    struct iterator_range
    {
//        using begin = Begin;
//        using end   = End;
    };

    template<typename Begin, typename End>
    auto begin(iterator_range<Begin, End>) -> Begin;

    template<typename Begin, typename End>
    auto end(iterator_range<Begin, End>) -> End;

    template<typename Begin, typename End>
    auto dereference(iterator_range<Begin, End>) -> dereference_t<Begin>;

    template<typename Begin, typename End>
    auto advance(iterator_range<Begin, End>) -> iterator_range<advance_t<Begin>, End>;

    template<typename Begin, typename End>
    auto is_sentinel(iterator_range<Begin, End>) -> decltype(is_sentinel(std::declval<Begin>()));

    //
    // filter_range
    //

    template<concepts::Range Range, template<typename> typename Filter>
    using filter_range_t = iterator_range<
        filter_iterator<begin_t<Range>, end_t<Range>, Filter>,
        filter_iterator<end_t<Range>,   end_t<Range>, Filter>
    >;

    //
    // Transformed range
    //

    template<concepts::Range T, template<typename> typename Transformer>
    struct transformed_range : public T
    {
    };

    template<concepts::Range T, template<typename> typename Transformer>
    auto dereference(transformed_range<T, Transformer>) -> typename Transformer< dereference_t<begin_t<T>> >::type;

    //
    // Filtered range
    //

    template<concepts::Range T, template<typename> typename Filter>
    struct filtered_range
    {};


}

#endif