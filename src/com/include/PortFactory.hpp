#ifndef __CPORTFACTORY_H__
#define __CPORTFACTORY_H__

#include "CPortInterface.hpp"

class PortFactory{
public:
    PortFactory();
    ~PortFactory();
    static CPortInterface* createProduct(int type);
};

#endif