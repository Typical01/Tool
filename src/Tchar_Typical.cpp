#include <libTypical/Tool/Tchar_Typical.h>



namespace Typical_Tool {
	namespace TcharStringManage {
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