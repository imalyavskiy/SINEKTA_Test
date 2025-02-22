#include <iostream>
#include <QCoreApplication>

#include "signalsource.h"
#include "signalsink.h"

//int main(int argc, char *argv[])
int main()
{
//    CCoreApplication app(argc, argv);
    auto printFunc = [](int value){
        std::cout << "[](int value){ ... }" << "\n" ;
        std::cout << "New Value: " << value << "\n";
    };

    SignalSource signalSource;
    SignalSink signalSink(printFunc);

    {
        QMetaObject::Connection conn = QObject::connect(&signalSource, &SignalSource::valueChanged, &signalSink, &SignalSink::onValueChanged);
        signalSource.setValue(10);
        QObject::disconnect(conn);
    }

    {
        QMetaObject::Connection conn = QObject::connect(&signalSource, &SignalSource::valueChanged, printFunc);
        signalSource.setValue(10);
        QObject::disconnect(conn);
    }

    std::cout << "Hello world!" << std::endl;

    return 0;
//    return app.exec()
}
