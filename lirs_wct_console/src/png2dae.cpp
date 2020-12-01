//
// Created by optio32 on 23.04.18.
//

#include <cstdint>
#include "png2dae.h"
#include "utils.h"
#include "stb_image.h"
#include "heightmap.h"
#include "ModelLoader.h"
#include <boost/filesystem.hpp>

#define aiProcess_Suitable ( \
    aiProcess_FindInvalidData |  \
    aiProcess_OptimizeMeshes |  \
    aiProcess_FixInfacingNormals |  \
    aiProcess_Triangulate |  \
    0 )

Image2Dae::Image2Dae(const IMAGE2DAE_CONFIG& daeConfig)
{
    this->model = std::make_shared<Model>();
    this->daeConfig = daeConfig;
    STL_CONFIG cfg;
    cfg.imageFile = daeConfig.inputImage;
    cfg.outputModel = boost::filesystem::path(daeConfig.outputModel).stem().string() + ".stl";
    cfg.outputDir = daeConfig.outputDir;
    cfg.zAxisScale = daeConfig.zAxisScale;
    this->stlConverter = std::make_shared<Png2Stl>(cfg);
}

Image2Dae::~Image2Dae()
{
    //delete [] _data;
    printf("Erased\n");
}
void Image2Dae::ToStl(std::string filename)
{
    stlConverter->Transform();
    stlConverter->Save(filename);    
}

bool Image2Dae::ImageToStl(const STL_CONFIG& stlConfig)
{
    bool bStatus = false;
    Png2Stl cnvrt(stlConfig);
    if(!cnvrt.Transform())
    {
        cnvrt.Close();
        return bStatus;
    }
    if(!cnvrt.Save())
    {
        cnvrt.Close();
        return bStatus;
    }    
    cnvrt.Close();
    return !bStatus;
}

bool Image2Dae::StlToDae(const std::string& inputStlModelPath, const DAE_CONFIG& daeConfig)
{
    bool bStatus = false;
    std::size_t pos = inputStlModelPath.find(".stl");
    if(pos == std::string::npos)
    {
        return bStatus;
    }
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(inputStlModelPath.c_str(), 0);
    Model mdl;
    mdl.loadParameters(daeConfig.texture, daeConfig.axisScale, daeConfig.xAxisRotationAngle);
    mdl.loadFromScene(scene);
    mdl.save(daeConfig.outputDir + "/" + daeConfig.outputModel);
    return !bStatus;
}

void Image2Dae::applyTexture(const aiScene* scene)
{   
    model->loadParameters(daeConfig.texture, daeConfig.axisScale, daeConfig.xAxisRotationAngle);
    model->loadFromScene(scene);
    //model->load(_cfg->outputModel+".dae");    
}

void Image2Dae::ApplyTexture(void)
{
    //boost::scoped_ptr<Model> model(new Model);
    /*std::unique_ptr<Model> model(new Model);    
    model->loadParameters(_cfg);
    model->load(_cfg->outputModel+".dae");
    model->save(_cfg->outputModel+".dae");*/
  
}

void Image2Dae::Transform()
{
    stlConverter->Transform();
    stlConverter->Save();
    std::string stlOutputFilePath = stlConverter->GetOutputFilePath();
    Assimp::Importer importer;
    scene = importer.ReadFile(stlOutputFilePath.c_str(), 0);
    boost::filesystem::remove(stlOutputFilePath);
    applyTexture(scene);
}

void Image2Dae::Save(std::string filename)
{
    if(filename != "")
    {
        model->save(daeConfig.outputDir + "/" + filename + ".dae");
    }
    else 
    {
        model->save(daeConfig.outputDir + "/" + daeConfig.outputModel);
    }    
}
