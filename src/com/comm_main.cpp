#include "CPortInterface.hpp"
#include "PortFactory.hpp"
#include "CPortBase.hpp"

void seconds_sleep(unsigned seconds)
{
    struct timeval tv;
    int err;
    do{
        tv.tv_sec = seconds;
        tv.tv_usec = 0;
        err = select(0, NULL, NULL, NULL, &tv);
    }while(err<0 && errno==EINTR);
}
#if 0
int main()
{	
    CPortInterface *p1 = PortFactory::createProduct(1);
    p1->getpM();
    p1->registfunc();
    p1->create();
    p1->run();

    CPortInterface *p2 = PortFactory::createProduct(2);
    p2->getpM();
    p2->create();
    p2->run();

    CPortInterface *p3 = PortFactory::createProduct(3);
    p3->getpM();
    p3->create();
    p3->run();

    while(1) seconds_sleep(10);
    p1->closePort();
    p2->closePort();
    p3->closePort();
}
#endif

