
/*
  main function
  lktOS wirelss config moudle for all platform 
  
  v1.0 support rt3050,rt3052,mt7620  by luot  20121207
  
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
	printf("\r\nlktos wireless config tool");
	printf("\r\nussgr:");
	printf("\r\nlktos_wificonfig <platform> <command>");
	printf("\r\ncurrrent support <paltform>:ra3050, mtk7620 ra5350<plat> <command>");
	printf("\r\ncurrrent support <command>:init  (to init the wifi)");
	printf("\r\nfor eg: lktos_wificonfig ra3050 init\r\n");
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
	else
	{
		usage();
		return 0;
	}
	if(!strncmp("init",argv[2],4))
	{
		result=lktos_wificonfig_initall_by_filemode(plattype,errBuf);
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

