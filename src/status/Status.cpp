#include <stdio.h>
#include <string.h>
#include "Status.hpp"
#include "config.hpp"
Status* Status::instance=NULL;

Status::Status():devid(0),displaymod(0),displaysensor(0),ptzpanspeed(0),ptztitlespeed(0),ptzpanodirection(0),ptztitledirection(0),workmode(-1),mouseevent(-1),mousex(-1),mousey(-1),mousemod(-1),
	storegonum(0),storemod(-1),storesavenum(0),storesavemod(0),displayresolution(-1),detectareanum(-1),detectareaenable(-1),movdetectenable(1),
	panodetectenable(1),researchangle(0),mvdetectresearch(0),usestepdetect(0),mvconfigenable(0),zeromod(0),calibration(0)
{
	memset(&correcttime, 0, sizeof(correcttime));
	mtdcfg.movedetectalgenable = -1;
	mtdcfg.speedpriority = -1;
	mtdcfg.sensitivity = -1;
	mtdcfg.moverecordtime = -1;
	livevideoflg = 0;

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
	displayresolution = Config::getinstance()->displayresolution;
	if(displayresolution < 0)
		displayresolution = 0;
	memcpy(&scan_platformcfg, &(Config::getinstance()->scan_platformcfg), sizeof(platformcfg_t));
	memcpy(&trk_platformcfg, &(Config::getinstance()->trk_platformcfg), sizeof(platformcfg_t));
	memcpy(&sensorcfg[0], &(Config::getinstance()->sensorcfg[0]), sizeof(sensorcfg_t)*3);
	memcpy(&radarcfg, &(Config::getinstance()->radarcfg), sizeof(radarcfg_t));
	memcpy(&trackcfg, &(Config::getinstance()->trackcfg), sizeof(trackcfg_t));
	memcpy(&panocfg, &(Config::getinstance()->panocfg), sizeof(panocfg_t));
	printf("%s load status form config\r\n",__func__);
}

void Status::saveCurrentConfig(long lParam)
{
	if(displayresolution == 0 || displayresolution == 1 || displayresolution == 2)
	{
		Config::getinstance()->displayresolution = displayresolution;
		Config::getinstance()->display_width = 1920;
		Config::getinstance()->display_height = 1080;
	}
	memcpy(&(Config::getinstance()->scan_platformcfg), &scan_platformcfg, sizeof(platformcfg_t));
	memcpy(&(Config::getinstance()->trk_platformcfg), &trk_platformcfg, sizeof(platformcfg_t));
	memcpy(&(Config::getinstance()->sensorcfg[0]), &sensorcfg[0], sizeof(sensorcfg_t)*3);
	memcpy(&(Config::getinstance()->radarcfg), &radarcfg, sizeof(radarcfg_t));
	memcpy(&(Config::getinstance()->trackcfg), &trackcfg, sizeof(trackcfg_t));
	memcpy(&(Config::getinstance()->panocfg), &panocfg, sizeof(panocfg_t));
	Config::getinstance()->SaveConfig();
	printf("%s save config form status\r\n",__func__);
}

void Status::resetDefaultConfig(long lParam)
{
	//Config::getinstance()->DefaultConfig();
	//displayresolution = Config::getinstance()->displayresolution;
	//memcpy(&scan_platformcfg, &(Config::getinstance()->scan_platformcfg), sizeof(platformcfg_t));
	//memcpy(&trk_platformcfg, &(Config::getinstance()->trk_platformcfg), sizeof(platformcfg_t));
	//memcpy(&sensorcfg[0], &(Config::getinstance()->sensorcfg[0]), sizeof(sensorcfg_t)*3);
	//memcpy(&radarcfg, &(Config::getinstance()->radarcfg), sizeof(radarcfg_t));
	//memcpy(&trackcfg, &(Config::getinstance()->trackcfg), sizeof(trackcfg_t));
	//memcpy(&panocfg, &(Config::getinstance()->panocfg), sizeof(panocfg_t));
	printf("%s reset config and status form default\r\n",__func__);
}



