#include "Test.h"
#include <QtAndroid>
#include <QAndroidJniExceptionCleaner>
#include <QtAndroidExtras>

CTest::CTest(QObject *parent) : QObject(parent)
{   
}

int CTest::TestTimeout()
{
    QAndroidJniObject::callStaticMethod<void>(
            "org/KangLinStudio/QtAndroidUtils/Test",
            "timeOut"
           );
    return 0;
}