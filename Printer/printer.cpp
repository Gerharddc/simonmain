#include "printer.h"
#include <thread>
#include <iostream>
#include <fstream>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

// Global singleton printer
Printer GlobalPrinter;
static QSerialPort *serial = nullptr;
static std::thread PrintThread;
static volatile bool WaitingForOk = false;
static volatile bool StopPrintThread = false;

Printer::Printer(QObject *parent) : QObject(parent)
{
}

// This has to be called from the main thread
void Printer::Connect()
{
    serial = new QSerialPort("rfcomm0");
    serial->setBaudRate(9600);

    if (serial->open(QSerialPort::ReadWrite))
    {
        QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(readPrinterOutput()));
        //QObject::connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(readPrinterOutput()));
        serial->waitForReadyRead(1000);
        qDebug() << serial->readAll();
        qDebug() << "Opened";
    }
    else
        qDebug() << "Couldn't open serial";
}

Printer::~Printer()
{
    if (serial != nullptr)
    {
        serial->close();
        serial->disconnect();
        delete serial;
    }

    if (m_printing)
    {
        StopPrintThread = true;
        PrintThread.join();
    }
}

void Printer::readPrinterOutput()
{
    while (serial->canReadLine())
    {
        QString line(serial->readLine());

        if (line.contains("ok"))
            WaitingForOk = false;

        qDebug() << line;
    }
}

void Printer::printerFinished()
{
    m_printing = false;
    emit printingChanged();
}

void Printer::sendCommand(QString cmd)
{
    if (serial != nullptr && serial->isOpen())
    {
        serial->write((cmd + '\n').toUtf8());
        serial->flush();
        qDebug() << "Wrote: " << cmd;
    }
    else
        std::cout << "Serial port not open" << std::endl;
}

static void PrintFile(std::string path)
{
    std::ifstream is(path);

    if (is)
    {
        std::string line;
        while (std::getline(is, line) && !StopPrintThread)
        {
            if (serial != nullptr && serial->isOpen())
            {
                // TODO: check for legal numbers two
                if (line[0] != 'G' && line[0] != 'M')
                    continue;

                // Check for any target temperature commands
                if (line.find("M104 S") != std::string::npos)
                    GlobalPrinter.SignalTargetTemp(std::stof(line.substr(line.find('S') + 1)));

                // TODO: check when to enable fan

                serial->write((QString::fromStdString(line) + '\n').toUtf8());
                serial->flush();
                std::cout << "Printed: " << line << std::endl;

                // wait for an ok
                WaitingForOk = true;
                while (WaitingForOk && !StopPrintThread)
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
            }
            else
            {
                std::cout << "Serial port not open, print stopped" << std::endl;
                goto close;
            }

            // Stall the thread while pausing but check for a complete stop
            while (GlobalPrinter.paused() && !StopPrintThread)
                std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
    else
    {
        // TODO: report error
        std::cout << "Could not open file for print";
    }

    close:
    is.close();
    GlobalPrinter.SignalPrintStop();

    // Stop the heater
    GlobalPrinter.setHeating(false);
}

void Printer::SignalPrintStop()
{
    if (m_printing)
    {
        m_printing = false;
        emit printingChanged();
    }
}

void Printer::startPrint(QString path)
{
    if (m_printing)
        return;

    StopPrintThread = false;
    PrintThread = std::thread(PrintFile, path.toStdString());
    PrintThread.detach();
    m_printing = true;
    emit printingChanged();
}

void Printer::pauseResume()
{
    m_paused = !m_paused;
    emit pausedChanged();
}

void Printer::stopPrint()
{
    std::cout << "Stop" << std::endl;

    if (!m_printing)
        return;

    StopPrintThread = true;
}

void Printer::emergencyStop()
{
    stopPrint();
    sendCommand("M112");
}

void Printer::homeAll()
{
    sendCommand("G28");
}

void Printer::homeX()
{
    sendCommand("G28 X0");
}

void Printer::homeY()
{
    sendCommand("G28 Y0");
}

void Printer::homeZ()
{
    sendCommand("G28 Z0");
}

void Printer::move(float x, float y, float z)
{
    moveX(x);
    moveY(y);
    moveZ(z);
}

void Printer::moveX(float distance)
{
    sendCommand("G91"); // relative
    sendCommand("G1 X" + QString::number(distance));
}

void Printer::moveY(float distance)
{
    sendCommand("G91"); // relative
    sendCommand("G1 Y" + QString::number(distance));
}

void Printer::extrude(float e)
{
    sendCommand("G91"); // relative
    sendCommand("G1 E" + QString::number(e));
}

void Printer::moveZ(float distance)
{
    sendCommand("G91"); // relative
    sendCommand("G1 Z" + QString::number(distance));
}

void Printer::setTargetTemp(float target)
{
    // Check for change and validity
    if (target != m_targetTemp && target >= 0.0f)
    {
        if (m_heating)
            sendCommand("M104 S" + QString::number(target));

        m_targetTemp = target;
        emit targetTempChanged();
    }
}

void Printer::setHeating(bool val)
{
    if (val != m_heating)
    {
        if (val)
            sendCommand("M104 S" + QString::number(m_targetTemp));
        else
            sendCommand("M104 S0");

        m_heating = val;
        emit heatingChanged();
    }
}

void Printer::SignalTargetTemp(float temp)
{
    if (temp == 0 && m_heating)
    {
        m_heating = false;
        emit heatingChanged();
    }
    else if (temp != m_targetTemp)
    {
        m_targetTemp = temp;
        emit targetTempChanged();
    }
}
