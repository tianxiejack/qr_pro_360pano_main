#ifndef __CPORTINTERFACE_H__
#define __CPORTINTERFACE_H__

#include "CMessage.hpp"
#include "globalDate.h"
#include "statCtrl.h"

class CPortInterface
{
public:
    virtual ~CPortInterface(){};
    virtual int create(){return 0;};
    virtual int closePort(){return 0;};
    virtual void run()=0;
    virtual int recvData(){return 0;};
    virtual int sendData(){return 0;};

    virtual void EnableSelfTest(){};
    virtual void EnableswitchVideoChannel(){};
    virtual void selectVideoChannel(){};
    virtual void EnableTrk(){};
    virtual void mouseevent(int event){};
    virtual void displaymod(){};
    virtual void AIMPOS_X(){};
    virtual void AIMPOS_Y(){};
    virtual void EnableParamBackToDefault(){};

    virtual void updatepano(){};;
    virtual void AxisMove(){};
    virtual void EnableTrkSearch(){};
    virtual void Enablealgosdrect(){};
    virtual void ZoomSpeedCtrl(){};
    virtual int ZoomShortCtrl(){return 0;};
    virtual int ZoomLongCtrl(){return 0;};
    virtual void IrisDown(){};
    virtual void IrisUp(){};
    virtual void FocusDown(){};
    virtual void FocusUp(){};
    virtual void AvtAxisCtrl(){};
    virtual void EnableOsdbuffer(){};
    virtual void EnablewordType(){};
    virtual void EnablewordSize(){};
    virtual void Config_Write_Save(){};
    virtual void Config_Read(){};
	
    virtual void GetsoftVersion(){};
	
    virtual void EnableSavePro(){};
    virtual void plantctl(){};
    virtual void AXIS_Y(){};
    virtual void Preset_Mtd(){};
    virtual void workMode(){};
    virtual void StoreMode(int mod){};
    virtual void targetCaptureMode(){};
    virtual void registfunc(){};
    virtual CMessage *getpM(){return 0;};
    virtual CGlobalDate* getDate()=0;
    virtual CStatusManager* getStatus()=0;


    virtual void playercontrl(){};
    virtual void playerquery(){};
    virtual void playerselect(){};

    virtual void panoenable(){};

    virtual void plantformconfig(){};
    virtual void sensorconfig(){};
    virtual void zeroconfig(){};
    virtual void recordconfig(){};
    virtual void movedetectconfig(){};
    virtual void movedetectareaconfig(){};
    virtual void displayconfig(){};
    virtual void correcttimeconfig(){};
    virtual void panoconfig(){};
    
    
};

#endif
