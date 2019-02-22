#include"configfile.hpp"
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include"osa.h"
ConfigFile* ConfigFile::instance=NULL;
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)
#define configdir "/home/ubuntu/config/"

#define detectconfigfile "/home/ubuntu/config/detect.txt"
#define RecordConfigfile "/home/ubuntu/config/record.txt"

ConfigFile::ConfigFile():mvconfigfile(NULL),recordconfigfile(NULL)
{
	memset(movarearect,0,sizeof(MovDetectAreaRect)*16);
}

ConfigFile::~ConfigFile()
{


}

int ConfigFile::mk_dir(char *dir)
{

    DIR *mydir = NULL;
    if((mydir= opendir(dir))==NULL)
    {
      int ret = mkdir(dir, MODE);
      if (ret != 0)
      {
          return -1;
      }
      printf("%s created sucess!/n", dir);
    }
    else
    {
        printf("%s exist!/n", dir);
    }
 
    return 0;
}

void ConfigFile::setdetectdate(MovDetectAreaRect *data)
{
	if(data==NULL)
		return ;
	for(int i=0;i<16;i++)
		{
			movarearect[i]=data[i];
		}
}
void ConfigFile::getdetectdate(MovDetectAreaRect *data)
{
	if(data==NULL)
		return ;
	for(int i=0;i<16;i++)
		{
			data[i]=movarearect[i];
		}
}
void ConfigFile::recordload()
{	
	
	recordconfigfile=fopen(RecordConfigfile,"r");
	if (recordconfigfile==NULL)			
		{
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Open Failed.\n");
			return;
		}
	int id=0;
	int classid=0;
	int weekid=0;
	
	if(recordconfigfile!=NULL)
		{
		
			while(!feof(recordconfigfile))
				{
					
					fgets(buffer,MAXSTORE_LINE,recordconfigfile);
					weekid=weekid%7;
					classid=classid%2;
					sscanf(buffer,"%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",\
						&recordpositionheld[classid][weekid][0],&recordpositionheld[classid][weekid][1],&recordpositionheld[classid][weekid][2],&recordpositionheld[classid][weekid][3],\
						&recordpositionheld[classid][weekid][4],&recordpositionheld[classid][weekid][5],&recordpositionheld[classid][weekid][6],&recordpositionheld[classid][weekid][7],\
						&recordpositionheld[classid][weekid][8],&recordpositionheld[classid][weekid][9],&recordpositionheld[classid][weekid][10],&recordpositionheld[classid][weekid][11],\
						&recordpositionheld[classid][weekid][12],&recordpositionheld[classid][weekid][13],&recordpositionheld[classid][weekid][14],&recordpositionheld[classid][weekid][15],\
						&recordpositionheld[classid][weekid][16],&recordpositionheld[classid][weekid][17],&recordpositionheld[classid][weekid][18],&recordpositionheld[classid][weekid][19],\
						&recordpositionheld[classid][weekid][20],&recordpositionheld[classid][weekid][21],&recordpositionheld[classid][weekid][22],&recordpositionheld[classid][weekid][23]\
					);
					weekid++;
					if(weekid==7)
						{
							weekid=0;
							classid=1;
						}
					//id++;
				}
		
		}
	/*
		int recordclass=0;
		printf("the time\n");
		for(int i=0;i<7;i++)
			{
				for(int j=0;j<24;j++)
					{
						if(j<8)
							printf("%d \t",recordpositionheld[recordclass][i][j]);
						else if(j<16)
							printf("%d \t",recordpositionheld[recordclass][i][j]);
						else if(j<24)
							printf("%d \t",recordpositionheld[recordclass][i][j]);					
					}
				printf("\n");

			}
		recordclass=1;
		printf("the mov\n");
		for(int i=0;i<7;i++)
			{
				for(int j=0;j<24;j++)
					{
						if(j<8)
							printf("%d \t",recordpositionheld[recordclass][i][j]);
						else if(j<16)
							printf("%d \t",recordpositionheld[recordclass][i][j]);
						else if(j<24)
							printf("%d \t",recordpositionheld[recordclass][i][j]);

						
					}
				printf("\n");

			}
	*/

	if(recordconfigfile!=NULL)
		fclose(recordconfigfile);
}

void ConfigFile::recordsave()
{

	
	if ((recordconfigfile=fopen(RecordConfigfile,"w"))==NULL)
		{
			printf("Open Failed.\n");
			return;
		} 
	int classid=0;
	int weekid=0;
	for(int i=0;i<2;i++)
		{
			classid=i;
			for(int weekid=0;weekid<7;weekid++)
				{
					
					sprintf(buffer,"%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",\
								recordpositionheld[classid][weekid][0],recordpositionheld[classid][weekid][1],recordpositionheld[classid][weekid][2],recordpositionheld[classid][weekid][3],\
								recordpositionheld[classid][weekid][4],recordpositionheld[classid][weekid][5],recordpositionheld[classid][weekid][6],recordpositionheld[classid][weekid][7],\
								recordpositionheld[classid][weekid][8],recordpositionheld[classid][weekid][9],recordpositionheld[classid][weekid][10],recordpositionheld[classid][weekid][11],\
								recordpositionheld[classid][weekid][12],recordpositionheld[classid][weekid][13],recordpositionheld[classid][weekid][14],recordpositionheld[classid][weekid][15],\
								recordpositionheld[classid][weekid][16],recordpositionheld[classid][weekid][17],recordpositionheld[classid][weekid][18],recordpositionheld[classid][weekid][19],\
								recordpositionheld[classid][weekid][20],recordpositionheld[classid][weekid][21],recordpositionheld[classid][weekid][22],recordpositionheld[classid][weekid][23]\
							);
					
					if(recordconfigfile!=NULL)
			  			fprintf(recordconfigfile,"%s\n",buffer);
					else 
						break;
					fflush(recordconfigfile);

				}
		}
	if(recordconfigfile!=NULL)
		fclose(recordconfigfile);
	

}


void ConfigFile::detectload()
{	
		mvconfigfile=fopen(detectconfigfile,"r");
	if (mvconfigfile==NULL)			
		{
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Open Failed.\n");
			return;
		}
	int id=0;
	if(mvconfigfile!=NULL)
		{
		
			while(!feof(mvconfigfile))
				{
					fgets(buffer,MAXSTORE_LINE,mvconfigfile);
					sscanf(buffer,"%d:%d,%d:%d,%d:%d,%d:%d,%d:",&movarearect[id].detectflag,&movarearect[id].area[0].point.x,&movarearect[id].area[0].point.y,\
						&movarearect[id].area[1].point.x,&movarearect[id].area[1].point.y,&movarearect[id].area[2].point.x,&movarearect[id].area[2].point.y,\
						&movarearect[id].area[3].point.x,&movarearect[id].area[3].point.y);
				
					id++;
				}
		
		}


	if(mvconfigfile!=NULL)
		fclose(mvconfigfile);


}
void ConfigFile::detectsave()
{
		if ((mvconfigfile=fopen(detectconfigfile,"w"))==NULL)			
		{
			printf("Open Failed.\n");
			return;
		} 

	for(int id=0;id<16;id++)
		{
			sprintf(buffer,"%d:%d,%d:%d,%d:%d,%d:%d,%d:",movarearect[id].detectflag,movarearect[id].area[0].point.x,movarearect[id].area[0].point.y,\
						movarearect[id].area[1].point.x,movarearect[id].area[1].point.y,movarearect[id].area[2].point.x,movarearect[id].area[2].point.y,\
						movarearect[id].area[3].point.x,movarearect[id].area[3].point.y);
			if(mvconfigfile!=NULL)
	  			fprintf(mvconfigfile,"%s\n",buffer);
			else 
				break;
			fflush(mvconfigfile);


		}
	if(mvconfigfile!=NULL)
		fclose(mvconfigfile);
	

}
void ConfigFile::filecreate()
{
	
	

}
void ConfigFile::create()
{
	mk_dir(configdir);
	filecreate();
	

}
ConfigFile *ConfigFile::getinstance()
{
	if(instance==NULL)
		instance= new ConfigFile;

	return instance;

}
