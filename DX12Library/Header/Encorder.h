#pragma once
#include <string>

namespace DX12Library {

	class Encorder
	{
	public:
		static std::wstring StrToWstr(const std::string& str);
		static std::string WstrToStr(const std::wstring& wstr);
		static std::string GetMergedHLSLI(void* pHlslData, int size, std::string hlslPath);
	};
}
