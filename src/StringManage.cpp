#include <TypicalTool/Tool/StringManage.h>



#ifdef _WINDOWS

#include <Windows.h>

namespace Typical_Tool {
    namespace StringManage {

        // 将 UTF-8 std::string 转换为 std::wstring
        std::wstring UTF8ToWstring(const std::string& utf8_str) {
            int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
            if (len == 0) {
                return L"";
            }
            std::wstring wstr(len - 1, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], len);
            return wstr;
        }

        // 将 std::wstring 转换为 UTF-8 std::string
        std::string WstringToUTF8(const std::wstring& wstr) {
            int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (len == 0) {
                return "";
            }
            std::string str(len - 1, '\0');
            WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], len, nullptr, nullptr);
            return str;
        }

    }
}


#elif _ICU4C_

//icu4c
#include <unicode/unistr.h>

namespace Typical_Tool {
    namespace StringManage
    {
        // 将std::string（UTF-8）转换为std::wstring（平台相关的宽字符编码）
        std::wstring UTF8ToWstring(const std::string& _UTF8_String) {
            UErrorCode errorCode = U_ZERO_ERROR;
            icu::UnicodeString unicodeStr = icu::UnicodeString::fromUTF8(icu::StringPiece(_UTF8_String));
            if (U_FAILURE(errorCode)) {
                throw Truntime_error(TEXT("ICU conversion from UTF-8 to UnicodeString failed"));
            }

            // 获取转换后的wchar_t数组长度（注意：这依赖于平台的wchar_t大小）
            // 在Windows上，wchar_t是16位宽，因此我们需要UTF-16字符串
            // 在其他平台上，可能需要调整
            std::vector<wchar_t> wcharArray(unicodeStr.extract(nullptr, unicodeStr.length(), nullptr, errorCode) + 1);
            if (U_FAILURE(errorCode)) {
                throw Truntime_error(TEXT("ICU extraction to wchar_t array failed"));
            }

            // 将UnicodeString内容复制到wchar_t数组
            unicodeStr.extract(wcharArray.data(), static_cast<int32_t>(wcharArray.size()), errorCode);
            if (U_FAILURE(errorCode)) {
                throw Truntime_error(TEXT("ICU extraction to wchar_t array (second call) failed"));
            }

            // 确保wchar_t数组以null字符结尾（虽然extract应该已经做到了）
            wcharArray[unicodeStr.length()] = L'\0';

            // 转换为std::wstring
            return std::wstring(wcharArray.data());
        }

        // 将std::wstring（平台相关的宽字符编码）转换为std::string（UTF-8）
        std::string WstringToUTF8(const std::wstring& _WString) {
            UErrorCode errorCode = U_ZERO_ERROR;
            // 创建一个icu::UnicodeString从wchar_t字符串
            // 注意：这里假设wchar_t字符串是以平台相关的Unicode编码（如UTF-16在Windows上）
            icu::UnicodeString unicodeStr(reinterpret_cast<const UChar*>(_WString.data()), static_cast<int32_t>(_WString.size()));

            // 获取所需的UTF-8字符串长度
            int32_t utf8Length = unicodeStr.length() * 3 + 1; // 一个Unicode字符最多可以转换为3个UTF-8字节，加1用于null字符
            std::string utf8Str(utf8Length, '\0');

            // 转换为UTF-8字符串
            unicodeStr.toUTF8String(utf8Str);

            // 修剪字符串以去除可能的额外null字符（虽然toUTF8String应该已经正确设置了长度）
            utf8Str.resize(strlen(utf8Str.c_str()));

            return utf8Str;
        }

        std::wstring Typical_Tool::StringManage::UTF8ToWstring(std::string&& _String) {
            return UTF8ToWstring(_String);  // 使用常量引用版本
        }

        std::string Typical_Tool::StringManage::WstringToUTF8(std::wstring&& _WString) {
            return WstringToUTF8(_WString);  // 使用常量引用版本
        }
    }
}

#else

namespace Typical_Tool {
    namespace StringManage
    {
        // 将 std::string 转换为 std::wstring
        std::wstring UTF8ToWstring(const std::string& _UTF8_String) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            return converter.from_bytes(_UTF8_String);
        }

        // 将 std::wstring 转换为 std::string
        std::string WstringToUTF8(const std::wstring& _WString) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            return converter.to_bytes(_WString);
        }
    }
}

#endif
