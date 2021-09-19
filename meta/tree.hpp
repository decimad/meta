#ifndef META_TREE_HPP__
#define META_TREE_HPP__

#include <meta/tlist.hpp>
#include <meta/iterator.hpp>
#include <meta/range.hpp>
#include <system_error>
#include <type_traits>

namespace meta {

    namespace contexts {

        //
        // Contexts
        //
        // Contexts are objects that encapsulate the child access of tree nodes
        // Instead of requiring special members in each node of a tree, contexts allow to define the tree traversal on node elements
        // "non-intrusively". They define the mapping of "Node -> # childrens" and conditionally "Node -> Child-of-Node[Index]"".
        //
        // This way, f.e. contexts::tlist can define a tree-like data structure where inner nodes are simple meta::tlist's, ie. a nested
        // type-list tree.
        //
        // The user may define additional custom tree contexts as required.
        //

        //
        // tlist
        //
        // Define a class of trees being represented as nested type lists, all non-tlist nodes are treated as leafs.
        //
        struct tlist {

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
        // intrusive
        //
        // Define a class of trees being implemented intrusively, ie. their node types define the value member num_children and,
        // if num_children != 0, get_child to access the Nth child node.
        //

        namespace intrusive_detail {
            template<typename T>
            struct num_children {
                static constexpr size_t value = T::num_children;
            };
        }

        struct intrusive {

            // Need explicit spezialization which is not allowed at class scope
            template<typename T>
            using num_children = intrusive_detail::num_children<T>;

            template<typename T, size_t Index>
            struct get_child
            {
                // this will be evaluated iff num_children != 0
                using type = typename T::template get_child<Index>;
            };

        };

    }

    namespace concepts {
        template<typename Node, typename Ctx>
        concept InnerNode = Ctx::template num_children<Node>::value != 0;

        template<typename Node, typename Ctx>
        concept LeafNode = Ctx::template num_children<Node>::value == 0;

        template<typename Node, typename Ctx>
        concept TreeNode = LeafNode<Node, Ctx> || InnerNode<Node, Ctx>;
    }


    // Alias template to get the number of children a node has
    template<typename Ctx, concepts::TreeNode<Ctx> Node>
    static constexpr size_t node_num_children_v = Ctx::template num_children<Node>::value;

    // Alias template to get the Nth child node of a node
    template<typename Ctx, concepts::InnerNode<Ctx> Node, size_t Index>
    using node_get_child_t = typename Ctx::template get_child<Node, Index>::type;

    namespace detail {

        // StackElement (abbreviated SE to keep the type names short)
        template<typename Node, size_t Pos>
        struct SE
        {
            using node = Node;
            static constexpr size_t pos = Pos;
        };

        template<typename Stack, typename Ctx>
        concept PathStack =
            meta::concepts::TypeList<Stack> &&
            (type_list::size<Stack> > 0) &&
            (concepts::TreeNode<type_list::front<Stack>, Ctx>);

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
        //
        // Note that the 'empty' Stack, is represented by meta::tlist<meta::sentinel, ...> as a fixed point for all traversal meta-functions,
        // so that the traversal algorithm does not need to treat the end of iteration specially.
        //

        template<typename Ctx, PathStack<Ctx> Stack>
        requires(type_list::size<Stack> > 1)
        using parent_node_t = typename meta::type_list::get<Stack, 1>::node;

        template<typename Ctx, PathStack<Ctx> Stack>
        requires(type_list::size<Stack> > 1)
        static constexpr size_t top_node_pos_v = meta::type_list::get<Stack, 1>::pos;

        template<typename Ctx, PathStack<Ctx> Stack>
        requires(type_list::size<Stack> > 1)
        static constexpr size_t parent_num_children_v = node_num_children_v<Ctx, parent_node_t<Ctx, Stack>>;

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
            static constexpr bool value = top_node_pos_v<Ctx, Stack> > 0;
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
            static constexpr bool value = (top_node_pos_v<Ctx, Stack> + 1) < parent_num_children_v<Ctx, Stack>;
        };

        template<typename Ctx, concepts::TypeList Stack>
        static constexpr bool has_right_sibling_v = has_right_sibling<Ctx, Stack>::value;

        //
        // ascend_one(_t)
        //

        template<typename Ctx, PathStack<Ctx> Stack, typename ParentElement = type_list::get<Stack, 1>>
        requires(type_list::size<Stack> > 1)
        struct ascend_one
        {
            // Remove the node and the StackElement containing the parent
            using type = type_list::set<type_list::pop_front<Stack>, 0, parent_node_t<Ctx, Stack>>;
        };

        template<typename Ctx, PathStack<Ctx> Stack>
        using ascend_one_t = typename ascend_one<Ctx, Stack>::type;

        //
        // descend_one(_t)
        //

        template<typename Ctx, concepts::TypeList Stack, size_t Pos, concepts::InnerNode<Ctx> Top = type_list::front<Stack>>
        requires(Pos < node_num_children_v<Ctx, Top>)
        struct descend_one
        {
            using type = type_list::push_front<type_list::set<Stack, 0, SE<Top, Pos>>, node_get_child_t<Ctx, Top, Pos>>;
        };

        template<typename Ctx, PathStack<Ctx> Stack, size_t Pos>
        using descend_one_t = typename descend_one<Ctx, Stack, Pos>::type;

        //
        // descend_deep_left: beginning at some node N, take the left-most child until reaching a leaf.
        //
        template<typename Ctx, PathStack<Ctx> Stack, concepts::TreeNode<Ctx> Top = meta::type_list::front<Stack>>
        struct descend_deep_left
        {
            using type = typename descend_deep_left<Ctx, descend_one_t<Ctx, Stack, 0>>::type;
        };

        template<typename Ctx, PathStack<Ctx> Stack, concepts::LeafNode<Ctx> Top>
        struct descend_deep_left<Ctx, Stack, Top>
        {
            using type = Stack;
        };

/*
        template<typename Ctx>
        struct descend_deep_left<Ctx, meta::tlist<sentinel>, sentinel>
        {
            using type = meta::tlist<sentinel>;
        };
*/

        template<typename Ctx, PathStack<Ctx> Stack>
        using descend_deep_left_t = typename descend_deep_left<Ctx, Stack>::type;

        //
        // descend_deep_right: beginning at some node N, take the right-most child until reaching a leaf.
        //
        template<typename Ctx, PathStack<Ctx> Stack, concepts::TreeNode<Ctx> Top = type_list::front<Stack>>
        struct descend_deep_right
        {
            using type = typename descend_deep_right<Ctx, descend_one_t<Ctx, Stack, node_num_children_v<Ctx, Top> - 1>>::type;
        };

        template<typename Ctx, PathStack<Ctx> Stack, concepts::LeafNode<Ctx> Top>
        struct descend_deep_right<Ctx, Stack, Top>
        {
            using type = Stack;
        };

/*
        template<typename Ctx>
        struct descend_deep_right<Ctx, meta::tlist<sentinel>, sentinel>
        {
            using type = meta::tlist<sentinel>;
        };
*/

        template<typename Ctx, PathStack<Ctx> Stack>
        using descend_deep_right_t = typename descend_deep_right<Ctx, Stack>::type;

        //
        // ascend node stack upwards until we find a right sibling and then switch to that right sibling
        //

        // PS: Inner node, no right sibling
        template<typename Ctx, PathStack<Ctx> Stack, size_t Size = type_list::size<Stack>>
        struct ascend_next_right_sibling
        {
            using type = typename ascend_next_right_sibling<Ctx, ascend_one_t<Ctx, Stack>>::type;
        };

        // PS: Inner node, right sibling
        template<typename Ctx, PathStack<Ctx> Stack, size_t Size>   // Inner node, right sibling
        requires(has_right_sibling_v<Ctx, Stack>)
        struct ascend_next_right_sibling<Ctx, Stack, Size>
        {
            using type = descend_one_t<Ctx, ascend_one_t<Ctx, Stack>, top_node_pos_v<Ctx, Stack> + 1>;
        };

        // PS: root
        template<typename Ctx, PathStack<Ctx> Stack>
        struct ascend_next_right_sibling<Ctx, Stack, 1>
        {
            using type = sentinel<>;
        };

        template<typename Ctx, PathStack<Ctx> Stack>
        using ascend_next_right_sibling_t = typename ascend_next_right_sibling<Ctx, Stack>::type;

    }

    namespace traversals {

        // ==========
        // traversals
        // ==========

        struct nlr
        {
            template<typename Ctx, concepts::TreeNode<Ctx> Root>
            struct begin {
                using type = meta::tlist<Root>;
            };

            template<typename Ctx, concepts::TreeNode<Ctx> Root>
            struct end {
                using type = detail::descend_deep_right_t<Ctx, meta::tlist<Root>>;
            };

            // Inner node on top of stack
            template<typename Ctx, detail::PathStack<Ctx> Stack, concepts::TreeNode<Ctx> Top = type_list::front<Stack>>
            struct advance {
                using type = detail::descend_one_t<Ctx, Stack, 0>;
            };

            // Leaf node on top of stack -> find next (ancestor) sibling, then descend-left
            template<typename Ctx, detail::PathStack<Ctx> Stack, concepts::LeafNode<Ctx> Top>
            struct advance<Ctx, Stack, Top>
            {
                using type = detail::ascend_next_right_sibling_t<Ctx, Stack>;
            };
        };

        template<typename Ctx, typename Stack, template<typename _Ctx, typename _Stack> typename Func>
        struct apply_if_not_sentinel {
            using type = Func<Ctx, Stack>;
        };

        template<typename Ctx, template<typename _Ctx, typename _Stack> typename Func>
        struct apply_if_not_sentinel<Ctx, sentinel<>, Func> {
            using type = sentinel<>;
        };

        template<typename Ctx, typename Stack, template<typename _Ctx, typename _Stack> typename Func>
        using apply_if_not_sentinel_t = typename apply_if_not_sentinel<Ctx, Stack, Func>::type;


        // Leaf-only traversal left-to-right. Note that this could also be implemented with a filter_iterator using traversals like
        // traversal_lr, but leaf-only traversal and then performing computations on the path of the current element is a common use case.
        struct lr
        {
            template<typename Ctx, concepts::TreeNode<Ctx> Root>
            struct begin {
                using type = detail::descend_deep_left_t<Ctx, meta::tlist<Root>>;
            };

            template<typename Ctx, concepts::TreeNode<Ctx> Root>
            struct end {
                using type = detail::descend_deep_right_t<Ctx, meta::tlist<Root>>;
            };

            template<typename Ctx, detail::PathStack<Ctx> Stack>
            struct advance {
                using type = apply_if_not_sentinel_t<Ctx, detail::ascend_next_right_sibling_t<Ctx, Stack>, detail::descend_deep_left_t>;
            };
        };

    }

    template<typename Ctx, detail::PathStack<Ctx> Stack, typename Traversal>
    struct tree_iterator;

    namespace detail {
        template<typename Ctx, typename OriginalStack, typename NewStackResult, typename Traversal>
        struct tree_advance_switch
        {
            using type = tree_iterator<Ctx, NewStackResult, Traversal>;
        };

        template<typename Ctx, typename OriginalStack, typename Traversal>
        struct tree_advance_switch<Ctx, OriginalStack, sentinel<>, Traversal>
        {
            using type = sentinel<tree_iterator<Ctx, OriginalStack, Traversal>>;
        };
    }

    //
    // tree_iterator
    //
    template<typename Ctx, detail::PathStack<Ctx> Stack, typename Traversal>
    struct tree_iterator
    {
        using dereference = type_list::front<Stack>;
        using advance     = typename detail::tree_advance_switch<Ctx, Stack, typename Traversal::template advance<Ctx, Stack>::type, Traversal>::type;
    };

    template<typename T>
    concept HasType = requires()
    {
        typename T::type;
    };

    //
    // tree_begin
    //
    template<typename Root, typename Ctx = contexts::intrusive, typename Traversal = traversals::nlr>
    requires(concepts::TreeNode<Root, Ctx>)
    using tree_begin = tree_iterator<Ctx, typename Traversal::template begin<Ctx, Root>::type, Traversal>;

    //
    // tree_end
    //
    template<typename Root, typename Ctx = contexts::intrusive, typename Traversal = traversals::nlr>
    requires(concepts::TreeNode<Root, Ctx>)
    using tree_end = sentinel< tree_iterator<Ctx, typename Traversal::template end<Ctx, Root>::type, Traversal> >;

    //
    // tree_enumerator
    //
    template<typename Root, typename Ctx = contexts::intrusive, typename Traversal = traversals::nlr>
    requires(concepts::TreeNode<Root, Ctx>)
    using tree_enumerator = tree_iterator<Ctx, typename Traversal::template begin<Ctx, Root>::type, Traversal>;

    //
    // traversal ranges
    //
    template<typename Root, typename Ctx = contexts::intrusive, typename Traversal = traversals::nlr>
    using tree_range_t = iterator_range<
        tree_begin<Root, Ctx, Traversal>,
        tree_end  <Root, Ctx, Traversal>
    >;

    template<typename Root, typename Ctx = contexts::intrusive>
    using range_lr  = tree_range_t<Root, Ctx, traversals::lr>;

    template<typename Root, typename Ctx = contexts::intrusive>
    using range_nlr = tree_range_t<Root, Ctx, traversals::nlr>;

    //
    // tree_context
    //
    template<typename Interator>
    struct tree_context;

    template<typename Ctx, concepts::TypeList Stack, typename Traversal>
    struct tree_context<tree_iterator<Ctx, Stack, Traversal>>
    {
        using type = Ctx;
    };

    template<typename Iterator>
    using tree_context_t = typename tree_context<Iterator>::type;

    namespace detail {
        template<typename T>
        struct strip_se
        {
            using type = T;
        };

        template<typename Node, size_t Pos>
        struct strip_se<SE<Node, Pos>>
        {
            using type = Node;
        };
    }

    //
    // The path of an iterator (ie. all the nodes on the path from the root of the tree up to the current element)
    //

    template<typename Ctx, concepts::TypeList Stack, typename Traversal>
    type_list::reverse<type_list::transform<Stack, detail::strip_se>> path(tree_iterator<Ctx, Stack, Traversal>);

    template<typename Iterator>
    using path_t = decltype(path(std::declval<Iterator>()));

    template<typename Iterator>
    using path_enumerator_t = begin_t<path_t<Iterator>>;

    namespace concepts {

        template<typename Iter>
        concept TreeIterator = Iterator<Iter> && (!ValidIterator<Iter> || requires {
            typename path_t<Iter>;
        });

    }

    template<concepts::TreeIterator Begin, concepts::TreeIterator End>
    auto path(meta::iterator_range<Begin, End>) -> decltype(path(std::declval<Begin>()));

}

#endif
