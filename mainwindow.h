#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QEvent>
#include <QResizeEvent>
#include <QLayout>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QImageReader>
#include <QTextCodec>
#include <QScrollBar>
#include <QMessageBox>
#include <QByteArray>


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
    void on_pushButton_clicked();
    void resizeEvent(QResizeEvent *event);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;

    void init();

    bool openFile();

    QByteArray ScanImageFormat(QString imagePath);

    int currentImage();

    void setHorizontalSingleStep();

private:
    QString folderPath;

    int imageTotalHeight;

    int imageCount;

    int scrollBarTotalHeight;

    QStringList supportImageList;            // 可開啟的圖片格式(EX:*.jpg *.png *.webp)

    QList<QByteArray> supportedImageFormats; // 可瀏覽的圖片格式(EX:jpg, png, webp)
};

#endif // MAINWINDOW_H
