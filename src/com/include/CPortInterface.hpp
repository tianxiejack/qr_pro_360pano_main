#ifndef __CPORTINTERFACE_H__
#define __CPORTINTERFACE_H__

#include "CMessage.hpp"
#include "globalDate.h"

class CPortInterface
{
public:
    virtual ~CPortInterface(){};
    virtual int create(){return 0;};
    virtual int closePort(){return 0;};
    virtual void run()=0;
    virtual int recvData(){return 0;};
    virtual int sendData(){return 0;};

    virtual CMessage *getpM(){return 0;};
    virtual CGlobalDate* getDate()=0;
};

#endif
