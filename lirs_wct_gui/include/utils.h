#ifndef UTILS_H
#define UTILS_H

#include <QImage>

namespace wct_image
{
    void separableGaussianBlurImage(QImage& wct_image, double sigma);
    void secondDerivImage(QImage& wct_image, double sigma);
    void sharpenImage(QImage &wct_image, double sigma, double alpha);

    void saveImage(const QImage& wct_image, const QString& fileName);
    void readImage(QImage& wct_image);
}

#endif // UTILS_H
