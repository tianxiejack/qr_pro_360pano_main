/*
 * ptzProxyMsg.h
 *
 *  Created on: 2019年3月19日
 *      Author: fsmdn121
 */

#ifndef PTZPROXYMSG_H_
#define PTZPROXYMSG_H_

class  ptzProxyMsg
{
	public:
	void initUDP();
#if 1
	void PackageToSend(char *buf);

	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();
	void moveUpLeft();
	void moveUpRight();
	void moveDownLeft();
	void moveDownRight();
	void Stop();
	void zoomIn();
	void zoomOut();
	void focusNear();
	void focusFar();
	void irisOpen();
	void irisClose();
	void Setspeed(int speed);
#endif
};

#endif /* PTZPROXYMSG_H_ */

