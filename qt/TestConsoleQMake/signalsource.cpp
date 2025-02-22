#include <iostream>
#include "signalsource.h"

SignalSource::SignalSource(QObject *parent)
    : QObject{parent}
{}

void SignalSource::setValue(int newValue)
{
    std::cout << "SignalSource::setValue(" << 10 << ")\n";
    emit valueChanged(newValue);
}
