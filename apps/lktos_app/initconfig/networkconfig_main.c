
/*
  main function
  lktOS wirelss config moudle for all platform 
  
  v1.0 support mt7620  by luot  20121211
  
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
	printf("\r\nfor eg: lktos_networkconfig mtk7620 initlan\r\n");
	printf("\r\nfor eg: lktos_networkconfig mtk7620 initwan\r\n");
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
	if(!strncmp("initlan",argv[2],7))
	{
		result=lktos_networkconfig_init_lan(plattype,errBuf);
		if(!result)
		{
			printf("\r\n error msg=%s",errBuf);
		}
	}
	else if(!strncmp("initwan",argv[2],7))
	{
		result=lktos_networkconfig_init_wan(plattype,errBuf);
		if(!result)
		{
			printf("\r\n error msg=%s",errBuf);
		}
	}
	else if(!strncmp("getdhcpdconfig",argv[2],15))
	{
		result=lktos_networkconfig_gen_lanDhcpdConfig(plattype,errBuf);
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

