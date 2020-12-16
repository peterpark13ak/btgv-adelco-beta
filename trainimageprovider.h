#ifndef TRAINIMAGEPROVIDER_H
#define TRAINIMAGEPROVIDER_H

#include <QObject>
#include <QQuickImageProvider>

class TrainImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit TrainImageProvider(int providerId = 1, QObject *parent = 0);
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    void getImageHSI(int x, int y, int*);

signals:
    void imageUpdated();

public slots:
    void updatePixmap(QPixmap*);

private:
    QPixmap pixmap;
    int providerId;
};

#endif // TRAINIMAGEPROVIDER_H
