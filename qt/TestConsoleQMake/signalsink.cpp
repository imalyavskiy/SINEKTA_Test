#include <iostream>
#include "signalsink.h"

SignalSink::SignalSink(QObject *parent)
    : QObject{parent}
{}

SignalSink::SignalSink(std::function<void (int)> f, QObject* parent)
    : QObject(parent)
    , f_(f)
{

}

void SignalSink::onValueChanged(int newValue)
{
    std::cout << "SignalSink::onValueChanged(" << 10 << ")\n" ;
    if(f_)
        f_(newValue);
}
