#pragma once
template <class, template <class> class>
struct IsTemplateInstanceOf : public std::false_type {};

template <class T, template <class> class U>
struct IsTemplateInstanceOf<U<T>, U> : public std::true_type {};

template <class ... T>
using FirstType = std::tuple_element_t<0, std::tuple<T...>>;

template <class ... T>
concept SameType = std::conjunction_v<std::is_same<FirstType<T...>, T>...>;