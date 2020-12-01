////
//// Created by optio32 on 13.04.18.
////

#define STB_IMAGE_IMPLEMENTATION

#include "heightmap.h"
#include <cstddef> //size_t
#include <cstdio>
#include <cstdlib>
#include <string>
#include "stb_image.h"
#include "libtrix.h"

void FreeHeightmap(HEIGHTMAP **hm)
{

    if (hm == NULL || *hm == NULL) {
        return;
    }

    if ((*hm)->data != NULL) {
        stbi_image_free((*hm)->data);
    }

    free(*hm);
    *hm = NULL;
}

HEIGHTMAP* PngToHeightmap(char *szFileName)
{
    int width, height, depth;
    unsigned char* data;
    HEIGHTMAP* heightmap;

    data = stbi_load(szFileName, &width, &height, &depth, 1);

    if(data != NULL)
    {
        heightmap = new HEIGHTMAP;
        heightmap->data = data;
        heightmap->width = width;
        heightmap->height = height;
        heightmap->size = width * height;
        heightmap->depth = depth;

        unsigned long i;
        unsigned char min, max;

        if (heightmap == NULL || heightmap->data == NULL) {
            return heightmap;
        }

        min = 255;
        max = 0;

        for (i = 0; i < heightmap->size; i++) {
            if (heightmap->data[i] < min) {
                min = heightmap->data[i];
            }
            if (heightmap->data[i] > max) {
                max = heightmap->data[i];
            }
        }

        heightmap->min = min;
        heightmap->max = max;
        heightmap->range = max - min;

        return heightmap;
    }
    return NULL;
}

static trix_result Wall(trix_mesh *mesh, const trix_vertex *a, const trix_vertex *b) {
    trix_vertex a0 = *a;
    trix_vertex b0 = *b;
    trix_triangle t1;
    trix_triangle t2;
    trix_result r;
    a0.z = 0;
    b0.z = 0;
    t1.a = *a;
    t1.b = *b;
    t1.c = b0;
    t2.a = b0;
    t2.b = a0;
    t2.c = *a;
    if ((r = trixAddTriangle(mesh, &t1)) != TRIX_OK) {
        return r;
    }
    if ((r = trixAddTriangle(mesh, &t2)) != TRIX_OK) {
        return r;
    }
    return TRIX_OK;
}

static float avgnonneg(float zp, float z1, float z2, float z3) {
    float sum = zp;
    float z[3] = {z1, z2, z3};
    int i, n = 1;

    for (i = 0; i < 3; i++) {
        if (z[i] >= 0) {
            sum += z[i];
            n++;
        }
    }

    return sum / (float)n;
}

static float hmzat(const HEIGHTMAP *hm, unsigned int x, unsigned int y, float z) {
    return 1.0 + (z * hm->data[(hm->width * y) + x]);
}

trix_result Surface(trix_mesh *mesh, const trix_vertex *v1, const trix_vertex *v2, const trix_vertex *v3, const trix_vertex *v4) {
    trix_triangle i, j;
    trix_result r;

    i.a = *v4;
    i.b = *v2;
    i.c = *v1;

    j.a = *v4;
    j.b = *v3;
    j.c = *v2;

    if ((r = trixAddTriangle(mesh, &i)) != TRIX_OK) {
        return r;
    }

    if ((r = trixAddTriangle(mesh, &j)) != TRIX_OK) {
        return r;
    }

    return TRIX_OK;
}

trix_result Mesh(const HEIGHTMAP *hm, trix_mesh *mesh, STL_CONFIG* cfg) {
    unsigned int x, y;
    float az, bz, cz, dz, ez, fz, gz, hz;
    trix_vertex vp, v1, v2, v3, v4;
    trix_result r;
    float z = cfg->zAxisScale;

    for (y = 0; y < hm->height; y++) {
        for (x = 0; x < hm->width; x++) {

            /*

            +---+---+---+
            |   |   |   |
            | A | B | C |
            |   |   |   |
            +---1---2---+
            |   |I /|   |
            | H | P | D |
            |   |/ J|   |
            +---4---3---+
            |   |   |   |
            | G | F | E |
            |   |   |   |
            +---+---+---+

            Current pixel position is marked at center as P.
            This pixel is output as two triangles, I and J.
            Points 1, 2, 3, and 4 are offset half a unit from P.
            Neighboring pixels are A, B, C, D, E, F, G, and H.

            Vertex 1 z is average of ABPH z
            Vertex 2 z is average of BCDP z
            Vertex 3 z is average of PDEF z
            Vertex 4 z is average of HPFG z

            Averages do not include neighbors that would lie
            outside the image, but do included masked values.

            */

            // determine elevation of neighboring pixels in order to
            // to interpolate height of corners 1, 2, 3, and 4.
            // -1 is used to flag edge pixels to disregard.
            // (Masked neighbors are still considered.)

            if (x == 0 || y == 0) {
                az = -1;
            } else {
                az = hmzat(hm, x - 1, y - 1, z);
            }

            if (y == 0) {
                bz = -1;
            } else {
                bz = hmzat(hm, x, y - 1, z);
            }

            if (y == 0 || x + 1 == hm->width) {
                cz = -1;
            } else {
                cz = hmzat(hm, x + 1, y - 1, z);
            }

            if (x + 1 == hm->width) {
                dz = -1;
            } else {
                dz = hmzat(hm, x + 1, y, z);
            }

            if (x + 1 == hm->width || y + 1 == hm->height) {
                ez = -1;
            } else {
                ez = hmzat(hm, x + 1, y + 1, z);
            }

            if (y + 1 == hm->height) {
                fz = -1;
            } else {
                fz = hmzat(hm, x, y + 1, z);
            }

            if (y + 1 == hm->height || x == 0) {
                gz = -1;
            } else {
                gz = hmzat(hm, x - 1, y + 1, z);
            }

            if (x == 0) {
                hz = -1;
            } else {
                hz = hmzat(hm, x - 1, y, z);
            }

            // pixel vertex
            vp.x = (float)x;
            vp.y = (float)(hm->height - y);
            vp.z = hmzat(hm, x, y, z);

            // Vertex 1
            v1.x = (float)x - 0.5;
            v1.y = ((float)hm->height - ((float)y - 0.5));
            v1.z = avgnonneg(vp.z, az, bz, hz);

            // Vertex 2
            v2.x = (float)x + 0.5;
            v2.y = v1.y;
            v2.z = avgnonneg(vp.z, bz, cz, dz);

            // Vertex 3
            v3.x = v2.x;
            v3.y = ((float)hm->height - ((float)y + 0.5));
            v3.z = avgnonneg(vp.z, dz, ez, fz);

            // Vertex 4
            v4.x = v1.x;
            v4.y = v3.y;
            v4.z = avgnonneg(vp.z, hz, fz, gz);

            // Upper surface
            if ((r = Surface(mesh, &v1, &v2, &v3, &v4)) != TRIX_OK) {
                return r;
            }

            // north wall (vertex 1 to 2)
            if (y == 0) {
                if ((r = Wall(mesh, &v1, &v2)) != TRIX_OK) {
                    return r;
                }
            }

            // east wall (vertex 2 to 3)
            if (x + 1 == hm->width) {
                if ((r = Wall(mesh, &v2, &v3)) != TRIX_OK) {
                    return r;
                }
            }

            // south wall (vertex 3 to 4)
            if (y + 1 == hm->height) {
                if ((r = Wall(mesh, &v3, &v4)) != TRIX_OK) {
                    return r;
                }
            }

            // west wall (vertex 4 to 1)
            if (x == 0) {
                if ((r = Wall(mesh, &v4, &v1)) != TRIX_OK) {
                    return r;
                }
            }

            // bottom surface - same as top, except with z = 0 and reverse winding
            v1.z = 0; v2.z = 0; v3.z = 0; v4.z = 0;
            if ((r = Surface(mesh, &v4, &v3, &v2, &v1)) != TRIX_OK) {
                return r;
            }
        }
    }

    return TRIX_OK;
}

int HeightmapToSTL(HEIGHTMAP *hm, STL_CONFIG* config, trix_mesh** outMesh)
{
    trix_result r;
    trix_mesh *mesh;    

    if ((r = trixCreate(outMesh, "hmstl")) != TRIX_OK) {
        return (int)r;
    }

    if ((r = Mesh(hm, *outMesh, config)) != TRIX_OK) {
        trixRelease(outMesh);
        return (int)r;
    }

    return 0;
}
