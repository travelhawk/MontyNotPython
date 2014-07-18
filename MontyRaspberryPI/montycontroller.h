#ifndef MONTYCONTROLLER_H
#define MONTYCONTROLLER_H

#include <QObject>
#include "hardware/ultrasonic.h"
#include "hardware/stepper.h"
#include "server.h"
#include <opencv/cv.hpp>

using namespace cv;

class MontyController : public QObject
{
    Q_OBJECT
public:
    explicit MontyController();
    enum InfoState{
        CONTROLL_AUTO,
        CONTROLL_MANUAL,
        TAKE_PHOTO
    };
    enum MounthState{
        FLASH_ON,
        OFF,
        START_ANIMATION
    };

signals:

public slots:


private:
    Ultrasonic* ultrasonic;
    Stepper* stepperRotate;
    Stepper* stepperZoom;
    Server* server;
    AutomaticControl* automaticControl;
    MovementController* movementController;
    Camera* camera;
    Servo* servo;

private slots:
    void receiveUltrasonicDistance(double value);
    void takePicture();
    void savePicture(Mat picture);
    void rotateLeft();
    void rotateRight();
    void zoomIn();
    void zoomOut();
    void armUp();
    void armDown();
};

#endif // MONTYCONTROLLER_H
