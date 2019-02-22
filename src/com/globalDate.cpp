#include "globalDate.h"

CGlobalDate* CGlobalDate::_Instance = 0;

vector<int>  CGlobalDate::defConfigBuffer;

vector<Read_config_buffer>  CGlobalDate::readConfigBuffer;



CGlobalDate::CGlobalDate():EXT_Ctrl(Cmd_Mesg_Max), Host_Ctrl(40), commode(0), feedback(0), choose(0), IrisAndFocus_Ret(0), time_stop(0), frame(0)
{
	joystick_flag = 0;
	captureMode = 0;
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
	OSA_semCreate(&m_semHndl_socket,1,0);
	OSA_semCreate(&m_semHndl_socket_s,1,0);

	OSA_semCreate(&m_semHndl,1,0);
	OSA_semCreate(&m_semHndl_s,1,0);

	//OSA_semCreate(&m_semHndl_retest,1,0);
	//OSA_semCreate(&m_semHndl_automtd,1,0);
	memset(&ipc_mouseptz, 0, sizeof(ipc_mouseptz));
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
