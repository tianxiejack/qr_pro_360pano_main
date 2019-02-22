#include <stdio.h>
#include <stdlib.h>
#include "statCtrl.h"

CStatusManager* CStatusManager::_Instance = 0;

	CStatusManager::CStatusManager()
	{
		status_Init();
	}
	CStatusManager::~CStatusManager()
	{

	}

CStatusManager* CStatusManager::Instance()
	{
		if(_Instance == 0)
			_Instance = new CStatusManager();
		return _Instance;
	}

void CStatusManager::status_Init()
{
	PID_X = PID_Y = TRK_Bleed;
	Two_Mode = STATE_PLANTFORM_ACQ;
	Three_Mode = THREE_CTRLPTZ;
	ThreeMode_back1 = THREE_CTRLPTZ;
	Avtmode = TRK_PID;
	BOX_ipc = BOX_FIXEDLY;
	ACQ_PTZ_switchInitX = ACQ_PTZ_switchInitY = TRK_Bleed;
}

	void CStatusManager::two_State_PlantForm_Acq()
	{
		Two_Mode = STATE_PLANTFORM_ACQ;
	}

	void CStatusManager::two_State_PlantForm_Trk()
	{
		Two_Mode = STATE_PLANTFORM_TRK;
	}

	void CStatusManager::two_State_PlantForm_Mtd()
	{
		Two_Mode = STATE_PLANTFORM_MTD;
	}

	void CStatusManager::three_State_CtrlPtz()
	{
		Three_Mode = THREE_CTRLPTZ;
	}

	void CStatusManager::three_State_CtrlBox()
	{
		Three_Mode = THREE_CTRLBOX;
	}

	void CStatusManager::three_State_Trksearch()
	{
		if(Three_Mode != THREE_TRK_SEARCH)
			ThreeMode_back1 = Three_Mode;
		Three_Mode = THREE_TRK_SEARCH;
	}

	void CStatusManager::three_State_MtdAuto()
	{
		Three_Mode = THREE_MTD_Auto;
	}

	void CStatusManager::three_State_MtdManual()
	{
		Three_Mode = THREE_MTD_Manual;
	}

	void CStatusManager::Quit_TrkSearch()
	{
		Three_Mode = ThreeMode_back1;
	}

	void CStatusManager::trk_State_TrkPIDInitX()
	{
		PIDInitX = 1;
	}

	void CStatusManager::trk_State_TrkPIDInitY()
	{
		PIDInitY = 1;
	}

	void CStatusManager::trk_State_PIDInitEndX()
	{
		PIDInitX = 0;
	}
	void CStatusManager::trk_State_PIDInitEndY()
	{
		PIDInitY = 0;
	}

	void CStatusManager::WorkMode_IrisAndFocus()
	{
		Avtmode = IRIS_FOCUS;
	}

	int CStatusManager::AvtInput_Switch()
	{
		if(isTwo_PlantForm_Acq() && isThree_CtrlPtz())
			AvtInput = VIRTUALINPUT_OPEN;
		else if(isTwo_PlantForm_Trk())
			AvtInput = VIRTUALINPUT_CLOSE;
		else if(isPtz_Iris_Focus())
			AvtInput = VIRTUALINPUT_CLOSE;


		return 0;
	}

	void CStatusManager::TimeOut()
	{
		PID_X = PID_Y = TRK_PID;
	}

	void CStatusManager::TimeReset()
	{
		PID_X = PID_Y = TRK_Bleed;
	}

	void CStatusManager::BoxCtrl_IPC(int CtrlTrkStat)
	{
		if (isThree_CtrlBox())
		{
			if(CtrlTrkStat == 1)
				BOX_ipc = BOX_TRK;
			else
				BOX_ipc = BOX_MOVE;
		}
		else
			BOX_ipc = BOX_FIXEDLY;
	}

int CStatusManager::BOX_ipc_Interface()
{
	return BOX_ipc;
}

void CStatusManager::StatusInit()
{
	PID_X = PID_Y = TRK_Bleed;
}

