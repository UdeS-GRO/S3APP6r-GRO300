#ifndef YOURMAINWINDOW_H
#define YOURMAINWINDOW_H

#include "mainwindow.h"
#include "robotdiag.h"

#include <QDebug>

class YourMainWindow : public MainWindow {
public:
    static const int UPDATE_RATE_MS = 1000;

    YourMainWindow()
        : MainWindow("/dev/ttyACM0", UPDATE_RATE_MS) // port name, update rate
    {
        robotdiag::init();
    }

    ~YourMainWindow()
    {
        robotdiag::stop_and_join();
    }

    // Available functions

    // sendMessage(QString JSONstring); send JSON messages to the arduino
    // setUpdateRate(int rateMs); change preriodic loop frequency

    void onMessageReceived(QString msg) override {

        //Function executed on message received
        qDebug() << "Message received from arduino" << msg;

        // TODO: robotdiag::push_event

    }

    void onPeriodicUpdate() override {

        //Main periodic loop
        qDebug() << "Periodic loop (nothing for now)";

    }
};

#endif // YOURMAINWINDOW_H
