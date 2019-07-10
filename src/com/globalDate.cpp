#include "globalDate.h"

CGlobalDate* CGlobalDate::_Instance = 0;

vector<int>  CGlobalDate::defConfigBuffer;

vector<Read_config_buffer>  CGlobalDate::readConfigBuffer;



CGlobalDate::CGlobalDate():Host_Ctrl(40), feedback(0), choose(0), time_stop(0), frame(0)
{
	target = time_start = respupgradefw_stat = respupgradefw_perc = respexpconfig_stat = respexpconfig_len = ImgMtdStat = mtdMode = ThreeMode_bak = 0;
	memset(respexpconfig_buf, 0, sizeof(respexpconfig_buf));
	Mtd_Limit = recv_AutoMtdDate;
	Sync_Query = 0;
	Mtd_Moitor = Mtd_Moitor_X = Mtd_Moitor_Y = 0;
	Mtd_ipc_target = 0;
	Mtd_ExitMode = Manual;
	MtdAutoredetect_sync = 1;
	rcv_zoomValue = 0;
	MtdAutoLoop = false;
	OSA_semCreate(&m_semHndl,1,0);
	OSA_semCreate(&m_semHndl_socket_client,1,0);
	set_sendfile_status(0);

	memset(&mtdconfig, 0, sizeof(mtdconfig));
	memset(&mainProStat, 0, sizeof(mainProStat));
	//mtdconfig.trktime = 3* 1000;

}

CGlobalDate* CGlobalDate::Instance()
{
	if(_Instance == 0)
		_Instance =  new CGlobalDate();
	return _Instance;
}

void CGlobalDate::milliseconds_sleep(unsigned long mSec)
{
    struct timeval tv;

    int err;
    do{
        tv.tv_sec = mSec/1000;
        tv.tv_usec = (mSec%1000)*1000;
        err = select(0, NULL, NULL, NULL, &tv);
    }while(err<0 && errno==EINTR);
}