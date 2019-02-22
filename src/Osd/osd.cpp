#include "osd.hpp"

Osdcontext context[OSDCONTEXLEN]={

		
		{SINGLEMODE,1700,10,REDColour,DISMODE,L"单显模式"},
		{PANOMODE,1700,10,REDColour,DISMODE,L"拼接模式"},
		{SELECTMODE,1700,10,REDColour,DISMODE,L"点选模式"},
		{SELECTZEROMODE,1700,10,REDColour,DISMODE,L"零位模式"},
		{MAXMODE,1700,10,REDColour,HIDEMODE,L"    "},
		{CALIBRATION,1650,40,REDColour,DISMODE,L"校零中请等待"},
		{MODELING,1700,40,REDColour,HIDEMODE,L"检测建模中"},
		{SELECTING,1700,80,REDColour,DISMODE,L"激活中"},


};

OSD::OSD()
{

}
OSD::~OSD()
{

}

Osdcontext * OSD::getOSDcontex()
{
	return context;

}

