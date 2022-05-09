#pragma once
#include <string>
#include <Windows.h>
#include <unordered_map>
#include <istream>

namespace DX12Library {

	class Archive
	{
		struct ArcFileInfo {
			int start;
			int size;
		};

	private:
		static HANDLE hFile;
		static HANDLE hMappedFile;
		static char* pFile;
		static std::unordered_map<std::string, ArcFileInfo> fileInfos;
		static bool isOpen;

	public:
		static void OpenArchive();
		static void OpenArchive(std::string archivePath);
		static void CloseArchive();
		static void* GetPData(const std::string& path, int* dstFileSize = nullptr);
		static int GetFileSize(const std::string& path);
		static std::string GetDataAsString(void* pData, int size);
		static bool IsOpenArchive() { return isOpen; }
	};

}