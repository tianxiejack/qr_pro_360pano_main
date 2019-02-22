#include"Status.hpp"
#include "stdio.h"
Status* Status::instance=NULL;

Status::Status():displaymod(0),ptzpanspeed(0),ptztitlespeed(0),ptzpanodirection(0),ptztitledirection(0),workmode(-1),mouseevent(-1),mousex(-1),mousey(-1),mousemod(-1),
	storegonum(0),storemod(-1),storesavenum(0),storesavemod(0),ptzaddress(-1),ptzprotocal(-1),ptzbaudrate(-1),ptzspeed(-1),movedetectalgenable(-1),
	speedpriority(-1),sensitivity(-1),moverecordtime(-1),displayresolution(-1),correctyear(-1),correctmonth(-1),correctday(-1),correcthour(-1),correctmin(-1),
	correctsec(-1),panoptzspeed(-1),panopiexfocus(-1),panopicturerate(-1),detectareanum(-1),detectareaenable(-1),movdetectenable(1),panodetectenable(1),researchangle(0),
	mvdetectresearch(0),usestepdetect(0),mvconfigenable(0),zeromod(0),calibration(0)
{


}
Status::~Status()
{


}
Status* Status::getinstance()
{
	if(instance==NULL)
		instance=new Status();
	return instance;

}
