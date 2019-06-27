#include "config.hpp"
#include "stdio.h"
#include <iostream>
#include <string>
#include "config.h"
#include "opencv2/core/core.hpp"
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ostream;
using namespace cv;
FileStorage configfilestore;
#define CONFIGFILENAME "/home/nvidia/config/config.xml"

Config *Config::instance=new Config();

Config::Config():cam_fov(0),ptzpaninverse(0),ptztitleinverse(0),panoprocessshift(0),panoprocesstailcut(0),angleinterval(10),panocalibration(1),cam_fixcamereafov(0),camsource(0)
{
	panozeroptz = 0;
	panozeroptztitle = 0;

	memset(&trk_platformcfg, 0, sizeof(trk_platformcfg));
	trk_platformcfg.address = 1;
	trk_platformcfg.baudrate = 2;
	trk_platformcfg.ptzspeed = 20;
	memset(&scan_platformcfg, 0, sizeof(scan_platformcfg));
	scan_platformcfg.address = 1;
	scan_platformcfg.baudrate = 2;
	scan_platformcfg.ptzspeed = 20;

	memset(&sensorcfg, 0, sizeof(sensorcfg));

	memset(&radarcfg, 0, sizeof(radarcfg));
	memset(&trackcfg, 0, sizeof(trackcfg));
	memset(&panocfg, 0, sizeof(panocfg));

	#if CONFIGINIT
	configfilestore=FileStorage(CONFIGFILENAME, FileStorage::WRITE);
	#else
	configfilestore=FileStorage(CONFIGFILENAME, FileStorage::READ);
	#endif

}

Config::~Config()
{
	configfilestore.release();

}

void Config::SaveConfig()
{
	configfilestore.release();
	configfilestore=FileStorage(CONFIGFILENAME, FileStorage::WRITE);
	saveconfig();
	configfilestore.release();

}

void Config::saveconfig()
{
	configfilestore <<"camera" << "{" << "cam_width" << cam_width << "cam_height" << cam_height << "cam_channel" << cam_channel << "cam_fx" << cam_fx << "cam_fy" << cam_fy<< "cam_ox" \
		<< cam_ox<< "cam_oy" << cam_oy<< "cam_k1" <<cam_k1 << "cam_k2" << cam_k2<< "cam_k3" << cam_k3<< "cam_k4" << cam_k4<< "cam_readfromfile"<<cam_readfromfile<<"cam_fixcamereafov"<<cam_fixcamereafov<<"}";

	configfilestore <<"display" << "{" << "displayresolution" << displayresolution << "display_width" << display_width << "display_height" << display_height << "display_channel" << display_channel << "}";

	configfilestore <<"panoalg" << "{" << "panoprocesswidth" << panoprocesswidth << "panoprocessheight" << panoprocessheight  <<"panozeroptz"<< panozeroptz << "panozeroptztitle"<<panozeroptztitle<<"panoprocessshift"<<panoprocessshift<<"panoprocesstailcut"<<panoprocesstailcut\
		<<"panocylinder"<<panocylinder<<"panofusion"<<panofusion<<"angleinterval"<<angleinterval<<"panocalibration"<<panocalibration<<"}";

	configfilestore <<"mvdetect" << "{" << "mvprocesswidth" << mvprocesswidth << "mvprocessheight" << mvprocessheight  <<"mvdownup"<<mvdownup<<"minarea"<<minarea<<"maxarea"<<maxarea<<"detectthread"<<detectthread<< "}";

	configfilestore <<"ptz" << "{" << "ptzpaninverse" << ptzpaninverse << "ptztitleinverse" << ptztitleinverse << "}";

	int senId=0;
	configfilestore <<"sensor_0" << "{" \
		<< "outputresol" << sensorcfg[senId].outputresol << "brightness" << sensorcfg[senId].brightness << "contract" << sensorcfg[senId].contract \
		<< "autobright" << sensorcfg[senId].autobright << "backlight" << sensorcfg[senId].backlight << "whitebalance" << sensorcfg[senId].whitebalance \
		<< "gain" << sensorcfg[senId].gain << "daynight" << sensorcfg[senId].daynight << "stronglightsup" << sensorcfg[senId].stronglightsup \
		<< "exposuremode" << sensorcfg[senId].exposuremode << "elecshutter_time" << sensorcfg[senId].elecshutter_time \
		<< "}";
	senId=1;
	configfilestore <<"sensor_1" << "{" \
		<< "outputresol" << sensorcfg[senId].outputresol << "brightness" << sensorcfg[senId].brightness << "contract" << sensorcfg[senId].contract \
		<< "autobright" << sensorcfg[senId].autobright << "backandwrite" << sensorcfg[senId].backandwrite << "correct" << sensorcfg[senId].correct \
		<< "digitfilter" << sensorcfg[senId].digitfilter << "digitenhance" << sensorcfg[senId].digitenhance << "mirror" << sensorcfg[senId].mirror \
		<< "}";
	senId=2;
	configfilestore <<"sensor_2" << "{" \
		<< "outputresol" << sensorcfg[senId].outputresol << "brightness" << sensorcfg[senId].brightness << "contract" << sensorcfg[senId].contract \
		<< "autobright" << sensorcfg[senId].autobright << "backlight" << sensorcfg[senId].backlight << "whitebalance" << sensorcfg[senId].whitebalance \
		<< "gain" << sensorcfg[senId].gain << "daynight" << sensorcfg[senId].daynight << "stronglightsup" << sensorcfg[senId].stronglightsup \
		<< "exposuremode" << sensorcfg[senId].exposuremode << "elecshutter_time" << sensorcfg[senId].elecshutter_time \
		<< "}";

	configfilestore <<"scanplt" << "{" \
		<< "address" << scan_platformcfg.address << "protocol" << scan_platformcfg.protocol << "baudrate" << scan_platformcfg.baudrate \
		<< "ptzspeed" << scan_platformcfg.ptzspeed << "start_signal" << scan_platformcfg.start_signal << "pt_check" << scan_platformcfg.pt_check \
		<< "}";
	configfilestore <<"trkplt" << "{" \
		<< "address" << trk_platformcfg.address << "protocol" << trk_platformcfg.protocol << "baudrate" << trk_platformcfg.baudrate \
		<< "ptzspeed" << trk_platformcfg.ptzspeed \
		<< "}";

	configfilestore <<"radar" << "{" \
		<< "hideline" << radarcfg.hideline \
		<< "offset50m_0" << radarcfg.offset50m[0] << "offset100m_0" << radarcfg.offset100m[0]  << "offset300m_0" << radarcfg.offset300m[0] \
		<< "offset50m_1" << radarcfg.offset50m[1] << "offset100m_1" << radarcfg.offset100m[1]  << "offset300m_1" << radarcfg.offset300m[1] \
		<<"}";

	configfilestore <<"track" << "{" \
		<< "priodis" << trackcfg.trkpriodis << "priobright" << trackcfg.trkpriobright << "priosize" << trackcfg.trkpriosize << "trktime" << trackcfg.trktime \
		<< "}";

	configfilestore <<"pano" << "{" \
		<< "ptzspeed_0" << panocfg.ptzspeed[0] << "piexfocus_0" << panocfg.piexfocus[0] << "circlefps_0" << panocfg.circlefps[0] \
		<< "ptzspeed_1" << panocfg.ptzspeed[1] << "piexfocus_1" << panocfg.piexfocus[1] << "circlefps_1" << panocfg.circlefps[1] \
		<< "}";

	configfilestore.release();
	
}
void Config::loadconfig()
{
	FileNode struct_node = configfilestore["camera"];
	cam_width=(int )struct_node["cam_width"];
	cam_height=(int )struct_node["cam_height"];
	cam_channel=(int )struct_node["cam_channel"];
	cam_fx=(double )struct_node["cam_fx"];
	cam_fy=(double )struct_node["cam_fy"];
	cam_ox=(double )struct_node["cam_ox"];
	cam_oy=(double )struct_node["cam_oy"];
	cam_k1=(double )struct_node["cam_k1"];
	cam_k2=(double )struct_node["cam_k2"];
	cam_k3=(double )struct_node["cam_k3"];
	cam_k4=(double )struct_node["cam_k4"];
	cam_readfromfile=(int)struct_node["cam_readfromfile"];
	cam_fixcamereafov=(int)struct_node["cam_fixcamereafov"];
	
	struct_node = configfilestore["display"];
	displayresolution=(int )struct_node["displayresolution"];
	display_width=(int )struct_node["display_width"];
	display_height=(int )struct_node["display_height"];
	display_channel=(int )struct_node["display_channel"];

	cam_fov=2*atan2(cam_width,2*cam_fx)*180/3.141592653;
	printf("%s cam w=%d h=%d  disp w=%d h=%d\r\n",__func__,
		cam_width,cam_height,display_width,display_height);
	
	struct_node = configfilestore["panoalg"];
	panoprocesswidth=(int )struct_node["panoprocesswidth"];
	panoprocessheight=(int )struct_node["panoprocessheight"];
	panozeroptz=(double)struct_node["panozeroptz"];
	panozeroptztitle=(double)struct_node["panozeroptztitle"];
	panocylinder=(int )struct_node["panocylinder"];
	panofusion=(int ) struct_node["panofusion"];
	angleinterval=(double )struct_node["angleinterval"];
	panocalibration=(int)struct_node["panocalibration"];
	panoprocessshift=(int)struct_node["panoprocessshift"];
	panoprocesstailcut=(int)struct_node["panoprocesstailcut"];

	struct_node = configfilestore["mvdetect"];
	mvprocesswidth=(int )struct_node["mvprocesswidth"];
	mvprocessheight=(int )struct_node["mvprocessheight"];
	mvdownup=(int )struct_node["mvdownup"];
	minarea=(int )struct_node["minarea"];
	maxarea=(int )struct_node["maxarea"];
	detectthread=(int )struct_node["detectthread"];

	//////////////////////////////////////
	struct_node = configfilestore["ptz"];
	ptzpaninverse=(int )struct_node["ptzpaninverse"];
	ptztitleinverse=(int )struct_node["ptztitleinverse"];

	//////////////////////////////////////
	for(int senId=0;senId<3;senId++)
	{
		char cfg_sen[30] = "sensor_0";
		sprintf(cfg_sen, "sensor_%d", senId);
		struct_node = configfilestore[cfg_sen];
		if(struct_node.empty())
			continue;
		if(senId == 0 || senId == 2)
		{
			sensorcfg[senId].outputresol=(int )struct_node["outputresol"];
			sensorcfg[senId].brightness=(int )struct_node["brightness"];
			sensorcfg[senId].contract=(int )struct_node["contract"];
			sensorcfg[senId].autobright=(int )struct_node["autobright"];
			sensorcfg[senId].backlight=(int )struct_node["backlight"];
			sensorcfg[senId].whitebalance=(int )struct_node["whitebalance"];
			sensorcfg[senId].gain=(int )struct_node["gain"];
			sensorcfg[senId].daynight=(int )struct_node["daynight"];
			sensorcfg[senId].stronglightsup=(int )struct_node["stronglightsup"];
			sensorcfg[senId].exposuremode=(int )struct_node["exposuremode"];
			sensorcfg[senId].elecshutter_time=(int )struct_node["elecshutter_time"];
		}
		else if(senId == 1)
		{
			sensorcfg[senId].outputresol=(int )struct_node["outputresol"];
			sensorcfg[senId].brightness=(int )struct_node["brightness"];
			sensorcfg[senId].contract=(int )struct_node["contract"];
			sensorcfg[senId].autobright=(int )struct_node["autobright"];
			sensorcfg[senId].backandwrite=(int )struct_node["backandwrite"];
			sensorcfg[senId].correct=(int )struct_node["correct"];
			sensorcfg[senId].digitfilter=(int )struct_node["digitfilter"];
			sensorcfg[senId].digitenhance=(int )struct_node["digitenhance"];
			sensorcfg[senId].mirror=(int )struct_node["mirror"];
		}
	}

	//////////////////////////////////////
	struct_node = configfilestore["trkplt"];
	if(struct_node.empty() != true)
	{
		trk_platformcfg.address=(int )struct_node["address"];
		trk_platformcfg.protocol=(int )struct_node["protocol"];
		trk_platformcfg.baudrate=(int )struct_node["baudrate"];
		trk_platformcfg.ptzspeed=(int )struct_node["ptzspeed"];
	}
	struct_node = configfilestore["scanplt"];
	if(struct_node.empty() != true)
	{
		scan_platformcfg.address=(int )struct_node["address"];
		scan_platformcfg.protocol=(int )struct_node["protocol"];
		scan_platformcfg.baudrate=(int )struct_node["baudrate"];
		scan_platformcfg.ptzspeed=(int )struct_node["ptzspeed"];
		scan_platformcfg.start_signal=(int )struct_node["start_signal"];
		scan_platformcfg.pt_check=(int )struct_node["pt_check"];
	}

	//////////////////////////////////////
	struct_node = configfilestore["radar"];
	if(struct_node.empty() != true)
	{
		radarcfg.hideline=(int )struct_node["hideline"];
		radarcfg.offset50m[0]=(int )struct_node["offset50m_0"];
		radarcfg.offset100m[0]=(int )struct_node["ffset100m_0"];
		radarcfg.offset300m[0]=(int )struct_node["offset300m_0"];
		radarcfg.offset50m[1]=(int )struct_node["offset50m_1"];
		radarcfg.offset100m[1]=(int )struct_node["offset100m_1"];
		radarcfg.offset300m[1]=(int )struct_node["offset300m_1"];
	}

	//////////////////////////////////////
	struct_node = configfilestore["track"];
	if(struct_node.empty() != true)
	{
		trackcfg.trkpriodis=(int )struct_node["priodis"];
		trackcfg.trkpriobright=(int )struct_node["priobright"];
		trackcfg.trkpriosize=(int )struct_node["priosize"];
		trackcfg.trktime=(int )struct_node["trktime"];
	}
	//////////////////////////////////////
	struct_node = configfilestore["pano"];
	if(struct_node.empty() != true)
	{
		panocfg.ptzspeed[0]=(int )struct_node["ptzspeed_0"];
		panocfg.piexfocus[0]=(int )struct_node["piexfocus_0"];
		panocfg.circlefps[0]=(int )struct_node["circlefps_0"];
		panocfg.ptzspeed[1]=(int )struct_node["ptzspeed_1"];
		panocfg.piexfocus[1]=(int )struct_node["piexfocus_1"];
		panocfg.circlefps[1]=(int )struct_node["circlefps_1"];
	}

}

Config *Config::getinstance()
{
	if(instance==NULL)
		instance=new Config();

	return instance;
}

