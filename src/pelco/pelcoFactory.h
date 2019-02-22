#ifndef _PELCO_Factory_H
#define _PELCO_Factory_H

#include "pelcoDformat.h"
#include "pelcoPformat.h"

const char pelco_D = 1;
const char pelco_P = 2;

class IPelcoBaseFormat;

class IPelcoFactory
{
public:
	IPelcoFactory();
	~IPelcoFactory();
	static IPelcoBaseFormat* createIpelco(char protocolType);


};


#endif
