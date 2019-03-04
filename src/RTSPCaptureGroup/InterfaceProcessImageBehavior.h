#ifndef _INTERFACE_PROCESS_IMAGE_BEHAVIOR_
#define	_INTERFACE_PROCESS_IMAGE_BEHAVIOR_

class InterfaceProcessImageBehavior{
public:
	virtual void processImage_setInfo(int height, int width )=0;
	virtual void processImage(unsigned char * data, unsigned int length)=0;
	virtual ~InterfaceProcessImageBehavior(){ };
};

#endif
