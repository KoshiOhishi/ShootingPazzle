#include "Encorder.h"
#include "Archive.h"
#include "sstream"
#include <Windows.h>
#include <iostream>

using namespace DX12Library;

std::wstring Encorder::StrToWstr(const std::string& str)
{
    int bufSize = MultiByteToWideChar(CP_ACP, 0, str.c_str()
        , -1, (wchar_t*)NULL, 0);

    wchar_t* buf = new wchar_t[bufSize];

    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buf
        , bufSize);

    std::wstring wstr(buf, buf + bufSize - 1);

    delete[] buf;

    return wstr;
}

std::string Encorder::WstrToStr(const std::wstring& wstr)
{
    int bufSize = WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str()
        , -1, (char*)NULL, 0, NULL, NULL);

    CHAR* buf = new CHAR[bufSize];

    WideCharToMultiByte(CP_OEMCP, 0, wstr.c_str(), -1, buf
        , bufSize, NULL, NULL);

    std::string oRet(buf, buf + bufSize - 1);

    delete[] buf;

    return(oRet);
}

std::string Encorder::GetMergedHLSLI(void* pHlslData, int size, std::string hlslPath)
{
    std::stringstream ss;
    ss << Archive::GetDataAsString(pHlslData, size);

    std::string hlsl = "";
    std::string hlsli = "";

    //#includeを探す
    std::string line;
    while (std::getline(ss, line)) {

        if (line.find("#include") != -1) {
            size_t start = line.find("\"") + 1;
            size_t end = line.rfind("\"");
            std::string hlsliName = line.substr(start, end - start);

            //アーカイブから読み込み
            //HLSLのディレクトリ+HLSLIのパス
            std::string dir = "";
            if (hlslPath.find("\\") != -1) {
                dir = hlslPath.substr(0, hlslPath.rfind("\\") + 1);
            }
            else if (hlslPath.find("/") != -1) {
                dir = hlslPath.substr(0, hlslPath.rfind("/") + 1);
            }

            //hlsliをアーカイブから持ってくる
            int size;
            char* pData = (char*)Archive::GetPData(dir + hlsliName, &size);

            if (pData != nullptr) {
                hlsli = Archive::GetDataAsString(pData, size);
                hlsli += "\n";
            }
        }
        else {
            hlsl += line + "\n";
        }
    }

    std::string result = hlsli + hlsl;

    return result;
}
