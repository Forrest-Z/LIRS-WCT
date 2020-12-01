//
// Created by optio32 on 13.04.18.
//

#include <unistd.h>
#include "utils.h"
#include <cstdlib>
#include <sys/stat.h>
#include <cstring>
#include <linux/limits.h>

std::string Random::GetRandomStringName(int len)
{
    srand(time(NULL));
    std::string str;
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        str += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return str;
}

long File::GetFileSize(IN std::ifstream &file)
{
    file.seekg(0, std::ios_base::end);
    return file.tellg();
}

long File::GetFileSize(IN char* szFileName)
{
    std::ifstream in(szFileName, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

void File::ReadFile(IN char* szFileName, OUT char* szBuffer, IN ulong size)
{
    std::ifstream inputFile(szFileName, std::ios::in | std::ios::binary);
    //fileSize = GetFileSize(inputFile);
    //ulong size = GetFileSize(inputFile);
    //printf("ISize:=%d, CSize:=%d\n", fileSize, size);
    inputFile.read(szBuffer, size);
    //return szBuffer;
}

void File::DeleteFile(const char* file)
{
    remove(file);
}

void File::CopyFile(const std::string& from, const std::string& to)
{
    boost::filesystem::copy_file(from, to);
}

void File::CopyFileToDirectory(const std::string& file, const std::string& dir)
{
    boost::filesystem::path directory(dir);
    boost::filesystem::create_directory(directory);
    boost::filesystem::copy_file(file, dir+"/"+file);
}

std::string Info::GetModuleFileName()
{
    std::string exec_path_str("");
    char exec_path[PATH_MAX];
    std::memset(exec_path, 0, sizeof(exec_path));
    if(readlink("/proc/self/exe", exec_path, PATH_MAX) != -1)
        exec_path_str = exec_path;
    return exec_path_str;
}

const char* Path::ExtractFileName(IN char* szPath)
{
    char* start, *end;

    start = szPath;
    end = start + strlen(start) - 1;
    while(*end != '/')
        end--;
    end++;
    return end;
}

int Exec::exec(char* command)
{
    return std::system(command);
}

const std::string Path::ExtractFilePath(IN char* szPath)
{
    size_t found;
    std::string str(szPath);
    found=str.find_last_of("/\\");
    return str.substr(0,found);
}

size_t Parse::StrToInt(char* szNumber)
{
    return atoi(szNumber);
}

float Parse::StrToFloat(char* szNumber)
{
    return atof(szNumber);
}

bool Img::CheckPngHeader(char* fileHeader)
{
    uint8_t png_signature[8] = { 137,80,78,71,13,10,26,10 };
    bool fOk = false;
    for(int i = 0; i < 8; i++)
    {
        if((uint8_t)fileHeader[i] != png_signature[i])
            return fOk;
    }
    return !fOk;
}

double Math::degreeToRadian(double degree)
{
    return ((M_PI * degree) / 180.0);
}

void Math::calculateTransformMatrix(aiMatrix4x4& transformMatrix, double rotAngle, double scale)
{
    double radianAngle = Math::degreeToRadian(rotAngle);
    transformMatrix.a1 = scale;
    transformMatrix.b2 = scale * cos(radianAngle);
    transformMatrix.b3 = scale * -1 * sin(radianAngle);
    transformMatrix.c2 = scale * sin(radianAngle);
    transformMatrix.c3 = scale * cos(radianAngle);
    transformMatrix.d4 = 1;
}


bool File::FileExistStat(const std::string &name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

boost::filesystem::path Info::GetModulePath()
{
    boost::filesystem::path p(Info::GetModuleFileName());
    return p.parent_path();
}

bool File::FileExistBoost(const std::string &name)
{
    return boost::filesystem::exists(name);
}
