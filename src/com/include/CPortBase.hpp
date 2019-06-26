#ifndef __CPORTBASE_H__
#define __CPORTBASE_H__

#include "CPortInterface.hpp"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include <stdint.h>

using namespace std;

class CPortBase : public CPortInterface
{
public:
	CPortBase();
	~CPortBase();

	CMessage *pM;
	CMessage *getpM();
	CGlobalDate* _globalDate;
	CGlobalDate* getDate();

	void seconds_sleep(unsigned seconds);
	//////////////////////////////
	void mouseevent(int event);
	void displaymod();
	void workMode();
	void StoreMode(int mod);
	void updatepano();
	void ZoomCtrl();
	void IrisCtrl();
	void FocusCtrl();
	void plantctl();
	void panoenable();

	//////////////////////////////
	void GetsoftVersion();
	void ConfigCurrentSave();
	void ConfigLoadDefault();
	void choosedev();
	void chooseptz();

	//////////////////////////////
	void playercontrl();
	void playerquery();
	void playerselect();
	void livevideo();
	void livephoto();
	void videoclip();

	//////////////////////////////
	void adddevconfig();
	void deletedevconfig();
	void recordconfig();
	void scan_plantformconfig();
	void trk_plantformconfig();
	void sensortvconfig();
	void sensortrkconfig();
	void sensorfrconfig();
	void zeroconfig();
	void movedetectconfig();
	void movedetectareaconfig();
	void correcttimeconfig();
	void radarconfig();
	void trackconfig();
	void panoconfig();

	void displayconfig();
	void querryconfig();
	void rebootconfig();

	/////////////////////////////
	int getSendInfo(int  respondId, sendInfo * psendBuf);
	void softVersion(sendInfo * spBuf);
	void mainVedioChannel(sendInfo * spBuf);
	void ack_updatepano(sendInfo * spBuf);
	void ack_fullscreenmode(sendInfo * spBuf);
	void trackErrOutput(sendInfo * spBuf);
	void mutilTargetNoticeStatus(sendInfo * spBuf);
	void multilTargetNumSelectStatus(sendInfo * spBuf);
	void  imageEnhanceStatus(sendInfo * spBuf);
	void trackFinetuningStat(sendInfo * spBuf);
	void confirmAxisStat(sendInfo * spBuf);
	void ElectronicZoomStat(sendInfo * spBuf);
	void trackSearchStat(sendInfo * spBuf);
	void moveTargetDetectedStat(sendInfo * spBuf);
	void frameFrequenceStat(sendInfo * spBuf);
	void vedioCompressStat(sendInfo * spBuf);
	void settingCmdRespon(sendInfo * spBuf);
	void readConfigSetting(sendInfo * spBuf);
	void extExtraInputResponse(sendInfo * spBuf);
	void paramtodef(sendInfo * spBuf);
	void  recordquerry(sendInfo * spBuf);
	void  ackplayertime(sendInfo * spBuf);
	void  ackscanplantformconfig(sendInfo * spBuf);
	void ackradarconfig(sendInfo * spBuf);
	void acktrackconfig(sendInfo * spBuf);
	void  ackplantformconfig(sendInfo * spBuf);
	void  ackrecordconfig(sendInfo * spBuf);
	void  ackmvconfig(sendInfo * spBuf);
	void ackdisplayconfig(sendInfo * spBuf);
	void  ackpanoconfig(sendInfo * spBuf);
	void  acksensortvconfig(sendInfo * spBuf);
	void  acksensortrkconfig(sendInfo * spBuf);
	void  acksensorfrconfig(sendInfo * spBuf);
	void  ackrecordconfig(sendInfo * spBuf,int classid);

	void upgradefwStat(sendInfo * spBuf);
	int upgradefw(unsigned char *swap_data_buf, unsigned int swap_data_len);
	int fw_update_runtar(void);
	int update_startsh(void);
	int update_fpga(void);
	u_int8_t  sendCheck_sum(uint len, u_int8_t *tmpbuf);
	u_int8_t package_frame(uint len, u_int8_t *tmpbuf);

	char *myStrncpy(char *dest, const char *src, int n);
	void GetSoftWareBuildTargetTime(void);

protected:
	int prcRcvFrameBufQue(int method);
	unsigned char check_sum(int len_t);
};

#endif
