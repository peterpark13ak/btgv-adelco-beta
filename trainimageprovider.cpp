#include "trainimageprovider.h"
#include "qtimer.h"
#include "qpainter.h"
#include "qdatetime.h"

TrainImageProvider::TrainImageProvider(int providerId, QObject *parent) : QObject(parent), QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    this->pixmap = QPixmap(500, 400);
    this->pixmap.fill(QColor("#303030").rgba());
    this->providerId = providerId;
}

/*!
 * \brief TrainImageProvider::requestPixmap
 * \param id
 * \param size
 * \param requestedSize
 * \return
 */
QPixmap TrainImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);

    int width = 100;
    int height = 100;

    const QPixmap *resultPixmap;

    if (this->pixmap.isNull()) {
        this->pixmap = QPixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
                      requestedSize.height() > 0 ? requestedSize.height() : height);
        this->pixmap.fill(QColor("#303030").rgba());
    }

    resultPixmap = &(this->pixmap);

    if (size)
        *size = QSize(resultPixmap->width(), resultPixmap->height());

    return *resultPixmap;
}

/*!
 * \brief TrainImageProvider::updatePixmap
 * \param input
 * \param result
 */
void TrainImageProvider::updatePixmap(QPixmap *input) {

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

    emit imageUpdated();
}

/*!
 * \brief TrainImageProvider::getImageHSI
 * \param x
 * \param y
 * \return
 */
void TrainImageProvider::getImageHSI(int x, int y, int *colorComponents) {

    int hue = 0, saturation = 0, intensity = 0;

    if ( ! this->pixmap.isNull() ) {

        QImage image = this->pixmap.toImage();

        if (x >= 0 && y >=0 && x < image.width() && y < image.height() ) {
            QColor color = image.pixelColor(x, y);
            hue = color.hslHueF() * 255;
            saturation = color.hslSaturationF() * 255;
            intensity = color.lightnessF() * 255;
        }
    }

    // int *result = new int(3);
    colorComponents[0] = hue;
    colorComponents[1] = saturation;
    colorComponents[2] = intensity;
}
