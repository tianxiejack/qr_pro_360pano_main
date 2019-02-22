#include "mvdectInterface.hpp"
#include "MovDetector.hpp"


CMvDectInterface *MvDetector_Create()
{
	CMvDectInterface	*pMvObj = NULL;
	CMoveDetector_mv *pTmpMV = NULL;

	pTmpMV= (CMoveDetector_mv*)new CMoveDetector_mv;
	pMvObj = (CMvDectInterface*)pTmpMV;
	CV_Assert(pMvObj != NULL);
	
	pTmpMV->creat();
	return (CMvDectInterface*)pMvObj;
}

void MvDetector_Destory(CMvDectInterface *obj)
{
	CMoveDetector_mv	*pMvObj = (CMoveDetector_mv*)obj;
	if(pMvObj != NULL){
		delete pMvObj;
		obj = NULL;
	}
}
