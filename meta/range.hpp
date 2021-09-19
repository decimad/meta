#ifndef META_RANGE_HPP___
#define META_RANGE_HPP___

#include "meta/iterator.hpp"

namespace meta {

    //
    //  iterator_range (also works as iterator)
    //

    namespace concepts {

        template<typename T>
        concept Range = requires (T) {
            typename T::begin;
            typename T::end;
        };

        template<typename T>
        concept EmptyRange = Range<T> && iterator_equal_v<typename T::begin, typename T::end>;

    }

    template<concepts::Iterator Begin, concepts::Iterator End>
    struct iterator_range
    {
        using dereference = dereference_t<Begin>;
        using advance     = iterator_range<advance_t<Begin>, End>;

        using begin = Begin;
        using end   = End;
    };

    template<concepts::Iterator Iter>
    struct iterator_range<Iter, Iter>
    {
        using begin = Iter;
        using end   = Iter;;
    };

    template<concepts::Range T>
    using begin_t = typename T::begin;

    template<concepts::Range T>
    using end_t   = typename T::end;

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
    auto dereference(transformed_range<T, Transformer>) -> typename Transformer<dereference_t<begin_t<T>> >::type;

    //
    // Filtered range
    //

    template<concepts::Range T, template<typename> typename Filter>
    struct filtered_range
    {};


}

#endif