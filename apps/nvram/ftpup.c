#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "../lktos_app/pctools/apmib.h"
//#include <linux/autoconf.h>


static int fwChecksumOk(char *data, int len)
{
	unsigned short sum=0;
	int i;

	for (i=0; i<len; i+=2) {
//#ifdef _LITTLE_ENDIAN_
		//sum += WORD_SWAP( *((unsigned short *)&data[i]) );
//#else
		sum += *((unsigned short *)&data[i]);
//#endif

	}
	return( (sum==0) ? 1 : 0);
}

static void *getMemInFile(char *filename, int offset, int len)
{
    void *result;
    FILE *fp;
    if( (fp = fopen(filename, "r")) == NULL ){
        return NULL;
    }
	fseek(fp, offset, SEEK_SET);
    result = malloc(sizeof(unsigned char) * len );
	if(!result)
		return NULL;
    if( fread(result, 1, len, fp) != len){
        free(result);
        return NULL;
    }
    return result;
}

int main(int argc,char **argv)
{
	struct stat fileInfo;
	char cmdBuf[128]={0};
	int flag=0;
	int kerflag=0;
	IMG_HEADER_Tp pHeader;
   	 int i=0;
   	 int checkOk=0;
   	 int firewareType=0;
	 char *line;
	  int file_begin=0, file_end;
   	 int line_begin, line_end;
	if(argc < 4)
	{
		printf("\r\npara err!\r\nusage:%s ftpServerIp fileName type user pass port\r\neg:%s 169.254.0.55 root_uImage 0 \r\n",argv[0],argv[0]);
		return 0;
	}
	if(atoi(argv[3]) == 0)
	{
		if(argv[4] != NULL)
		{
			sprintf(cmdBuf,"ftpget -P %d  %s /var/userFire %s ",atoi(argv[3]),argv[1],argv[2]);
		}
		else
		{
			sprintf(cmdBuf,"ftpget %s /var/userFire %s ",argv[1],argv[2]);
		}
	}
	else if(atoi(argv[3] )== 1)
	{
		if( argc < 6)
		{
			printf("\r\npara error");
			return 0;
		}
		if(argv[4] != NULL)
		{
			sprintf(cmdBuf,"ftpget -u %s",argv[4]);
		}
		if(argv[5] != NULL && (strcmp(argv[5],"##")))
		{
			sprintf(cmdBuf,"%s -p %s",cmdBuf,argv[5]);
		}
		if(argv[6] != NULL)
		{
			sprintf(cmdBuf,"%s -P %d",cmdBuf,atoi(argv[6]));
		}
		sprintf(cmdBuf,"%s %s /var/userFire %s ",cmdBuf,argv[1],argv[2]);
	}
	else
	{
		printf("\r\nerror load type");
		return 0;
	}
	system(cmdBuf);
	
	flag=stat("/var/userFire", &fileInfo);
  if( flag == -1)
  {
  	printf("\r\nupgrade error!no file");
  	return 0;
  }
  //read our flag
	line = getMemInFile("/var/userFire", file_begin, 16);
	//for(i=0;i<16;i++)
	//{
		//printf("%02x-",*(line+i));
	//}
	pHeader = (IMG_HEADER_Tp) line;
	if(pHeader->signature[0] == 'k' && pHeader->signature[1] == 'f')
	{
		if(pHeader->len > 851960)
		{
			printf("too large fireware");
			return 0;
		}
		firewareType=1;
	}
	else if(pHeader->signature[0] == 'r' && pHeader->signature[1] == 'f')
	{
		if(pHeader->len > 2883580)
		{
			printf("too large fireware");
			return 0;
		}
		firewareType=2;
	}
	else
	{
		printf("not valid firetype");
		return 0;
	}
	//printf("head-flag=%c%c%c%c--len=%d--%0x--%d",pHeader->signature[0],pHeader->signature[1],pHeader->signature[2],pHeader->signature[3],pHeader->len,pHeader->len,file_end - file_begin);
	file_begin+=16;
	free(line);
	line = getMemInFile("/var/userFire", file_begin,pHeader->len);
	checkOk=fwChecksumOk(line, pHeader->len);
	free(line);
	//printf("checksum=%d--size=%d",checkOk,file_end - file_begin);
	if(!checkOk)
	{
		printf("checksum error");
		return 0;
	}
	file_end = pHeader->len -2;
	//system("echo 884 > /proc/simple_config/simple_config_led");
	 //system("echo 1 > /tmp/rebootFlag");
	if( 1 == firewareType)
  	{
  		snprintf(cmdBuf, sizeof(cmdBuf), "/sbin/mtd_write -o %d -l %d write %s uImage", 16, file_end, "/var/userFire");
		system("echo 883 > /proc/simple_config/simple_config_led");
  	}
  	else if( 2 == firewareType)
  	{
  		snprintf(cmdBuf, sizeof(cmdBuf), "/sbin/mtd_write -o %d -l %d write %s rootfs", 16, file_end, "/var/userFire");
		system("echo 884 > /proc/simple_config/simple_config_led");
  	}
	else
	{
		printf("\r\nerror firewre");
		return 0;
	}
 
	system(cmdBuf);
	printf("\r\nDone...rebooting now please wait\r\n");
	system("/tmp/busybox sleep 3");
	system("/tmp/busybox reboot");
	return 1;
}

