#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using ::testing::NiceMock;
using ::testing::StrictMock;

using ::testing::_;
using ::testing::Args;
using ::testing::ElementsAre;
using ::testing::Return;

using ::testing::Eq;
using ::testing::Ge;
using ::testing::StrEq;

// This is a matcher like ElementsAreArray, but it allows you to match against a void *.
template <typename T>
testing::Matcher<std::tuple<const void*, size_t>> ElementsAreArrayVoidPointer(const T* ptr, size_t size)
{
    class TupleConverter
        : public std::tuple<const T*, size_t>
    {
    public:
        TupleConverter(const std::tuple<const void*, size_t>& t)
            : std::tuple<const T*, size_t>(static_cast<const T*>(std::get<0>(t)), std::get<1>(t))
        {}
    };

    return testing::SafeMatcherCast<std::tuple<const void*, size_t>>(
        testing::SafeMatcherCast<TupleConverter>(
            testing::SafeMatcherCast<std::tuple<const T*, size_t>>(
                testing::ElementsAreArray(ptr, size))));
}
