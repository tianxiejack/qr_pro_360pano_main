#include "PortFactory.hpp"
#include "CUartProcess.hpp"
#include "CNetWork.hpp"

PortFactory::PortFactory()
{

}
  
PortFactory::~PortFactory()
{
 
}

CPortInterface* PortFactory::createProduct(int type)
{
    CPortInterface* temp = NULL;
    switch(type)
    {
        case 1:
            temp = new CUartProcess();
            break;
        case 2:
            temp = new CNetWork();
            break;
        default:
            break;
    }
    return temp;
}
