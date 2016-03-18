#include "lianliankan.h"
#include <chrono>
#include <thread>
#include <QWindow>
#include <QScreen>
#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QImageWriter>
#include <QDebug>
#include <QtTest/QtTest>
#include <iostream>
#include "qt_windows.h"


Lianliankan::Lianliankan(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    map = cv::Mat::zeros(11, 19, CV_32SC1);
    raise_process_priority();

    context = interception_create_context();
    interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_LEFT_BUTTON_DOWN);
    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);

    std::thread t(&Lianliankan::ScanInput, this);
    t.detach();
}

Lianliankan::~Lianliankan()
{
  interception_destroy_context(context);
}

void Lianliankan::ScanInput()
{
  while (interception_receive(context, device = interception_wait(context), &stroke, 1) > 0)
  {
    if (interception_is_keyboard(device))
    {
      InterceptionKeyStroke &keystroke = *(InterceptionKeyStroke *)&stroke;

      cout << "INTERCEPTION_KEYBOARD(" << device - INTERCEPTION_KEYBOARD(0) << ")" << endl;

      if (keystroke.code == scancode::SCANCODE_ESC) {
        interception_send(context, device, &stroke, 1);
        interception_destroy_context(context);
        this->destroy();
        break;
      }
    }

    if (interception_is_mouse(device))
    {
      cout << "INTERCEPTION_MOUSE(" << device - INTERCEPTION_MOUSE(0) << ")" << endl;
    }
    interception_send(context, device, &stroke, 1);
  }
}

// If inImage exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inImage's
// data with the cv::Mat directly
//    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
inline cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true)
{
  switch (inImage.format())
  {
    // 8-bit, 4 channel
  case QImage::Format_RGB32:
  {
    cv::Mat  mat(inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine());

    return (inCloneImageData ? mat.clone() : mat);
  }

  // 8-bit, 3 channel
  case QImage::Format_RGB888:
  {
    if (!inCloneImageData)
      qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

    QImage   swapped = inImage.rgbSwapped();

    return cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine()).clone();
  }

  // 8-bit, 1 channel
  case QImage::Format_Indexed8:
  {
    cv::Mat  mat(inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine());

    return (inCloneImageData ? mat.clone() : mat);
  }

  default:
    qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
    break;
  }

  return cv::Mat();
}

// If inPixmap exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inPixmap's data
// with the cv::Mat directly
//    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
inline cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true)
{
  return QImageToCvMat(inPixmap.toImage(), inCloneImageData);
}

inline bool ValidPath(const cv::Point2f& p1, const cv::Point2f& p2, const cv::Mat& map)
{
  cv::Rect rect(cv::Point(), map.size());
  if (p1.y > p2.y)
  {
    return ValidPath(p2, p1, map);
  }

  // check horizontal
  int p1x1 = p1.x-1, p1x2 = p1.x+1;
  while (rect.contains(cv::Point2f(p1x1, p1.y)) && map.at<int>(p1.y, p1x1) == 0)
  {
    p1x1--;
  }
  p1x1++;
  while (rect.contains(cv::Point2f(p1x2, p1.y)) && map.at<int>(p1.y, p1x2) == 0)
  {
    p1x2++;
  }
  p1x2--;

  int p2x1 = p2.x - 1, p2x2 = p2.x + 1;
  while (rect.contains(cv::Point2f(p2x1, p2.y)) && map.at<int>(p2.y, p2x1) == 0)
  {
    p2x1--;
  }
  p2x1++;
  while (rect.contains(cv::Point2f(p2x2, p2.y)) && map.at<int>(p2.y, p2x2) == 0)
  {
    p2x2++;
  }
  p2x2--;

  for (size_t i = MAX(p1x1, p2x1); i <= MIN(p1x2, p2x2); i++)
  {
    if (abs(p2.y - p1.y) <= 1)
    {
      return true;
    }
    bool blocked = false;
    for (size_t j = MIN(p1.y, p2.y)+1; j < MAX(p1.y, p2.y); j++)
    {
      if (map.at<int>(j, i) != 0)
      {
        blocked = true;
      }
    }
    if (!blocked)
    {
      return true;
    }
  }

  // check vertical
  int p1y1 = p1.y - 1, p1y2 = p1.y + 1;
  while (rect.contains(cv::Point2f(p1.x, p1y1)) && map.at<int>(p1y1, p1.x) == 0)
  {
    p1y1--;
  }
  p1y1++;
  while (rect.contains(cv::Point2f(p1.x, p1y2)) && map.at<int>(p1y2, p1.x) == 0)
  {
    p1y2++;
  }
  p1y2--;

  int p2y1 = p2.y - 1, p2y2 = p2.y + 1;
  while (rect.contains(cv::Point2f(p2.x, p2y1)) && map.at<int>(p2y1, p2.x) == 0)
  {
    p2y1--;
  }
  p2y1++;
  while (rect.contains(cv::Point2f(p2.x, p2y2)) && map.at<int>(p2y2, p2.x) == 0)
  {
    p2y2++;
  }
  p2y2--;

  for (size_t j = MAX(p1y1, p2y1); j <= MIN(p1y2, p2y2); j++)
  {
    if (abs(p2.x - p1.x) <= 1)
    {
      return true;
    }
    bool blocked = false;
    for (size_t i = MIN(p1.x, p2.x) + 1; i < MAX(p1.x, p2.x); i++)
    {
      if (map.at<int>(j, i) != 0)
      {
        blocked = true;
      }
    }
    if (!blocked)
    {
      return true;
    }
  }
  return false;
}

void Lianliankan::on_beginButton_clicked()
{
 // mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
  //return;
  QScreen *screen = QGuiApplication::primaryScreen();
  if (const QWindow *window = windowHandle())
    screen = window->screen();
  if (!screen)
    return;

  originalPixmap = screen->grabWindow(0);
  
  auto raw = QPixmapToCvMat(originalPixmap);
  //auto mat = cv::imread("C:\\Users\\Neil\\Desktop\\test.png");
  //cvtColor(mat, mat, CV_BGR2GRAY);

  auto mark = cv::imread("C:\\Users\\Neil\\Desktop\\icon\\mark.jpg");
  cv::cvtColor(raw, raw, CV_BGRA2BGR);
  //cv::imshow("raw", mat);
  //cv::waitKey();
  //cvtColor(mark, mark, CV_BGR2GRAY);

  // find left-up point
  cv::Mat matches0;
  cv::Point2f startPoint;
  matchTemplate(raw, mark, matches0, cv::TM_CCOEFF_NORMED);
  for (int i = 0; i < matches0.rows; i++) {
    for (int j = 0; j < matches0.cols; j++) {
      if (matches0.at<float>(i, j) > 0.95)
      {
        startPoint.x = j + 6;
        startPoint.y = i + 178;
        //circle(mat, Point2f(j+6, i+178), 5, Scalar(255), -1);
        break;
      }
    }
  }

  auto mat = raw(cv::Range(startPoint.y - 10, startPoint.y + 600 - 170), cv::Range(startPoint.x - 10, startPoint.x + 800));
  // read all icons
  std::vector<cv::Mat> templates;
  int i = 1;
  cv::Mat temp;
  while (1) {
    std::stringstream ss;
    ss << dataPath << i++ << ".jpg";
    cv::Mat temp = cv::imread(ss.str());
    if (temp.empty())
    {
      break;
    }
    //cvtColor(temp, temp, CV_BGR2GRAY);
    templates.push_back(temp);
  }

  cv::Mat result = mat.clone();
  for (size_t t = 0; t < templates.size();t++)
  {
    cv::Mat matches;
    matchTemplate(mat, templates[t], matches, cv::TM_CCOEFF_NORMED);
    for (int i = 0; i < matches.rows; i++) {
      for (int j = 0; j < matches.cols; j++) {
        if (matches.at<float>(i, j) > 0.95)
        {
          char text[5];
          sprintf(text, "%d", t+1);
          cv::putText(result, text, cv::Point2f(j, i), cv::HersheyFonts::FONT_HERSHEY_COMPLEX, 0.5, cv::Scalar(255, 255, 255));
          //cv::circle(mat, cv::Point2f(j, i), 5, cv::Scalar(255), -1);
          int row = (int)(i / 35.0 + 0.5);
          int col = (int)(j / 31.0 + 0.5);
          map.at<int>(row, col) = t + 1;
        }
      }
    }
  }

  
  //cv::imshow("screen", result);
  //cv::waitKey();
  std::cout << map << std::endl;
  while (1) {
    bool found = false;
    for (int i = 0; i < map.rows; i++) {
      for (int j = 0; j < map.cols; j++) {
        if (map.at<int>(i, j) != 0)
        {
          found = true;
          int tag = map.at<int>(i, j);
          for (int m = i; m < map.rows; m++) {
            for (int n = 0; n < map.cols; n++) {
              if (j == n && i == m)
              {
                continue;
              }
              if (map.at<int>(m, n) == tag) {
                if (ValidPath(cv::Point2f(j, i), cv::Point2f(n, m), map))
                {
                  Sleep(100);
                  SetCursorPos(startPoint.x + j * 31 + 15, startPoint.y + i * 35 + 17);
                  interception_send(context, device, &stroke, 1);
                  Sleep(100);
                  SetCursorPos(startPoint.x + n * 31 + 15, startPoint.y + m * 35 + 17);
                  interception_send(context, device, &stroke, 1);

                  map.at<int>(i, j) = map.at<int>(m, n) = 0;
                  std::cout << i << ", " << j << ": " << m << ", " << n << std::endl;
                  m = map.rows;
                  n = map.cols;
                }
              }
            }
          }
        }
      }
    }
    if (!found)
    {
      break;
    }
  }
  std::cout << map << std::endl;
  
}
