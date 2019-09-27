/**************************************************************
*    File Name		: debug.h
*    Create Date	: 2019/09/27
*    Version 		: 1.0
*    Author/Corporation	: zzq/Chamrun
*    Description 	:  Define macros to print debugging information at different levels
*    History 		: 
**************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <stdlib.h>

#define _DEBUG_		0
#define _WARNING_	1
#define _ERROR_		1

#if _DEBUG_
	#define CR_DEBUG_S(fmt, ...) \
		printf("[DEBUG]file:%s func:%s() line:%d : "fmt"", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
	#define CR_DEBUG(fmt, ...) \
		printf(fmt, ##__VA_ARGS__)
#else
	#define CR_DEBUG_S(fmt, ...)
	#define CR_DEBUG(fmt, ...)
#endif


#if _WARNING_
	#define CR_WARNING_S(fmt, ...) \
	    	printf("[WARNING]file:%s func:%s() line:%d : "fmt"", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
	#define CR_WARNING(fmt, ...) \
	    	printf(fmt, ##__VA_ARGS__)
#else
	#define CR_WARNING_S(fmt, ...)
	#define CR_WARNING(fmt, ...)
#endif


#if _ERROR_
	#define CR_ERROR(fmt, ...) perror(fmt)
#else
	#define CR_ERROR(fmt, ...)
#endif


#endif //_DEBUG_H_
