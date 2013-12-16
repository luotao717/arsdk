#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include<linux/reboot.h>
//#include <linux/autoconf.h>


int main(int argc,char **argv)
{
	struct stat fileInfo;
	char cmdBuf[128]={0};
	int flag=0;
	int kerflag=0;
	unsigned char tmpbuf11[0x2000]={0};
	int i=0;
	printf("\r\n auto upgrade start!!!");
	printf("\r\nstep 1 : upload file");
	sprintf(cmdBuf,"tftp -g -l /var/userFireboot -r %s %s","yueboot.bin","192.168.1.29");
	system(cmdBuf);
	
	flag=stat("/var/userFireboot", &fileInfo);
  if( flag == -1)
  {
  	printf("\r\nupload file1 error so exit!\r\n");
  	return 0;
  }
 if((fileInfo.st_size != 65536))
  {
  	printf("\r\nfile 1 size error so exit!\r\n");
  	return 0;
 }
 	sprintf(cmdBuf,"tftp -g -l /var/userFirenvram -r %s %s","nvram.bin","192.168.1.29");
	system(cmdBuf);
	
	flag=stat("/var/userFirenvram", &fileInfo);
  if( flag == -1)
  {
  	printf("\r\nupload file2 error so exit!\r\n");
  	return 0;
  }
 if((fileInfo.st_size != 65536))
  {
  	printf("\r\nfile 2 size error so exit!\r\n");
  	return 0;
 }
 printf("\r\nupload file ok!");
 printf("\r\nstep 2 : write init file");
 sprintf(cmdBuf,"/sbin/mtd_write -o 0 -l %ld write /var/userFireboot u-boot",65536);
 system(cmdBuf);
 sprintf(cmdBuf,"/sbin/mtd_write -o 0 -l %ld write /var/userFirenvram NVRAM",65536);
 system(cmdBuf);
 printf("\r\nwrite init file ok!");
 printf("\r\nstep 3 : reboot and load all fireware!");
 printf("\r\ndo not power off during the time while you can access web ");
 sleep(3);
 reboot(LINUX_REBOOT_CMD_RESTART);
  /*
	flash_read(tmpbuf11,0,0x2000);
	for(i=5;i<64;i++)
	{
		tmpbuf11[i]=255;
	}
	flash_write_nvram_boot(tmpbuf11);
	
	system(cmdBuf);
	
	flash_read(tmpbuf11,0,0x2000);
	tmpbuf11[0x5]='a';
	tmpbuf11[0x6]='u';
	tmpbuf11[0x7]='t';
	tmpbuf11[0x8]='o';
	tmpbuf11[0x9]='o';
	tmpbuf11[0xa]='k';
	tmpbuf11[0xb]=';';
	flash_write_nvram_boot(tmpbuf11);
	*/
	return 1;
}

