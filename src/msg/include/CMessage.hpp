#ifndef __CMESSAGE_H__
#define __CMESSAGE_H__

#include <osa_thr.h>
#include <osa_msgq.h>
#include <assert.h>

#define SDK_SOK 0
#define SDK_EFAIL -1
#define MSGDRIV_FREE_CMD 0xFFFF
#define SDK_MSGQUE_SEND(to,from,cmd,prm,msgFlags,msg) OSA_msgqSendMsg(to,from,cmd,prm,msgFlags,msg)
#define SDK_ASSERT(f) assert(f)
#define SDK_TIMEOUT_FOREVER OSA_TIMEOUT_FOREVER

typedef OSA_MsgHndl MSG_ID;
typedef void (*MsgApiFun)(long lParam);

typedef enum _sys_msg_id_ {
    MSGID_SYS_INIT  = 0,
    MSGID_SYS_RESET,
    MSGID_EXT_INPUT_DISMOD,
    MSGID_EXT_INPUT_WorkModeCTRL,
    MSGID_EXT_INPUT_SIGLEinterrupt,
    MSGID_EXT_INPUT_Updatapano,
    MSGID_EXT_INPUT_MouseEvent,
    MSGID_EXT_INPUT_StoreMod,
    MSGID_EXT_INPUT_TRACKSEARCHCTRL,
    MSGID_EXT_INPUT_FuncMenu,
    MSGID_EXT_INPUT_AxisCtrl,
    MSGID_EXT_INPUT_IRISUPCTRL,
    MSGID_EXT_INPUT_IRISDOWNCTRL,
    MSGID_EXT_INPUT_FOCUSFARCHCTRL,
    MSGID_EXT_INPUT_FOCUSNEARCTRL,
    MSGID_EXT_INPUT_IMGENHCTRL,
    MSGID_EXT_INPUT_MMTCRTL,
    MSGID_EXT_INPUT_MMTSelect,
    MSGID_EXT_INPUT_AIMPOSXCTRL,
    MSGID_EXT_INPUT_AIMPOSYCTRL,
    MSGID_EXT_INPUT_PLATCTRL,
    MSGID_EXT_INPUT_SwitchSensor,
    MSGID_EXT_INPUT_MtdPreset,
    MSGID_EXT_INPUT_configWrite,
    MSGID_EXT_INPUT_configWrite_Save,
    MSGID_EXT_INPUT_config_Read,
    MSGID_EXT_INPUT_kboard,
    MSGID_EXT_INPUT_OSD,
    MSGID_EXT_INPUT_PRMBACK,
    MSGID_EXT_INPUT_zoomSpeed,
    MSGID_EXT_INPUT_captureSwitch,


    MSGID_EXT_INPUT_Getversion,

    MSGID_EXT_INPUT_PlayerCtl,
    MSGID_EXT_INPUT_PlayerSelect,
    MSGID_EXT_INPUT_PlayerQuerry,
    MSGID_EXT_INPUT_MVCONFIGENABLE,
    

    MSGID_EXT_INPUT_PlantfromConfig,
    MSGID_EXT_INPUT_SensorConfig,
    MSGID_EXT_INPUT_ZeroConfig,
    MSGID_EXT_INPUT_RecordConfig,
    MSGID_EXT_INPUT_MoveDetectConfig,
    MSGID_EXT_INPUT_MoveDetectAreaConfig,
    MSGID_EXT_INPUT_DisplayConfig,
    
    MSGID_EXT_INPUT_CorrectTimeConfig,
    MSGID_EXT_INPUT_PanoConfig,

    MSGID_EXT_INPUT_MVDETECTGO,

    MSGID_IPC_INPUT_TRACKCTRL,
    MSGID_IPC_INPUT_MTDCTRL,
    MSGID_IPC_INPUT_MMTCRTL,
    MSGID_IPC_INPUT_IMGENHCTRL,
    MSGID_IPC_INPUT_TRCKBOXSIZECTRL,
    MSGID_IPC_MainElectronicZoom,
    MSGID_IPC_Config,
    MSGID_IPC_OSD,
    MSGID_IPC_OSDTEXT,
    MSGID_IPC_UTC,
    MSGID_IPC_LOSTTIME,
    MSGID_IPC_WORDFONT,
    MSGID_IPC_WORDSIZE,
    MSGID_IPC_Camera,
    MSGID_IPC_Channel_binding,
    MSGID_IPC_AxisMove,
    MSGID_IPC_saveAxis,
    MSGID_IPC_picp,
    MSGID_IPC_chooseVideoChannel,
    MSGID_IPC_frameCtrl,
    MSGID_IPC_compression_quality,
    MSGID_IPC_word,
    MSGID_IPC_BoresightPos,
    MSGID_IPC_Algosdrect,
    MSGID_IPC_Resolution,
    MSGID_IPC_switchSensor,
    MSGID_IPC_AutoMtd,
    MSGID_IPC_Mtdoutput,
    MSGID_IPC_Mtdpolar,
    MSGID_IPC_QueryPos,
    MSGID_IPC_INPUT_Pos,
    MSGID_IPC_INPUT_PosAndZoom,
    MSGID_IPC_INPUT_NameAndPos,
    MSGID_EXT_INPUT_fontsize,
    MSGID_IPC_INPUT_reset_swtarget_timer,
    MAX_MSG_NUM
}eSysMsgId, MSG_PROC_ID;

typedef enum comm_id{
//jos  cmd message
	Cmd_Mesg_TrkCtrl = 0, 		//1
	Cmd_Mesg_Mtd,					//2
	Cmd_Mesg_ZoomLong,		//3
	Cmd_Mesg_ZoomShort,		//4
	Cmd_Mesg_IrisAndFocusAndExit,		//5
	Cmd_Mesg_TrkSearch,			//6
	Cmd_Mesg_FuncMenu,				//7
	Cmd_Mesg_AxisCtrl,		//8
			//9
			//10
	Cmd_Mesg_SensorCtrl = 11, 		//11
	Cmd_Mesg_Mmt,					//12
	Cmd_Mesg_AIMPOS_X,					//pov
	Cmd_Mesg_AIMPOS_Y,					//pov
	Cmd_Mesg_AXISX,				//15
	Cmd_Mesg_AXISY,				//16
	Cmd_Mesg_ImgEnh,				//17
	Cmd_Mesg_PresetCtrl,        //18
	Cmd_Mesg_IrisUp,     //19
	Cmd_Mesg_IrisDown,  //20
	Cmd_Mesg_FocusFar,  //21
	Cmd_Mesg_FocusNear,  //22

	//ipc
	Cmd_IPC_TrkCtrl,
	Cmd_IPC_Config,
	Cmd_IPC_OSD,
	Cmd_IPC_UTC,
	Cmd_IPC_losttime,
	Cmd_IPC_Camera,
	Cmd_IPC_BoresightPos,
	Cmd_IPC_Algosdrect,
	Cmd_IPC_Resolution,
	Cmd_IPC_Resolution1,
	Cmd_IPC_Resolution2,
	Cmd_IPC_Resolution3,
	Cmd_IPC_Resolution4,
	Cmd_Set_Resolution,

	//network
	Cmd_Mesg_SelfTest,
	Cmd_Mesg_mainVideoSwitch,
	Cmd_Mesg_Channel_binding,
	Cmd_Mesg_AxisMove,
	Cmd_Mesg_MainElectronicZoom,
	Cmd_Mesg_PipElectronicZoom,
	Cmd_Mesg_saveAxis,
	Cmd_Mesg_Picp,
	Cmd_Mesg_switchVideoChannel,
	Cmd_Mesg_frameCtrl,
	Cmd_Mesg_compression_quality,
	Cmd_Mesg_wordColor,
	Cmd_Mesg_transparency,
	Cmd_Mesg_send,
	Cmd_Mesg_word,
	Cmd_Mesg_CharacterID,
	Cmd_Mesg_wordType,
	Cmd_Mesg_wordSize,
	Cmd_Mesg_wordDisEnable,
	Cmd_Mesg_position,
	Cmd_Mesg_config_Write,
	Cmd_Mesg_config_Write_Save,
	Cmd_Mesg_config_Read,
	Cmd_Mesg_jos_kboard,
	Cmd_Mesg_WorkMode,
	Cmd_Mesg_Osd,
	Cmd_Mesg_SensorMode,
	Cmd_Mesg_TVFov,
	Cmd_Mesg_PALFov,
	Cmd_Mesg_Pan,   //53
	Cmd_Mesg_Tilt,    //54
	Cmd_Mesg_PanSpeed,    //55
	Cmd_Mesg_TiltSpeed,     //56
	Cmd_IPC_Pan,
	Cmd_IPC_Tilt,
	Cmd_Mesg_VideoCompression,
	Cmd_Mesg_TrkMode,
	Cmd_Mesg_CheckMode,
	Cmd_Mesg_EnhMode,
	Cmd_Mesg_zoomSpeed,

	Cmd_Mesg_MmtSelect,
	Cmd_Mesg_paramToDef,
	Cmd_Mesg_switchSensor,
	Cmd_Mesg_Max,
}comm_inHouse;

typedef enum SpecialEvent{
	MSGID_INPUT_AXISX = 0,
	MSGID_INPUT_AXISY = 1,
	MSGID_INPUT_ImgEnh = 3,				//throttle
	MSGID_INPUT__POVX = 5,
	MSGID_INPUT__POVY = 6,
}Jos_Special;

typedef enum ButtonEvent{
	MSGID_INPUT_TrkCtrl = 0, 		//1
	MSGID_INPUT_Mtd,					//2
	MSGID_INPUT_ZoomLong,		//3
	MSGID_INPUT_ZoomShort,		//4
	MSGID_INPUT_IrisAndFocusAndExit,		//5
	MSGID_INPUT_TrkSearch,		//6
	MSGID_INPUT_FuncMenu,				//7
	MSGID_INPUT_AxisCtrl,			//8
	MSGID_INPUT_9,		//9
	MSGID_INPUT_10,	//10
	MSGID_INPUT_SensorCtrl = 10 ,		//11
	//MSGID_INPUT_IMG,
	MSGID_INPUT_MtdMode,				//12
	// 3 + 4
	MSGID_INPUT_Max
}Jos_Button;

typedef enum {
	selfTest = 0,
	mainVideo,
	Channel_binding,
	moveAxisX,
	moveAxisY,
	ElectronicZoom,
	saveAxis,
	picp,
	picpChannel,
	chooseVideoChannel,
	frameCtrl,
	frameChannel,
	compression_quality,
	compressionChannel,
	wordColor,
	transparencyM,
	cNum,
	wordType,
	wordSize,
	wordDisEnable,
	XpositionM,
	YpositionM,
	config_Wblock,
	config_Wfield,
	config_Wvalue,
	config_Rblock,
	config_Rfield,
	config_Rvalue,
	config_read,
	zoomSpeed,
	jos_Kboard,
	jos_Kboard2
}Host_CtrlInput;

typedef enum{
	ACK_selfTest_value = 0,  //
	ACK_Channel_bindingStatus_value,  //
	ACK_posMove_value,    //
	ACK_moveAxis_value = 4, //
	ACK_mmtSelect_value = 6,   //
	ACK_ElectronicZoom_value,
	ACK_TrkSearch_value,
	ACK_config_Wblock,
	ACK_config_Wfield,
	ACK_config_Wvalue,
	ACK_config_Rblock = 12,
	ACK_config_Rfield,
	ACK_config_Rvalue,
	ACK_wordColor_value,   //
	ACK_wordType_value,  //
	ACK_wordSize_value,		//
	NAK_wordColor_value,	//
	NAK_wordType_value,	  //
	NAK_wordSize_value,		//
	ACK_jos_Kboard_value,		//
	ACK_param_todef_value,
	ACK_value_max,
}ACK_Host_Value;

typedef enum{
	ACK_selfTest = 0,
	ACK_wordColor,
	ACK_wordType,
	ACK_wordSize,
	NAK_wordColor,
	NAK_wordType,
	NAK_wordSize,
	ACK_mainVideoStatus,
	ACK_softVersion,
	ACK_Channel_bindingStatus,  //
	ACK_avtTrkStatus,
	ACK_avtTrkType,
	ACK_avtErrorOutput,
	ACK_mmtStatus,
	ACK_mmtSelectStatus,  //
	ACK_EnhStatus,
	ACK_MtdStatus,
	ACK_TrkSearchStatus,
	ACK_posMoveStatus,   //
	ACK_moveAxisStatus,  //
	ACK_ElectronicZoomStatus,
	ACK_picpStatus,
	ACK_VideoChannelStatus,
	ACK_frameCtrlStatus,
	ACK_compression_quality,
	ACK_config_Write,
	ACK_config_Read,
	ACK_jos_Kboard,
	ACK_upgradefw,

	
	ACK_param_todef,
	ACK_playerquerry=0x62,
	ACK_plantformconfig=0x80,
	ACK_sensorconfig,
	ACK_zeroconfig,
	ACK_recordconfig,
	ACK_mvconfig,
	ACK_mvareaconfig,
	ACK_displayconfig,
	ACK_correcttimeconfig,
	ACK_panoconfig,
	ACK_recordconfigmv,
	
	ACK_MAX,
}ACK_Host_CtrlInput;

typedef struct _Msg_Tab
{
    int msgId;
    long refContext;
    MsgApiFun pRtnFun;
}MSGTAB_Class;

typedef struct msgdriv
{
    unsigned short  bIntial;
    OSA_ThrHndl tskHndl;
    unsigned short tskLoop;
    unsigned short istskStopDone;
    OSA_MsgqHndl msgQue;
    MSGTAB_Class msgTab[MAX_MSG_NUM];
}MSGDRIV_Class, *MSGDRIV_Handle;

class CMessage{
public: 
    MSGDRIV_Handle  MSGDRIV_create();
    void MSGDRIV_destroy(MSGDRIV_Handle handle);
    void MSGDRIV_register(int msgId, MsgApiFun pRtnFun, int context);
    void MSGDRIV_send(int msgId, void *prm);
    MSGDRIV_Class g_MsgDrvObj;
	
    static CMessage* getInstance();

private:
    CMessage();
    CMessage(const CMessage&);
    CMessage& operator=(const CMessage&);
    static CMessage* instance;

    ~CMessage();
    void* MSGDRIV_ProcTask();
	
protected:
    static void *pMSGDRIV_ProcTask(void * mContext)
    {
        CMessage* sPthis = (CMessage*)mContext;
        sPthis->MSGDRIV_ProcTask();
        return NULL;
    }
};

#endif
