#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "nvram.h"

#define TRUE 		0
#define FALSE		-1

#define PS_LOG_PATH     	"/var/ps.log"
#define DEV_LOG_PATH    	"/var/dev.log"
#define OPENL2TPD_PID_PATH	"/var/run/openl2tpd.pid"
#define PPPD_PID_PATH		"/var/run/ppp0.pid"	
#define PPPD_OPTIONS_FILE	"/etc/options.pptp"

char *dev = "ppp0";
char *l2tp = "openl2tpd";
char *pppd = "pppd";

int search_str(const char *dst, const char *src)
{	
    while ('\0' != *src)
    {
        if (0 == strncmp(dst, src, strlen(dst)))
            return TRUE;

		src ++;
    }
 
    return FALSE;
}

int check_dev(char *dev)
{   
	int nRet;
    char dev_info[200];
    char cmd[50];
    FILE *fp;
    
    sprintf(cmd, "ifconfig | grep %s >"DEV_LOG_PATH, dev);
    system(cmd),
    
    fp = fopen(DEV_LOG_PATH, "r");
    if (NULL == fp)
        return FALSE;

    nRet = fread(dev_info, 1, sizeof(dev_info), fp);
	if (nRet == 0)
	{
		fclose(fp);
		return FALSE;
		
	}
	
    fclose(fp);
	return search_str(dev, dev_info);
}

#define MAX_BUF_SIZE		256

int check_ps(char *ps)
{
	int nRet;
    char ps_info[MAX_BUF_SIZE];
    char cmd[100];
    FILE *fp;

    sprintf(cmd, "ps -ef | grep %s | grep -v ""grep"" >"PS_LOG_PATH, ps);
    system(cmd);

    fp = fopen(PS_LOG_PATH, "r");
    if (NULL == fp)
        return FALSE;
	
   	nRet = fread(ps_info, 1, sizeof(ps_info), fp);
	if (nRet <= 0)
	{
		fclose(fp);
		return FALSE;
	}
	
    fclose(fp);
	return search_str(ps, ps_info);
}

int main(int argc, char **argv)
{
	char *cmd[128]={0};
	char *wan_mode;
	wan_mode = nvram_bufget(RT2860_NVRAM, "wanConnectionMode");
	while(1)
	{
		usleep(20 * 1000 * 1000);
		if (TRUE != check_dev(dev))
		{
			 if (0 == strncmp(wan_mode, "PPPOE", 6))
			{
				if (FALSE != access(PPPD_PID_PATH, F_OK))
				{
					system("rm "PPPD_PID_PATH);
				}
				
		            if (TRUE == check_ps(pppd))
		            {
						//sprintf(cmd, "killall %s", pppd);
						//usleep(100 * 1000);
						//system(cmd);
						continue;
		            }
		            else
		            {
		            	
		           		 sprintf(cmd,"config-pppoe.sh %s %s %s 1480 1480",nvram_bufget(RT2860_NVRAM, "wan_pppoe_user"),nvram_bufget(RT2860_NVRAM, "wan_pppoe_pass"),"br1");
					 system(cmd);
		            }
			}
		}	
	}
}
