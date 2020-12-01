//
// Created by optio32 on 13.04.18.
//

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "png2stl.h"

typedef struct _HEIGHTMAP{

    // xy dimensions (size = width * height)
    unsigned int width, height;
    unsigned long size;

    unsigned int depth;

    // z dimensions (range = max - min = relief)
    unsigned char min, max, range;

    // raster with size pixels ranging in value from min to max
    unsigned char* data;

} HEIGHTMAP, *PHEIGHTMAP;

HEIGHTMAP* PngToHeightmap(char* szFileName);
int HeightmapToSTL(HEIGHTMAP* hm, STL_CONFIG* cfg, trix_mesh** outMesh);
void FreeHeightmap(HEIGHTMAP** hm);

#endif //HEIGHTMAP_H
