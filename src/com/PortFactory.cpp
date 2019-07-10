#include "PortFactory.hpp"
#include "CUartProc.hpp"
#include "CNetProc.hpp"
#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>

using namespace cv;

PortFactory::PortFactory()
{

}
  
PortFactory::~PortFactory()
{
 
}

CPortInterface* PortFactory::createProduct(int type)
{
	CPortInterface* temp = NULL;
	uartparams_t param = {9600, 0, 8, 'N', 1};
    	switch(type)
    	{
	        case 1:
			temp = new CUartProc("unknown", 115200, 0, 8, 'N', 1);
			break;
	        case 2:
			temp = new CNetProc(6666);
		   	break;
	        case 3:
			//ReadUartParams(param);
			//temp = new CUartProc("/dev/ttyTHS2", param.baud_rate, param.flow, param.data_bits, param.parity, param.stop_bits);
	            	break;
	        default:
	            	break;
	}
    	return temp;
}

