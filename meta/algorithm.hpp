#ifndef META_ALGORITHM_HPP___
#define META_ALGORITHM_HPP___

#include <meta/iterator.hpp>
#include <meta/range.hpp>
#include <meta/tlist.hpp>

namespace meta {

    //
    // accumulate
    // accumulate all elements of an iterator range into a type list (meta::tlist<> per default)
    //
    template<concepts::Range Range, typename Fields = tlist<>>
    struct accumulate {
        using type = typename accumulate<
            meta::advance_t<iterator_range<begin_t<Range>, end_t<Range>>>,
            meta::type_list::push_back<Fields, meta::dereference_t<begin_t<Range>>>
        >::type;
    };

    template<concepts::EmptyRange Range, typename Fields>
    struct accumulate<Range, Fields>
    {
        using type = Fields;
    };

    template<typename Range>
    using accumulate_t = typename accumulate<Range>::type;

}

#endif