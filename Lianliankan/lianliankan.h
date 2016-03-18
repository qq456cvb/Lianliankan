#ifndef LIANLIANKAN_H
#define LIANLIANKAN_H

#include <QtWidgets/QMainWindow>
#include "ui_lianliankan.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "utils.h"
#include <interception.h>

using namespace std;

namespace scancode {
  enum {
    esc = 0x01,
    ctrl = 0x1D,
    alt = 0x38,
    del = 0x53,
    SCANCODE_ESC = 0x01
  };
}

class Lianliankan : public QMainWindow
{
    Q_OBJECT

public:
    Lianliankan(QWidget *parent = 0);
    ~Lianliankan();
    public slots:
    void on_beginButton_clicked();
    void ScanInput();

private:
  InterceptionContext context;
  InterceptionDevice device;
  InterceptionStroke stroke;
  std::string dataPath = "C:\\Users\\Neil\\Desktop\\icon\\";
  cv::Mat map;
    QPixmap originalPixmap;
    Ui::LianliankanClass ui;
};

#endif // LIANLIANKAN_H
