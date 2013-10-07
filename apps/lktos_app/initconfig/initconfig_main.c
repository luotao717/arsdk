
/*
  main function
  lktOS init config moudle for all platform 
  
  v1.0 support mt7620 & ra5350  by luot  20130211
  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "lktInitConfig.h"

void usage(void)
{
	printf("\r\nlktos network config tool");
	printf("\r\nussgr:");
	printf("\r\nlktos_networkconfig <platform> <command>");
	printf("\r\ncurrrent support <paltform>:ra3050, mtk7620 ra5350<plat> <command>");
	printf("\r\ncurrrent support <command>:initlan,initwan  (to init the wifi)");
	printf("\r\nfor eg: lktos_initconfig mtk7620 all\r\n");
	printf("\r\nfor eg: lktos_initconfig ra5350 all\r\n");
}

int main(int argc, char* argv[])
{
	T_LKTOS_INITCONFIG_PLATFORM_TYPE_ plattype=UNDEFINED;
	char errBuf[64]={0};
	int result=0;
	if(argc!=3)
	{
		usage();
		return 0;
	}
	if(!strncmp("ra3050",argv[1],6))
	{
		plattype=RALINK3050_STD;
	}
	else if(!strncmp("mtk7620",argv[1],7))
	{
		plattype=MTK7620_STD;
	}
	else if(!strncmp("ra5350",argv[1],6))
	{
		plattype=RALINK5350_STD;
	}
	else if(!strncmp("ar9331hc",argv[1],8))
	{
		plattype=AR9331_HEXICOM;
	}
	else
	{
		usage();
		return 0;
	}
	if(!strncmp("all",argv[2],3))
	{
		result=lktos_initconfig_init_all(plattype,errBuf);
		if(!result)
		{
			printf("\r\n error msg=%s",errBuf);
		}
	}
	else if(!strncmp("wan",argv[2],3))
	{
		result=lktos_initconfig_init_wan(plattype,errBuf);
		if(!result)
		{
			printf("\r\n error msg=%s",errBuf);
		}
	}
	else if(!strncmp("lan",argv[2],3))
	{
		result=lktos_networkconfig_init_lan(plattype,errBuf);
		if(!result)
		{
			printf("\r\n error msg=%s",errBuf);
		}
	}
	else
	{
		usage();
		return 0;
	}
}

