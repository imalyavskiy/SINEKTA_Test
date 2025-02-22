#ifndef TOUCHSTONEDATAREADER_H
#define TOUCHSTONEDATAREADER_H

#include <QObject>

class TouchstoneDataReader : public QObject
{
    Q_OBJECT
public:
    explicit TouchstoneDataReader(QObject *parent = nullptr);

signals:
};

#endif // TOUCHSTONEDATAREADER_H
