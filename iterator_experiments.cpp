#include <type_traits>
#include <cstddef>

template<typename T = void>
struct iterator_sentinel
{
};

template<typename Iter>
struct IsSentinel {
    static constexpr bool value = false;
};

template<typename Iter>
struct IsSentinel<iterator_sentinel<Iter>> {
    static constexpr bool value = true;
};

namespace concepts {

    template<typename Iter>
    concept ValidForwardIterator = requires(Iter) {
        dereference(std::declval<Iter>());
        advance(std::declval<Iter>());
    };

    template<typename Iter>
    concept ForwardIterator = ValidForwardIterator<Iter> || IsSentinel<Iter>::value;
}

template<concepts::ValidForwardIterator Iter>
using advance_t = decltype(advance(std::declval<Iter>()));

template<concepts::ValidForwardIterator Iter>
using dereference_t = decltype(dereference(std::declval<Iter>()));


template<size_t Value>
struct my_iterator {};

template<size_t Value>
my_iterator<1 / (10 - Value)> advance(my_iterator<Value>);

iterator_sentinel<my_iterator<9>> advance(my_iterator<9>);

template<size_t Value>
my_iterator<Value> dereference(my_iterator<Value>);


static_assert(std::is_same_v<advance_t<my_iterator<9>>, iterator_sentinel<my_iterator<9>>>, "");

