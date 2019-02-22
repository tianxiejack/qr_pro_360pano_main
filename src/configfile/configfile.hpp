/*
 * configfile.hpp
 *
 *  Created on: Jan 12, 2019
 *      Author: wj
 */

#ifndef CONFIGFILE_HPP_
#define CONFIGFILE_HPP_
#include <osa_file.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
typedef struct
{
	Point point;


}MovDetectAreaPoint;

typedef struct
{
	//Rect rectare;
	MovDetectAreaPoint area[4];
	int movupdown;
	int  detectflag;
	int singleshowflag;


}MovDetectAreaRect;
#define MAXSTORE_LINE (100)
class ConfigFile
{

private:
	ConfigFile();
	~ConfigFile();
	int mk_dir(char *dir);
	;
	void filecreate();
	char buffer[100];
	
	static ConfigFile* instance;
private:
	FILE * mvconfigfile;
	FILE * recordconfigfile;
	MovDetectAreaRect movarearect[MAXSTORE_LINE];

	int recordpositionheld[2][7][24];
public:
	void setdetectdate(MovDetectAreaRect *data);
	void getdetectdate(MovDetectAreaRect *data);

	void setdataheldrecord(int a[2][7][24])
			{
				 for(int i=0;i<2;i++)
	 				for(int j=0;j<7;j++)
						for(int k=0;k<24;k++)
							recordpositionheld[i][j][k]=a[i][j][k];
				
			};
	void getdataheldrecord(int a[2][7][24])
			{
				 for(int i=0;i<2;i++)
	 				for(int j=0;j<7;j++)
						for(int k=0;k<24;k++)
							a[i][j][k]=recordpositionheld[i][j][k] ;
				
			};
	
	void detectload();
	void detectsave();
	void recordload();
	void recordsave();
	void create();
	static ConfigFile *getinstance();
};



#endif /* CONFIGFILE_HPP_ */
