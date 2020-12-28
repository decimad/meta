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
        are_same_v< // do not hover descend_left_t as this crashes clangd
            detail::descend_deep_left_t<
                tlist_tree_ctx,
                tlist<
                    test_tree<test_tree<leaf<0>, leaf<1>>, leaf<2>>
                >
            >,
            tlist<
                leaf<0>,
                detail::SE<test_tree<leaf<0>, leaf<1>>, 0>,
                detail::SE<test_tree<test_tree<leaf<0>, leaf<1>>, leaf<2>>, 0>
            >
        >
    );

    //
    // ascend_one_t
    //
    static_assert(
        are_same_v<
            detail::ascend_one_t<
                tlist<
                    leaf<0>,
                    detail::SE<test_tree<leaf<0>, leaf<1>>, 0>,
                    detail::SE<test_tree<test_tree<leaf<0>, leaf<1>>, leaf<2>>, 0>
                >
            >,
            tlist<
                test_tree<leaf<0>, leaf<1>>,
                detail::SE<test_tree<test_tree<leaf<0>, leaf<1>>, leaf<2>>, 0>
            >
        >
    );

    //
    // descend_one_t
    //
    static_assert(
        are_same_v<
            detail::descend_one_t<
                tlist_tree_ctx,
                tlist<
                    test_tree<leaf<0>, leaf<1>>,
                    detail::SE<test_tree<test_tree<leaf<0>, leaf<1>>, leaf<2>>, 0>
                >,
                0
            >,
            tlist<
                leaf<0>,
                detail::SE<test_tree<leaf<0>, leaf<1>>, 0>,
                detail::SE<test_tree<test_tree<leaf<0>, leaf<1>>, leaf<2>>, 0>
            >
        >
    );

    //
    // ascend_next_right_sibling
    //

    static_assert(
        are_same_v<
            typename detail::ascend_next_right_sibling<
                tlist_tree_ctx,
                tlist<
                    leaf<0>,
                    detail::SE<test_tree<leaf<0>, leaf<1>>, 0>,
                    detail::SE<test_tree<test_tree<leaf<0>, leaf<1>>, leaf<2>>, 0>
                >
            >::type,
            tlist<
                leaf<1>,
                detail::SE<test_tree<leaf<0>, leaf<1>>, 1>,
                detail::SE<test_tree<test_tree<leaf<0>, leaf<1>>, leaf<2>>, 0>
            >
        >
    );


    //
    // begin iterator
    //

    static_assert(
        are_same_v<
            dereference_t<
                tree_begin<
                    test_tree<
                        test_tree<
                            leaf<0>,
                            leaf<1>
                        >,
                        leaf<2>
                    >,
                    tlist_tree_ctx
                >
            >,

            test_tree<
                test_tree<
                    leaf<0>,
                    leaf<1>
                >,
                leaf<2>
            >
        >
    );

    //
    // advance iterator
    //

    using advance_test_tree =
        test_tree<
            test_tree<
                leaf<0>,
                leaf<1>
            >,
            leaf<2>
        >;

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, tlist_tree_ctx>, 0>>,
            advance_test_tree
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, tlist_tree_ctx>, 1>>,
            test_tree<leaf<0>, leaf<1>>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, tlist_tree_ctx>, 2>>,
            leaf<0>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, tlist_tree_ctx>, 3>>,
            leaf<1>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, tlist_tree_ctx>, 4>>,
            leaf<2>
        >
    );

    static_assert(
        are_same_v<
            advance_n_t<tree_begin<advance_test_tree, tlist_tree_ctx>, 5>,
            tree_end<advance_test_tree, tlist_tree_ctx>
        >
    );

    //
    // advance iterator range
    //

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_range_t<advance_test_tree, tlist_tree_ctx>, 0>>,
            advance_test_tree
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_range_t<advance_test_tree, tlist_tree_ctx>, 1>>,
            test_tree<leaf<0>, leaf<1>>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_range_t<advance_test_tree, tlist_tree_ctx>, 2>>,
            leaf<0>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_range_t<advance_test_tree, tlist_tree_ctx>, 3>>,
            leaf<1>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_range_t<advance_test_tree, tlist_tree_ctx>, 4>>,
            leaf<2>
        >
    );

    static_assert(
        are_same_v<
            advance_n_t<tree_range_t<advance_test_tree, tlist_tree_ctx>, 5>,
            iterator_range<tree_end<advance_test_tree, tlist_tree_ctx>, tree_end<advance_test_tree, tlist_tree_ctx>>
        >
    );

}
