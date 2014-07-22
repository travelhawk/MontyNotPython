/*
 * movementController.cpp
 *
 *  Created on: 18.07.2014
 *      Author: Dennis
 */

#include "movementController.h"
#include "movementstates.h"

MovementController::MovementController()
{
	buttonRotate = new Button(Button::ROTATE_BUTTON);
	buttonZoom = new Button(Button::ZOOM_BUTTON);
	wiringPiSetup();
	soll = HOLD_POSITION;
	ist = FLEXIBLE_POSITION;
	ultrasonic = new Ultrasonic();
    //stepper rotate at pin  2,3,4,11 (wiring pi pins 8,9,7,14)
    stepperRotate = new Stepper(8,9,7,14);
    //stepper zoom at Pin 17,10,27,22; (wiring pi pins 0,12,2,3)
    stepperZoom = new Stepper(0,12,2,3);
    servo = new Servo(0);
    thread = new QThread();
    this->moveToThread(thread);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
    servoAngle = servo->getCurrentAngle();
}

void MovementController::performMovement(int movementState) {
	int distance;
	soll = movementState;
	servoAngle = servo->getCurrentAngle();
	// ist-Wert aktualisieren
	if(servoAngle >= 45) ist |= LOWER_END_REACHED;
	else if(servoAngle <= -45) ist |= UPPER_END_REACHED;
	else ist &= SERVO_RESET_MASK;
	// falls Grenzwert für die Bewegung bereits erreicht..
	if((soll & ist) > 0) {
		if(soll == MOVE_UP && (ist & ZOOM_OUT_POSITION) == 0 ) soll = ZOOM_OUT;
		else if(soll == MOVE_DOWN && (ist & ZOOM_IN_POSITION) == 0) soll = ZOOM_IN;
		else soll = HOLD_POSITION;
	}
	// Falls Hindernis vor dem Ultraschallsensor erkannt..
	if(soll == HOLD_POSITION) {
		distance = ultrasonic->getDistance();
		if((ist & ZOOM_OUT_POSITION) == 0 && distance < 15) soll = ZOOM_OUT;
	}
	switch(soll) {
	case MOVE_UP: moveUp();
	break;
	case MOVE_DOWN: moveDown();
	break;
	case MOVE_LEFT: moveLeft();
	break;
	case MOVE_RIGHT: moveRight();
	break;
	case ZOOM_IN: zoomIn();
	break;
	case ZOOM_OUT: zoomOut();
	break;
	}
}

void MovementController::moveUp()
{
    servo->setAngle(servoAngle - 2);
    servo->setAngle(servoAngle - 1);
	servoAngle = servo->getCurrentAngle();
}

void MovementController::moveDown()
{
    servo->setAngle(servoAngle + 2);
    servo->setAngle(servoAngle + 1);
	servoAngle = servo->getCurrentAngle();
}

void MovementController::moveLeft()
{
    if(!stepperRotate->isActive()) {
        stepperRotate->counterclockwise(50);
    	if(buttonRotate->isPressed()) ist |= LEFT_END_REACHED;
    	else ist &= ROTATION_RESET_MASK;
    }
}

void MovementController::moveRight()
{
	if(!stepperRotate->isActive()) {
        stepperRotate->clockwise(50);
    	if(buttonRotate->isPressed()) ist |= RIGHT_END_REACHED;
    	else ist &= ROTATION_RESET_MASK;
	}
}

void MovementController::zoomIn()
{
	if(!stepperZoom->isActive()) {
        stepperZoom->clockwise(50);
    	if(buttonZoom->isPressed()) ist |= ZOOM_IN_POSITION;
    	else ist &= ZOOM_RESET_MASK;
	}
}

void MovementController::zoomOut()
{
	if(!stepperZoom->isActive()) {
        stepperZoom->counterclockwise(50);
    	if(buttonZoom->isPressed()) ist |= ZOOM_OUT_POSITION;
    	else ist &= ZOOM_RESET_MASK;
	}
}
