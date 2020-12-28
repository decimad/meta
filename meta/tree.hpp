#ifndef META_TREE_HPP__
#define META_TREE_HPP__

#include <meta/tlist.hpp>
#include <meta/iterator.hpp>
#include <system_error>
#include <type_traits>

namespace meta {

    //
    // Contexts
    //
    // Contexts are objects that encapsulate the child access of tree nodes
    // Instead of requiring special members in each node of a tree, contexts allow to define the tree traversal on node elements
    // "non-intrusively". They define the mapping of "Node -> # childrens" and conditionally "Node -> Child-of-Node[Index]"".
    //
    // This way, f.e. tlist_tree_ctx can define a tree-like data structure where inner nodes are simple meta::tlist's, ie. a nested
    // type-list tree.
    //
    // The user may define custom tree contexts freely.
    //

    //
    // tlist_tree_ctx
    //
    // Define a class of trees being represented as nested type lists, all non-type-list nodes are treated as leafs.
    //
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

    //
    // intrusive_tree_ctx
    //
    // Define a class of trees being implemented intrusively, ie. their node types define the value member num_children and,
    // if num_children != 0, get_child to access the Nth child node.
    //
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

    // Alias template to get the number of children a node has
    template<typename Ctx, typename Node>
    static constexpr size_t node_num_children_v = Ctx::template num_children<Node>::value;;

    // Alias template to get the Nth child node of a node
    template<typename Ctx, typename Node, size_t Index>
    using node_get_child_t = typename Ctx::template get_child<Node, Index>::type;

    namespace concepts {
        template<typename Node, typename Ctx>
        concept InnerNode = (node_num_children_v<Ctx, Node> != 0) && std::is_convertible_v<decltype(node_num_children_v<Ctx, node_get_child_t<Ctx, Node, 0>>), const size_t>;

        template<typename Node, typename Ctx>
        concept LeafNode = node_num_children_v<Ctx, Node> == 0;

        template<typename Node, typename Ctx>
        concept TreeNode = InnerNode<Node, Ctx> || LeafNode<Node, Ctx>;
    }

    namespace detail {

        // StackElement (abbreviated SE to keep the type names short)
        template<typename Node, size_t Pos>
        struct SE
        {
            using node = Node;
            static constexpr size_t pos = Pos;
        };

        // All traversal algorithms work on a tree path represented as a TypeList representing a "stack", where the front element is
        // the top of the stack.
        // The top of the stack is a plain tree node represending the "Current" node.
        // The elements below the CurrentNode consist of pair of "Node" and "Position", where Node[Position] results in the Node of the pair
        // above or "CurrentNode", if that is the top of the stack.
        //
        // Example:
        //
        // meta::tlist<
        //    Current Node,
        //    SE<Parent of "CurrentNode", Index of "Current Node">,
        //    SE<Parent of "Parent of CurrentNode", Index of "Parent of CurrentNode">,
        //    SE<Root, Index of "Parent of "Parent of CurrentNode"">
        // >

        template<concepts::TypeList Stack>
        requires(type_list::size<Stack> > 1)
        using parent_node_t = typename meta::type_list::get<Stack, 1>::node;

        template<concepts::TypeList Stack>
        requires(type_list::size<Stack> > 1)
        static constexpr size_t top_node_pos_v = meta::type_list::get<Stack, 1>::pos;

        template<typename Ctx, concepts::TypeList Stack>
        requires(type_list::size<Stack> > 1)
        static constexpr size_t parent_num_children_v = node_num_children_v<Ctx, parent_node_t<Stack>>;

        //
        // has_left_sibling(_v)
        // check if the node addressed by the path stack has a right sibling
        //
        template<typename Ctx, concepts::TypeList Stack>
        struct has_left_sibling
        {
            static constexpr bool value = false;
        };

        template<typename Ctx, concepts::TypeList Stack>
        requires(type_list::size<Stack> > 1)
        struct has_left_sibling<Ctx, Stack>
        {
            static constexpr bool value = top_node_pos_v<Stack> > 0;
        };

        template<typename Ctx, concepts::TypeList Stack>
        static constexpr bool has_left_sibling_v = has_left_sibling<Ctx, Stack>::value;

        //
        // has_right_sibling(_v)
        // check if the node addressed by the path stack has a right sibling
        //
        template<typename Ctx, concepts::TypeList Stack>
        struct has_right_sibling
        {
            static constexpr bool value = false;
        };

        template<typename Ctx, concepts::TypeList Stack>
        requires(type_list::size<Stack> > 1)
        struct has_right_sibling<Ctx, Stack>
        {
            static constexpr bool value = (top_node_pos_v<Stack> + 1) < parent_num_children_v<Ctx, Stack>;
        };

        template<typename Ctx, concepts::TypeList Stack>
        static constexpr bool has_right_sibling_v = has_right_sibling<Ctx, Stack>::value;

        //
        // ascend_one(_t)
        //

        template<concepts::TypeList Stack, typename ParentElement = type_list::get<Stack, 1>>
        requires(type_list::size<Stack> > 1)
        struct ascend_one
        {
            // Remove the node and the StackElement containing the parent
            using type = type_list::set<type_list::pop_front<Stack>, 0, parent_node_t<Stack>>;
        };

        template<concepts::TypeList Stack>
        using ascend_one_t = typename ascend_one<Stack>::type;

        //
        // descend_one(_t)
        //

        template<typename Ctx, concepts::TypeList Stack, size_t Pos, concepts::InnerNode<Ctx> Top = type_list::front<Stack>>
        requires(Pos < node_num_children_v<Ctx, Top>)
        struct descend_one
        {
            using type = type_list::push_front<type_list::set<Stack, 0, SE<Top, Pos>>, node_get_child_t<Ctx, Top, Pos>>;
        };

        template<typename Ctx, concepts::TypeList Stack, size_t Pos>
        using descend_one_t = typename descend_one<Ctx, Stack, Pos>::type;

        //
        // descend_deep_left: beginning at some node N, take the left-most child until reaching a leaf.
        //
        template<typename Ctx, meta::concepts::TypeList Stack, concepts::TreeNode<Ctx> Top = meta::type_list::front<Stack>>
        struct descend_deep_left
        {
            using type = typename descend_deep_left<Ctx, descend_one_t<Ctx, Stack, 0>>::type;
        };

        template<typename Ctx, meta::concepts::TypeList Stack, concepts::LeafNode<Ctx> Top>
        struct descend_deep_left<Ctx, Stack, Top>
        {
            using type = Stack;
        };

        template<typename Ctx, meta::concepts::TypeList Stack>
        using descend_deep_left_t = typename descend_deep_left<Ctx, Stack, meta::type_list::front<Stack>>::type;

        //
        // descend_deep_right: beginning at some node N, take the right-most child until reaching a leaf.
        //
        template<typename Ctx, meta::concepts::TypeList Stack, concepts::TreeNode<Ctx> Top = type_list::front<Stack>>
        struct descend_deep_right
        {
            using type = typename descend_deep_right<Ctx, descend_one_t<Ctx, Stack, node_num_children_v<Ctx, Top> - 1>>::type;
        };

        template<typename Ctx, meta::concepts::TypeList Stack, concepts::LeafNode<Ctx> Top>
        requires(concepts::LeafNode<Ctx, Top>)
        struct descend_deep_right<Ctx, Stack, Top>
        {
            using type = Stack;
        };

        template<typename Ctx, concepts::TreeNode<Ctx> Node>
        using descend_deep_right_t = typename descend_deep_right<Ctx, meta::tlist<Node>>::type;

        //
        // ascend node stack upwards until we find a right sibling and then switch to that right sibling
        //

        // PS: Inner node, no right sibling
        template<typename Ctx, concepts::TypeList Stack, size_t Size = type_list::size<Stack>>
        struct ascend_next_right_sibling
        {
            using type = typename ascend_next_right_sibling<Ctx, ascend_one_t<Stack>>::type;
        };

        // PS: Inner node, right sibling
        template<typename Ctx, concepts::TypeList Stack, size_t Size>   // Inner node, right sibling
        requires(has_right_sibling_v<Ctx, Stack>)
        struct ascend_next_right_sibling<Ctx, Stack, Size>
        {
            using type = descend_one_t<Ctx, ascend_one_t<Stack>, top_node_pos_v<Stack> + 1>;
        };

        // PS: root
        template<typename Ctx, concepts::TypeList Stack>
        struct ascend_next_right_sibling<Ctx, Stack, 1>                 // Root node has no right sibling
        {
            using type = meta::tlist<>;
        };

        template<typename Ctx, concepts::TypeList Stack>
        using ascend_next_right_sibling_t = typename ascend_next_right_sibling<Ctx, Stack>::type;

        // ==========
        // traversals
        // ==========


        struct traversal_pre_lr
        {
            template<typename Ctx, concepts::TreeNode<Ctx> Root>
            struct begin {
                using type = meta::tlist<Root>;
            };

            template<typename Ctx, concepts::TreeNode<Ctx> Root>
            struct end {
                using type = meta::tlist<>;
            };

            // Inner node on top of stack
            template<typename Ctx, concepts::TypeList Stack, concepts::TreeNode<Ctx> Top = type_list::front<Stack>>
            struct advance {
                using type = descend_one_t<Ctx, Stack, 0>;
            };

            // Leaf node on top of stack -> find next (ancestor) sibling, then descend-left
            template<typename Ctx, concepts::TypeList Stack, concepts::LeafNode<Ctx> Top>
            struct advance<Ctx, Stack, Top>
            {
                using type = ascend_next_right_sibling_t<Ctx, Stack>;
            };
        };

    }

    template<typename Ctx, concepts::TypeList Stack, typename Traversal>
    requires(concepts::TreeNode<type_list::front<Stack>, Ctx> || type_list::is_empty<Stack>)
    struct tree_iterator
    {
    };

    template<typename Root, typename Ctx = intrusive_tree_ctx, typename Traversal = detail::traversal_pre_lr>
    requires(concepts::TreeNode<Root, Ctx>)
    using tree_begin = tree_iterator<Ctx, typename Traversal::template begin<Ctx, Root>::type, Traversal>;

    template<typename Root, typename Ctx = intrusive_tree_ctx, typename Traversal = detail::traversal_pre_lr>
    requires(concepts::TreeNode<Root, Ctx>)
    using tree_end   = tree_iterator<Ctx, typename Traversal::template end<Ctx, Root>::type, Traversal>;

    template<typename Interator>
    struct tree_context;

    template<typename Ctx, concepts::TypeList Stack, typename Traversal>
    struct tree_context<tree_iterator<Ctx, Stack, Traversal>>
    {
        using type = Ctx;
    };

    template<typename Iterator>
    using tree_context_t = typename tree_context<Iterator>::type;

    //
    // dereference(it)
    //

    template<typename Ctx, concepts::TypeList Stack, typename Traversal>
    struct dereference<tree_iterator<Ctx, Stack, Traversal>>
    {
        using type = type_list::front<Stack>;
    };

    //
    // advance(it)
    //

    template<typename Ctx, concepts::TypeList Stack, typename Traversal>
    struct advance<tree_iterator<Ctx, Stack, Traversal>>
    {
        using type = tree_iterator<Ctx, typename Traversal::template advance<Ctx, Stack>::type, Traversal>;
    };

    //
    // Tree range
    //

    template<typename Root, typename Ctx = intrusive_tree_ctx, typename Traversal = detail::traversal_pre_lr>
    using tree_range_t = iterator_range<
        tree_begin<Root, Ctx, Traversal>,
        tree_end  <Root, Ctx, Traversal>
    >;

}

#endif
