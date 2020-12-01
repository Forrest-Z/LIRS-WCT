#include "utils.h"
#include <cmath>

void wct_image::separableGaussianBlurImage(QImage& image, double sigma)
{
        // Add your code here.  Done right, you should be able to copy most of the code from GaussianBlurImage.
        int start_s = clock();
        double rc, rr, s = 2.0 * sigma * sigma;
        // sum is for normalization --sum should =1
        double sum = 0.000001;
        int rd, cd, i, j;
        int radius= 5 * sigma-1;
        int const size = 2 * radius + 1;
        QRgb pixel;
        // Create a buffer image so we're not reading and writing to the same image during filtering.
        QImage buffer;
        int w = image.width();
        int h = image.height();

        // This creates an image of size (w + 2*radius, h + 2*radius) with black borders.
        // This could be improved by filling the pixels using a different padding technique (reflected, fixed, etc.)
        buffer = image.copy(-radius, -radius, w + 2*radius, h + 2*radius);
        //define two separated matrix: cKernel is size by 1 matrix, rKernel is 1 by size matrix:
        double *cKernel = new double [size];
        //double *rKernel = new double [size];

        for(i=0;i<size;i++)
        {
            rc = (i-radius)*(i-radius);
            cKernel[i] = (exp(-(rc)/s))/(M_PI * s);
        }
            for(int i = 0; i < size;i++)
            sum += cKernel[i];

            for(i=0;i<size;i++)
            cKernel[i] /= sum;

    //for each pixel in the image
        for(int y=0;y<h;y++)
        {
            for(int x=0;x<w;x++)
            {
                double rgb[3];

                rgb[0] = 0.0;
                rgb[1] = 0.0;
                rgb[2] = 0.0;
                // Convolve the kernel at each pixel
                for(cd=-radius;cd<=radius;cd++)
                {
                // Get the pixel value
                pixel = buffer.pixel(x + cd + radius, y + radius);
                // Get the value of the kernel
                double weight = cKernel[cd + radius];
                rgb[0] += weight*(double) qRed(pixel);
                rgb[1] += weight*(double) qGreen(pixel);
                rgb[2] += weight*(double) qBlue(pixel);
                }
             image.setPixel(x, y, qRgb((int) floor(rgb[0]+0.5), (int) floor(rgb[1]+0.5), (int) floor(rgb[2]+0.5)));
            //convlove the resulted matrix with row matrix:
            }
        }

    //for each pixel in the image
        for(int y=0;y<h;y++)
        {
            for(int x=0;x<w;x++)
            {
                double rgb[3];

                rgb[0] = 0.0;
                rgb[1] = 0.0;
                rgb[2] = 0.0;
                // Convolve the kernel at each pixel
                for(rd=-radius;rd<=radius;rd++)
                {
                    // Get the pixel value
                    pixel = buffer.pixel(x + radius, y + rd + radius);
                    // Get the value of the kernel
                    double weight = cKernel[rd + radius];
                    rgb[0] += weight*(double) qRed(pixel);
                    rgb[1] += weight*(double) qGreen(pixel);
                    rgb[2] += weight*(double) qBlue(pixel);
                }
                image.setPixel(x, y, qRgb((int) floor(rgb[0]+0.5), (int) floor(rgb[1]+0.5), (int) floor(rgb[2]+0.5)));
            }
        }
        int stop_s=clock();
        qDebug("Running time:%f seconds",double(stop_s-start_s)/double(CLOCKS_PER_SEC));
}

void wct_image::secondDerivImage(QImage& image, double sigma)
{
        int rd, cd, i,j;
        QRgb pixel;

        // Create a buffer image so we're not reading and writing to the same image during filtering.
        QImage buffer;
        int w = image.width();
        int h = image.height();

        // This creates an image of size (w + 2*radius, h + 2*radius) with black borders.
        // This could be improved by filling the pixels using a different padding technique (reflected, fixed, etc.)
        buffer = image.copy(0,0, w, h);
        separableGaussianBlurImage(image, sigma);

        for(int y=0;y<h;y++)
           {
            for(int x=0;x<w;x++)
            {
                QRgb current=buffer.pixel(x,y);
                QRgb next=image.pixel(x,y);
                double rgb[3];

                rgb[0] = (double)qRed(next) - (double)qRed(current) + 128;
                rgb[1] = (double)qGreen(next) - (double)qGreen(current) + 128;
                rgb[2] = (double)qBlue(next) - (double)qBlue(current) + 128;

                //Make sure we don't over or under saturate
                for (int k=0;k<3;k++){
                    if(rgb[k] > 255) rgb[k] = 255;
                    if(rgb[k] < 0) rgb[k] = 0;
                }

             image.setPixel(x, y, qRgb((int) floor(rgb[0]+0.5), (int) floor(rgb[1]+0.5), (int) floor(rgb[2]+0.5)));
            }
          }
}

void wct_image::sharpenImage(QImage &image, double sigma, double alpha)
{

        int w = image.width();
        int h = image.height();
        QImage buffer = image.copy(0,0,w,h);
        secondDerivImage(buffer, sigma);

        for(int y=0;y<h;y++)
        {
            for(int x=0;x<w;x++)
            {
                QRgb current=buffer.pixel(x,y);
                QRgb next=image.pixel(x,y);
                double rgb[3];

                rgb[0] = (double)qRed(next) - alpha*((double)qRed(current) - 128);
                rgb[1] = (double)qGreen(next) -alpha* ((double)qGreen(current) - 128);
                rgb[2] = (double)qBlue(next) -alpha* ((double)qBlue(current) - 128);

                //Make sure we don't over or under saturate
                for (int k=0;k<3;k++)
                {
                    if(rgb[k] > 255) rgb[k] = 255;
                    if(rgb[k] < 0) rgb[k] = 0;
                 }

             image.setPixel(x, y, qRgb((int) floor(rgb[0]+0.5), (int) floor(rgb[1]+0.5), (int) floor(rgb[2]+0.5)));
            }
          }
}
