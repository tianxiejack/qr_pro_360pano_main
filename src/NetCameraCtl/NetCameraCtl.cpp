/*
 * NetCameraCtl.cpp
 *
 *  Created on: 2019年3月7日
 *      Author: fsmdn121
 */

#include "NetCameraCtl.h"


	void NetCameraCtl::init()
	{

	}
	void	NetCameraCtl::MoveUp()
	{

	}
	void NetCameraCtl::MoveDown()
	{

	}
	void NetCameraCtl::MoveLeft()
	{

	}
	void NetCameraCtl::MoveRight()
	{

	}
	void NetCameraCtl::SetFocal(bool plus)
	{
		int step=5;
		if(plus)
		{
			focalvalue+=step;
		}
		else
		{
			focalvalue-=step;
		}
	}
	void NetCameraCtl::SetAperture(bool	plus)
	{
		int step=5;
			if(plus)
			{
				aperturevalue+=step;
			}
			else
			{
				aperturevalue-=step;
			}
	}
	void NetCameraCtl::SetSpeed(bool plus,int fixedValue)
	{
		int step=5;
		if(fixedValue==-1)
		{
			if(plus)
			{
				speedvalue+=step;
			}
			else
			{
				speedvalue-=step;
			}
		}
		else
		{
			speedvalue=fixedValue;
		}
	}
	void NetCameraCtl::GotoSpecifiedLocation(double az,double el)
	{
	}

