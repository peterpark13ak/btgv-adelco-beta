#include "bufferimageprovider.h"

BufferImageProvider::BufferImageProvider( int providerId, QObject *parent) : QObject(parent), QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    this->providerId = providerId;
    initializeFailedImages();
}


void BufferImageProvider::initializeFailedImages() {

    this->failedPixmaps.clear();

    this->pixmap = QPixmap(500, 400);
    this->pixmap.fill(QColor("#303030").rgba());

    // fill in the queue with 5 empty images
    this->failedPixmaps.enqueue(this->pixmap);
    this->failedPixmaps.enqueue(this->pixmap);
    this->failedPixmaps.enqueue(this->pixmap);
    this->failedPixmaps.enqueue(this->pixmap);
    this->failedPixmaps.enqueue(this->pixmap);
}

/*!
 * \brief BufferImageProvider::clearFailedImages
 */
void BufferImageProvider::clearFailedImages() {
    initializeFailedImages();
}

int BufferImageProvider::getProviderId() {
    return this->providerId;
}

/**
 * @brief BufferImageProvider::requestPixmap
 * @param id
 * @param size
 * @param requestedSize
 * @return
 */
QPixmap BufferImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
   int width = 100;
   int height = 100;

   const QPixmap *resultPixmap;
   int imgIndex = id.at(1).digitValue();

   if (QString::compare(id, "backup") == 0) {
       imgIndex = 10;
   }

   switch (imgIndex) {
   case 0:
   case 1:
   case 2:
   case 3:
   case 4:
       resultPixmap = &(this->failedPixmaps.at(imgIndex));
       break;
   case 10:
       resultPixmap = &imageBackup;
       pixmap = QPixmap(imageBackup);
       break;
   default:
       if (this->pixmap.isNull()) {
           this->pixmap = QPixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
                          requestedSize.height() > 0 ? requestedSize.height() : height);
           this->pixmap.fill(QColor("#303030").rgba());
       }

       resultPixmap = &(this->pixmap);
   }

   if (size)
       *size = QSize(resultPixmap->width(), resultPixmap->height());

   return *resultPixmap;
}

/**
 * @brief BufferImageProvider::updatePixmap
 * @param pixmap
 */
void BufferImageProvider::appendPixmap(QPixmap *input, bool result) {

    this->imageBackup = QPixmap(this->pixmap);
    this->pixmap = QPixmap(*input);

    QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    timeStamp += " Inspection" + QString::number(this->providerId + 1);

    QRect imgRect = this->pixmap.rect();
    int imgWidth = imgRect.width();
    int imgHeight = imgRect.height();
    int labelWidth = imgWidth/3;
    int labelHeight = imgHeight/20;
    int fontSize = imgHeight/50;

    imgRect.setTopLeft(QPoint(imgWidth - labelWidth - 5, imgHeight - labelHeight -5));

    QPainter painter(&this->pixmap);
    painter.setBrush(QBrush(QColor(0, 0, 0, 128)));
    painter.setFont(QFont("Arial", fontSize));
    painter.drawRect(imgRect);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(imgRect, Qt::AlignCenter, timeStamp);

    if (! result) {
        this->failedPixmaps.dequeue();
        this->failedPixmaps.enqueue(this->pixmap);
    }

    emit newImage(result);
}

void BufferImageProvider::appendFailedPixmap(QPixmap *input, int inspectionId) {
    qDebug() << "failed Image loaded" ;

    this->imageBackup = QPixmap(this->pixmap);
    this->pixmap = QPixmap(*input);

    QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    timeStamp += " Inspection" + QString::number(inspectionId + 1);

    QRect imgRect = this->pixmap.rect();
    int imgWidth = imgRect.width();
    int imgHeight = imgRect.height();
    int labelWidth = imgWidth/3;
    int labelHeight = imgHeight/20;
    int fontSize = imgHeight/50;

    imgRect.setTopLeft(QPoint(imgWidth - labelWidth - 5, imgHeight - labelHeight -5));

    QPainter painter(&this->pixmap);
    painter.setBrush(QBrush(QColor(0, 0, 0, 128)));
    painter.setFont(QFont("Arial", fontSize));
    painter.drawRect(imgRect);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(imgRect, Qt::AlignCenter, timeStamp);

    this->failedPixmaps.dequeue();
    this->failedPixmaps.enqueue(this->pixmap);

//    emit newImage(result);
}

/**
 * @brief BufferImageProvider::saveToFile
 * @param pixmap
 */
void BufferImageProvider::saveToFile(QPixmap pixmap, QString fname) {
    QFile file(fname + ".png");
    file.open(QIODevice::WriteOnly);
    pixmap.save(&file, "PNG");
}

