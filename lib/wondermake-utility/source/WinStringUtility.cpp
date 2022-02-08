#include "wondermake-utility/StringUtility.h"

#include "wondermake-utility/WinPlatform.h"

namespace Utility
{
    std::string Utf8Encode(const std::wstring& aWideString)
    {
        if (aWideString.empty())
            return std::string();

        const int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, aWideString.data(), static_cast<int>(aWideString.size()), NULL, 0, NULL, NULL);

        std::string strTo(static_cast<size_t>(sizeNeeded), 0);

        WideCharToMultiByte(CP_UTF8, 0, aWideString.data(), static_cast<int>(aWideString.size()), strTo.data(), sizeNeeded, NULL, NULL);

        return strTo;
    }

    std::wstring Utf8Decode(const std::string& aUtf8String)
    {
        if (aUtf8String.empty())
            return std::wstring();

        const int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, aUtf8String.data(), (int)aUtf8String.size(), NULL, 0);

        std::wstring wstrTo(static_cast<size_t>(sizeNeeded), 0);

        MultiByteToWideChar(CP_UTF8, 0, aUtf8String.data(), (int)aUtf8String.size(), wstrTo.data(), sizeNeeded);

        return wstrTo;
    }
}