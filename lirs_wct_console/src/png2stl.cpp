//
// Created by optio32 on 13.04.18.
//

#include "png2stl.h"
#include "heightmap.h"
#include <iostream>
#include <boost/filesystem.hpp>

/*
void PNG2STL::Convert(PPNG2DAE_CONFIG config)
{
    HEIGHTMAP* hm = NULL;
    hm = PngToHeightmap((char*)config->inputImage.c_str());
    printf("Width:= %d, Height:= %d, Size:= %d, Depth:= %d/n", hm->width, hm->height, hm->size, hm->depth);
    if(HeightmapToSTL(hm, config) != 0)
    {
        printf("[ERROR]HeightmapToSTL");
        return;
    }

    FreeHeightmap(&hm);

}

bool PNG2STL::CheckPngHeader(char* fileHeader)
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
*/

Png2Stl::Png2Stl(const STL_CONFIG& config)
{
    if(config.imageFile.empty() || config.zAxisScale <= 0)
    {
        printf("Check the config!");
        return;
    }
    stlConfig = config; 
    if(!load(stlConfig.imageFile))
    {
        printf("Loader error!");
        return;
    }     
}

Png2Stl::~Png2Stl()
{
    if(mesh != NULL)
        trixRelease(&mesh);    
}

bool Png2Stl::Transform()
{
    bool bStatus = false;
    if(mesh != NULL)
    {
        trixRelease(&mesh);
        this->mesh = NULL;
    }
    if(convert())
    {
        printf("Transform is successful!\n");
        boost::filesystem::remove(stlConfig.imageFile);
        bStatus = true;
    }
    return bStatus;
}

bool Png2Stl::Save(const std::string filename)
{
    bool bStatus = false;
    std::string destination;
    if(filename != "")
    {
        destination = stlConfig.outputDir + "/" + filename + ".stl";
    }
    else
    {
       destination = stlConfig.outputDir + "/" + stlConfig.outputModel;
    }
    
    if (trixWrite(mesh, destination.c_str(), TRIX_STL_BINARY) != TRIX_OK) {
        return bStatus;
    }
    return !bStatus;    
}

void Png2Stl::Close()
{
    trixRelease(&mesh);
    this->mesh = NULL;
    this->stlConfig.imageFile = "";
    this->stlConfig.outputDir = "";
    this->stlConfig.outputModel = "";
}

void Png2Stl::PrintArgs()
{
    std::cout << "--------------------------";
    std::cout << "Image file: " << stlConfig.imageFile << '\n';
    std::cout << "Ouput dir: " << stlConfig.outputDir << '\n';
    std::cout << "Output model: " << stlConfig.outputModel << '\n';
    std::cout << "Z axis scale: " << stlConfig.zAxisScale << '\n';
    std::cout << "--------------------------";
}

std::string Png2Stl::GetOutputFilePath()
{
    return stlConfig.outputDir + "/" + stlConfig.outputModel;
}

bool Png2Stl::isPNGImage(unsigned char* fileHeader)
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

bool Png2Stl::load(const std::string& imageFile)
{
    bool bStatus = false;
    std::string str(imageFile);
    std::size_t pos = str.find(".png");
    if(pos == std::string::npos)
        return bStatus;
    boost::filesystem::path p(str);
    //auto fileName = p.stem().string();
    //stlConfig.outputModel = fileName + ".stl";
    stlConfig.outputDir = boost::filesystem::canonical(boost::filesystem::path(stlConfig.outputDir)).string();
    return !bStatus;
}

bool Png2Stl::convert()
{
    int convert_status = 0;
    bool bStatus = false;
    HEIGHTMAP* hm = NULL;
    hm = PngToHeightmap((char*)stlConfig.imageFile.c_str());
    if(!hm)
        return bStatus;
    //printf("Width:= %d, Height:= %d, Size:= %d, Depth:= %d/n", hm->width, hm->height, hm->size, hm->depth);
    if(convert_status = HeightmapToSTL(hm, &stlConfig, &mesh))
    {
        printf("Convert error = %d\n", convert_status);
        FreeHeightmap(&hm);
        return bStatus;
    }   
    FreeHeightmap(&hm);
    if(mesh == NULL)
        std::cout << "\nMesh null\n";
    return !bStatus;
}
