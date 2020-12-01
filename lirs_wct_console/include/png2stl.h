//
// Created by optio32 on 13.04.18.
//

#ifndef PNG2STL_H
#define PNG2STL_H

#include <string>
#include "libtrix.h"

typedef struct _STL_CONFIG
{
  double zAxisScale = 1;
  std::string imageFile;
  std::string outputDir;
  std::string outputModel;
} STL_CONFIG, *PSTL_CONFIG;

/*
namespace PNG2STL_H
{
    void Convert(PPNG2DAE_CONFIG Config);
    bool CheckPngHeader(char* szFile);

}
*/

class Png2Stl
{
  public:
    Png2Stl(const STL_CONFIG& stlConfig);
    ~Png2Stl();
    bool Transform();
    void PrintArgs();
    bool Save(const std::string filename = "");
    void Close();
    std::string GetOutputFilePath();

  private:
    bool load(const std::string& imageFile);
    bool convert();
    bool isPNGImage(unsigned char* fileHeader);

    //vars
    STL_CONFIG stlConfig;
    trix_mesh* mesh = NULL;
};

#endif //PNG2STL_H
