
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "nvram.h"

static int rebootCount=15;

#if 0
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT
#endif

int main(int argc, char *argv[])
{
	FILE *fp;
	int i;
	while(1) 
	{  	  

		fp=fopen("/proc/simple_config/push_button","r");
		fscanf(fp,"%d",&i);
		if (i==1) 
		{   
	    		printf("Going to Reload Default\n");
			system("ralink_init clear 2860");        
			system("ralink_init renew 2860 /sbin/RT2860_default_vlan");
			sleep(3);
			system("reboot");
		}
		else if (i==2) 
		{   
			system("reboot");
		}
		else
		{
		}
		fclose(fp);
		fp=fopen("/tmp/rebootFlag","r");
		if(fp != NULL)
		{
			fscanf(fp,"%d",&i);
			fclose(fp);
			if(i==1)
			{
				rebootCount--;
			}
			if(!rebootCount)
			{
				printf("\r\ndadsadasd");
				//execl("/tmp/reboot","echo","rebooting now!",(char *)0);
				execl("/tmp/reboot","reboot",(char *)0);
				//system("/tmp/busybox reboot");
			}
			
		}
		sleep(3);
	}
}
	
