#ifndef TRANSFORMATION_INFO_H
#define TRANSFORMATION_INFO_H

#include <cstdint>
#include <QString>

typedef struct _TRANSFORMATION_INFO
{
    QString inputImageFilename;
    QString outputModelFilename;
    QString outputFolderPath = ".";
    QString inputTextureImageFilename = ".";
    std::uint8_t convertMode;
    bool colorInverse = false;
    bool grayscale = false;
    double smoothingCoeff = 0.0;
    bool smoothing = false;
    double sharpnessCoeff = 0.0;
    bool sharpness = false;
    double heightScale = 0.25;
    double sizesScale = 0.05;
    double xAxisRotateAngle = -90.0;

} TRANSFORMATION_INFO, *PTRANSFORMATION_INFO;

#endif // TRANSFORMATION_INFO
