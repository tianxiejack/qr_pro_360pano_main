/*=======================================================
* Copyright (C), 2011-2012, ChamRun Tech. Co., Ltd.
* FileName:			// filename
* Author:			//author
* Date:			// date
* Description:			// description
* Version:			// version
* Function List:		// funs and description
*     1. -------
* History:			// histroy modfiy record
*     <author>  <time>   <version >   <desc>
   =======================================================*/
#ifndef __DX_H_
#define __DX_H_

#include <osa.h>
#include <osa_thr.h>
#include <osa_tsk.h>
#include <osa_sem.h>
#include <osa_mutex.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

Int32 Dx_run( );
Int32 Dx_stop( );


#endif /* CR_DX_DRIVER_DXD_H */
/************************************** The End Of File **************************************/
