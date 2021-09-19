#include <meta/tree.hpp>
#include <meta/tlist.hpp>
#include <meta/util.hpp>
#include <meta/algorithm.hpp>

namespace meta::verification {

    template<size_t Index>
    struct tlist_leaf {};

    // Type-List-Type tree for simplicity
    template<typename... Ts>
    struct tlist_tree : public range_lr<tlist_tree<Ts...>, contexts::tlist>
    {
    };

    template<size_t Index>
    struct intrusive_leaf {
        static constexpr size_t num_children = 0;
    };

    // Type-List-Type tree for simplicity
    template<typename... Ts>
    struct intrusive_tree /*: public range_lr<intrusive_tree<Ts...>, contexts::intrusive>*/
    {
        template<size_t Idx>
        using get_child = type_list::get<intrusive_tree, Idx>;

        static constexpr size_t num_children = type_list::size<intrusive_tree>;
    };

    //
    // descend_left
    //

    static_assert(
        are_same_v< // do not hover descend_left_t as this crashes clangd
            detail::descend_deep_left_t<
                contexts::tlist,
                tlist<
                    tlist_tree<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, tlist_leaf<2>>
                >
            >,
            tlist<
                tlist_leaf<0>,
                detail::SE<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, 0>,
                detail::SE<tlist_tree<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, tlist_leaf<2>>, 0>
            >
        >
    );

    static_assert(
        are_same_v< // do not hover descend_left_t as this crashes clangd
            detail::descend_deep_left_t<
                contexts::intrusive,
                tlist<
                    intrusive_tree<intrusive_tree<intrusive_leaf<0>, intrusive_leaf<1>>, intrusive_leaf<2>>
                >
            >,
            tlist<
                intrusive_leaf<0>,
                detail::SE<intrusive_tree<intrusive_leaf<0>, intrusive_leaf<1>>, 0>,
                detail::SE<intrusive_tree<intrusive_tree<intrusive_leaf<0>, intrusive_leaf<1>>, intrusive_leaf<2>>, 0>
            >
        >
    );

    //
    // ascend_one_t
    //
    static_assert(
        are_same_v<
            detail::ascend_one_t<
                contexts::tlist,
                tlist<
                    tlist_leaf<0>,
                    detail::SE<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, 0>,
                    detail::SE<tlist_tree<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, tlist_leaf<2>>, 0>
                >
            >,
            tlist<
                tlist_tree<tlist_leaf<0>, tlist_leaf<1>>,
                detail::SE<tlist_tree<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, tlist_leaf<2>>, 0>
            >
        >
    );

    //
    // descend_one_t
    //
    static_assert(
        are_same_v<
            detail::descend_one_t<
                contexts::tlist,
                tlist<
                    tlist_tree<tlist_leaf<0>, tlist_leaf<1>>,
                    detail::SE<tlist_tree<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, tlist_leaf<2>>, 0>
                >,
                0
            >,
            tlist<
                tlist_leaf<0>,
                detail::SE<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, 0>,
                detail::SE<tlist_tree<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, tlist_leaf<2>>, 0>
            >
        >
    );

    //
    // ascend_next_right_sibling
    //

    static_assert(
        are_same_v<
            detail::ascend_next_right_sibling_t<
                contexts::tlist,
                tlist<
                    tlist_leaf<0>,
                    detail::SE<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, 0>,
                    detail::SE<tlist_tree<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, tlist_leaf<2>>, 0>
                >
            >,
            tlist<
                tlist_leaf<1>,
                detail::SE<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, 1>,
                detail::SE<tlist_tree<tlist_tree<tlist_leaf<0>, tlist_leaf<1>>, tlist_leaf<2>>, 0>
            >
        >
    );

    // =============================
    // traversal 'traversal_nlr'
    // =============================

    //
    // begin iterator
    //

    static_assert(
        are_same_v<
            dereference_t<
                tree_begin<
                    tlist_tree<
                        tlist_tree<
                            tlist_leaf<0>,
                            tlist_leaf<1>
                        >,
                        tlist_leaf<2>
                    >,
                    contexts::tlist
                >
            >,

            tlist_tree<
                tlist_tree<
                    tlist_leaf<0>,
                    tlist_leaf<1>
                >,
                tlist_leaf<2>
            >
        >
    );

    //
    // advance iterator
    //

    using advance_test_tree =
        tlist_tree<
            tlist_tree<
                tlist_leaf<0>,
                tlist_leaf<1>
            >,
            tlist_leaf<2>
        >;

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, contexts::tlist>, 0>>,
            advance_test_tree
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, contexts::tlist>, 1>>,
            tlist_tree<tlist_leaf<0>, tlist_leaf<1>>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, contexts::tlist>, 2>>,
            tlist_leaf<0>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, contexts::tlist>, 3>>,
            tlist_leaf<1>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, contexts::tlist>, 4>>,
            tlist_leaf<2>
        >
    );

    static_assert(
        are_same_v<
            advance_n_t<tree_begin<advance_test_tree, contexts::tlist>, 5>,
            tree_end<advance_test_tree, contexts::tlist>
        >
    );

    //
    // advance iterator range
    //

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_range_t<advance_test_tree, contexts::tlist>, 0>>,
            advance_test_tree
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_range_t<advance_test_tree, contexts::tlist>, 1>>,
            tlist_tree<tlist_leaf<0>, tlist_leaf<1>>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_range_t<advance_test_tree, contexts::tlist>, 2>>,
            tlist_leaf<0>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_range_t<advance_test_tree, contexts::tlist>, 3>>,
            tlist_leaf<1>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_range_t<advance_test_tree, contexts::tlist>, 4>>,
            tlist_leaf<2>
        >
    );

    static_assert(
        are_same_v<
            advance_n_t<tree_range_t<advance_test_tree, contexts::tlist>, 5>,
            iterator_range<tree_end<advance_test_tree, contexts::tlist>, tree_end<advance_test_tree, contexts::tlist>>
        >
    );

    // ================================================
    // traversal 'traversal_lr' (ie. iterate on leafs)
    // ================================================

    //
    // begin iterator
    //

    static_assert(
        are_same_v<
            dereference_t<
                tree_begin<
                    tlist_tree<
                        tlist_tree<
                            tlist_leaf<0>,
                            tlist_leaf<1>
                        >,
                        tlist_leaf<2>
                    >,
                    contexts::tlist,
                    traversals::lr
                >
            >,

            tlist_leaf<0>
        >
    );

    //
    // advance iterator
    //

    using advance_test_tree =
        tlist_tree<
            tlist_tree<
                tlist_leaf<0>,
                tlist_leaf<1>
            >,
            tlist_leaf<2>
        >;

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, contexts::tlist, traversals::lr>, 0>>,
            tlist_leaf<0>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, contexts::tlist, traversals::lr>, 1>>,
            tlist_leaf<1>
        >
    );

    static_assert(
        are_same_v<
            dereference_t<advance_n_t<tree_begin<advance_test_tree, contexts::tlist, traversals::lr>, 2>>,
            tlist_leaf<2>
        >
    );

    static_assert(
        are_same_v<
            advance_n_t<tree_begin<advance_test_tree, contexts::tlist, traversals::lr>, 3>,
            tree_end<advance_test_tree, contexts::tlist, traversals::lr>
        >
    );

    //
    // range interface for trees shall be possible
    //

    static_assert(
        concepts::Range<
            tlist_tree<
                tlist_tree<
                    tlist_leaf<0>,
                    tlist_leaf<1>
                >,
                tlist_leaf<2>
            >
        >
    );

    static_assert(
        are_same_v<
            dereference_t<
                begin_t<
                    tlist_tree<
                        tlist_tree<
                            tlist_leaf<0>,
                            tlist_leaf<1>
                        >,
                        tlist_leaf<2>
                    >
                >
            >,
            tlist_leaf<0>
        >
    );

    template<concepts::Range Range>
    struct test {
        static constexpr bool empty = false;
    };

    template<concepts::EmptyRange Range>
    struct test<Range> {
        static constexpr bool empty = true;
    };

    constexpr bool is_empty = test<tlist_tree<
                    tlist_tree<
                        tlist_leaf<0>,
                        tlist_leaf<1>
                    >,
                    tlist_leaf<2>
                >>::empty;
    // If the iterators are of different type
    static_assert(
        !are_same_v<
            begin_t<tlist_tree<tlist_tree<tlist_leaf<0>,tlist_leaf<1>>,tlist_leaf<2>>>,
            end_t<tlist_tree<tlist_tree<tlist_leaf<0>,tlist_leaf<1>>,tlist_leaf<2>>>
        >
    );

    // Then iterator_equal_v should return false
    static_assert(
        !iterator_equal_v<
            begin_t<tlist_tree<tlist_tree<tlist_leaf<0>,tlist_leaf<1>>,tlist_leaf<2>>>,
            end_t<tlist_tree<tlist_tree<tlist_leaf<0>,tlist_leaf<1>>,tlist_leaf<2>>>
        >
    );

    static_assert(
        are_same_v<
            accumulate_t<
                tlist_tree<
                    tlist_tree<
                        tlist_leaf<0>,
                        tlist_leaf<1>
                    >,
                    tlist_leaf<2>
                >
            >,
            meta::tlist<tlist_leaf<0>, tlist_leaf<1>, tlist_leaf<2>>
        >

    );

    static_assert(
        are_same_v<
            accumulate_t<
                range_lr<
                    intrusive_tree<
                        intrusive_tree<
                            intrusive_leaf<0>,
                            intrusive_leaf<1>
                        >,
                        intrusive_leaf<2>
                    >,
                    contexts::intrusive
                >
            >,
            meta::tlist<intrusive_leaf<0>, intrusive_leaf<1>, intrusive_leaf<2>>
        >
    );

}
