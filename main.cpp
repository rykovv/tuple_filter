#include <type_traits>
#include <tuple>
#include <iostream>


template <typename ...Is>
struct index_sequence_concat;

template <std::size_t... Ls>
struct index_sequence_concat<std::index_sequence<Ls...>> {
    using type = std::index_sequence<Ls...>;
};

template <std::size_t... Ls, std::size_t... Rs>
struct index_sequence_concat<std::index_sequence<Ls...>, std::index_sequence<Rs...>> {
    using type = std::index_sequence<Ls..., Rs...>;
};

template <typename Is0, typename Is1, typename ...Is>
struct index_sequence_concat<Is0, Is1, Is...> {
    using type = typename index_sequence_concat<
        typename index_sequence_concat<Is0, Is1>::type, Is...>
        ::type;
};

template <bool B, std::size_t I>
struct conditional_index_sequence {
    using type = std::index_sequence<>;
};

template <std::size_t I>
struct conditional_index_sequence<true, I> {
    using type = std::index_sequence<I>;
};

template <bool B, std::size_t I>
using conditional_index_sequence_t = typename conditional_index_sequence<B, I>::type;

template <template <typename> class Predicate, typename Tuple, std::size_t... Is>
constexpr auto filtered_index_sequence_helper(const Tuple& tuple, std::index_sequence<Is...>) {
    using r = typename index_sequence_concat<
        conditional_index_sequence_t<
            Predicate<std::tuple_element_t<Is, Tuple>>::value,
            Is
        >...
    >::type;
    return r{};
}

template <template <typename> class Predicate, typename Tuple>
constexpr auto filtered_index_sequence(const Tuple& tuple) {
    return filtered_index_sequence_helper<Predicate>(tuple, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
}

template <typename Tuple, std::size_t... Is>
constexpr auto tuple_filter_helper(const Tuple& tuple, std::index_sequence<Is...>) {
    return std::make_tuple(std::get<Is>(tuple)...);
}

template <template <typename> class Predicate, typename Tuple>
auto tuple_filter(const Tuple& tuple) {
    return tuple_filter_helper(tuple, filtered_index_sequence<Predicate>(tuple));
}


template<typename T, T... ints>
void print_sequence(std::integer_sequence<T, ints...> int_seq)
{
    std::cout << "The sequence of size " << int_seq.size() << ": ";
    ((std::cout << ints << ' '), ...);
    std::cout << '\n';
}

int main() {
    auto tuple = std::make_tuple(1, 2.0, 3, "hello", 4.5, 5);
    auto filteredTuple = tuple_filter<std::is_integral>(tuple);

    // Print the filtered tuple
    std::apply([](auto&&... args) {
        ((std::cout << args << " "), ...);
    }, filteredTuple);

    return 0;
}
