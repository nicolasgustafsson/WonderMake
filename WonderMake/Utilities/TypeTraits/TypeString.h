#pragma once

#include <source_location>
#include <string_view>

namespace _Impl
{
    template <typename T>
    consteval auto func_name() {
        //const auto& loc = std::source_location::current();
        return __FUNCSIG__;//loc.function_name();
    }

    template <typename T>
    consteval std::string_view type_of_impl_() {
        constexpr std::string_view func_name_ = func_name<T>();
        // since func_name_ is 'consteval auto func_name() [with T = ...]'
        // we can simply get the subrange
        // because the position after the equal will never change since 
        // the same function name is used

        constexpr std::string_view func_name_without_trash{ func_name_.begin() + 30, func_name_.end() - 7 };
        i32 offset = 0;

    	if constexpr (func_name_without_trash.starts_with("class"))
            offset = 6;

    	if constexpr (func_name_without_trash.starts_with("struct"))
            offset = 7;

        // another notice: these may not work on other compilers;
        return { func_name_without_trash.begin() + offset, func_name_without_trash.end()};
    }
}

template <typename T>
constexpr auto TypeString(T&& arg) {
    return _Impl::type_of_impl_<decltype(arg)>();
}

template <typename T>
constexpr auto TypeString() {
    return _Impl::type_of_impl_<T>();
}