#ifndef POB_FBO_FACADE_
#define POB_FBO_FACADE_
#include "FBOManager.h"
#include "PBOManager.h"

using namespace std;
typedef  class PBO_FBO_Facade
{
public:
	PBO_FBO_Facade(FBOManager  &FBOMgr,PBOReceiver  &PBORcr);
	~PBO_FBO_Facade(){};
	void DrawAndGet(int idx);
	void Render2Front(int w,int h);
	GLuint GetFboTextureID(){return m_FBOMgr.GetFboTextureID();};
	bool IsFboUsed(){
		return m_FBOMgr.IsFboUsed();
	};
private:
	FBOManager  &m_FBOMgr;
	PBOReceiver  &m_PBORcr;
}* pPBO_FBO_Facade;
#endif

