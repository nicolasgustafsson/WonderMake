#pragma once
#include <string>
#include "Vector.h"


template<class T>
concept ToStringable = requires (T t){ToString(t);};

template<class TRep, u32 Size>
[[nodiscard]] std::string ToString(const SVector<TRep, Size>& aVector)
{
    std::ostringstream string;
    string.precision(2);
    for(u32 i = 0; i < Size; i++)
    {
        string << aVector.GetMemberName(i);
        string << ": ";
        const i32 preSize = string.str().size();
        if (aVector[i] >= 0.f)
            string << " ";
        string << std::fixed;
        string << aVector[i];
        const i32 postSize = string.str().size();

        for(i32 i = 0; i < ((preSize - postSize) + 8); i++)
            string << " ";

        if (i+1 != Size)
            string << " | ";
    }

    return string.str();
}

template <ToStringable T>
std::ostream & operator<<(std::ostream &aOut, const T& aToString )
{
    return aOut << ToString(aToString);
}
