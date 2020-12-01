#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <memory>
#include "structs.h"
#include "utils.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // Qt callbacks

    void on_convertStartButton_clicked();

    void on_selectImageButton_clicked();

    void on_pngToDaeSelect_clicked();

    void on_zAxisScaleLabel_clicked();

    void on_dimensionScaleLabel_clicked();

    void on_xAxisRotateLabel_clicked();

    void on_outputFolderSelect_clicked();

    void on_pngToStlSelect_clicked();

    void on_stlToDaeSelect_clicked();

    void on_textureImageSelect_clicked();

    void on_smoothingOptionCheckbox_clicked();

    void on_sharpnessOptionCheckbox_clicked();

    void on_colorInverseOptionCheckbox_clicked();

    void on_grayscaleOptionCheckbox_clicked();

    void onProcessFinished(int);

    void onProcessStarted();

    void onSmoothingSliderValueChanged(int);

    void onSmoothingSpinBoxValueChanged(double);

    void onSharpnessSliderValueChanged(int);

    void onSharpnessSpinBoxValueChanged(double);


private:

    typedef enum _INPUT_FILE_TYPE
    {
        STL, IMAGE
    } INPUT_FILE_TYPE;

    Ui::MainWindow *ui;
    int WINDOW_WIDTH = 724;
    int WINDOW_HEIGHT = 621;
    QString TEMPORARY_IMAGE = "/tmp/PNG2DAEIMAGE.png";
    bool isZFieldReadOnly = true;
    bool isDimensionFieldReadOnly = true;
    bool isXRotateFieldReadOnly = true;
    bool isWindowResized = false;
    bool isSmoothingOption = false;
    bool isSharpnessOption = false;
    bool isColorInverseOption = false;
    bool isGrayscaleOption = false;
    double sharpnessValue, smoothingValue;
    INPUT_FILE_TYPE inputFileType = INPUT_FILE_TYPE::IMAGE;
    std::unique_ptr<QProcess> pExecutedProcess;
    QImage readImage, changedAfterSmoothingImage, changedAfterSharpnessImage;
    TRANSFORMATION_INFO transformation_info;
    QString inputDataFilename = "", outputModelFilename, outputFolderPath, inputTextureImageFilename = "";
    QString imageFileName;
    uint8_t max_pixel_val;

    void initMainWindow();
    void initWidgets();
    void startProcess(QString app, QStringList args);
    void fillProgramDataStructure();
    void showParameters();
    QStringList createArgs();
    bool isRequiredFieldsFilled();
    void setSmoothing(double value);
    void setSharpness(double value);
};

#endif // MAINWINDOW_H
