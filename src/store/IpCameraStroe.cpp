/*
 * IpCameraStroe.cpp
 *
 *  Created on: 2019年3月7日
 *      Author: fsmdn121
 */

#include"IpCameraStroe.h"
#include "stdio.h"
#include "stdlib.h"
 IpCameraStroe:: IpCameraStroe(char *dir)
 {
	 filename=( char *)malloc(128);
	 if(dir==NULL)
		 filename=IPDIRRECTDIR;
	 else
		 filename=dir;

 }

	void IpCameraStroe::Load()
	{
		Structipstore sis;
		IpCamearastore.clear();
			filefp=fopen(filename,"r");
			if (filefp==NULL)
				{
					printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Open Failed.\n");
					return;
				}
			if(filefp!=NULL)
				{
					while(!feof(filefp))
						{
							fgets(buffer,MAXIPSTORE_LINE,filefp);
							sscanf(buffer,"value:%d_az:%lf_el:%lf",&sis.value,&sis.az,&sis.el);
							IpCamearastore.push_back(sis);
						}
				}
			if(filefp!=NULL)
				fclose(filefp);
	}
	void IpCameraStroe:: AddPos(float az,float el)
	{
			Structipstore sis;
			sis.az=az;
			sis.el=el;
			sis.value=1;
			printf("the az=%f el=%f\n",sis.az,sis.el);
			IpCamearastore.push_back(sis);
	}

	void  IpCameraStroe::AddPic()
	{

	}
	Structipstore  IpCameraStroe::GetStore(int idx)
	{
		list<Structipstore>::iterator iter;
		Structipstore sis;
		int count =0;
				for(iter=IpCamearastore.begin();iter!=IpCamearastore.end();iter++)
				{
					if(count==idx)
					{
						sis=*iter;
						break;
					}
					count++;
				}
		if(count>=IpCamearastore.size())
		{
			sis.az=-1;
			sis.el=-1;
			sis.value=-1;
		}
		return sis;
	}

	void  IpCameraStroe::Erase(int idx)
	{
		list<Structipstore>::iterator iter;
		if(idx!=-1)
		{
			int count=0;
			for(iter=IpCamearastore.begin();iter!=IpCamearastore.end();iter++)
				{
					if(count==idx)
						break;
					count++;
				}
			if(count>=IpCamearastore.size())
				return;
			IpCamearastore.erase(iter);
		}
		else
		{
			for(iter=IpCamearastore.begin();iter!=IpCamearastore.end();iter++)
			{
				IpCamearastore.erase(iter);
			}
			IpCamearastore.clear();
		}
	}
	void  IpCameraStroe::Save()
	{
		list<Structipstore>::iterator iter;
		Structipstore sis;
			if ((filefp=fopen(filename,"w"))==NULL)			//\u6253\u5f00\u6307\u5b9a\u6587\u4ef6\uff0c\u5982\u679c\u6587\u4ef6\u4e0d\u5b58\u5728\u5219\u65b0\u5efa\u8be5\u6587\u4ef6
				{
					printf("Open Failed.\n");
					return;
				}
			for(iter=IpCamearastore.begin();iter!=IpCamearastore.end();iter++)
				{
				sis=*iter;
					if(sis.value!=1)
						continue;
					sprintf(buffer,"value:%d_az:%f_el:%f",sis.value,sis.az,sis.el);
					if(filefp!=NULL)
			  			fprintf(filefp,"%s\n",buffer);
					else
						break;
					fflush(filefp);
				}
			if(filefp!=NULL)
				fclose(filefp);
	}

