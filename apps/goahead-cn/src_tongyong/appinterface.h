#ifndef __APPINTERFACE_H__
#define __APPINTERFACE_H__


#define APP_DEBUG	1
#ifdef APP_DEBUG
#define APP_TRACE(fmt, args...)	\
{	\
	printf("func:[%s]-line:[%d]\t",__FUNCTION__, __LINE__);	\
	printf(fmt, ##args);	\
}
#else
#define APP_TRACE(fmt, args...)	/\
	/APP_TRACE(fmt, args...)	
#endif


void formDefineAppInterface(void);

#endif
