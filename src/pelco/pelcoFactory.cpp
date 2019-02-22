#include <iostream>
#include "pelcoFactory.h"

using namespace std;

IPelcoFactory::IPelcoFactory()
{
	cout<<"IPelcoFactory ..."<<endl;
}

IPelcoFactory::~IPelcoFactory()
{
	
}

IPelcoBaseFormat* IPelcoFactory::createIpelco(char protocolType)
{
	switch(protocolType)
	{
		case pelco_D:
			cout<<"*********pelco_D create success"<<endl;
			return new IPelcoDFormat();

			break;
		case pelco_P:
			cout<<"*********pelco_P create success"<<endl;
			return new IPelcoPFormat();
			break;
		default:
			break;
	}
}

