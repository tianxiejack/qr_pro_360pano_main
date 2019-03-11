/*
 * NetCameraCtl.h
 *
 *  Created on: 2019年3月7日
 *      Author: fsmdn121
 */

#ifndef NETCAMERACTL_H_
#define NETCAMERACTL_H_


class NetCameraCtl
{
public:
	void init();
	void	MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void SetFocal(bool plus);
	void SetAperture(bool	plus);
	void SetSpeed(bool plus,int fixedValue=-1);
	double GetCurAZvalue(){return AZvalue;};
	double	GetCurELvalue(){return ELvalue;};
	void GotoSpecifiedLocation(double az,double el);
private:
	int speedvalue;
	int focalvalue;
	int aperturevalue;
	double	AZvalue;//fangwei
	double	ELvalue;//fuyang
};






#endif /* NETCAMERACTL_H_ */
