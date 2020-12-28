#ifndef META_TREE_HPP__
#define META_TREE_HPP__

#include <meta/tlist.hpp>
#include <meta/iterator.hpp>
#include <type_traits>

namespace meta {

    enum class traversal {
        pre_lr,  // node, then children left-to-right
        pre_rl,  // node, then children right-to-left
        post_lr, // children left-to-right, then node
        post_rl  // children right-to-left, then node
    };

    struct tlist_tree_ctx {

        template<typename T>
        struct num_children {
            static constexpr size_t value = 0;
        };

        template<meta::concepts::TypeList T>
        struct num_children<T>
        {
            static constexpr size_t value = meta::type_list::size<T>;
        };

        template<typename T, size_t Index>
        struct get_child;

        template<meta::concepts::TypeList T, size_t Index>
        struct get_child<T, Index>
        {
            using type = meta::type_list::get<T, Index>;
        };

    };

    struct intrusive_tree_ctx {

        template<typename T>
        struct num_children {
            static constexpr size_t value = T::num_children;
        };

        template<typename T, size_t Index>
        struct get_child
        {
            // this will be evaluated iff num_children != 0
            using type = typename T::template get_child<Index>;
        };

    };


    template<typename Ctx, typename Node>
    static constexpr size_t node_num_children_v = Ctx::template num_children<Node>::value;;

    template<typename Ctx, typename Node, size_t Index>
    using node_get_child_t = typename Ctx::template get_child<Node, Index>::type;


    template<typename Ctx, typename Node, size_t Pos>
    struct StackElement
    {
        using node = Node;
        static constexpr size_t pos = Pos;

        using dereference = node_get_child_t<Ctx, Node, pos>;
    };

    template<typename Node, size_t Pos>
    struct StackElement2
    {
        using node = Node;
        static constexpr size_t pos = Pos;
    };

    namespace detail {

        struct begin_type {
            static constexpr size_t num_children = 0;
        };

        struct end_type {
            static constexpr size_t num_children = 0;
        };

    }

    namespace concepts {
        template<typename Ctx, typename Node>
        concept InnerNode = (node_num_children_v<Ctx, Node> != 0) && std::is_convertible_v<decltype(node_num_children_v<Ctx, node_get_child_t<Ctx, Node, 0>>), const size_t>;

        template<typename Ctx, typename Node>
        concept LeafNode = node_num_children_v<Ctx, Node> == 0;

        template<typename Ctx, typename Node>
        concept Sentinel = std::is_same_v<Node, meta::detail::end_type>;
    }

    namespace detail {

        // Sample traversal stack:
        // meta::tlist<
        //    StackElement<Root,     0>,
        //    StackElement<child_l1, 0>,
        //    StackElement<child_l2, 0>,
        //    child3
        // >

        template<typename Ctx, meta::concepts::TypeList Stack, typename Top = meta::type_list::front<Stack>>
        requires(concepts::InnerNode<Ctx, Top> || concepts::LeafNode<Ctx, Top>)
        struct descend_left
        {
            using type = typename descend_left<Ctx, meta::type_list::push_front<meta::type_list::set<Stack, 0, StackElement2<Top, 0>>, node_get_child_t<Ctx, Top, 0>>>::type;
        };

        template<typename Ctx, meta::concepts::TypeList Stack, typename Top>
        requires(concepts::LeafNode<Ctx, Top>)
        struct descend_left<Ctx, Stack, Top>
        {
            using type = Stack;
        };

        template<typename Ctx, typename Node>
        requires(concepts::InnerNode<Ctx, Node> || concepts::LeafNode<Ctx, Node>)
        using descend_left_t = typename descend_left<Ctx, meta::tlist<Node>>::type;


        template<typename Ctx, meta::concepts::TypeList Stack, typename Top = meta::type_list::front<Stack>>
        requires(concepts::InnerNode<Ctx, Top> || concepts::LeafNode<Ctx, Top>)
        struct descend_right
        {
            using type = typename descend_right<Ctx, meta::type_list::push_front<meta::type_list::set<Stack, 0, StackElement2<Top, node_num_children_v<Ctx, Top>-1>>, node_get_child_t<Ctx, Top, node_num_children_v<Ctx, Top>-1>>>::type;
        };

        template<typename Ctx, meta::concepts::TypeList Stack, typename Top>
        requires(concepts::LeafNode<Ctx, Top>)
        struct descend_right<Ctx, Stack, Top>
        {
            using type = Stack;
        };

        template<typename Ctx, typename Node>
        requires(concepts::InnerNode<Ctx, Node> || concepts::LeafNode<Ctx, Node>)
        using descend_right_t = typename descend_right<Ctx, meta::tlist<Node>>::type;


        template<typename Ctx, meta::concepts::TypeList Stack, bool Empty = meta::type_list::is_empty<Stack>>
        struct walk_stack_right_helper
        {
            using front = meta::type_list::front<Stack>;    // StackElement!

            using type = std::conditional_t<(node_num_children_v<Ctx, typename front::node> > (front::pos + 1)),
                // Replace top
                meta::type_list::push_front<meta::type_list::pop_front<Stack>, StackElement<Ctx, typename front::node, front::pos+1>>,
                // Pop top, continue
                typename walk_stack_right_helper<Ctx, meta::type_list::pop_front<Stack>>::type
            >;
        };

        template<typename Ctx, meta::concepts::TypeList Stack>
        struct walk_stack_right_helper<Ctx, Stack, true> {
            using type = Stack;
        };

        template<typename Ctx, meta::concepts::TypeList Stack, meta::concepts::TypeList NewStack = typename walk_stack_right_helper<Ctx, Stack>::type, bool NewStackEmpty = meta::type_list::is_empty<NewStack>>
        struct walk_stack_right {
            using stack = NewStack;
            using node  = typename meta::type_list::front<NewStack>::dereference;
        };

        template<typename Ctx, meta::concepts::TypeList Stack, meta::concepts::TypeList NewStack>
        struct walk_stack_right<Ctx, Stack, NewStack, true>
        {
            using stack = tlist<>;
            using node  = end_type;
        };
    }



    template<typename Tree, typename Node, typename Ctx = intrusive_tree_ctx, meta::concepts::TypeList PathStack = meta::tlist<>>
    requires( concepts::InnerNode<Ctx, Node> || concepts::LeafNode<Ctx, Node> || concepts::Sentinel<Ctx, Node> )
    struct tree_iterator
    {
    };

    template<typename Tree, typename Ctx = intrusive_tree_ctx>
    using tree_begin = tree_iterator<Tree, Tree, Ctx>;

    template<typename Tree, typename Ctx = intrusive_tree_ctx>
    using tree_end   = tree_iterator<Tree, detail::end_type, Ctx, meta::tlist<>>;

    template<typename Interator>
    struct tree_context;

    template<typename Tree, typename Node, typename Ctx, typename Stack>
    struct tree_context<tree_iterator<Tree, Node, Ctx, Stack>>
    {
        using type = Ctx;
    };

    template<typename Iterator>
    using tree_context_t = typename tree_context<Iterator>::type;

    // dereference(it)
    //

    template<typename Tree, typename Node, typename Ctx, typename Stack>
    struct dereference<tree_iterator<Tree, Node, Ctx, Stack>>
    {
        using type = Node;
    };

    namespace detail {

        template<typename Tree, typename Node, typename Ctx, typename Stack, size_t NumChildern>
        struct advance_tree {
            using type = tree_iterator<Tree, node_get_child_t<Ctx, Node, 0>, Ctx>;
        };

        template<typename Tree, typename Node, typename Ctx, typename Stack>
        struct advance_tree<Tree, Node, Ctx, Stack, 0>
        {
            using type = tree_iterator<Tree, typename detail::walk_stack_right<Ctx, Stack>::node, Ctx, typename detail::walk_stack_right<Ctx, Stack>::stack >;
        };

    }

    // advance(it)
    //
    template<typename Tree, typename Node, typename Ctx, typename Stack>
    struct advance<tree_iterator<Tree, Node, Ctx, Stack>>
    {
        using type = typename detail::advance_tree<Tree, Node, Ctx, Stack, node_num_children_v<Ctx, Node>>::type;
    };

    //
    //  tree(it)
    //
    template<typename TreeIterator>
    struct tree;

    template<typename Tree, typename Node, typename Ctx, typename Stack>
    struct tree<tree_iterator<Tree, Node, Ctx, Stack>>
    {
        using type = Tree;
    };

    template<typename TreeIterator>
    using tree_t = typename tree<TreeIterator>::type;

    //
    // node(it)
    //
    template<typename TreeIterator>
    struct node;

    template<typename Tree, typename Node, typename Ctx, typename Stack>
    struct node<tree_iterator<Tree, Node, Ctx, Stack>>
    {
        using type = Node;
    };

    template<typename TreeIterator>
    using node_t = typename node<TreeIterator>::type;

    //
    // Tree range
    //
    template<typename Tree, typename Ctx = intrusive_tree_ctx>
    using tree_range_t = iterator_range<
        tree_begin<Tree, Ctx>,
        tree_end<Tree, Ctx>
    >;

}

#endif
