#ifndef TOUCHSTONEDATA_H
#define TOUCHSTONEDATA_H

#include <QAbstractListModel>

class TouchstoneData : public QObject
{
    Q_OBJECT

public:
    explicit TouchstoneData(QObject *parent = nullptr);

    Q_INVOKABLE int rowCount() const;
    Q_INVOKABLE QVariantMap get(int row) const;

    Q_INVOKABLE void loadData(QString filePath, int viewportWidth, int viewportHeight);

signals:
    // signal intended to be connected in QML MUST follow lowerCamelCase style in C++
    // to connect in QML the signal MUST be mentioned in the same style but starting with 'on'
    // e.g. if 'fileLoadFailure' then 'onFileLoadFailure'
    void fileLoadFailure(const QString& filePath);
    void fileLoadSuccess(const QString& filePath);

private:
    struct TouchstoneDataItem
    {
        double freq;
        struct
        {
            double re;
            double im;
        } s11;
    };

    QList<TouchstoneDataItem> m_data;
};


#endif // TOUCHSTONEDATA_H
