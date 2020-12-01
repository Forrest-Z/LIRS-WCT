//
// Created by optio32 on 23.04.18.
//
#include "stl2dae.h"

/*
void STL2DAE::Convert(char *szStlFilename)
{
    std::string file(szStlFilename);
    file += ".dae";
    std::string file2(szStlFilename);
    file2 += ".stl";
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file2.c_str(), 0);

    Assimp::Exporter exporter;
    aiReturn result = exporter.Export(scene, "collada", file.c_str(), 0);
}

void STL2DAE::Convert(PPNG2DAE_CONFIG _cfg)
{
    std::string dae_file(_cfg->inputImage);
    dae_file += ".dae";
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_cfg->inputImage, 0);

    Assimp::Exporter exporter;
    aiReturn result = exporter.Export(scene, "collada", dae_file.c_str(), 0);
}
*/
