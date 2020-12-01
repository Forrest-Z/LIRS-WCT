//
// Created by optio32 on 23.04.18.
//

#ifndef PNG2DAE_H
#define PNG2DAE_H

#include <string>
#include <memory>
#include "png2stl.h"
#include "ModelLoader.h"
#include "Common.h"

enum ConvertMode {IMAGE2DAE=0, IMAGE2STL, STL2DAE};

typedef struct _IMAGE2DAE_CONFIG
{
    double zAxisScale;
    std::string outputModel;
    std::string inputImage;
    std::string outputDir;
    std::string texture;
    double axisScale;
    double xAxisRotationAngle;
} IMAGE2DAE_CONFIG, *PIMAGE2DAE_CONFIG, DAE_CONFIG;

class Image2Dae
{  
  public:
    Image2Dae(const IMAGE2DAE_CONFIG& daeConfig);
    ~Image2Dae();
    void ToStl(std::string filename = "");
    void Transform();
    void Save(std::string filename = "");
    static bool ImageToStl(const STL_CONFIG& stlConfig);
    static bool StlToDae(const std::string& inputStlModelPath, const DAE_CONFIG& daeConfig);  

  private:  
    char* _data;
    std::shared_ptr<Model> model;
    const aiScene* scene = nullptr;
    IMAGE2DAE_CONFIG daeConfig;
    std::shared_ptr<Png2Stl> stlConverter;
    void ApplyTexture(void);  
    void applyTexture(const aiScene* scene);
    //bool IsPng();
};

#endif //PNG2DAE_H
