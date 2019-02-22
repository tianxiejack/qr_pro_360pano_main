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
#define CONFIGFILENAME "./config.xml"


Config *Config::instance=new Config();

Config::Config():panozeroptz(0),intergralenable(1),cam_fov(0),ptzpaninverse(0),ptztitleinverse(0),panoprocessshift(0),panoprocesstailcut(0),ptzspeed(20),angleinterval(10),panocalibration(1),cam_fixcamereafov(0),camsource(0)
{
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

	configfilestore <<"display" << "{" << "display_width" << display_width << "display_height" << display_height << "display_channel" << display_channel << "}";

	configfilestore <<"panoalg" << "{" << "panoprocesswidth" << panoprocesswidth << "panoprocessheight" << panoprocessheight  <<"panozeroptz"<< panozeroptz << "panozeroptztitle"<<panozeroptztitle<<"panoprocessshift"<<panoprocessshift<<"panoprocesstailcut"<<panoprocesstailcut\
		<<"panocylinder"<<panocylinder<<"panofusion"<<panofusion<<"angleinterval"<<angleinterval<<"panocalibration"<<panocalibration<<"}";

	configfilestore <<"mvdetect" << "{" << "mvprocesswidth" << mvprocesswidth << "mvprocessheight" << mvprocessheight  <<"mvdownup"<<mvdownup<<"minarea"<<minarea<<"maxarea"<<maxarea<<"detectthread"<<detectthread<< "}";

	configfilestore <<"ptz" << "{" << "ptzwait" << ptzwait << "ptzbroad" << ptzbroad  <<"ptzaddres"<<ptzaddres<<"ptzdp"<<ptzdp<<"ptzpaninverse"<< ptzpaninverse<<"ptztitleinverse"<<ptztitleinverse<<"ptzspeed"<<ptzspeed<<"}";

	
	
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
	display_width=(int )struct_node["display_width"];
	display_height=(int )struct_node["display_height"];
	display_channel=(int )struct_node["display_channel"];
	
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

	struct_node = configfilestore["ptz"];
	ptzwait=(int )struct_node["ptzwait"];
	ptzbroad=(int )struct_node["ptzbroad"];
	ptzaddres=(int )struct_node["ptzaddres"];
	ptzdp=(int )struct_node["ptzdp"];
	ptzpaninverse=(int )struct_node["ptzpaninverse"];
	ptztitleinverse=(int )struct_node["ptztitleinverse"];

	ptzspeed=(int )struct_node["ptzspeed"];


	cam_fov=2*atan2(cam_width,2*cam_fx)*180/3.141592653;
	

	printf("%s c w=%d c h=%d  d w=%d d h=%d",__func__,cam_width,cam_height,display_width,display_height);
	

}
Config *Config::getinstance()
{
	if(instance==NULL)
		instance=new Config();

	return instance;


}








 
  
