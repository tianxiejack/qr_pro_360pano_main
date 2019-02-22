#ifndef _CHOSEN_CAP_GROUP_H
#define _CHOSEN_CAP_GROUP_H
#include "CaptureGroup.h"
class ChosenCaptureGroup:public HDCaptureGroup
{
public:
	static ChosenCaptureGroup * GetTVInstance();
	static ChosenCaptureGroup * GetHOTInstance();
		~ChosenCaptureGroup();
	virtual void CreateProducers();
	virtual void OpenProducers();
private:
	ChosenCaptureGroup(unsigned int w,unsigned int h,int NCHAN,unsigned int capCount=1);
	ChosenCaptureGroup(){};
	ChosenCaptureGroup(const ChosenCaptureGroup&){};
	static ChosenCaptureGroup TVChosenGroup;
	static ChosenCaptureGroup HOTChosenGroup;
};
#endif


