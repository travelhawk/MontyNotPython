/*
 * movementController.h
 *
 *  Created on: 18.07.2014
 *      Author: Dennis
 */

#ifndef MOVEMENTCONTROLLER_H_
#define MOVEMENTCONTROLLER_H_

#include <QObject>
#include <wiringPi.h>
#include "ultrasonic.h"
#include "stepper.h"
#include "servo.h"

class MovementController : public QObject
{
    Q_OBJECT
public:
    MovementController();
private:
    QThread* thread;
    Ultrasonic* ultrasonic;
    Stepper* stepperRotate;
    Stepper* stepperZoom;
    Servo* servo;
    int soll;
    int ist;
    int servoAngle;
    void moveUp(void);
    void moveDown(void);
    void moveLeft(void);
    void moveRight(void);
    void zoomIn(void);
    void zoomOut(void);
public slots:
    void performMovement(int movementstate);
};


#endif /* MOVEMENTCONTROLLER_H_ */
