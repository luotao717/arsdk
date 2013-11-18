#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
//#include <linux/autoconf.h>


int main(int argc,char **argv)
{
	struct stat fileInfo;
	char cmdBuf[128]={0};
	int flag=0;
	int kerflag=0;
	if(argc!= 3)
	{
		printf("\r\npara err!\r\nusage:%s tftpServerIp fileName\r\neg:%s 169.254.0.55 root_uImage\r\n",argv[0],argv[0]);
		return 0;
	}
	sprintf(cmdBuf,"tftp -g -l /var/userFire -r %s %s",argv[2],argv[1]);
	system(cmdBuf);
	
	flag=stat("/var/userFire", &fileInfo);
  if( flag == -1)
  {
  	printf("\r\nupgrade error!no file");
  	return 0;
  }
  if(!strcmp(argv[2],"kerneltao"))
	kerflag=1;
  if(!kerflag)
{
 if((fileInfo.st_size < 2000000) || (fileInfo.st_size > 3145728))
  {
  	printf("\r\nfile too larger or too small\r\n");
  	return 0;
 }
}
 
  printf("\r\nsize=%ld\n", fileInfo.st_size);
  //system("echo 1 > /tmp/rebootFlag");
  if(kerflag)
  {
     sprintf(cmdBuf,"/sbin/mtd_write -o 0 -l %ld write /var/userFire uImage",fileInfo.st_size);
     //system("echo 883 > /proc/simple_config/simple_config_led");
  }
  else
  {
     sprintf(cmdBuf,"/sbin/mtd_write -o 0 -l %ld write /var/userFire flash",fileInfo.st_size);
     //system("echo 884 > /proc/simple_config/simple_config_led");
  }
	//printf("\r\nupcmd=%s\r\n",cmdBuf);
	
	//system("nvram_set 2860 autodefaultttt 1");
	//system("/tmp/busybox sleep 3");
	system(cmdBuf);
	printf("\r\nDone...rebooting now please wait\r\n");
	//system("/tmp/busybox sleep 3");
	//system("/tmp/busybox reboot");
	//while (1) {
		//pause();
	//}
	return 1;
}

