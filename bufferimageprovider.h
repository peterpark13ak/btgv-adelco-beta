#ifndef BUFFERIMAGEPROVIDER_H
#define BUFFERIMAGEPROVIDER_H

#include <QObject>
#include <QQuickImageProvider>
#include <QQueue>
#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <QFile>

class BufferImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit BufferImageProvider(int providerId = 0, QObject *parent = 0);
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    void clearFailedImages();
    int getProviderId();

signals:
    void newImage(bool);

public slots:
    void appendPixmap(QPixmap*, bool);
    void appendFailedPixmap(QPixmap*, int);

private:
    QQueue<QPixmap> failedPixmaps;
    QPixmap pixmap;
    QPixmap imageBackup;
    int providerId;
    void saveToFile(QPixmap pixmap, QString fname);
    void initializeFailedImages();
};

#endif // BUFFERIMAGEPROVIDER_H
