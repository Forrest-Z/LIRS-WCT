//
// Created by optio32 on 13.04.18.
//

#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <assimp/matrix4x4.h>
#include <boost/filesystem.hpp>

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

namespace Random
{
    std::string GetRandomStringName(int len);
}

namespace Info
{
    std::string GetModuleFileName();
    boost::filesystem::path GetModulePath();
}

namespace File
{
    void CopyFile(const std::string& from, const std::string& to);
    void CopyFileToDirectory(const std::string& file, const std::string& dir);
    void DeleteFile(const char* file);
    long GetFileSize(IN std::ifstream& file);
    long GetFileSize(IN char* szFileName);
    void ReadFile(IN char* szFileName, OUT char* szBuffer, IN ulong size);
    bool FileExistStat(const std::string& name);
    bool FileExistBoost(const std::string& name);
}

namespace Path
{
    const std::string ExtractFilePath(IN char* szPath);
    const char* ExtractFileName(IN char* szPath);
}

namespace Parse
{
    size_t StrToInt(char* szNumber);
    float StrToFloat(char* szNumber);
}

namespace Img
{
    bool CheckPngHeader(char* fileHeader);
}

namespace Exec
{
    int exec(char* command);
}

namespace Math
{
    double degreeToRadian(double degree);
    void calculateTransformMatrix(aiMatrix4x4& transformMatrix, double rotAngle, double scale);
}

#endif //UTILS_H
