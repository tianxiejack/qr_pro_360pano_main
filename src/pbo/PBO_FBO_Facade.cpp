#include "PBO_FBO_Facade.h"

 PBO_FBO_Facade:: PBO_FBO_Facade(FBOManager  &FBOMgr,PBOReceiver & PBORcr):
	m_FBOMgr(FBOMgr),
	m_PBORcr(PBORcr)
 {
}
void PBO_FBO_Facade::DrawAndGet(int idx)
{
	m_FBOMgr.PboDraw(m_PBORcr,idx);
}

void PBO_FBO_Facade::Render2Front(int w,int h)
{
	m_FBOMgr.DrawTex2Front(w,h);
}
