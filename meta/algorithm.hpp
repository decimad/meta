#ifndef META_ALGORITHM_HPP___
#define META_ALGORITHM_HPP___

#include <meta/iterator.hpp>
#include <meta/tlist.hpp>

namespace meta {

    //
    // accumulate
    // accumulate all elements of an iterator range into a meta::tlist<>
    //
    template<typename Range, typename Fields = tlist<>>
    struct accumulate {
        using type = typename accumulate<meta::advance_t<Range>, meta::type_list::push_back<Fields, meta::dereference_t<Range>>>::type;
    };

    template<meta::EmptyRange Range, typename Fields>
    struct accumulate<Range, Fields>
    {
        using type = Fields;
    };

    template<typename Range>
    using accumulate_t = typename accumulate<Range>::type;

}

#endif