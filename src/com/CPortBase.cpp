#include "CPortBase.hpp"
#include"Status.hpp"

CPortBase::CPortBase()
{
	pM = getpM();
	_globalDate = getDate();
	_StateManager = getStatus();
}

CPortBase::~CPortBase()
{

}

CMessage * CPortBase::getpM()
{
    pM =  CMessage::getInstance();
    return pM;
}

CGlobalDate* CPortBase::getDate()
{
	_globalDate = CGlobalDate::Instance();
	return _globalDate;
}

CStatusManager* CPortBase::getStatus()
{
	_StateManager = CStatusManager::Instance();
	return _StateManager;
}

void CPortBase::EnableSelfTest()
{
    printf("send %s msg\n",__FUNCTION__);
}

void CPortBase::EnableswitchVideoChannel()
{
  	
}

void CPortBase:: selectVideoChannel()
{
   
}

void CPortBase::EnableTrk()
{
   
}
void CPortBase::mouseevent(int event)
{
	if(event==Status::MOUSEBUTTON)
		{
			if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->mouseleftright)
				Status::getinstance()->mouseleftright=_globalDate->rcvBufQue.at(6);
			if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->mousebuttonstaus)
				Status::getinstance()->mousebuttonstaus=_globalDate->rcvBufQue.at(7);
			Status::getinstance()->mousex=_globalDate->rcvBufQue.at(8)<<8|_globalDate->rcvBufQue.at(9);
			Status::getinstance()->mousey=_globalDate->rcvBufQue.at(10)<<8|_globalDate->rcvBufQue.at(11);

			
			OSA_printf("%s the x1=%d x2=%d y1=%d y2=%d \n",__func__,_globalDate->rcvBufQue.at(8),_globalDate->rcvBufQue.at(9),
				_globalDate->rcvBufQue.at(10),_globalDate->rcvBufQue.at(11));
			OSA_printf("%s the x=%d y=%d \n",__func__,Status::getinstance()->mousex,Status::getinstance()->mousey);
			
			pM->MSGDRIV_send(MSGID_EXT_INPUT_MouseEvent, (void *)(Status::MOUSEBUTTON));
		}
	else if(event==Status::MOUSEROLLER)
		{
			Status::getinstance()->setrigion(_globalDate->rcvBufQue.at(5));
			Status::getinstance()->setzoomstat(_globalDate->rcvBufQue.at(6));
			pM->MSGDRIV_send(MSGID_EXT_INPUT_MouseEvent, (void *)(Status::MOUSEROLLER));
		}

	

}




void CPortBase::displaymod()
{
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->getdisplaymod())
	{
		Status::getinstance()->setdisplaymod(_globalDate->rcvBufQue.at(5));
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->getdisplaysensor())
	{
		Status::getinstance()->setdisplaysensor(_globalDate->rcvBufQue.at(6));
	}
		

	if(Status::getinstance()->getdisplaymod()==Status::LIVEMOD)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_DISMOD, (void *)(Status::LIVEMOD));
	else if(Status::getinstance()->getdisplaymod()==Status::PLAYCALLBACK)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_DISMOD, (void *)(Status::PLAYCALLBACK));

	
	

	//OSA_semSignal(&_globalDate->m_semHndl_socket_s);
  
}


void CPortBase::playercontrl()
	{
		if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->playercontrl)
		{
			Status::getinstance()->playercontrl=_globalDate->rcvBufQue.at(5);
		}

		if(Status::getinstance()->playercontrl==Status::PLAYERSTOP)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerCtl, (void *)(Status::PLAYERSTOP));
		if(Status::getinstance()->playercontrl==Status::PLAYERRUN)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerCtl, (void *)(Status::PLAYERRUN));
		if(Status::getinstance()->playercontrl==Status::PLAYERACC)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerCtl, (void *)(Status::PLAYERDEC));
		if(Status::getinstance()->playercontrl==Status::PLAYERDEC)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerCtl, (void *)(Status::PLAYERACC));
			
	};
void CPortBase::playerquery()
	{
		
		if((_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6))!=Status::getinstance()->playerqueryyear)
		{
			Status::getinstance()->playerqueryyear=_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6);
		}
		if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->playerquerymon)
		{
			Status::getinstance()->playerquerymon=_globalDate->rcvBufQue.at(7);
		}
		if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->playerqueryday)
		{
			Status::getinstance()->playerqueryday=_globalDate->rcvBufQue.at(8);
		}

		OSA_printf("%s the year=%d mon=%d day=%d\n",__func__,Status::getinstance()->playerqueryyear,Status::getinstance()->playerquerymon,Status::getinstance()->playerqueryday);

		pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerQuerry, 0);
		
	};
void CPortBase::playerselect()
	{
		if((_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6))!=Status::getinstance()->playeryear)
		{
			Status::getinstance()->playeryear=_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6);
		}
		if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->playermonth)
		{
			Status::getinstance()->playermonth=_globalDate->rcvBufQue.at(7);
		}
		if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->playerday)
		{
			Status::getinstance()->playerday=_globalDate->rcvBufQue.at(8);
		}
		if(_globalDate->rcvBufQue.at(9)!=Status::getinstance()->playerhour)
		{
			Status::getinstance()->playerhour=_globalDate->rcvBufQue.at(9);
		}
		if(_globalDate->rcvBufQue.at(10)!=Status::getinstance()->playermin)
		{
			Status::getinstance()->playermin=_globalDate->rcvBufQue.at(10);
		}
		if(_globalDate->rcvBufQue.at(11)!=Status::getinstance()->playersec)
		{
			Status::getinstance()->playersec=_globalDate->rcvBufQue.at(11);
		}

		OSA_printf("%s the h=%d m=%d s=%d\n",__func__,Status::getinstance()->playerhour,Status::getinstance()->playermin,Status::getinstance()->playersec);

		pM->MSGDRIV_send(MSGID_EXT_INPUT_PlayerSelect, 0);
		
	};

void CPortBase::livevideo()
{
	if((_globalDate->rcvBufQue.at(5))!=Status::getinstance()->livevideoflg)
	{
		Status::getinstance()->livevideoflg=_globalDate->rcvBufQue.at(5);
	}

	pM->MSGDRIV_send(MSGID_EXT_INPUT_LIVEVIDEO, 0);
}

void CPortBase::panoenable()
{
	if((_globalDate->rcvBufQue.at(5))!=Status::getinstance()->mvconfigenable)
		{
			Status::getinstance()->mvconfigenable=_globalDate->rcvBufQue.at(5);
		}
	//printf("Status::getinstance()->mvconfigenable=%d\n",Status::getinstance()->mvconfigenable);
	pM->MSGDRIV_send(MSGID_EXT_INPUT_MVCONFIGENABLE, 0);
}

void CPortBase::scan_plantformconfig()
{
	int configchange=0;
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->scan_platformcfg.address)
	{
		Status::getinstance()->scan_platformcfg.address=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->scan_platformcfg.protocol)
	{
		Status::getinstance()->scan_platformcfg.protocol=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->scan_platformcfg.baudrate)
	{
		Status::getinstance()->scan_platformcfg.baudrate=_globalDate->rcvBufQue.at(7);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->scan_platformcfg.start_signal)
	{
		Status::getinstance()->scan_platformcfg.start_signal=_globalDate->rcvBufQue.at(8);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(9)!=Status::getinstance()->scan_platformcfg.pt_check)
	{
		Status::getinstance()->scan_platformcfg.pt_check=_globalDate->rcvBufQue.at(9);
		configchange=1;
	}
	
	
	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_ScanPlantfromConfig, 0);
}

void CPortBase::radarconfig()
{
	int configchange=0;
	int offset50 = _globalDate->rcvBufQue.at(7) | (_globalDate->rcvBufQue.at(8) << 8);
	int offset100 = _globalDate->rcvBufQue.at(9) | (_globalDate->rcvBufQue.at(10) << 10);
	int offset300 = _globalDate->rcvBufQue.at(11) | (_globalDate->rcvBufQue.at(12) << 12);
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->radarcfg.sensor)
	{
		Status::getinstance()->radarcfg.sensor=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->radarcfg.hideline)
	{
		Status::getinstance()->radarcfg.hideline=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(offset50!=Status::getinstance()->radarcfg.offset50m)
	{
		Status::getinstance()->radarcfg.offset50m=offset50;
		configchange=1;
	}
	if(offset100!=Status::getinstance()->radarcfg.offset100m)
	{
		Status::getinstance()->radarcfg.offset100m=offset100;
		configchange=1;
	}
	if(offset300!=Status::getinstance()->radarcfg.offset300m)
	{
		Status::getinstance()->radarcfg.offset300m=offset300;
		configchange=1;
	}
	
	
	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_RadarConfig, 0);
}

void CPortBase::trackconfig()
{
	int configchange=0;

	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->trackcfg.trkprio)
	{
		Status::getinstance()->trackcfg.trkprio=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->trackcfg.trktime)
	{
		Status::getinstance()->trackcfg.trktime=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	
	
	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_TrackConfig, 0);
}

void CPortBase::adddevconfig()
{
	Status::getinstance()->adddevcfg.devid=_globalDate->rcvBufQue.at(5);
	for(int i = 0; i < 16; i++)
	{
		Status::getinstance()->adddevcfg.ip[i] = _globalDate->rcvBufQue.at(i+6);
	}
	
	pM->MSGDRIV_send(MSGID_EXT_INPUT_AdddevConfig, 0);

}

void CPortBase::deletedevconfig()
{
	Status::getinstance()->deldevid=_globalDate->rcvBufQue.at(5);
	pM->MSGDRIV_send(MSGID_EXT_INPUT_DeldevConfig, 0);
}

void CPortBase::plantformconfig()
{
	int configchange=0;
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->ptzaddress)
		{
			Status::getinstance()->ptzaddress=_globalDate->rcvBufQue.at(5);
			configchange=1;
		}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->ptzprotocal)
		{
			Status::getinstance()->ptzprotocal=_globalDate->rcvBufQue.at(6);
			configchange=1;
		}
	if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->ptzbaudrate)
		{
			Status::getinstance()->ptzbaudrate=_globalDate->rcvBufQue.at(7);
			configchange=1;
		}
	if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->ptzspeed)
		{
			Status::getinstance()->ptzspeed=_globalDate->rcvBufQue.at(8);
			configchange=1;
		}
	
	
	if(configchange)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_PlantfromConfig, 0);
	
}

void CPortBase::sensortvconfig()
{
	int configchange=0;
	int time;
	
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->sensortvcfg.outputresol)
	{
		Status::getinstance()->sensortvcfg.outputresol=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->sensortvcfg.brightness)
	{
		Status::getinstance()->sensortvcfg.brightness=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->sensortvcfg.contract)
	{
		Status::getinstance()->sensortvcfg.contract=_globalDate->rcvBufQue.at(7);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->sensortvcfg.autobright)
	{
		Status::getinstance()->sensortvcfg.autobright=_globalDate->rcvBufQue.at(8);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(9)!=Status::getinstance()->sensortvcfg.backlight)
	{
		Status::getinstance()->sensortvcfg.backlight=_globalDate->rcvBufQue.at(9);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(10)!=Status::getinstance()->sensortvcfg.whitebalance)
	{
		Status::getinstance()->sensortvcfg.whitebalance=_globalDate->rcvBufQue.at(10);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(11)!=Status::getinstance()->sensortvcfg.gain)
	{
		Status::getinstance()->sensortvcfg.gain=_globalDate->rcvBufQue.at(11);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(12)!=Status::getinstance()->sensortvcfg.daynight)
	{
		Status::getinstance()->sensortvcfg.daynight=_globalDate->rcvBufQue.at(12);
		configchange=1;
	}

	if(_globalDate->rcvBufQue.at(13)!=Status::getinstance()->sensortvcfg.stronglightsup)
	{
		Status::getinstance()->sensortvcfg.stronglightsup=_globalDate->rcvBufQue.at(13);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(14)!=Status::getinstance()->sensortvcfg.exposuremode)
	{
		Status::getinstance()->sensortvcfg.exposuremode=_globalDate->rcvBufQue.at(14);
		configchange=1;
	}

	time = _globalDate->rcvBufQue.at(15) | (_globalDate->rcvBufQue.at(16) << 8);
	if(time!=Status::getinstance()->sensortvcfg.elecshutter_time)
	{
		Status::getinstance()->sensortvcfg.elecshutter_time=time;
		configchange=1;
	}


	if(configchange)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_SensorTVConfig, 0);
}

void CPortBase::sensortrkconfig()
{
	int configchange=0;
	int time;
	
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->sensortrkcfg.outputresol)
	{
		Status::getinstance()->sensortrkcfg.outputresol=_globalDate->rcvBufQue.at(5);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->sensortrkcfg.brightness)
	{
		Status::getinstance()->sensortrkcfg.brightness=_globalDate->rcvBufQue.at(6);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->sensortrkcfg.contract)
	{
		Status::getinstance()->sensortrkcfg.contract=_globalDate->rcvBufQue.at(7);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->sensortrkcfg.autobright)
	{
		Status::getinstance()->sensortrkcfg.autobright=_globalDate->rcvBufQue.at(8);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(9)!=Status::getinstance()->sensortrkcfg.backlight)
	{
		Status::getinstance()->sensortrkcfg.backlight=_globalDate->rcvBufQue.at(9);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(10)!=Status::getinstance()->sensortrkcfg.whitebalance)
	{
		Status::getinstance()->sensortrkcfg.whitebalance=_globalDate->rcvBufQue.at(10);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(11)!=Status::getinstance()->sensortrkcfg.gain)
	{
		Status::getinstance()->sensortrkcfg.gain=_globalDate->rcvBufQue.at(11);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(12)!=Status::getinstance()->sensortrkcfg.daynight)
	{
		Status::getinstance()->sensortrkcfg.daynight=_globalDate->rcvBufQue.at(12);
		configchange=1;
	}

	if(_globalDate->rcvBufQue.at(13)!=Status::getinstance()->sensortrkcfg.stronglightsup)
	{
		Status::getinstance()->sensortrkcfg.stronglightsup=_globalDate->rcvBufQue.at(13);
		configchange=1;
	}
	if(_globalDate->rcvBufQue.at(14)!=Status::getinstance()->sensortrkcfg.exposuremode)
	{
		Status::getinstance()->sensortrkcfg.exposuremode=_globalDate->rcvBufQue.at(14);
		configchange=1;
	}

	time = _globalDate->rcvBufQue.at(15) | (_globalDate->rcvBufQue.at(16) << 8);
	if(time!=Status::getinstance()->sensortrkcfg.elecshutter_time)
	{
		Status::getinstance()->sensortrkcfg.elecshutter_time=time;
		configchange=1;
	}


	if(configchange)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_SensorTRKConfig, 0);
}

 void CPortBase::sensorconfig()
 	{
 		int configchange=0;
		if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->brightness)
			{
				Status::getinstance()->brightness=_globalDate->rcvBufQue.at(5);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->contract)
			{
				Status::getinstance()->contract=_globalDate->rcvBufQue.at(6);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->autobright)
			{
				Status::getinstance()->autobright=_globalDate->rcvBufQue.at(7);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->backandwrite)
			{
				
				Status::getinstance()->backandwrite=_globalDate->rcvBufQue.at(8);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(9)!=Status::getinstance()->correct)
			{
				Status::getinstance()->correct=_globalDate->rcvBufQue.at(9);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(10)!=Status::getinstance()->digitfilter)
			{
				Status::getinstance()->digitfilter=_globalDate->rcvBufQue.at(10);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(11)!=Status::getinstance()->digitenhance)
			{
				Status::getinstance()->digitenhance=_globalDate->rcvBufQue.at(11);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(12)!=Status::getinstance()->mirror)
			{
				Status::getinstance()->mirror=_globalDate->rcvBufQue.at(12);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(13)!=Status::getinstance()->outputresol)
			{
				Status::getinstance()->outputresol=_globalDate->rcvBufQue.at(13);
				configchange=1;
			}
		
		
		if(configchange)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_SensorConfig, 0);


 	};
 void CPortBase::zeroconfig()
 	{
 		int configchange=0;
		configchange=0;
 		if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->zeromod)
			{
				Status::getinstance()->zeromod=_globalDate->rcvBufQue.at(5);
				configchange=1;
			}
		OSA_printf("%s zeromod=%d\n",__func__,Status::getinstance()->zeromod);
		if(configchange)
			{
				if(Status::getinstance()->zeromod==0)
					pM->MSGDRIV_send(MSGID_EXT_INPUT_ZeroConfig, (void *)Status::ZEROSTOP);
				else if(Status::getinstance()->zeromod==1)
				pM->MSGDRIV_send(MSGID_EXT_INPUT_ZeroConfig, (void *)Status::ZEROSTART);
				else if(Status::getinstance()->zeromod==2)
				pM->MSGDRIV_send(MSGID_EXT_INPUT_ZeroConfig, (void *)Status::ZEROSAVE);
			}

		
 		
 	};
void CPortBase::recordconfig()
	{
		
		int configchange=1;
		
		int recordclass=_globalDate->rcvBufQue.at(5);
		int bitnum=8;
		
		for(int i=0;i<HELDWEEK;i++)
			{
				for(int j=0;j<HELDHOUR;j++)
					{
						//printf("%d \t",_globalDate->rcvBufQue.at(6+i*3));
						if(j<8)
							{
								Status::getinstance()->recordpositionheld[recordclass][i][bitnum-1-j]=(_globalDate->rcvBufQue.at(6+i*3)>>j)&0x01;
							//printf("test%d \t",Status::getinstance()->recordpositionheld[recordclass][i][bitnum-1-j]);
							}
						else if(j<16)
								Status::getinstance()->recordpositionheld[recordclass][i][2*bitnum-1-(j-8)]=(_globalDate->rcvBufQue.at(6+i*3+1)>>(j-bitnum))&0x01;
						else if(j<24)
							{
								Status::getinstance()->recordpositionheld[recordclass][i][3*bitnum-1-(j-16)]=(_globalDate->rcvBufQue.at(6+i*3+2)>>(j-2*bitnum))&0x01;
							//printf("test_%d \t",Status::getinstance()->recordpositionheld[recordclass][i][3*bitnum-1-(j-24)]);
							}
					}

			}
		
		/*
		printf("the time\n");
		for(int i=0;i<HELDWEEK;i++)
			{
				for(int j=0;j<HELDHOUR;j++)
					{
						if(j<8)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);
						else if(j<16)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);
						else if(j<24)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);					
					}
				printf("\n");

			}
		*/
			
		/*
		printf("the mov\n");
		for(int i=0;i<HELDWEEK;i++)
			{
				for(int j=0;j<HELDHOUR;j++)
					{
						if(j<8)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);
						else if(j<16)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);
						else if(j<24)
							printf("%d \t",Status::getinstance()->recordpositionheld[recordclass][i][j]);

						
					}
				printf("\n");

			}
			*/
		if(configchange)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_RecordConfig,0);
	}
 void CPortBase::movedetectconfig()
 	{
 		int configchange=0;
		if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->mtdcfg.movedetectalgenable)
			{
				Status::getinstance()->mtdcfg.movedetectalgenable=_globalDate->rcvBufQue.at(5);
				configchange=1;
			}
		if((_globalDate->rcvBufQue.at(6))!=Status::getinstance()->mtdcfg.sensitivity)
			{
				Status::getinstance()->mtdcfg.sensitivity=_globalDate->rcvBufQue.at(6);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(7)!=Status::getinstance()->mtdcfg.speedpriority)
			{
				Status::getinstance()->mtdcfg.speedpriority=_globalDate->rcvBufQue.at(7);
				configchange=1;
			}
		if((_globalDate->rcvBufQue.at(8)<<8|_globalDate->rcvBufQue.at(9))!=Status::getinstance()->mtdcfg.movmaxwidth)
			{
				Status::getinstance()->mtdcfg.movmaxwidth=_globalDate->rcvBufQue.at(8)<<8|_globalDate->rcvBufQue.at(9);
				configchange=1;
			}
		if((_globalDate->rcvBufQue.at(10)<<8|_globalDate->rcvBufQue.at(11))!=Status::getinstance()->mtdcfg.movmaxheight)
			{
				Status::getinstance()->mtdcfg.movmaxheight=_globalDate->rcvBufQue.at(10)<<8|_globalDate->rcvBufQue.at(11);
				configchange=1;
			}

		if((_globalDate->rcvBufQue.at(12)<<8|_globalDate->rcvBufQue.at(13))!=Status::getinstance()->mtdcfg.movminwidth)
			{
				Status::getinstance()->mtdcfg.movminwidth=_globalDate->rcvBufQue.at(12)<<8|_globalDate->rcvBufQue.at(13);
				configchange=1;
			}
		if((_globalDate->rcvBufQue.at(14)<<8|_globalDate->rcvBufQue.at(15))!=Status::getinstance()->mtdcfg.movminheight)
			{
				Status::getinstance()->mtdcfg.movminheight=_globalDate->rcvBufQue.at(14)<<8|_globalDate->rcvBufQue.at(15);
				configchange=1;
			}

		

		if((_globalDate->rcvBufQue.at(16)<<8|_globalDate->rcvBufQue.at(17))!=Status::getinstance()->mtdcfg.moverecordtime)
			{
				Status::getinstance()->mtdcfg.moverecordtime=_globalDate->rcvBufQue.at(16)<<8|_globalDate->rcvBufQue.at(17);
				configchange=1;
			}

		

		OSA_printf("movedetectalgenable=%d  sensitivity=%d  speedpriority=%d movminwidth=%d movminheight=%d moverecordtime=%d\n",Status::getinstance()->mtdcfg.movedetectalgenable,Status::getinstance()->mtdcfg.sensitivity,Status::getinstance()->mtdcfg.speedpriority,\
			Status::getinstance()->mtdcfg.movminwidth,Status::getinstance()->mtdcfg.movminheight,Status::getinstance()->mtdcfg.moverecordtime);

		if(configchange)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_MoveDetectConfig,0);
		


 	};
 void CPortBase::movedetectareaconfig()
 	{
		int configchange=1;
 		if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->detectareanum+1)
			{
				Status::getinstance()->detectareanum=_globalDate->rcvBufQue.at(5)-1;
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(6)!=Status::getinstance()->detectareaenable)
			{
				Status::getinstance()->detectareaenable=_globalDate->rcvBufQue.at(6);
				configchange=1;
			}
		if(configchange)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_MoveDetectAreaConfig,0);

 	};
 
 void CPortBase::displayconfig()
 	{
 		int configchange=0;
 		if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->displayresolution)
			{
				Status::getinstance()->displayresolution=_globalDate->rcvBufQue.at(5);
				configchange=1;
			}
		if(configchange)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_DisplayConfig,0);

 	};
 void CPortBase::correcttimeconfig()
 	{
 		int configchange=0;
		if((_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6))!=Status::getinstance()->correctyear)
			{
				Status::getinstance()->correctyear=_globalDate->rcvBufQue.at(5)<<8|_globalDate->rcvBufQue.at(6);
				configchange=1;
			}
		if((_globalDate->rcvBufQue.at(7))!=Status::getinstance()->correctmonth)
			{
				Status::getinstance()->correctmonth=_globalDate->rcvBufQue.at(7);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->correctday)
			{
				Status::getinstance()->correctday=_globalDate->rcvBufQue.at(8);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(9)!=Status::getinstance()->correcthour)
			{
				Status::getinstance()->correcthour=_globalDate->rcvBufQue.at(9);
				configchange=1;
			}

		
		if(_globalDate->rcvBufQue.at(10)!=Status::getinstance()->correctmin)
			{
				Status::getinstance()->correctmin=_globalDate->rcvBufQue.at(10);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(11)!=Status::getinstance()->correctsec)
			{
				Status::getinstance()->correctsec=_globalDate->rcvBufQue.at(11);
				configchange=1;
			}
		

		if(configchange)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_CorrectTimeConfig,0);
 		

 	};
 void CPortBase::panoconfig()
 	{
 			int configchange=0;
		if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->panoptzspeed)
			{
				Status::getinstance()->panoptzspeed=_globalDate->rcvBufQue.at(5);
				configchange=1;
			}
		if((_globalDate->rcvBufQue.at(6)<<8|_globalDate->rcvBufQue.at(7))!=Status::getinstance()->panopiexfocus)
			{
				Status::getinstance()->panopiexfocus=_globalDate->rcvBufQue.at(6)<<8|_globalDate->rcvBufQue.at(7);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(8)!=Status::getinstance()->panopicturerate)
			{
				Status::getinstance()->panopicturerate=_globalDate->rcvBufQue.at(8);
				configchange=1;
			}
		if(_globalDate->rcvBufQue.at(9)!=Status::getinstance()->panoresolution)
			{
				Status::getinstance()->panoresolution=_globalDate->rcvBufQue.at(9);
				configchange=1;
			}
		OSA_printf("%s:%d panoptzspeed=%d panopiexfocus=%d panopicturerate=%d\n",__func__,__LINE__,Status::getinstance()->panoptzspeed,Status::getinstance()->panopiexfocus,Status::getinstance()->panopicturerate);
		if(configchange)
			pM->MSGDRIV_send(MSGID_EXT_INPUT_PanoConfig,0);

 	};

 void  CPortBase::rebootconfig()
 	{
 		printf("%s\n",__func__);
 		char cmdBuf[128];
		sprintf(cmdBuf, "reboot");
		system(cmdBuf);

 	}
  void CPortBase::querryconfig()
 	{
 			int configchange=0;
		if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->querryconfig)
			{
				Status::getinstance()->querryconfig=_globalDate->rcvBufQue.at(5);
				configchange=1;
			}
		
		switch(Status::getinstance()->querryconfig)
			{
				case Status::CFG_TRKPLARFORM:
					CGlobalDate::Instance()->feedback=ACK_plantformconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);	
					break;

				case Status::CFG_SENSORFR:
					CGlobalDate::Instance()->feedback=ACK_sensorconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);	
					break;
				case Status::CFG_PLAYBACK:
					CGlobalDate::Instance()->feedback=ACK_recordconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);	
					break;
				case Status::CFG_MTD:
					CGlobalDate::Instance()->feedback=ACK_mvconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);	
					break;
				case Status::CFG_OUTPUTRESOL:
					CGlobalDate::Instance()->feedback=ACK_displayconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);
					break;
				case Status::CFG_VIDEO:
					CGlobalDate::Instance()->feedback=ACK_recordconfigmv;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);	
					break;
				case Status::CFG_MONTAGE:
					CGlobalDate::Instance()->feedback=ACK_panoconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);	
					break;
				case Status::CFG_SCANPLATFORM:
					CGlobalDate::Instance()->feedback=ACK_scanplantformconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);	
					break;
				case Status::CFG_SENSORTV:
					CGlobalDate::Instance()->feedback=ACK_sensortvconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);	
					break;
				case Status::CFG_SENSORTRK:
					CGlobalDate::Instance()->feedback=ACK_sensortrkconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);
					break;
				case Status::CFG_SENSORRADAR:
					CGlobalDate::Instance()->feedback=ACK_radarconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);
					break;
				case Status::CFG_TRK:
					CGlobalDate::Instance()->feedback=ACK_trackconfig;
					OSA_semSignal(&CGlobalDate::Instance()->m_semHndl_socket);
					break;
				default:
					break;



			}
		
	

 	};
    
void CPortBase::SetResolution()
{

}
void CPortBase::AIMPOS_X()
{

}

void CPortBase::AIMPOS_Y()
{

}

void CPortBase::EnableParamBackToDefault()
{
}
void CPortBase::updatepano()
{
	pM->MSGDRIV_send(MSGID_EXT_INPUT_Updatapano, 0);

}
void CPortBase::AxisMove()
{

}

void CPortBase::EnableTrkSearch()
{

}

void CPortBase::Enablealgosdrect()
{

}

void CPortBase::ZoomSpeedCtrl()
{

}

int CPortBase::ZoomCtrl()
{
	int focallength = _globalDate->rcvBufQue.at(5);

	if(0 == focallength)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCALLENGTHCTRL, (void *)(Status::PTZFOCUSLENGTHSTOP));
	else if(1 == focallength)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCALLENGTHCTRL, (void *)(Status::PTZFOCUSLENGTHOWN));
	else if(2 == focallength)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCALLENGTHCTRL, (void *)(Status::PTZFOCUSLENGTHUP));
	
    return 0;
}

void CPortBase::IrisCtrl()
{
	int iris = _globalDate->rcvBufQue.at(5);

	if(0 == iris)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_IRISCTRL, (void *)(Status::PTZIRISSTOP));
	else if(1 == iris)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_IRISCTRL, (void *)(Status::PTZIRISDOWN));
	else if(2 == iris)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_IRISCTRL, (void *)(Status::PTZIRISUP));
	
}

void CPortBase::FocusCtrl()
{
	int focus = _globalDate->rcvBufQue.at(5);

	if(0 == focus)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCUSCTRL, (void *)(Status::PTZFOCUSSTOP));
	else if(1 == focus)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCUSCTRL, (void *)(Status::PTZFOCUSSNEAR));
	else if(2 == focus)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_FOCUSCTRL, (void *)(Status::PTZFOCUSSFAR));

}

void  CPortBase::AvtAxisCtrl()
{
	
}

void CPortBase::EnableOsdbuffer()
{

}

void CPortBase::EnablewordType()
{

}

void CPortBase::EnablewordSize()
{
 
}

void CPortBase::Config_Write_Save()
{
	
}

void CPortBase::Config_Read()
{

}
void CPortBase::GetsoftVersion()
{
	GetSoftWareBuildTargetTime();
	pM->MSGDRIV_send(MSGID_EXT_INPUT_Getversion, 0);
	_globalDate->feedback=ACK_softVersion;
	OSA_semSignal(&_globalDate->m_semHndl_socket);

}

void CPortBase::EnableSavePro()
{
  
}

void CPortBase::plantctl()
{
	int mod=0;
	int mod1=0;
	int change=0;

	Status::getinstance()->ptzpanodirection=_globalDate->rcvBufQue.at(5);
	Status::getinstance()->ptztitledirection=_globalDate->rcvBufQue.at(7);
	Status::getinstance()->ptzpanspeed=_globalDate->rcvBufQue.at(6);
	Status::getinstance()->ptztitlespeed=_globalDate->rcvBufQue.at(8);
	
	pM->MSGDRIV_send(MSGID_EXT_INPUT_PLATCTRL, 0);
	
}

void CPortBase::AXIS_Y()
{
	
	
}

void CPortBase::Preset_Mtd()
{

}

void CPortBase::StoreMode(int mod)
	{
		if(mod==Status::STOREGO)
			{
				if(Status::getinstance()->getstoremod()!=mod)
					Status::getinstance()->setstoremod(mod);

				if(Status::getinstance()->storegonum!=_globalDate->rcvBufQue.at(5))
					{
						
						Status::getinstance()->storegonum=_globalDate->rcvBufQue.at(5);
					}
				
				OSA_printf("the mod=%d storegonum=%d \n ",Status::getinstance()->getstoremod(),Status::getinstance()->storegonum);
				
				pM->MSGDRIV_send(MSGID_EXT_INPUT_StoreMod, (void *)(Status::STOREGO));
			}
		else if(mod==Status::STORESAVE)
			{
				
				if(Status::getinstance()->getstoremod()!=mod)
					Status::getinstance()->setstoremod(mod);
				
				if(Status::getinstance()->storesavenum!=_globalDate->rcvBufQue.at(5))
					{
						Status::getinstance()->storesavenum=_globalDate->rcvBufQue.at(5);
					}
				
				if(Status::getinstance()->storesavemod!=_globalDate->rcvBufQue.at(6))
					{
						Status::getinstance()->storesavemod=_globalDate->rcvBufQue.at(6);
					}
				
				OSA_printf("the mod=%d savenum=%d mod=%d\n ",Status::getinstance()->getstoremod(),Status::getinstance()->storesavenum,Status::getinstance()->storesavemod);

				pM->MSGDRIV_send(MSGID_EXT_INPUT_StoreMod, (void *)(Status::STORESAVE));
			}
		
		

	};

void CPortBase::workMode()
{

	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->getworkmod())
		Status::getinstance()->setworkmod(_globalDate->rcvBufQue.at(5));
	else
		return;
	if(Status::getinstance()->getworkmod()==0)
	{
		pM->MSGDRIV_send(MSGID_EXT_INPUT_WorkModeCTRL, (void *)(Status::PANOAUTO));
	}
	else if(Status::getinstance()->getworkmod()==1)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_WorkModeCTRL, (void *)(Status::PANOPTZ));
	else if(Status::getinstance()->getworkmod()==2)
		pM->MSGDRIV_send(MSGID_EXT_INPUT_WorkModeCTRL, (void *)(Status::PANOSELECT));
	
	//PANOAUTO

}
void CPortBase::targetCaptureMode()
{
	
}

void CPortBase::choosedev()
{
	if(_globalDate->rcvBufQue.at(5)!=Status::getinstance()->getdevid())
		Status::getinstance()->setdevid(_globalDate->rcvBufQue.at(5));
	else
		return;

	if(1==Status::getinstance()->getdevid())
		pM->MSGDRIV_send(MSGID_EXT_CHOOSEDEV, (void *)(Status::DEV1));
	else if(2==Status::getinstance()->getdevid())
		pM->MSGDRIV_send(MSGID_EXT_CHOOSEDEV, (void *)(Status::DEV2));
	else if(3==Status::getinstance()->getdevid())
		pM->MSGDRIV_send(MSGID_EXT_CHOOSEDEV, (void *)(Status::DEV3));

}

void CPortBase::chooseptz()
{
	int ptzid = _globalDate->rcvBufQue.at(5);

	if(1 == ptzid)
		pM->MSGDRIV_send(MSGID_EXT_CHOOSEPTZ, (void *)(Status::PTZ_SCAN));
	else if(2 == ptzid)
		pM->MSGDRIV_send(MSGID_EXT_CHOOSEPTZ, (void *)(Status::PTZ_TRK));
}

int CPortBase::prcRcvFrameBufQue(int method)
{
    int ret =  -1;
    int cmdLength= (_globalDate->rcvBufQue.at(2)|_globalDate->rcvBufQue.at(3)<<8)+5;

    if(cmdLength<6){
        printf("Warning::Invaild frame\r\n");
        _globalDate->rcvBufQue.erase(_globalDate->rcvBufQue.begin(),_globalDate->rcvBufQue.begin()+cmdLength);
        return ret;
    }
    unsigned char checkSum = check_sum(cmdLength);

    if(checkSum== _globalDate->rcvBufQue.at(cmdLength-1))
    {	
    	_globalDate->commode = method;
	printf("the id=%d \n",_globalDate->rcvBufQue.at(4));
	int cmdid=_globalDate->rcvBufQue.at(4);
	if(Status::getinstance()==0)
		cmdid=0;
		
        switch(cmdid)
        {
            case 0x01:
                //startSelfCheak();
                break;
            case 0x02:
                EnableswitchVideoChannel();
                break;
            case 0x03:
                mouseevent(Status::MOUSEBUTTON);
                break;
            case 0x04:
                mouseevent(Status::MOUSEROLLER);
                break;
            case 0x05:
                displaymod();
                break;
            case 0x06:
                workMode();
                break;
		case 0x07:
		   StoreMode(Status::STOREGO);
		   break;
            case 0x08:
		//_globalDate->feedback=ACK_mainVideoStatus;
		//   OSA_semSignal(&_globalDate->m_semHndl_socket);
		//   printf("the_globalDate->feedback =%d\n",_globalDate->feedback);
                StoreMode(Status::STORESAVE);
                break;
            case 0x09:
                updatepano();
                break;
            case 0x0a:
                AxisMove();
                break;
            case 0x0b:
                EnableTrkSearch();
                break;
            case 0x0d:
                Enablealgosdrect();
                break;
            case 0x11:                             
                ZoomSpeedCtrl();
                break;
            case 0x12:
                ZoomCtrl();
                break;
            case 0x13:
                IrisCtrl();
                break;
            case 0x14:
                FocusCtrl();
                break;
            case 0x15:
                plantctl();
                //AXIS_Y();
                break;
            case 0x16:
                //wait to finish
                break;
            case 0x17:
                //wait to finish
                break;
            case 0x18:
                AvtAxisCtrl();
                break;
            case 0x20:
                EnableOsdbuffer();
                break;
            case 0x21:
                EnablewordType();
                EnablewordSize();
                break;
            case 0x30:
                GetsoftVersion();
                break;
            case 0x31:
                //Config_Read();
                break;
            case 0x32:
                //provided by a single server
                break;
            case 0x33:
                //provided by a single server
                break;
            case 0x34:
                EnableSavePro();
                break;
            case 0x35:
                //provided by a single server
                break;
            case 0x36:
                plantctl();
               // AXIS_Y();
                break;
            case 0x41:
            		Preset_Mtd();
            		break;
            case 0x42:
               	 workMode();
            		break;
            case 0x43:
            		targetCaptureMode();
            		break;
		case 0x45:
			choosedev();
			break;
		case 0x46:
			chooseptz();
			break;
		case 0x60:
			playercontrl();
			break;
		case 0x61:
			playerselect();
			break;
		case 0x62:
			playerquery();
			break;
		case 0x63:
			livevideo();
			break;
		case 0x65:
			panoenable();
			break;
		case 0x77:
			sensortvconfig();
			break;
		case 0x78:
			sensortrkconfig();
			break;
		case 0x79:
			scan_plantformconfig();
			break;
		case 0x7a:
			radarconfig();
			break;
		case 0x7b:
			trackconfig();
			break;
		case 0x7c:
			adddevconfig();
			break;
		case 0x7d:
			deletedevconfig();
			break;
		case 0x80:
			plantformconfig();
			break;
		case 0x81:
			sensorconfig();
			break;
		case 0x82:
			zeroconfig();
			break;
		case 0x83:
			recordconfig();
			break;
		case 0x84:
			movedetectconfig();
			break;
		case 0x85:
			movedetectareaconfig();
			break;
		case 0x86:
			displayconfig();
			break;
		case 0x87:
			correcttimeconfig();
			break;
		case 0x88:
			panoconfig();
			break;
		case 0x8a:
			rebootconfig();
			break;
		case 0x90:
			querryconfig();
			break;
            default:
                printf("INFO: Unknow  Control Command, please check!!!\r\n ");
                ret =0;
                break;
        }
    }
    else
        printf("%s,%d, checksum error:,cal is %02x,recv is %02x\n",__FILE__,__LINE__,checkSum,_globalDate->rcvBufQue.at(cmdLength-1));
    _globalDate->rcvBufQue.erase(_globalDate->rcvBufQue.begin(),_globalDate->rcvBufQue.begin()+cmdLength);
    return 1;
}

int  CPortBase::getSendInfo(int  respondId, sendInfo * psendBuf)
{
	//printf("respondId = %d\n", respondId);
	switch(respondId){
		case ACK_selfTest:
			//startCheckAnswer(psendBuf);
			break;
		case ACK_wordColor:
			break;
		case ACK_wordType:
			break;
		case ACK_wordSize:
			break;
		case NAK_wordColor:
			break;
		case NAK_wordType:
			break;
		case NAK_wordSize:
			break;
		case ACK_softVersion:
			softVersion(psendBuf);
			break;
		case ACK_mainVideoStatus:
			mainVedioChannel(psendBuf);
			break;
		case ACK_updatepano:
			ack_updatepano(psendBuf);
			break;
		case ACK_fullscreenmode:
			ack_fullscreenmode(psendBuf);
			break;
		case ACK_avtTrkType:
			trackTypes(psendBuf);
			break;
		case ACK_avtErrorOutput:
			trackErrOutput(psendBuf);
			break;
		case ACK_mmtStatus:
			mutilTargetNoticeStatus(psendBuf);
			break;
		case ACK_mmtSelectStatus:
			multilTargetNumSelectStatus(psendBuf);
			break;
		case ACK_EnhStatus:
			imageEnhanceStatus(psendBuf);
			break;
		case ACK_MtdStatus:
			moveTargetDetectedStat(psendBuf);
			break;
		case ACK_TrkSearchStatus:
			trackSearchStat(psendBuf);
			break;
		case ACK_posMoveStatus:
			trackFinetuningStat(psendBuf);
			break;
		case ACK_moveAxisStatus:
			confirmAxisStat(psendBuf);
			break;
		case ACK_ElectronicZoomStatus:
			ElectronicZoomStat(psendBuf);
			break;
		case ACK_picpStatus:
			pictureInPictureStat(psendBuf);
			break;
		case ACK_VideoChannelStatus:
			vedioTransChannelStat(psendBuf);
			break;
		case ACK_frameCtrlStatus:
			break;
		case ACK_compression_quality:
			break;
		case ACK_config_Write:
			settingCmdRespon(psendBuf);
			break;
		case ACK_config_Read:
			readConfigSetting(psendBuf);
			break;
		case ACK_jos_Kboard:
			extExtraInputResponse(psendBuf);
			break;
		case ACK_upgradefw:
			//printf("%s,%d, upgradefw response!!!\n",__FILE__,__LINE__);
			upgradefwStat(psendBuf);
			break;
		case ACK_param_todef:
			paramtodef(psendBuf);
			break;
		case ACK_playerquerry:
			recordquerry(psendBuf);
			break;
		case ACK_plantformconfig:
			ackplantformconfig(psendBuf);
			break;
		case ACK_sensortvconfig:
			acksensortvconfig(psendBuf);
			break;
		case ACK_sensortrkconfig:
			acksensortrkconfig(psendBuf);
			break;
		case ACK_sensorconfig:
			acksensorconfig(psendBuf);
			break;
		case ACK_zeroconfig:
			//recordquerry(psendBuf);
			break;
		case ACK_recordconfig:
			ackrecordconfig(psendBuf,0);
			break;
		case ACK_mvconfig:
			ackmvconfig(psendBuf);
			break;
		case ACK_mvareaconfig:
			//recordquerry(psendBuf);
			break;
		case ACK_displayconfig:
			ackdisplayconfig(psendBuf);
			break;
		case ACK_correcttimeconfig:
			//recordquerry(psendBuf);
			break;
		case ACK_recordconfigmv:
			ackrecordconfig(psendBuf,1);
			break;
		case ACK_panoconfig:
			ackpanoconfig(psendBuf);
			//recordquerry(psendBuf);
			break;
		case ACK_scanplantformconfig:
			ackscanplantformconfig(psendBuf);
			break;
		case ACK_radarconfig:
			ackradarconfig(psendBuf);
			break;
		case ACK_trackconfig:
			acktrackconfig(psendBuf);
			break;
		default:
			break;
	}
	return 0;
}

void  CPortBase:: mainVedioChannel(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  mainVedioChannel[msg_length+5];
	mainVedioChannel[4] = 0x04;
	//mainVedioChannel[5]=_globalDate->avt_status.SensorStat;
	package_frame(msg_length, mainVedioChannel);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,mainVedioChannel,msg_length+5);
}


char *CPortBase::myStrncpy(char *dest, const char *src, int n) 
{
    int size = sizeof(char)*(n + 1);
    char *tmp = (char*)malloc(size);  
    if (tmp) 
    {
        memset(tmp, '\0', size); 
        memcpy(tmp, src, size - 1);
        memcpy(dest, tmp, size);
        free(tmp);
        return dest;
    }
    else 
    {
        return NULL;
    }
}


void CPortBase::GetSoftWareBuildTargetTime(void)
{
    char arrDate[20]; //Jul 03 2018
    char arrTime[20]; //06:17:05
    char pDest[20];

    _globalDate->softversion;
    
   // RTC_TIME_DEF stTime;

    sprintf(arrDate,"%s",__DATE__);//Jul 03 2018
    sprintf(arrTime,"%s",__TIME__);//06:17:05
    
    //char *strncpy(char *dest, const char *src, int n)
    //(char*)(&(pDest[0])) = myStrncpy(pDest, arrDate, 3);
    sprintf(pDest, "%s", myStrncpy(pDest, arrDate, 3));

    if (strcmp(pDest, "Jan") == 0) _globalDate->softversion.mon= 1;
    else if (strcmp(pDest, "Feb") == 0) _globalDate->softversion.mon= 2;
    else if (strcmp(pDest, "Mar") == 0) _globalDate->softversion.mon = 3;
    else if (strcmp(pDest, "Apr") == 0) _globalDate->softversion.mon = 4;
    else if (strcmp(pDest, "May") == 0) _globalDate->softversion.mon = 5;
    else if (strcmp(pDest, "Jun") == 0) _globalDate->softversion.mon = 6;
    else if (strcmp(pDest, "Jul") == 0) _globalDate->softversion.mon = 7;
    else if (strcmp(pDest, "Aug") == 0) _globalDate->softversion.mon = 8;
    else if (strcmp(pDest, "Sep") == 0) _globalDate->softversion.mon = 9;
    else if (strcmp(pDest, "Oct") == 0) _globalDate->softversion.mon = 10;
    else if (strcmp(pDest, "Nov") == 0) _globalDate->softversion.mon = 11;
    else if (strcmp(pDest, "Dec") == 0) _globalDate->softversion.mon = 12;
    else _globalDate->softversion.mon = 1;

	


     sprintf(pDest, "%s", myStrncpy(pDest, arrDate+4, 2));
    //int atoi(const char *nptr);
   _globalDate->softversion.day= atoi(pDest);
    sprintf(pDest, "%s", myStrncpy(pDest, arrDate + 4 + 3, 4));
    //int atoi(const char *nptr);
    _globalDate->softversion.year= atoi(pDest);

    _globalDate->softversion.year=_globalDate->softversion.year%100;
    //time
    
    sprintf(pDest, "%s", myStrncpy(pDest, arrTime, 2));
   _globalDate->softversion.hour= atoi(pDest);
    sprintf(pDest, "%s", myStrncpy(pDest, arrTime+3, 2));
    _globalDate->softversion.min= atoi(pDest);
    sprintf(pDest, "%s", myStrncpy(pDest, arrTime + 3 + 3, 2));
    _globalDate->softversion.sec= atoi(pDest);

	_globalDate->softversion.major=MAJORVERSION;
	_globalDate->softversion.secmajor=MAJORVERSION;
	_globalDate->softversion.testversion=MAJORVERSION;
	_globalDate->softversion.softstage=0;
    return ;
}



void  CPortBase:: softVersion(sendInfo * spBuf)
{
	
	
	int msg_length = 11;
	u_int8_t  mainVedioChannel[msg_length+5];
	mainVedioChannel[4] = 0x30;
	mainVedioChannel[5]=_globalDate->softversion.major;
	mainVedioChannel[6]=_globalDate->softversion.secmajor;
	mainVedioChannel[7]=_globalDate->softversion.testversion;
	mainVedioChannel[8]=_globalDate->softversion.year;
	mainVedioChannel[9]=_globalDate->softversion.mon;
	mainVedioChannel[10]=_globalDate->softversion.day;
	mainVedioChannel[11]=_globalDate->softversion.hour;
	mainVedioChannel[12]=_globalDate->softversion.min;
	mainVedioChannel[13]=_globalDate->softversion.sec;
	mainVedioChannel[14]=_globalDate->softversion.softstage;
	
	//mainVedioChannel[5]=_globalDate->avt_status.SensorStat;
	package_frame(msg_length, mainVedioChannel);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,mainVedioChannel,msg_length+5);

	
	
	
	
	
}

void  CPortBase:: ack_updatepano(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  restartCheckAnswer[msg_length+5];
	restartCheckAnswer[4] = 0x09;
	restartCheckAnswer[5]=Status::getinstance()->ack_updatepano;
	package_frame(msg_length, restartCheckAnswer);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,restartCheckAnswer,msg_length+5);
}

void  CPortBase:: ack_fullscreenmode(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  retrackStatus[msg_length+5];

	retrackStatus[4] = 0x0a;
	retrackStatus[5]=Status::getinstance()->ack_fullscreenmode;
	package_frame(msg_length, retrackStatus);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,retrackStatus,msg_length+5);
}

void  CPortBase:: trackTypes(sendInfo * spBuf)
{

}

void  CPortBase:: trackErrOutput(sendInfo * spBuf)
{
	int msg_length = 5;
	u_int8_t  retrackErrOutput[msg_length+5];
	retrackErrOutput[4] = 0x08;
	retrackErrOutput[5]= _globalDate->errorOutPut[0];
	retrackErrOutput[5] = retrackErrOutput[5]&0xff;

	retrackErrOutput[6]= _globalDate->errorOutPut[0];
	retrackErrOutput[6] = (retrackErrOutput[6]>>8)&0xff;

	retrackErrOutput[7]= _globalDate->errorOutPut[1];
	retrackErrOutput[7] = retrackErrOutput[7]&0xff;

	retrackErrOutput[8]= _globalDate->errorOutPut[1];
	retrackErrOutput[8] = (retrackErrOutput[8]>>8)&0xff;

	package_frame(msg_length, retrackErrOutput);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,retrackErrOutput,msg_length+5);
}

void  CPortBase:: mutilTargetNoticeStatus(sendInfo * spBuf)
{
	int msg_length = 2;
	channelTable    mutilTargetSta;
	u_int8_t  remutilTargetNoticeStatus[msg_length+5];
	/*
	mutilTargetSta.channel0=_globalDate->avt_status.MtdState[0];
	mutilTargetSta.channel1=_globalDate->avt_status.MtdState[1];
	mutilTargetSta.channel2=_globalDate->avt_status.MtdState[2];
	mutilTargetSta.channel3=_globalDate->avt_status.MtdState[3];
	mutilTargetSta.channel4=_globalDate->avt_status.MtdState[4];
	mutilTargetSta.channel5=_globalDate->avt_status.MtdState[5];
	*/
	remutilTargetNoticeStatus[4] = 0x09;
	remutilTargetNoticeStatus[5]=*(u_int8_t*)(&mutilTargetSta);
	package_frame(msg_length, remutilTargetNoticeStatus);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,remutilTargetNoticeStatus,msg_length+5);
}

void CPortBase::multilTargetNumSelectStatus(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  tmpbuf[msg_length+5];
	tmpbuf[4] = 0x0a;
	tmpbuf[5]=(u_int8_t) (_globalDate->mainProStat[ACK_mmtSelect_value]&0xff);
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void  CPortBase::imageEnhanceStatus(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  tmpbuf[msg_length+5];
	channelTable    enhanceChannelSta;
	/*
	enhanceChannelSta.channel0=_globalDate->avt_status.ImgEnhStat[0];
	enhanceChannelSta.channel1=_globalDate->avt_status.ImgEnhStat[1];
	enhanceChannelSta.channel2=_globalDate->avt_status.ImgEnhStat[2];
	enhanceChannelSta.channel3=_globalDate->avt_status.ImgEnhStat[3];
	enhanceChannelSta.channel4=_globalDate->avt_status.ImgEnhStat[4];
	enhanceChannelSta.channel5=_globalDate->avt_status.ImgEnhStat[5];
	*/
	tmpbuf[4] = 0x0b;
	tmpbuf[5]=*(u_int8_t*)(&enhanceChannelSta);;
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CPortBase::trackFinetuningStat(sendInfo * spBuf)
{
	int msg_length = 3;
	u_int8_t  retrackFinetuningStat[msg_length+5];
	retrackFinetuningStat[4] = 0x0c;
	retrackFinetuningStat[5] = (u_int8_t) (_globalDate->mainProStat[ACK_posMove_value]&0xff);
	retrackFinetuningStat[6] = (u_int8_t) (_globalDate->mainProStat[ACK_posMove_value+1]&0xff);
	package_frame(msg_length, retrackFinetuningStat);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,retrackFinetuningStat,msg_length+5);
}

void CPortBase::confirmAxisStat(sendInfo * spBuf)
{
	int msg_length = 3;
	u_int8_t  tmpbuf[msg_length+5];
	tmpbuf[4] = 0x0d;
	tmpbuf[5]=(u_int8_t) (_globalDate->mainProStat[ACK_moveAxis_value]&0xff);
	tmpbuf[6]=(u_int8_t) (_globalDate->mainProStat[ACK_moveAxis_value+1]&0xff);
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CPortBase::ElectronicZoomStat(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  tmpbuf[msg_length+5];
	tmpbuf[4] = 0x0e;
	tmpbuf[5]=(u_int8_t) (_globalDate->mainProStat[ACK_ElectronicZoom_value]&0xff);
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CPortBase::trackSearchStat(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  retrackSearchStat[msg_length+5];
	retrackSearchStat[4] = 0x0f;
	unsigned int sectrkstat = 0;//_globalDate->avt_status.AvtTrkStat;
	if(sectrkstat == 4)
		sectrkstat = 1;
	else
		sectrkstat = 2;
	retrackSearchStat[5] = sectrkstat;
	package_frame(msg_length, retrackSearchStat);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,retrackSearchStat,msg_length+5);
}

void CPortBase::moveTargetDetectedStat(sendInfo * spBuf)
{
	int msg_length = 2;
	u_int8_t  tmpbuf[msg_length+5];
	channelTable    currtmoveTargetDet;
	/*
	currtmoveTargetDet.channel0=_globalDate->avt_status.MtdState[0];
	currtmoveTargetDet.channel1=_globalDate->avt_status.MtdState[1];
	currtmoveTargetDet.channel2=_globalDate->avt_status.MtdState[2];
	currtmoveTargetDet.channel3=_globalDate->avt_status.MtdState[3];
	currtmoveTargetDet.channel4=_globalDate->avt_status.MtdState[4];
	currtmoveTargetDet.channel5=_globalDate->avt_status.MtdState[5];
	*/
	tmpbuf[4] = 0x10;
	tmpbuf[5]=*(u_int8_t*)(&currtmoveTargetDet);
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CPortBase::pictureInPictureStat(sendInfo * spBuf)
{


}

void  CPortBase::vedioTransChannelStat(sendInfo * spBuf)
{

}

void CPortBase::frameFrequenceStat(sendInfo * spBuf)
{
	int msg_length = 3;
	u_int8_t  tmpbuf[msg_length+5];
	int  frameFrequenceStat[2]={0};
	tmpbuf[4] = 0x21;
	tmpbuf[5] = frameFrequenceStat[0];
	tmpbuf[6] = frameFrequenceStat[1];
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CPortBase::vedioCompressStat(sendInfo * spBuf)
{
	int msg_length = 3;
	u_int8_t  tmpbuf[msg_length+5];
	int  vedioCompressStat[2]={0};
	tmpbuf[4] = 0x22;
	tmpbuf[5] = vedioCompressStat[0];
	tmpbuf[6] = vedioCompressStat[1];
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CPortBase::settingCmdRespon(sendInfo * spBuf)
{
	int msg_length = 7;
	u_int8_t  tmpbuf[msg_length+5];
	tmpbuf[4] = 0x30;
	tmpbuf[5] = (u_int8_t) (_globalDate->mainProStat[ACK_config_Wblock]&0xff);
	tmpbuf[6] = (u_int8_t) (_globalDate->mainProStat[ACK_config_Wfield]&0xff);
	memcpy(tmpbuf+7,&(_globalDate->Host_Ctrl[config_Wvalue]),4);
	package_frame(msg_length, tmpbuf);
	spBuf->byteSizeSend=msg_length+5;
	memcpy(spBuf->sendBuff,tmpbuf,msg_length+5);
}

void CPortBase::readConfigSetting(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t  readCfgSetting[8];
	float tmp ;
	int msglen = 0x07;
	spBuf->byteSizeSend = 0x0c;

	int len ;
	if(_globalDate->choose){
		len = strlen(_globalDate->ACK_read_content);
		msglen = msglen -4 + len;
		spBuf->byteSizeSend = msglen + 5;
	}

	spBuf->sendBuff[0] = 0xEB;
	spBuf->sendBuff[1] = 0x53;
	spBuf->sendBuff[2]= msglen&0xff;
	spBuf->sendBuff[3]= (msglen>>8)&0xff;
	spBuf->sendBuff[4]= 0x31;
	spBuf->sendBuff[5]= (u_int8_t) (_globalDate->mainProStat[ACK_config_Rblock]&0xff);
	spBuf->sendBuff[6]= (u_int8_t) (_globalDate->mainProStat[ACK_config_Rfield]&0xff);

	if(_globalDate->choose){
		memcpy((void*)&spBuf->sendBuff[7],(void*)_globalDate->ACK_read_content,len);
		sumCheck=sendCheck_sum(msglen+3, spBuf->sendBuff+1);
		spBuf->sendBuff[msglen+4]=(sumCheck&0xff);
		_globalDate->choose = 0;
	}else{
		tmp = _globalDate->ACK_read[0];
		printf("%s: %d      block = %d , field = %d ,value = %f \n",__func__,__LINE__,spBuf->sendBuff[5],spBuf->sendBuff[6],tmp);
		memcpy(spBuf->sendBuff+7,&tmp,4);
		sumCheck=sendCheck_sum(msglen+3, spBuf->sendBuff+1);
		spBuf->sendBuff[msglen+4]=(sumCheck&0xff);
		_globalDate->ACK_read.erase(_globalDate->ACK_read.begin());
	}

	//printf("read <=====> is  back\n");
}

void CPortBase::extExtraInputResponse(sendInfo * spBuf)
{

}


void  CPortBase:: upgradefwStat(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	u_int8_t trackStatus[3];
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=0x03;
	spBuf->sendBuff[3]=0x00;
	spBuf->sendBuff[4]=0x35;
	spBuf->sendBuff[5]=_globalDate->respupgradefw_stat;
	spBuf->sendBuff[6]=_globalDate->respupgradefw_perc;
	sumCheck=sendCheck_sum(6,spBuf->sendBuff+1);
	spBuf->sendBuff[7]=(sumCheck&0xff);
	spBuf->byteSizeSend=0x08;
}
void  CPortBase:: paramtodef(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x53;
	spBuf->sendBuff[2]=0x02;
	spBuf->sendBuff[3]=0x00;
	spBuf->sendBuff[4]=0x0d;
	spBuf->sendBuff[5]= (u_int8_t) (_globalDate->mainProStat[ACK_config_Rblock]&0xff);
	sumCheck=sendCheck_sum(5,spBuf->sendBuff+1);
	spBuf->sendBuff[6]=(sumCheck&0xff);
	spBuf->byteSizeSend=0x07;
}

void  CPortBase:: recordquerry(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=14*_globalDate->querrytime.size()+1;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_playerquerry;
	for(int i=0;i<_globalDate->querrytime.size();i++)
		{
			spBuf->sendBuff[5+i*14]=(_globalDate->querrytime[i].startyear>>8)&0xff;
			spBuf->sendBuff[6+i*14]=_globalDate->querrytime[i].startyear&0xff;
			spBuf->sendBuff[7+i*14]=_globalDate->querrytime[i].startmon;
			spBuf->sendBuff[8+i*14]=_globalDate->querrytime[i].startday;
			spBuf->sendBuff[9+i*14]=_globalDate->querrytime[i].starthour;
			spBuf->sendBuff[10+i*14]=_globalDate->querrytime[i].startmin;
			spBuf->sendBuff[11+i*14]=_globalDate->querrytime[i].startsec;
			spBuf->sendBuff[12+i*14]=(_globalDate->querrytime[i].endyear>>8)&0xff;
			spBuf->sendBuff[13+i*14]=_globalDate->querrytime[i].endyear&0xff;
			spBuf->sendBuff[14+i*14]=_globalDate->querrytime[i].endmon;
			spBuf->sendBuff[15+i*14]=_globalDate->querrytime[i].endday;
			spBuf->sendBuff[16+i*14]=_globalDate->querrytime[i].endhour;
			spBuf->sendBuff[17+i*14]=_globalDate->querrytime[i].endtmin;
			spBuf->sendBuff[18+i*14]=_globalDate->querrytime[i].endsec;

		}
	//spBuf->sendBuff[5]= (u_int8_t) (_globalDate->mainProStat[ACK_config_Rblock]&0xff);
	sumCheck=sendCheck_sum(4+14*_globalDate->querrytime.size(),spBuf->sendBuff+1);
	spBuf->sendBuff[4+14*_globalDate->querrytime.size()+1]=(sumCheck&0xff);
	spBuf->byteSizeSend=4+14*_globalDate->querrytime.size()+2;
	printf("%s\n",__func__);
}


void  CPortBase::ackscanplantformconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=6;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_scanplantformconfig;
	spBuf->sendBuff[5]=Status::getinstance()->scan_platformcfg.address&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->scan_platformcfg.protocol&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->scan_platformcfg.baudrate&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->scan_platformcfg.start_signal&0xff;
	spBuf->sendBuff[9]=Status::getinstance()->scan_platformcfg.pt_check&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void CPortBase::ackradarconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=9;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_radarconfig;
	spBuf->sendBuff[5]=Status::getinstance()->radarcfg.sensor&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->radarcfg.hideline&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->radarcfg.offset50m&0xff;
	spBuf->sendBuff[8]=(Status::getinstance()->radarcfg.offset50m>>8)&0xff;
	spBuf->sendBuff[9]=Status::getinstance()->radarcfg.offset100m&0xff;
	spBuf->sendBuff[10]=(Status::getinstance()->radarcfg.offset100m>>8)&0xff;
	spBuf->sendBuff[11]=Status::getinstance()->radarcfg.offset300m&0xff;
	spBuf->sendBuff[12]=(Status::getinstance()->radarcfg.offset300m>>8)&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void CPortBase::acktrackconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=3;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_trackconfig;
	spBuf->sendBuff[5]=Status::getinstance()->trackcfg.trkprio&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->trackcfg.trktime&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void  CPortBase::ackplantformconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=5;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_plantformconfig;
	spBuf->sendBuff[5]=Status::getinstance()->ptzaddress&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->ptzprotocal&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->ptzbaudrate&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->ptzspeed&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
	printf("%s\n",__func__);
}

void  CPortBase::acksensortvconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=13;
	
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_sensortvconfig;

	spBuf->sendBuff[5]=Status::getinstance()->sensortvcfg.outputresol&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->sensortvcfg.brightness&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->sensortvcfg.contract&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->sensortvcfg.autobright&0xff;
	spBuf->sendBuff[9]=Status::getinstance()->sensortvcfg.backlight&0xff;
	
	spBuf->sendBuff[10]=Status::getinstance()->sensortvcfg.whitebalance&0xff;
	spBuf->sendBuff[11]=Status::getinstance()->sensortvcfg.gain&0xff;
	spBuf->sendBuff[12]=Status::getinstance()->sensortvcfg.daynight&0xff;
	spBuf->sendBuff[13]=Status::getinstance()->sensortvcfg.stronglightsup&0xff;
	spBuf->sendBuff[14]=Status::getinstance()->sensortvcfg.exposuremode&0xff;
	spBuf->sendBuff[15]=Status::getinstance()->sensortvcfg.elecshutter_time&0xff;
	spBuf->sendBuff[16]=(Status::getinstance()->sensortvcfg.elecshutter_time>>8)&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void  CPortBase::acksensortrkconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=13;
	
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_sensortrkconfig;

	spBuf->sendBuff[5]=Status::getinstance()->sensortrkcfg.outputresol&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->sensortrkcfg.brightness&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->sensortrkcfg.contract&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->sensortrkcfg.autobright&0xff;
	spBuf->sendBuff[9]=Status::getinstance()->sensortrkcfg.backlight&0xff;
	
	spBuf->sendBuff[10]=Status::getinstance()->sensortrkcfg.whitebalance&0xff;
	spBuf->sendBuff[11]=Status::getinstance()->sensortrkcfg.gain&0xff;
	spBuf->sendBuff[12]=Status::getinstance()->sensortrkcfg.daynight&0xff;
	spBuf->sendBuff[13]=Status::getinstance()->sensortrkcfg.stronglightsup&0xff;
	spBuf->sendBuff[14]=Status::getinstance()->sensortrkcfg.exposuremode&0xff;
	spBuf->sendBuff[15]=Status::getinstance()->sensortrkcfg.elecshutter_time&0xff;
	spBuf->sendBuff[16]=(Status::getinstance()->sensortrkcfg.elecshutter_time>>8)&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void  CPortBase:: acksensorconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=10;
	
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_sensorconfig;
	
	spBuf->sendBuff[5]=Status::getinstance()->brightness&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->contract&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->autobright&0xff;
	spBuf->sendBuff[8]=Status::getinstance()->backandwrite&0xff;
	
	spBuf->sendBuff[9]=Status::getinstance()->correct&0xff;
	spBuf->sendBuff[10]=Status::getinstance()->digitfilter&0xff;
	spBuf->sendBuff[11]=Status::getinstance()->digitenhance&0xff;
	spBuf->sendBuff[12]=Status::getinstance()->mirror&0xff;
	spBuf->sendBuff[13]=Status::getinstance()->outputresol&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
	printf("%s\n",__func__);
}

void  CPortBase:: ackrecordconfig(sendInfo * spBuf,int classid)
{
	u_int8_t sumCheck;
	int infosize=23;
	//int classid=0;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_recordconfig;
	spBuf->sendBuff[5]=classid&0xff;
	
	for(int k=0;k<7;k++)
		{
			spBuf->sendBuff[6+k*3+0]=0;
			spBuf->sendBuff[6+k*3+1]=0;
			spBuf->sendBuff[6+k*3+2]=0;
			for(int i=0;i<24;i++)
				{
					if(i<8)
						spBuf->sendBuff[6+k*3+0]|=Status::getinstance()->recordpositionheld[classid][k][i]<<(7-i);
					else if(i<16)
						spBuf->sendBuff[6+k*3+1]|=Status::getinstance()->recordpositionheld[classid][k][i]<<(15-i);
					else if(i<24)
						spBuf->sendBuff[6+k*3+2]|=Status::getinstance()->recordpositionheld[classid][k][i]<<(23-i);
				}

		}
	
	printf("the classid=%d\n",classid);
	for(int i=0;i<HELDWEEK;i++)
		{
			for(int j=0;j<HELDHOUR;j++)
				{
					if(j<8)
						printf("%d \t",Status::getinstance()->recordpositionheld[classid][i][j]);
					else if(j<16)
						printf("%d \t",Status::getinstance()->recordpositionheld[classid][i][j]);
					else if(j<24)
						printf("%d \t",Status::getinstance()->recordpositionheld[classid][i][j]);					
				}
			printf("\n");

		}
	for(int i=0;i<21;i++)
		{
			printf("%d \t",spBuf->sendBuff[6+i]);

		}
	printf("\n");
	
	//spBuf->sendBuff[5]= (u_int8_t) (_globalDate->mainProStat[ACK_config_Rblock]&0xff);
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
	printf("%s\n",__func__);
}

void  CPortBase:: ackmvconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=14;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_mvconfig;
	spBuf->sendBuff[5]=Status::getinstance()->mtdcfg.movedetectalgenable&0xff;
	spBuf->sendBuff[6]=Status::getinstance()->mtdcfg.sensitivity&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->mtdcfg.speedpriority&0xff;
	spBuf->sendBuff[8]=(Status::getinstance()->mtdcfg.movmaxwidth>>8)&0xff;
	spBuf->sendBuff[9]=Status::getinstance()->mtdcfg.movmaxwidth&0xff;
	spBuf->sendBuff[10]=(Status::getinstance()->mtdcfg.movmaxheight>>8)&0xff;
	spBuf->sendBuff[11]=Status::getinstance()->mtdcfg.movmaxheight&0xff;
	spBuf->sendBuff[12]=(Status::getinstance()->mtdcfg.movminwidth>>8)&0xff;
	spBuf->sendBuff[13]=Status::getinstance()->mtdcfg.movminwidth&0xff;
	spBuf->sendBuff[14]=(Status::getinstance()->mtdcfg.movminheight>>8)&0xff;
	spBuf->sendBuff[15]=Status::getinstance()->mtdcfg.movminheight&0xff;
	spBuf->sendBuff[16]=(Status::getinstance()->mtdcfg.moverecordtime>>8)&0xff;
	spBuf->sendBuff[17]=Status::getinstance()->mtdcfg.moverecordtime&0xff;
	
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
	printf("%s\n",__func__);
}

void CPortBase::ackdisplayconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=2;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_displayconfig;
	spBuf->sendBuff[5]=Status::getinstance()->displayresolution&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
}

void  CPortBase:: ackpanoconfig(sendInfo * spBuf)
{
	u_int8_t sumCheck;
	int infosize=8;
	spBuf->sendBuff[0]=0xEB;
	spBuf->sendBuff[1]=0x51;
	spBuf->sendBuff[2]=infosize&0xff;
	spBuf->sendBuff[3]=(infosize>>8)&0xff;
	spBuf->sendBuff[4]=ACK_panoconfig;
	spBuf->sendBuff[5]=Status::getinstance()->panoptzspeed&0xff;
	spBuf->sendBuff[6]=(Status::getinstance()->panopiexfocus>>8)&0xff;
	spBuf->sendBuff[7]=Status::getinstance()->panopiexfocus&0xff;
	spBuf->sendBuff[8]=(Status::getinstance()->panopicturerate)&0xff;
	spBuf->sendBuff[9]=(Status::getinstance()->panoresolution)&0xff;
	
	sumCheck=sendCheck_sum(infosize+3,spBuf->sendBuff+1);
	
	spBuf->sendBuff[infosize+4]=(sumCheck&0xff);
	spBuf->byteSizeSend=infosize+5;
	printf("%s\n",__func__);
}

u_int8_t CPortBase:: sendCheck_sum(uint len, u_int8_t *tmpbuf)
{
	u_int8_t  ckeSum=0;
	for(int n=0;n<len;n++)
		{
		ckeSum ^= tmpbuf[n];
		//printf("crcsum=%d\n",ckeSum);
		}
	return ckeSum;
}

u_int8_t CPortBase:: package_frame(uint len, u_int8_t *tmpbuf)
{
	tmpbuf[0] = 0xEB;
	tmpbuf[1] = 0x51;
	tmpbuf[2] = len&0xff;
	tmpbuf[3] = (len>>8)&0xff;
	tmpbuf[len+4]= sendCheck_sum(len+3,tmpbuf+1);
	//printf("%s check sum=%d\n",__func__,tmpbuf[len+4]);
	return 0;
}

unsigned char CPortBase::check_sum(int len_t)
{
    unsigned char cksum = 0;
    for(int n=1; n<len_t-1; n++)
    {
        cksum ^= _globalDate->rcvBufQue.at(n);
    }
    return  cksum;
}

FILE *fp = NULL;
FILE *fp2 = NULL;
unsigned int current_len = 0;
unsigned int current_len2 = 0;
int CPortBase::upgradefw(unsigned char *swap_data_buf, unsigned int swap_data_len)
{
	int status;
	int write_len;
	unsigned int file_len;

	int recv_len = swap_data_len-13;
	unsigned char buf[8] = {0xEB,0x53,0x03,0x00,0x35,0x00,0x00,0x00};

	unsigned char cksum = 0;
	for(int n=1;n<swap_data_len-1;n++)
	{
		cksum ^= swap_data_buf[n];
	}
	if(cksum !=swap_data_buf[swap_data_len-1])
	{
		printf("checksum error,cal is %02x, recv is %02x\n",cksum,swap_data_buf[swap_data_len-1]);
		return -1;
	}
	memcpy(&file_len,swap_data_buf+5,4);
	if(NULL==fp)
	{
		if(NULL ==(fp = fopen("dss_pkt.tar.gz","w")))
		{
			perror("fopen\r\n");
			return -1;
		}
		else
		{
			printf("openfile dss_pkt.tar.gz success\n");
		}
	}

	write_len = fwrite(swap_data_buf+12,1,recv_len,fp);
	fflush(fp);
	if(write_len < recv_len)
	{
		printf("Write upgrade tgz file error!\r\n");
		return -1;
	}
	current_len += recv_len;
	if(current_len == file_len)
	{
		current_len = 0;
		fclose(fp);
		fp = NULL;

		if(fw_update_runtar() == 0)
			_globalDate->respupgradefw_stat= 0x01;
		else
			_globalDate->respupgradefw_stat = 0x02;

		_globalDate->respupgradefw_perc = 100;

		status = update_startsh();
		printf("update start.sh return %d\n",status);

		status = update_fpga();
		printf("update load_fpga.ko return %d\n",status);

		system("sync");
		
		_globalDate->feedback=ACK_upgradefw;
		OSA_semSignal(&_globalDate->m_semHndl_socket);
	}
	else
	{
		_globalDate->respupgradefw_stat= 0x00;
		_globalDate->respupgradefw_perc = (current_len*1.0/file_len)*100;
		_globalDate->feedback=ACK_upgradefw;
		OSA_semSignal(&_globalDate->m_semHndl_socket);
	}

	//printf("_globalDate->respupgradefw_perc=%d__%d__ %d\n",_globalDate->respupgradefw_perc,current_len,file_len);

}
int CPortBase::fw_update_runtar(void)
{
	printf("tar zxvf dss_pkt.tar.gz...\r\n"); 
	int iRtn=-1;
	char cmdBuf[128];
	sprintf(cmdBuf, "tar -zxvf %s -C ~/", "dss_pkt.tar.gz");
	iRtn = system(cmdBuf);
	return iRtn;
}

int CPortBase::update_startsh(void)
{
	printf("update start.sh...\r\n"); 
	int iRtn=-1;
	char cmdBuf[128];	
	sprintf(cmdBuf, "mv ~/Release/start.sh ~/");
	iRtn = system(cmdBuf);
	return iRtn;
}

int CPortBase::update_fpga(void)
{
	printf("update load_fpga.ko...\r\n"); 
	int iRtn=-1;
	char cmdBuf[128];	
	sprintf(cmdBuf, "mv ~/Release/load_fpga.ko ~/dss_bin/");
	iRtn = system(cmdBuf);
	return iRtn;
}

void CPortBase::seconds_sleep(unsigned seconds)
{
    struct timeval tv;

    int err;
    do{
        tv.tv_sec = seconds;
        tv.tv_usec = 0;
        err = select(0, NULL, NULL, NULL, &tv);
    }while(err<0 && errno==EINTR);
}

