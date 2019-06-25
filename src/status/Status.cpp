#include <stdio.h>
#include <string.h>
#include "Status.hpp"
#include "config.hpp"
Status* Status::instance=NULL;

Status::Status():devid(0),displaymod(0),displaysensor(0),ptzpanspeed(0),ptztitlespeed(0),ptzpanodirection(0),ptztitledirection(0),workmode(-1),mouseevent(-1),mousex(-1),mousey(-1),mousemod(-1),
	storegonum(0),storemod(-1),storesavenum(0),storesavemod(0),ptzaddress(-1),ptzprotocal(-1),ptzbaudrate(-1),ptzspeed(-1),displayresolution(-1),correctyear(-1),correctmonth(-1),
	correctday(-1),correcthour(-1),correctmin(-1),correctsec(-1),panoptzspeed(-1),panopiexfocus(-1),panopicturerate(-1),detectareanum(-1),detectareaenable(-1),movdetectenable(1),
	panodetectenable(1),researchangle(0),mvdetectresearch(0),usestepdetect(0),mvconfigenable(0),zeromod(0),calibration(0)
{

	mtdcfg.movedetectalgenable = -1;
	mtdcfg.speedpriority = -1;
	mtdcfg.sensitivity = -1;
	mtdcfg.moverecordtime = -1;

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

void Status::loadconfig()
{
	memcpy(&scan_platformcfg, &(Config::getinstance()->scan_platformcfg), sizeof(scan_platformcfg_t));
	memcpy(&sensorcfg[0], &(Config::getinstance()->sensorcfg[0]), sizeof(sensorcfg_t)*3);
	memcpy(&radarcfg, &(Config::getinstance()->radarcfg), sizeof(radarcfg_t));
	memcpy(&trackcfg, &(Config::getinstance()->trackcfg) , sizeof(trackcfg_t));
	printf("%s load status form config\r\n",__func__);
}


