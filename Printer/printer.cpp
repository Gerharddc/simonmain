#include "printer.h"
#include <thread>

// Global singleton printer
Printer GlobalPrinter;

Printer::Printer(QObject *parent) : QObject(parent)
{
    printProcess = new QProcess();
    QObject::connect(printProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readPrinterOutput()));
    QObject::connect(printProcess, SIGNAL(finished(int)), this, SLOT(printerFinished()));
}

Printer::~Printer()
{
    delete printProcess;
}

void Printer::readPrinterOutput()
{
    // TODO: implement
}

void Printer::printerFinished()
{
    m_printing = false;
    emit printingChanged();
}

void Printer::startPrintProcess(QString path)
{
    // TODO finsish

    QStringList arguments;
    arguments << "-o" << path;

    printProcess->start("pronsole.py", arguments);
}

void Printer::startPrint(QString path)
{
    // TODO: implement

    std::thread([=]() {
        // Starting the new process can be slow
        QMetaObject::invokeMethod(this, "startPrintProcess", Q_ARG(QString, path));
    }).detach();

    // TODO: check for repsonse that says we started printing
}

void Printer::pauseResume()
{
    // TODO: implement

    m_paused = !m_paused;
    emit pausedChanged();
}

void Printer::stopPrint()
{
    // TODO: implement
}

void Printer::emergencyStop()
{
    // TODO: implement
}

void Printer::homeAll()
{
    // TODO: implement
}

void Printer::homeX()
{
    // TODO: implement
}

void Printer::homeY()
{
    // TODO: implement
}

void Printer::homeZ()
{
    // TODO: implement
}

void Printer::move(float x, float y, float z)
{
    // TODO: implement
}

void Printer::moveX(float distance)
{
    // TODO: implement
}

void Printer::moveY(float distance)
{
    // TODO: implement
}

void Printer::extrude(float e)
{
    // TODO: implement
}

void Printer::moveZ(float distance)
{
    // TODO: implement
}

void Printer::setTargetTemp(float target)
{
    // TODO: implement
}

void Printer::setHeating(bool val)
{
    // TODO: implement
}
