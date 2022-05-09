#include "Archive.h"
#include <cassert>
#include <sstream>

using DX12Library::Archive;

HANDLE Archive::hFile;
HANDLE Archive::hMappedFile;
char* Archive::pFile;
std::unordered_map<std::string, Archive::ArcFileInfo> Archive::fileInfos;
bool Archive::isOpen = false;

void Archive::OpenArchive()
{
    std::string path;
    WIN32_FIND_DATAA win32fd;

    HANDLE hFind = FindFirstFileA("*.ark", &win32fd);

    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    //�Ƃ肠�����ŏ��Ɍ��������t�@�C��
    OpenArchive(win32fd.cFileName);
}

void Archive::OpenArchive(std::string archivePath)
{
    //�t�@�C���쐬
    hFile = CreateFileA(archivePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

    if (hFile == INVALID_HANDLE_VALUE) {
        assert(0);
    }

    //�������}�b�v�h�t�@�C���Ƃ��ĊJ��
    hMappedFile = CreateFileMappingA(hFile, 0, PAGE_READONLY, 0, 0, archivePath.c_str());

    if (hMappedFile <= 0) {
        assert(0);
    }

    //�t�@�C���̃|�C���^�擾
    pFile = (char*)MapViewOfFile(hMappedFile, FILE_MAP_READ, 0, 0, 0);

    //ARCK�ƃw�b�_�[�T�C�Y�ǂݎ��
    char identifier[5];
    for (int i = 0; i < 4; i++) {
        identifier[i] = pFile[i];
    }

    identifier[4] = '\0';

    //���ʂ��Ⴄ�Ȃ�A�T�[�g
    if (strcmp(identifier, "ARCK") != 0) {
        assert(0);
    }

    //�w�b�_�[�T�C�Y�ǂݎ��
    std::string strHeaderSize;
    for (int i = 4; i < 16; i++) {
        strHeaderSize += pFile[i];
    }

    int headerSize = std::stoi(strHeaderSize);

    //�t�@�C�����X�g�ǂݍ���
    for (int i = 16; i < headerSize + 16;) {

        std::string strFileName;
        std::string strStart;
        std::string strSize;

        //"���΂�
        i++;

        //�t�@�C�����擾
        while (pFile[i] != '"') {
            strFileName += pFile[i];
            i++;
        }

        i++;

        //�J�n�ʒu�擾
        while (pFile[i] != ',') {
            strStart += pFile[i];
            i++;
        }

        i++;

        //�t�@�C���T�C�Y�擾
        while (pFile[i] != '"' && i < headerSize + 16) {
            strSize += pFile[i];
            i++;
        }


        //�}�b�v�ɓo�^
        fileInfos.emplace(strFileName, ArcFileInfo{ std::stoi(strStart) + headerSize, std::stoi(strSize) });
    }

    isOpen = true;
}

void Archive::CloseArchive()
{
    if (isOpen == false) {
        return;
    }

    if (UnmapViewOfFile(pFile) == FALSE) {
        assert(0);
    }

    CloseHandle(hMappedFile);
    CloseHandle(hFile);
}

void* Archive::GetPData(const std::string& path, int* dstFileSize)
{
    auto itr = fileInfos.find(path);
    if (itr != fileInfos.end()) {
        if (dstFileSize != nullptr) {
            *dstFileSize = itr->second.size;
        }

        return(&pFile[itr->second.start]);
    }

    return nullptr;
}

int Archive::GetFileSize(const std::string& path)
{
    auto itr = fileInfos.find(path);
    if (itr != fileInfos.end()) {
        return itr->second.size;
    }

    return -1;
}

std::string Archive::GetDataAsString(void* pData, int size)
{
    if (pData == nullptr) {
        assert(0);
    }

    std::string str((char*)pData, size);

    return str;
}
