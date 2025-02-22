#ifndef SIGNALSOURCE_H
#define SIGNALSOURCE_H

#include <QObject>

class SignalSource : public QObject
{
    Q_OBJECT
public:
    explicit SignalSource(QObject *parent = nullptr);

    void setValue(int newValue);

signals:
    int valueChanged(int newValue);
};

#endif // SIGNALSOURCE_H
