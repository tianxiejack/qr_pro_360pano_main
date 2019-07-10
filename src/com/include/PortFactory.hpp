#ifndef __CPORTFACTORY_H__
#define __CPORTFACTORY_H__

#include "CPortInterface.hpp"

typedef struct{
	int baud_rate;
	int flow;
	int data_bits;
	char parity;
	int stop_bits;
}uartparams_t;

class PortFactory{
public:
    	PortFactory();
   	 ~PortFactory();
    	static CPortInterface* createProduct(int type);
};

#endif