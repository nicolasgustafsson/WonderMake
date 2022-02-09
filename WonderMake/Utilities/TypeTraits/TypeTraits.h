#pragma once

#include <tuple>

template <class, template <class, class...> class>
struct IsTemplateInstanceOf : public std::false_type {};

template <class...Ts, template <class, class...> class U>
struct IsTemplateInstanceOf<U<Ts...>, U> : public std::true_type {};

template <class ... T>
using FirstType = std::tuple_element_t<0, std::tuple<T...>>;

template <class ... T>
concept SameType = std::conjunction_v<std::is_same<FirstType<T...>, T>...>;

