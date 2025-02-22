#ifndef SIGNALSINK_H
#define SIGNALSINK_H

#include <QObject>

class SignalSink : public QObject
{
    Q_OBJECT
public:
    explicit SignalSink(QObject *parent = nullptr);
    explicit SignalSink(std::function<void(int)> f, QObject *parent = nullptr);

public slots:
    void onValueChanged(int newValue);
signals:

protected:
    std::function<void(int)> f_{nullptr};
};

#endif // SIGNALSINK_H
