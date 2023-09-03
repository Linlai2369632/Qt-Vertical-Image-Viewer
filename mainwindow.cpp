#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString iconPath = QDir::currentPath() + "/th.jpg";
    if(QFile::exists(iconPath)) {
        QIcon con(iconPath);
        ui->pushButton->setIcon(con);
    }
    else {
        ui->pushButton->setText("Open");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init() {
    imageTotalHeight = 0;
    imageCount = 0;
    scrollBarTotalHeight = 0;

    supportedImageFormats = QImageReader::supportedImageFormats();
    foreach(QByteArray byteArray , supportedImageFormats) {
        QString str = QString::fromUtf8(byteArray);
        supportImageList.append("*." + str);
    }
//    qDebug() << supportImageList;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    ui->scrollArea->resize(size);
    if (ui->scrollArea->widget()) {
        ui->scrollArea->widget()->resize(size);
    }
    // 設定垂直滾動條每次的位移量
    setHorizontalSingleStep();

    // 設定 QLabel 的大小為視窗大小
//    QScrollArea *scrollArea = qobject_cast<QScrollArea*>(centralWidget());
//    if (scrollArea) {
//        QLabel *label = qobject_cast<QLabel*>(scrollArea->widget());
//        if (label) {
//            label->setFixedSize(ui->scrollArea->viewport()->size());
////            label->resize(ui->scrollArea->viewport()->size());
//        }
//    }
}

bool MainWindow::openFile() {
    folderPath = QFileDialog::getExistingDirectory(this, "請選擇資料夾", ui->lineEdit->text());
    if(folderPath.isEmpty())
        return false;
    ui->lineEdit->setText(folderPath);
    return true;
}

QByteArray MainWindow::ScanImageFormat(QString imagePath) {
    foreach (QByteArray format, supportedImageFormats) {
        QImageReader reader(imagePath, format);
        if(reader.canRead()) {
//            qDebug() << "Image format " << format;
            return format;
        }
    }
    qDebug() << "Skipped file (not a supported image format):" << imagePath;
    return NULL;
}

void MainWindow::on_pushButton_clicked()
{
    init();
    // 開啟資料夾
    if(openFile() == false) {
        return;
    }

    // 新增 QScrollArea 控制捲動區域
    QScrollArea* scrollArea = new QScrollArea(ui->scrollArea);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->scrollArea->setFrameShape(QFrame::Box);
    ui->scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // 新增一個 QWidget 作為 QScrollArea 的 viewport
    QWidget* scrollWidget = new QWidget(scrollArea);
    scrollWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    scrollWidget->setLayout(new QVBoxLayout(scrollWidget)); //設置成直立式
    scrollWidget->layout()->setSpacing(0);
//    scrollWidget->layout()->setMargin(0);

    int scrollAreaWidth = ui->scrollArea->viewport()->width();
    QDir dir(folderPath);
    QDirIterator it(folderPath, supportImageList , QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString imagePath = it.next();
        qDebug() << imagePath;
        // 檢測圖片的編碼格式
        QByteArray format = ScanImageFormat((imagePath));
        if(format.isNull())
            return;
        QImageReader reader(imagePath, format);
        QImage image = reader.read();
        if(image.isNull()) {
            qDebug() << "Image read failed:" << imagePath;
            return;
        }
        QPixmap pixmap = QPixmap::fromImage(image); // 載入圖片
        QLabel* imageLabel = new QLabel(scrollWidget);
        int w = pixmap.width();
        int h = pixmap.height();
        if(w > (scrollAreaWidth - 50)) {
            w = scrollAreaWidth - 50;
        }
        pixmap = pixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation); // 保持原始的長寬比例
        imageLabel->setPixmap(pixmap);
        imageLabel->setAlignment(Qt::AlignCenter); // 讓圖片維持置中
        scrollWidget->layout()->addWidget(imageLabel);
        imageCount++;
        imageTotalHeight += h;
    }
    // 設定 scrollWidget 為 scrollArea 的 viewport，以便顯示所有影象
    ui->scrollArea->setWidget(scrollWidget);
    // 設定垂直滾動條每次的位移量
    setHorizontalSingleStep();
}

void MainWindow::setHorizontalSingleStep() {
    scrollBarTotalHeight = 0;
    scrollBarTotalHeight = ui->scrollArea->verticalScrollBar()->maximum();
    if(imageCount == 0 || scrollBarTotalHeight == 0) {
        return;
    }
    int singleStep = qMax(1, (scrollBarTotalHeight / imageCount) / 25);
    ui->scrollArea->verticalScrollBar()->setSingleStep(singleStep);
}

int MainWindow::currentImage()
{
    int scrollPosition = ui->scrollArea->verticalScrollBar()->value();
    int totalImageHeight = 0;
    int imageIndex = 1;
    QList<QLabel*> labels = ui->scrollArea->widget()->findChildren<QLabel*>();
    foreach (QLabel* label, labels) {
        QPixmap originalPixmap  = *label->pixmap();
        int imageHeight = originalPixmap.height();
        totalImageHeight += imageHeight;
        if(totalImageHeight > scrollPosition)
            return imageIndex;
        imageIndex++;
    }
    return 0;
}

void MainWindow::on_pushButton_2_clicked()
{
    int imageIndex, totalScrollOffset = 0;
    // 取得當前圖片頁數
    imageIndex = currentImage();
    qDebug() << "在第" << imageIndex << "張圖片";
    int scrollPosition = ui->scrollArea->verticalScrollBar()->value();

    QList<QLabel*> labels = ui->scrollArea->widget()->findChildren<QLabel*>();
    foreach(QLabel* label, labels) {
        QPixmap originalPixmap  = *label->pixmap();

        int originalWidth = originalPixmap.width();
        int originalHeight = originalPixmap.height();

        int newWidth = originalWidth * 1.1; // 圖片寬度放大 1.1 倍
        int newHeight = originalHeight * 1.1; // 圖片高度放大 1.1 倍

        QPixmap scaledPixmap = originalPixmap.scaled(newWidth, newHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(scaledPixmap);

        label->setAlignment(Qt::AlignCenter); // 將對齊方式設置為居中
        label->setScaledContents(false);
        // 計算修正進度
        if(imageIndex > 1) {
            totalScrollOffset += (newHeight - originalHeight);
            imageIndex--;
        }
    }

    ui->scrollArea->verticalScrollBar()->setValue(scrollPosition + totalScrollOffset);

    setHorizontalSingleStep();
}

void MainWindow::on_pushButton_3_clicked()
{
    int imageIndex, totalScrollOffset = 0;
    // 取得當前圖片頁數
    imageIndex = currentImage();
    qDebug() << "在第" << imageIndex << "張圖片";
    int scrollPosition = ui->scrollArea->verticalScrollBar()->value();

    QList<QLabel*> labels = ui->scrollArea->widget()->findChildren<QLabel*>();
    foreach(QLabel* label, labels) {
        QPixmap originalPixmap = *label->pixmap();

        int originalWidth = originalPixmap.width();
        int originalHeight = originalPixmap.height();

        int newWidth = originalWidth * 0.9;   // 圖片寬度縮小 0.9 倍
        int newHeight = originalHeight * 0.9; // 圖片高度縮小 0.9 倍

        QPixmap scaledPixmap = originalPixmap.scaled(newWidth, newHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(scaledPixmap);

        label->setAlignment(Qt::AlignCenter); // 將對齊方式設置為居中
        label->setScaledContents(false);
        // 計算修正進度
        if(imageIndex > 1) {
            totalScrollOffset += (newHeight - originalHeight);
            imageIndex--;
        }
    }

    ui->scrollArea->verticalScrollBar()->setValue(scrollPosition + totalScrollOffset);

    setHorizontalSingleStep();
}

