#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QProgressBar>
#include <QDebug>

Q_GUI_EXPORT void qt_blurImage(QImage& blurImage, qreal radius, bool quality, int transposed = 0);

using namespace wct_image;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // init the main window settings
    initMainWindow();
    // init the widgets settings
    initWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setSmoothing(double value)
{
    if(ui->smoothingSlider->value() == 0.0 && ui->sharpnessSlider->value() == 0.0)
    {
        changedAfterSharpnessImage = readImage;
        changedAfterSmoothingImage = readImage;
    }

    changedAfterSmoothingImage = changedAfterSharpnessImage;
    qt_blurImage(changedAfterSmoothingImage, value, true);
    int w = ui->imageViewer->width();
    int h = ui->imageViewer->height();
    ui->imageViewer->setPixmap(QPixmap::fromImage(changedAfterSmoothingImage).scaled(w, h, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

}

void MainWindow::setSharpness(double value)
{
    if(ui->smoothingSlider->value() == 0.0 && ui->sharpnessSlider->value() == 0.0)
    {
        changedAfterSharpnessImage = readImage;
        changedAfterSmoothingImage = readImage;
    }
    sharpnessValue = value;
    changedAfterSharpnessImage = changedAfterSmoothingImage;
    sharpenImage(changedAfterSharpnessImage, 1.0, sharpnessValue);
    int w = ui->imageViewer->width();
    int h = ui->imageViewer->height();
    ui->imageViewer->setPixmap(QPixmap::fromImage(changedAfterSmoothingImage).scaled(w, h, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

void MainWindow::onSmoothingSliderValueChanged(int value)
{
    ui->smoothingSpinBox->setValue(value);
    setSmoothing(value);
}

void MainWindow::initMainWindow()
{
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void MainWindow::initWidgets()
{
    ui->heightScale->setEnabled(false);
    ui->sizesScale->setEnabled(false);
    ui->xAxisRotateAngle->setEnabled(false);
    ui->smoothingSlider->setEnabled(false);
    ui->sharpnessSlider->setEnabled(false);

    // signals
    QObject::connect(ui->heightScale,
             QOverload<double>::of(&QDoubleSpinBox::valueChanged),
             [=](double value) {
               if(!readImage.isNull())
               {
                 auto init_model_z =
                   max_pixel_val * value * ui->sizesScale->text().replace(",", ".").toDouble();
                 ui->heightValue->setValue(init_model_z);
               }
             });

    QObject::connect(ui->sizesScale,
             QOverload<double>::of(&QDoubleSpinBox::valueChanged),
             [=](double value) {
               if(!readImage.isNull())
               {
                 auto init_model_x = readImage.width() * value;
                 ui->widthValue->setValue(init_model_x);
                 auto init_model_y = readImage.height() * value;
                 ui->lengthValue->setValue(init_model_y);
                 auto init_model_z =
                   max_pixel_val * value * ui->heightScale->text().replace(",", ".").toDouble();
                 ui->heightValue->setValue(init_model_z);
               }
             });

    connect(ui->smoothingSlider, SIGNAL(valueChanged(int)), SLOT(onSmoothingSliderValueChanged(int)));
    connect(ui->smoothingSpinBox, SIGNAL(valueChanged(double)), SLOT(onSmoothingSpinBoxValueChanged(double)));
    connect(ui->sharpnessSlider, SIGNAL(valueChanged(int)), SLOT(onSharpnessSliderValueChanged(int)));
    connect(ui->sharpnessSpinBox, SIGNAL(valueChanged(double)), SLOT(onSharpnessSpinBoxValueChanged(double)));
}

void MainWindow::onSharpnessSliderValueChanged(int value)
{
    ui->sharpnessSpinBox->setValue(value);
    setSharpness(value);
}

void MainWindow::onSharpnessSpinBoxValueChanged(double value)
{
    ui->sharpnessSlider->setValue(value);
    setSharpness(value);
}

void MainWindow::onSmoothingSpinBoxValueChanged(double value)
{
    ui->smoothingSlider->setValue(value);
    setSmoothing(value);
}

void MainWindow::onProcessFinished(int code)
{
    QString stdout_buffer = pExecutedProcess->readAllStandardOutput();
    QString stderr_buffer = pExecutedProcess->readAllStandardError();

    QFile(TEMPORARY_IMAGE).remove();
    QMessageBox::information(this, "Process stdout Output", stdout_buffer);
    QMessageBox::information(this, "Process stderr Output", stderr_buffer);

    ui->convertStatusProgressBar->setMaximum(1);
    ui->convertStatusProgressBar->setMinimum(0);

    ui->convertStartButton->setEnabled(true);

    pExecutedProcess->close();
}

void MainWindow::onProcessStarted()
{
    qDebug() << "Process started.";
    ui->convertStartButton->setEnabled(false);
}

void MainWindow::startProcess(QString app, QStringList args)
{
    pExecutedProcess.reset(new QProcess());

    ui->convertStatusProgressBar->setMaximum(0);
    ui->convertStatusProgressBar->setMinimum(0);

    connect(pExecutedProcess.get(), SIGNAL(finished(int)), SLOT(onProcessFinished(int)));
    connect(pExecutedProcess.get(), SIGNAL(started()), SLOT(onProcessStarted()));

    pExecutedProcess->start(app, args);    
}

void MainWindow::showParameters()
{
    qDebug() << "Selected conversion mode: " << transformation_info.convertMode;
    qDebug() << "Input image filename: " << transformation_info.inputImageFilename;
    qDebug() << "Output model filename: " << transformation_info.outputModelFilename;
    qDebug() << "Output folder path: " << transformation_info.outputFolderPath;
    qDebug() << "Texture image: " << transformation_info.inputTextureImageFilename;
    qDebug() << "Grayscale option: " << transformation_info.grayscale;
    qDebug() << "Inverse-color option: " << transformation_info.colorInverse;
    qDebug() << "Smoothing: " << transformation_info.smoothing;
    qDebug() << "Smoothing coefficient: " << transformation_info.smoothingCoeff;
    qDebug() << "Sharpness: " << transformation_info.sharpness;
    qDebug() << "Sharpness coefficient: " << transformation_info.sharpnessCoeff;
    qDebug() << "Height scale: " << transformation_info.heightScale;
    qDebug() << "Dimension scale: " << transformation_info.sizesScale;
    qDebug() << "X axis rotate angle: " << transformation_info.xAxisRotateAngle;
}

QStringList MainWindow::createArgs()
{
    QString argsCmdLine = "";
    QStringList args;

    args << "-i" << transformation_info.inputImageFilename;
    args << "-o" << transformation_info.outputModelFilename;
    args << "-f" << transformation_info.outputFolderPath;
    if(!transformation_info.inputTextureImageFilename.isEmpty())
        args << "-t" << transformation_info.inputTextureImageFilename;
    // check convert mode
    switch (transformation_info.convertMode)
    {
        case 0:
            args << "-k";
            break;
        case 1:
            args << "-p";
            break;
        case 2:
            args << "-s";
            break;
        default:
            args << "-k";
            break;
    }
    if(transformation_info.colorInverse)
        args << "-c";
    if(transformation_info.grayscale)
        args << "-g";
    args << "-z" << QString::number(transformation_info.heightScale);
    args << "-l" << QString::number(transformation_info.sizesScale);
    args << "-x" << QString::number(transformation_info.xAxisRotateAngle);
    return args;
}

bool MainWindow::isRequiredFieldsFilled()
{
    bool isInputImageSelected = !inputDataFilename.isEmpty();
    bool isOutputModelFilenamePresented = !outputModelFilename.isEmpty();
    bool isOutputFolderPathPresented = !outputFolderPath.isEmpty();

    return isInputImageSelected && isOutputFolderPathPresented && isOutputModelFilenamePresented;
}

void MainWindow::fillProgramDataStructure()
{
    outputModelFilename = ui->outputFilenameEdit->text();
    if(outputModelFilename.isNull() || outputModelFilename.isEmpty())
        return;
    if(inputFileType == INPUT_FILE_TYPE::IMAGE)
        transformation_info.inputImageFilename = QString("'%1'").arg(TEMPORARY_IMAGE);
    else
        transformation_info.inputImageFilename = QString("'%1'").arg(inputDataFilename);
    transformation_info.outputFolderPath = QString("'%1'").arg(outputFolderPath);
    transformation_info.outputModelFilename = QString("'%1'").arg(outputModelFilename);
    transformation_info.inputTextureImageFilename = QString("'%1'").arg(inputTextureImageFilename);
    transformation_info.smoothing = isSmoothingOption;
    if(ui->smoothingOptionCheckbox->isChecked())
        transformation_info.smoothingCoeff = ui->smoothingSpinBox->value();
    transformation_info.sharpness = isSharpnessOption;
    if(ui->sharpnessOptionCheckbox->isChecked())
        transformation_info.sharpnessCoeff = ui->sharpnessSpinBox->value();
    transformation_info.grayscale = isGrayscaleOption;
    transformation_info.colorInverse = isColorInverseOption;
    if(ui->zAxisScaleLabel->isChecked() && !ui->heightScale->text().isEmpty())
        transformation_info.heightScale = ui->heightScale->text().replace(",", ".").toDouble();
    if(ui->xAxisRotateLabel->isChecked() && !ui->xAxisRotateAngle->text().isEmpty())
        transformation_info.xAxisRotateAngle = ui->xAxisRotateAngle->text().replace(",", ".").toDouble();
    if(ui->dimensionScaleLabel->isChecked() && !ui->sizesScale->text().isEmpty())
        transformation_info.sizesScale = ui->sizesScale->text().replace(",", ".").toDouble();
}

void MainWindow::on_convertStartButton_clicked()
{
    if(!isRequiredFieldsFilled())
    {
        QMessageBox::critical(this, "Error", "Complete the required fields!");
        return;
    }

    if(inputFileType == INPUT_FILE_TYPE::IMAGE)
    {
        sharpnessValue = ui->sharpnessSpinBox->value();
        smoothingValue = ui->smoothingSpinBox->value();

        QImage image(readImage);
        qt_blurImage(image, smoothingValue, true);
        sharpenImage(image, 1.0, sharpnessValue);
        image.save(TEMPORARY_IMAGE);
    }

    fillProgramDataStructure();
    showParameters();
    QStringList args = createArgs();

    startProcess("lirs_wct_console", args);
}

void MainWindow::on_textureImageSelect_clicked()
{
    QString selfilter = tr("InputFiles (*.jpg *.jpeg *.png");
    QString sc = tr("Images (*.jpg *.jpeg *.png)");
    QString fileName = QFileDialog::getOpenFileName(this, "Open a texture file", "/home", sc, &selfilter);
    if(fileName.isNull() || fileName.isEmpty())
        return;

    inputTextureImageFilename = fileName;
    ui->statusBar->setFocus();
    ui->selectedTexturePath->setText(inputTextureImageFilename);

    QPixmap pix(inputTextureImageFilename);
    int w = ui->textureViewer->width();
    int h = ui->textureViewer->height();
    ui->textureViewer->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

void MainWindow::on_selectImageButton_clicked()
{
    QString selfilter = tr("InputFiles (*.jpg *.jpeg *.png *.stl)");
    QString sc = tr("Images (*.jpg *.jpeg *.png);;STL (*.stl)");
    QString fileNamePath = QFileDialog::getOpenFileName(this, "Open an input file", "/home", sc, &selfilter);
    if(fileNamePath.isNull() || fileNamePath.isEmpty())
        return;
    QFileInfo fi(fileNamePath);
    QString extesion = fi.fileName().right(fi.fileName().lastIndexOf("."));
    QString fileName = fi.fileName();
    QString fileNameWithoutExtension = fileName.left(fileName.lastIndexOf("."));
    ui->outputFilenameEdit->setText(fileNameWithoutExtension);
    if(extesion != ".stl")
    {
        ui->pngToDaeSelect->setEnabled(true);
        ui->pngToStlSelect->setEnabled(true);
        ui->stlToDaeSelect->setEnabled(false);
        ui->pngToDaeSelect->click();

        readImage.load(fileNamePath);

        auto sizes_scale = ui->sizesScale->text().replace(",", ".").toDouble();
        ui->widthValue->setValue(readImage.width() * sizes_scale);
        ui->lengthValue->setValue(readImage.height() * sizes_scale);
        auto grayscale_img = readImage.convertToFormat(QImage::Format_Grayscale8);

        auto fmpv = [](QImage& image) {
          auto bits = image.bits();
          uint8_t max_value = bits[0];
          for(int y = 0; y < image.height(); ++y)
          {
            for(int x = 0; x < image.width(); ++x)
            {
              if(max_value < bits[y + x * y])
                max_value = bits[y + x * y];
            }
          }
          return max_value;
        };

        max_pixel_val = fmpv(grayscale_img);
        ui->heightValue->setValue(max_pixel_val * 0.05 * 0.25);


        changedAfterSmoothingImage = readImage;
        changedAfterSharpnessImage = readImage;
        outputModelFilename = fileNameWithoutExtension;
        inputDataFilename = fileNamePath;
        imageFileName = fileName;

        QPixmap pix(fileNamePath);
        int w = ui->imageViewer->width();
        int h = ui->imageViewer->height();
        ui->imageViewer->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
    else
    {
        ui->pngToDaeSelect->setEnabled(false);
        ui->pngToStlSelect->setEnabled(false);
        ui->stlToDaeSelect->setEnabled(true);
        ui->stlToDaeSelect->click();

        inputFileType = INPUT_FILE_TYPE::STL;
        inputDataFilename = fileNamePath;
        ui->outputFilenameEdit->setText(fileNameWithoutExtension);
        outputModelFilename = fileNameWithoutExtension;
    }
}

void MainWindow::on_zAxisScaleLabel_clicked()
{
    if(ui->zAxisScaleLabel->isChecked())
        ui->heightScale->setEnabled(true);
    else
        ui->heightScale->setEnabled(false);
}

void MainWindow::on_dimensionScaleLabel_clicked()
{
    if(ui->dimensionScaleLabel->isChecked())
        ui->sizesScale->setEnabled(true);
    else
        ui->sizesScale->setEnabled(false);
}

void MainWindow::on_xAxisRotateLabel_clicked()
{
    if(ui->xAxisRotateLabel->isChecked())
        ui->xAxisRotateAngle->setEnabled(true);
    else
        ui->xAxisRotateAngle->setEnabled(false);
}

void MainWindow::on_outputFolderSelect_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open a directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir.isNull() || dir.isEmpty())
        return;
    outputFolderPath = dir;

    ui->outputFolderPathEdit->setText(dir);
}

void MainWindow::on_pngToDaeSelect_clicked()
{
    transformation_info.convertMode = 0;
    ui->textureImageSelect->setEnabled(true);
}

void MainWindow::on_pngToStlSelect_clicked()
{
    transformation_info.convertMode = 1;
    ui->textureImageSelect->setEnabled(false);
}

void MainWindow::on_stlToDaeSelect_clicked()
{
    transformation_info.convertMode = 2;
    ui->textureImageSelect->setEnabled(true);
}

void MainWindow::on_smoothingOptionCheckbox_clicked()
{
    isSmoothingOption = !isSmoothingOption;
    ui->smoothingSlider->setEnabled(isSmoothingOption);
    ui->smoothingSpinBox->setReadOnly(!isSmoothingOption);
}

void MainWindow::on_sharpnessOptionCheckbox_clicked()
{
    isSharpnessOption = !isSharpnessOption;
    ui->sharpnessSlider->setEnabled(isSharpnessOption);
    ui->sharpnessSpinBox->setReadOnly(!isSharpnessOption);
}

void MainWindow::on_colorInverseOptionCheckbox_clicked()
{
    isColorInverseOption = !isColorInverseOption;
}

void MainWindow::on_grayscaleOptionCheckbox_clicked()
{    
    isGrayscaleOption = !isGrayscaleOption;
}
