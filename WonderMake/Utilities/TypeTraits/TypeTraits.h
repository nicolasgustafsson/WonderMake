#pragma once

template <class, template <class, class...> class>
struct IsTemplateInstanceOf : public std::false_type {};

template <class...Ts, template <class, class...> class U>
struct IsTemplateInstanceOf<U<Ts...>, U> : public std::true_type {};