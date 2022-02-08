#pragma once
template <class, template <class> class>
struct IsTemplateInstanceOf : public std::false_type {};

template <class T, template <class> class U>
struct IsTemplateInstanceOf<U<T>, U> : public std::true_type {};
