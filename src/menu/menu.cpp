

#include"menu.hpp"
#include "stdio.h"

MENU* MENU::instance;

Submenu SUbmen[SUBMENUMAX]={
		{NULLMOD, NULLMOD   ,	NULLMOD,     NULLMOD,    NULLMOD,   	0,   400,   	200,  50,	GREENColour, HIDEMODE,	L"工作模式",	NOACTIVE,	NULL,NULL},
		{WORKMOD, NULLMOD,   FIRSTLEVEL,     NULLLEVEL,    SECONDLEVEL,   0,   400,   	200,  50,	GREENColour, HIDEMODE,	L"工作模式",	NOACTIVE,	workmod,NULL},
		{SINGLESHOW,WORKMOD,SECONDLEVEL,   FIRSTLEVEL,   NULLLEVEL,   200,   400,       200,      50,		GREENColour, HIDEMODE,	L"单显模式",	NOACTIVE,	singlemod,NULL},
		{PANOSHOW,  WORKMOD,SECONDLEVEL,   FIRSTLEVEL,   NULLLEVEL,   200,   450,       200,      50,	GREENColour,	HIDEMODE,	L"拼接模式",	NOACTIVE,	panomod,NULL},
		{SELECTSHOW,WORKMOD,SECONDLEVEL,   FIRSTLEVEL,   NULLLEVEL,   200,   500,       200,      50,	GREENColour,	HIDEMODE,	L"点选模式",	NOACTIVE,	selectmod,NULL},
		{ZEROSHOW,WORKMOD,SECONDLEVEL,   FIRSTLEVEL,   NULLLEVEL,   200,   550,       200,      50,	GREENColour,	HIDEMODE,	L"零位模式",	NOACTIVE,	zeromod,NULL},



};
Panelmenu PAnelmenu[PANELMAX]={
	{NULLLEVEL,    0,   400,   200,  200,	GREENColour, HIDEMODE,	NOACTIVE,	NULL},
	{FIRSTLEVEL,    0,   400,   200,  200,	GREENColour, HIDEMODE,	NOACTIVE,	NULL},
	{SECONDLEVEL,  200,  400,   200,  200,	GREENColour, HIDEMODE,	NOACTIVE,	NULL},


};
void workmod(void *context)
{
	
	int show=DISMODE;

	printf("%s show=%d\n",__func__,show);
	if(context!=NULL)
		 show=*(int *)context;
	for(int i=SINGLESHOW;i<=ZEROSHOW;i++)
		{
			SUbmen[i].displayvalid=show;
			
			PAnelmenu[SUbmen[i].level].displayvalid=show;
		}

}

void singlemod(void *context)
{
	if(MENU::getinstance()==NULL)
		return ;
	int mod=SINGLESHOW;
	if(SUbmen[SINGLESHOW].renderfun!=NULL)
		SUbmen[SINGLESHOW].renderfun(&mod);
	
	
}

void panomod(void *context)
{
	if(MENU::getinstance()==NULL)
		return ;
	int mod=PANOSHOW;
	if(SUbmen[PANOSHOW].renderfun!=NULL)
		SUbmen[PANOSHOW].renderfun(&mod);

}

void selectmod(void *context)
{
	if(MENU::getinstance()==NULL)
		return ;
	int mod=SELECTSHOW;
	if(SUbmen[SELECTSHOW].renderfun!=NULL)
		SUbmen[SELECTSHOW].renderfun(&mod);

}


void zeromod(void *context)
{
	if(MENU::getinstance()==NULL)
		return ;
	int mod=ZEROSHOW;
	if(SUbmen[ZEROSHOW].renderfun!=NULL)
		SUbmen[ZEROSHOW].renderfun(&mod);

}

void MENU::menureset()
{
	for(int i=FIRSTLEVEL;i<LEVELMAX;i++)
		{
			PAnelmenu[i].displayvalid=HIDEMODE;
			PAnelmenu[i].active=NOACTIVE;
		}

	for(int i=WORKMOD;i<MENUMAX;i++)
		{
			SUbmen[i].displayvalid=HIDEMODE;
			SUbmen[i].active=NOACTIVE;
		}

}

void MENU::menushow()
{
	PAnelmenu[FIRSTLEVEL].displayvalid=DISMODE;
	PAnelmenu[FIRSTLEVEL].active=ACTIVE;


	for(int i=WORKMOD;i<=WORKMOD;i++)
		{
			SUbmen[i].displayvalid=DISMODE;
		}

}
void MENU::setcallback(int menu,CALLBACKFUNC rendcall)
{
	SUbmen[menu].renderfun=rendcall;


}

void MENU::menuselect(int menu,int active)
{


		int show=HIDEMODE;
		if(active==ACTIVE)
			show=DISMODE;

		printf("%s show[%d]=%d\n",__func__,menu,show);
	
		for(int i=WORKMOD;i<MENUMAX;i++)
		{
			if(menu==i)
				{
					if(active==ACTIVE)
						SUbmen[menu].active=ACTIVE;
					else
						{
							SUbmen[menu].active=NOACTIVE;
						}
					if(SUbmen[menu].fun!=NULL)
						SUbmen[menu].fun(&show);
					for(int j=FIRSTLEVEL;j<LEVELMAX;j++)
						{	
							if(j==SUbmen[menu].level)
								{
									
								
								//	if(PAnelmenu[menu].fun!=NULL)
								//		PAnelmenu[menu].fun(NULL);
								
								}
							
						}
				}
			else
				{
					SUbmen[i].active=NOACTIVE;

				}
		}

		

}


MENU*MENU::getinstance()
{
	if(instance==NULL)
		instance=new MENU();
	instance->submenu=SUbmen;
	instance->panelmenu=PAnelmenu;
	return instance;
	
}
