#include <meta/tree.hpp>
#include <meta/tlist.hpp>
#include <meta/util.hpp>

namespace meta::verification {

    template<size_t Index>
    struct leaf {};

    // Type-List-Type tree for simplicity
    template<typename... Ts>
    struct test_tree
    {
        // can't use us on tree_iterator inside here, because the requires-clause becomes non-constants according to clang
    };

    //
    // descend_left
    //

    static_assert(
        meta::are_same_v<
            meta::detail::descend_left_t<meta::tlist_tree_ctx, test_tree<test_tree<leaf<0>, leaf<1>>, leaf<2>>>,
            meta::tlist<
                leaf<0>,
                meta::StackElement2<test_tree<leaf<0>, leaf<1>>, 0>,
                meta::StackElement2<test_tree<test_tree<leaf<0>, leaf<1>>, leaf<2>>, 0>
            >
        >
    );


}