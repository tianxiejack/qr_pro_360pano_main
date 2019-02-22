/*
 * statCtrl.h
 *
 *  Created on: 2018年9月12日
 *      Author: jet
 */

#ifndef STATCTRL_H_
#define STATCTRL_H_

#include <iostream>
#include <time.h>
#include"osa.h"
#include"osa_sem.h"


using namespace std;


typedef enum Level_one_state{
	MODE_SELFTEST 	= 0x0,
	MODE_WORK,
} Level_one_state;


typedef enum Level_two_state{
	STATE_PLANTFORM_ACQ = 0,
	STATE_PLANTFORM_TRK,
	STATE_PLANTFORM_MTD
} Level_two_state;

typedef enum Level_three_state{
	THREE_CTRLPTZ,
	THREE_CTRLBOX,
	THREE_TRK_SEARCH,
	THREE_MTD_Auto,
	THREE_MTD_Manual
}Level_three_state;


typedef enum WorkMode{
	TRK_PID = 0,
	TRK_Bleed,
	IRIS_FOCUS,
	VIRTUALINPUT_OPEN,
	VIRTUALINPUT_CLOSE
}WorkMode;

typedef enum BOX_IPC{
	BOX_FIXEDLY = 0,
	BOX_TRK,
	BOX_MOVE
}BOX_IPC;


class CStatusManager
{
public:

	static CStatusManager* Instance();
	~CStatusManager();

	Level_two_state Two_Mode;

	Level_three_state Three_Mode, ThreeMode_back1;

	WorkMode PID_X, PID_Y;

	WorkMode ACQ_PTZ_switchInitX, ACQ_PTZ_switchInitY;

	WorkMode AvtInput, Avtmode;

	BOX_IPC BOX_ipc;

public:
	void two_State_PlantForm_Acq();
	void two_State_PlantForm_Trk();
	void two_State_PlantForm_Mtd();

	void three_State_CtrlPtz();
	void three_State_CtrlBox();
	void three_State_Trksearch();
	void three_State_MtdAuto();
	void three_State_MtdManual();
	void Quit_TrkSearch();

	void trk_State_TrkPIDInitX();
	void trk_State_TrkPIDInitY();
	void trk_State_PIDInitEndX();
	void trk_State_PIDInitEndY();

	void WorkMode_IrisAndFocus();

	int AvtInput_Switch();
	void TimeOut();
	void TimeReset();
	void StatusInit();

	void BoxCtrl_IPC(int CtrlTrkStat);
	int BOX_ipc_Interface();

private:

	static CStatusManager* _Instance;
	CStatusManager();
	int PIDInitX, PIDInitY;

	void status_Init();

public:
	const bool isSelfTest()
	{
		return 0;
	}
	const bool isWork()
	{
		return 0;
	}

//

	const bool isTwo_PlantForm_Acq()
	{
		return Two_Mode == STATE_PLANTFORM_ACQ;
	}

	const bool isTwo_PlantForm_Trk()
	{
		return Two_Mode == STATE_PLANTFORM_TRK;
	}

	const bool isTwo_PlantForm_Mtd()
	{
		return Two_Mode == STATE_PLANTFORM_MTD;
	}

//

	const bool isTimeOut_x()
	{
		return PID_X == TRK_PID;
	}

	const bool isTimeOut_Y()
	{
		return PID_Y == TRK_PID;
	}

	const bool isThree_CtrlPtz()
	{
		return Three_Mode == 	THREE_CTRLPTZ;
	}

	const bool isThree_CtrlBox()
	{
		return Three_Mode == 	THREE_CTRLBOX;
	}

	const bool isThree_TrkSearch()
	{
		return Three_Mode == THREE_TRK_SEARCH;
	}

	const bool isThree_MtdAuto()
	{
		return Three_Mode == THREE_MTD_Auto;
	}

	const bool isThree_MtdManual()
	{
		return Three_Mode == THREE_MTD_Manual;
	}

//

	const bool isPtz_Iris_Focus()
	{
		return Avtmode == IRIS_FOCUS;
	}

	const bool isCloseExtInput()
	{
		return AvtInput == VIRTUALINPUT_CLOSE;
	}

	const bool isTrkPIDInitX()
	{
		return PIDInitX == 1;
	}

	const bool isTrkPIDInitY()
	{
		return PIDInitY == 1;
	}

	const bool isBOX_Trk()
	{
		return BOX_ipc == BOX_TRK;
	}

};



#endif /* STATCTRL_H_ */
