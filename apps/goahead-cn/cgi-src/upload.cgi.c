#include "upload.cgi.h"
#include "../../lktos_app/pctools/apmib.h"

#define REFRESH_TIMEOUT		"40000"		/* 40000 = 40 secs*/

#define RFC_ERROR "RFC1867 error"
#define CONFIG_RT2880_ROOTFS_IN_RAM 1

void *memmem(const void *buf, size_t buf_len, const void *byte_line, size_t byte_line_len)
{
    unsigned char *bl = (unsigned char *)byte_line;
    unsigned char *bf = (unsigned char *)buf;
    unsigned char *p  = bf;

    while (byte_line_len <= (buf_len - (p - bf))){
        unsigned int b = *bl & 0xff;
        if ((p = (unsigned char *) memchr(p, b, buf_len - (p - bf))) != NULL){
            if ( (memcmp(p, byte_line, byte_line_len)) == 0)
                return p;
            else
                p++;
        }else{
            break;
        }
    }
    return NULL;
}

#define MEM_SIZE	1024
#define MEM_HALT	512
int findStrInFile(char *filename, int offset, unsigned char *str, int str_len)
{
	int pos = 0, rc;
	FILE *fp;
	unsigned char mem[MEM_SIZE];

	if(str_len > MEM_HALT)
		return -1;
	if(offset <0)
		return -1;

	fp = fopen(filename, "rb");
	if(!fp)
		return -1;

	rewind(fp);
	fseek(fp, offset + pos, SEEK_SET);
	rc = fread(mem, 1, MEM_SIZE, fp);
	while(rc){
		unsigned char *mem_offset;
		mem_offset = (unsigned char*)memmem(mem, rc, str, str_len);
		if(mem_offset){
			fclose(fp);	//found it
			return (mem_offset - mem) + pos + offset;
		}

		if(rc == MEM_SIZE){
			pos += MEM_HALT;	// 8
		}else
			break;
		
		rewind(fp);
		fseek(fp, offset+pos, SEEK_SET);
		rc = fread(mem, 1, MEM_SIZE, fp);
	}

	fclose(fp);
	return -1;
}

/*
 *  ps. callee must free memory...
 */
void *getMemInFile(char *filename, int offset, int len)
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


#if defined (UPLOAD_FIRMWARE_SUPPORT)
/*
 *  taken from "mkimage -l" with few modified....
 */


 #if 0
int check(char *imagefile, int offset, int len, char *err_msg)
{
	struct stat sbuf;

	int  data_len;
	char *data;
	unsigned char *ptr;
	unsigned long checksum;

	image_header_t header;
	image_header_t *hdr = &header;

	int ifd;

	if ((unsigned)len < sizeof(image_header_t)) {
		sprintf (err_msg, "Bad size: \"%s\" is no valid image\n", imagefile);
		return 0;
	}

	ifd = open(imagefile, O_RDONLY);
	if(!ifd){
		sprintf (err_msg, "Can't open %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	if (fstat(ifd, &sbuf) < 0) {
		close(ifd);
		sprintf (err_msg, "Can't stat %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	ptr = (unsigned char *) mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, ifd, 0);
	if ((caddr_t)ptr == (caddr_t)-1) {
		close(ifd);
		sprintf (err_msg, "Can't mmap %s: %s\n", imagefile, strerror(errno));
		return 0;
    }
	ptr += offset;

	/*
	 *  handle Header CRC32
	 */
    memcpy (hdr, ptr, sizeof(image_header_t));

    if (ntohl(hdr->ih_magic) != IH_MAGIC) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "Bad Magic Number: \"%s\" is no valid image\n", imagefile);
		return 0;
	}

	data = (char *)hdr;

    checksum = ntohl(hdr->ih_hcrc);
    hdr->ih_hcrc = htonl(0);	/* clear for re-calculation */

    if (crc32 (0, data, sizeof(image_header_t)) != checksum) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "*** Warning: \"%s\" has bad header checksum!\n", imagefile);
		return 0;
    }

	/*
	 *  handle Data CRC32
	 */
    data = (char *)(ptr + sizeof(image_header_t));
    data_len  = len - sizeof(image_header_t) ;

    if (crc32 (0, data, data_len) != ntohl(hdr->ih_dcrc)) {
		munmap(ptr, len);
		close(ifd);
		sprintf (err_msg, "*** Warning: \"%s\" has corrupted data!\n", imagefile);
		return 0;
    }

#if 1
	/*
	 * compare MTD partition size and image size
	 */
#if defined (CONFIG_RT2880_ROOTFS_IN_RAM)
	if(len > getMTDPartSize("\"Kernel\"")){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) is bigger than Kernel MTD partition.\n", len);
		return 0;
	}
#elif defined (CONFIG_RT2880_ROOTFS_IN_FLASH)
  #ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
	if(len > getMTDPartSize("\"Kernel_RootFS\"")){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) is bigger than Kernel_RootFS MTD partition.\n", len);
		return 0;
	}
  #else
	if(len < CONFIG_MTD_KERNEL_PART_SIZ){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) size doesn't make sense.\n", len);
		return 0;
	}

	if((len - CONFIG_MTD_KERNEL_PART_SIZ) > getMTDPartSize("\"RootFS\"")){
		munmap(ptr, len);
		close(ifd);
		sprintf(err_msg, "*** Warning: the image file(0x%x) is bigger than RootFS MTD partition.\n", len - CONFIG_MTD_KERNEL_PART_SIZ);
		return 0;
	}
  #endif
#else
#error "goahead: no CONFIG_RT2880_ROOTFS defined!"
#endif
#endif

	munmap(ptr, len);
	close(ifd);

	return 1;
}
 #endif
#endif /* UPLOAD_FIRMWARE_SUPPORT */

/*
 * arguments: ifname  - interface name
 *            if_addr - a 16-byte buffer to store ip address
 * description: fetch ip address, netmask associated to given interface name
 */
int getIfIp(char *ifname, char *if_addr)
{
    struct ifreq ifr;
    int skfd = 0;

    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("getIfIp: open socket error");
        return -1;
    }

    strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
    if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
        printf("getIfIp: ioctl SIOCGIFADDR error for %s", ifname);
        return -1;
    }
    strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    close(skfd);
    return 0;
}

/*
 * I'm too lazy to use popen() instead of system()....
 * ( note:  static buffer used)
 */
#define DEFAULT_LAN_IP "192.168.0.1"
char *getLanIP(void)
{
	static char buf[64];
	char *nl;
	FILE *fp;

	memset(buf, 0, sizeof(buf));
	if( (fp = popen("nvram_get 2860 lan_ipaddr", "r")) == NULL )
		goto error;

	if(!fgets(buf, sizeof(buf), fp)){
		pclose(fp);
		goto error;
	}

	if(!strlen(buf)){
		pclose(fp);
		goto error;
	}
	pclose(fp);

	if(nl = strchr(buf, '\n'))
		*nl = '\0';

	return buf;

error:
	fprintf(stderr, "warning, cant find lan ip\n");
	return DEFAULT_LAN_IP;
}

#define DEFAULT_LANGTYPE "cn"
char *getLangType(void)
{
	static char buf[64];
	char *nl;
	FILE *fp;

	memset(buf, 0, sizeof(buf));
	if( (fp = popen("nvram_get 2860 LanguageType", "r")) == NULL )
		goto error;

	if(!fgets(buf, sizeof(buf), fp)){
		pclose(fp);
		goto error;
	}

	if(!strlen(buf)){
		pclose(fp);
		goto error;
	}
	pclose(fp);

	if(nl = strchr(buf, '\n'))
		*nl = '\0';

	return buf;

error:
	fprintf(stderr, "warning, cant find default language type\n");
	return DEFAULT_LANGTYPE;
}


void javascriptUpdate(int success)
{
    printf("<script language=\"javascript\">\n");
    if(success){
        printf(" \n\
function refresh_all(){	\n\
  top.location.href = \"http://%s/home.asp\"; \n\
} \n\
function update(){ \n\
  self.setTimeout(\"refresh_all()\", %s);\n\
}\n", getLanIP(), REFRESH_TIMEOUT);
    }else{
        printf("function update(){ }\n");
    }
    printf("</script>\n");
}

inline void webFoot(void)
{
    printf("\n</body>\n</html>\n");
}


int main (int argc, char *argv[])
{
    int file_begin, file_end;
    int line_begin, line_end;
    char err_msg[256];
    char *boundary; int boundary_len;
    IMG_HEADER_Tp pHeader;
    int i=0;
    int checkOk=0;
    int firewareType=0;
    int pid;
    char *filename = getenv("UPLOAD_FILENAME");

    printf(
"\
Server: %s\n\
Pragma: no-cache\n\
Content-type: text/html\n",
	getenv("SERVER_SOFTWARE"));

    printf("\n\
<html>\n\
<head>\n\
<title>My Title111111--%s</title>\n\
<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">\n\
<script language=\"javascript\" src=\"/js/language_%s.js\"></script>\n\
<SCRIPT language=javascript type=text/javascript>var rebootInf = new Array(\"%s\",80,65,1,0,0 );</SCRIPT><LINK href=\"/dynaform/css_main.css\" type=text/css rel=stylesheet><SCRIPT language=javascript src=\"/dynaform/common.js\"  type=text/javascript></SCRIPT><script language=JavaScript><!--\n\
function LoadMain(){parent.location.href = 'http://' + rebootInf[0] + ':' + rebootInf[1];}function doLoad(){doReFresh();}var timeout = rebootInf[2] * 10;var rate = 0;function doReFresh(){window.setTimeout(\"growUp()\",timeout);}function growUp(){rate = rate + 1;document.getElementById(\"td1\").style.width = rate + \"%%\";var per=document.getElementById(\"percent\");per.innerHTML=rate+\"%%\";if(rate >= 100){var obj=document.getElementById(\"t_notice\"); obj.style.display=\"block\";document.getElementById(\"t_restart\").style.display=\"none\";document.getElementById(\"t_complete\").style.display=\"block\";document.getElementById(\"t_complete\").style.color=\"red\";per.style.color=\"red\";LoadMain();return;}doReFresh();}\n\
//--></script>\n\
<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n\
</head>\n\
<body onload=\"return doLoad()\">\n",filename,getLangType(),getLanIP());

      line_begin = 0;
      if((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1){
          printf("%s %d", RFC_ERROR, 1);
          return -1;
      }
      boundary_len = line_end - line_begin;
      boundary = getMemInFile(filename, line_begin, boundary_len);
  //  printf("boundary:%s\n", boundary);

      // sth like this..
      // Content-Disposition: form-data; name="filename"; filename="\\192.168.3.171\tftpboot\a.out"
      //
      char *line, *semicolon, *user_filename;
      line_begin = line_end + 2;
      if((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1){
          printf("%s %d", RFC_ERROR, 2);
          goto err;
      }
      line = getMemInFile(filename, line_begin, line_end - line_begin);
      if(strncasecmp(line, "content-disposition: form-data;", strlen("content-disposition: form-data;"))){
          printf("%s %d", RFC_ERROR, 3);
          goto err;
      }
      semicolon = line + strlen("content-disposition: form-data;") + 1;
      if(! (semicolon = strchr(semicolon, ';'))  ){
          printf("We dont support multi-field upload.\n");
          goto err;
      }
      user_filename = semicolon + 2;
      if( strncasecmp(user_filename, "filename=", strlen("filename="))  ){
          printf("%s %d", RFC_ERROR, 4);
          goto err;
      }
      user_filename += strlen("filename=");
      //until now we dont care about what the true filename is.
      free(line);

      // We may check a string  "Content-Type: application/octet-stream" here,
      // but if our firmware extension name is the same with other known ones, 
      // the browser would use other content-type instead.
      // So we dont check Content-type here...
      line_begin = line_end + 2;
      if((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1){
          printf("%s %d", RFC_ERROR, 5);
          goto err;
      }

      line_begin = line_end + 2;
      if((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1){
          printf("%s %d", RFC_ERROR, 6);
          goto err;
      }

      file_begin = line_end + 2;

      if( (file_end = findStrInFile(filename, file_begin, boundary, boundary_len)) == -1){
          printf("%s %d", RFC_ERROR, 7);
          goto err;
      }
      file_end -= 2;		// back 2 chars.(\r\n);


	//read our flag
	line = getMemInFile(filename, file_begin, 16);
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
			javascriptUpdate(0);
       		goto err;
		}
		firewareType=1;
	}
	else if(pHeader->signature[0] == 'r' && pHeader->signature[1] == 'f')
	{
		if(pHeader->len > 2883580)
		{
			printf("too large fireware");
			javascriptUpdate(0);
       		goto err;
		}
		firewareType=2;
	}
	else
	{
		printf("not valid firetype");
		javascriptUpdate(0);
		goto err;
	}
	//printf("head-flag=%c%c%c%c--len=%d--%0x--%d",pHeader->signature[0],pHeader->signature[1],pHeader->signature[2],pHeader->signature[3],pHeader->len,pHeader->len,file_end - file_begin);
	file_begin+=16;
	free(line);
	line = getMemInFile(filename, file_begin, file_end - file_begin);
	checkOk=fwChecksumOk(line, file_end - file_begin);
	free(line);
	//printf("checksum=%d--size=%d",checkOk,file_end - file_begin);
	if(!checkOk)
	{
		printf("checksum error");
		javascriptUpdate(0);
       	goto err;
	}
	file_end -= 2;
      // printf("file:%s, file_begin:%d, len:%d<br>\n", filename, file_begin, file_end - file_begin);

    // examination
#if defined (UPLOAD_FIRMWARE_SUPPORT)
#if 0
    if(!check(filename, file_begin, file_end - file_begin, err_msg) ){
        //printf("Not a valid firmware. %s", err_msg);
       	if (!strcmp(getLangType(), "cn"))
			printf("<script>dw(JS_None)</script>");
		else if (!strcmp(getLangType(), "pt"))
			printf("Não é um arquivo de Firmware válido.");
		else
			printf("Not a valid firmware.");

		javascriptUpdate(0);
        goto err;
    }
#endif
    /*
     * write the current linux version into flash.
     */
    //write_flash_kernel_version(filename, file_begin);
#ifdef CONFIG_RT2880_DRAM_8M
    system("killall goahead");
#endif

   system("echo 88 > /proc/simple_config/simple_config_led");
//     pid = fork();
//    if(pid==0)
//    {
//	   printf("<CENTER><FORM><TABLE border=0><TBODY> <TR><TD class=h1 id=t_title>Restart</TD></TR><TR><TD class=blue></TD></TR><TR><TD class=h2 id=t_restart>Restarting...</TD></TR><TR><TD class=h2 id = \"t_upgrade\">Software Upgraded Successfully!</TD></TR><TR><TD class=h2 id=t_complete style=\"DISPLAY: none\">Completed!</TD></TR> <TR><TD style=\"PADDING-LEFT: 5%%\" align=middle><TABLE class=space height=10 cellSpacing=0 cellPadding=0 width=\"40%%\" align=center border=0><TBODY><TR><TD><DIV id=percent></DIV></TD><TD style=\"WIDTH: 98\%%\"><TABLE class=guage id=td1 style=\"WIDTH: 0px\" height=10 cellSpacing=0 cellPadding=0 border=0><TBODY><TR><TD></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR><TR> <TD class=h2 id=t_notice style=\"DISPLAY: none\">Please wait a moment, if the browser does not refresh automatically, click Refresh on the top of your browser.</TD></TR><TR><TD class=blue></TD></TR></TBODY></TABLE></FORM></CENTER>");
//	   webFoot();
//	   exit(0);
//    }
    // flash write
    if( mtd_write_firmware(filename, file_begin, file_end - file_begin,firewareType) == -1){
      printf("mtd_write fatal error! The corrupted image has ruined the flash!!");
        javascriptUpdate(0);
        goto err;
   }
#elif defined (UPLOAD_BOOTLOADER_SUPPORT)
    mtd_write_bootloader(filename, file_begin, file_end - file_begin);
#else
	#error "no upload support defined!"
#endif

	//if (!strcmp(getLangType(), "cn"))
   		//printf("<blockquote>\n\<br><b><script>dw(JS_Update)</script></b></blockquote>");
    //else if (!strcmp(getLangType(), "pt"))
		//printf("<blockquote>\n\<br><b>Atualizado com sucesso! Reiniciando....</b></blockquote>");
	//else
		//printf("<blockquote>\n\<br><b>Upgrade firmware successfully! rebooting....</b></blockquote>");
	//sprintf(err_msg,"cp %s /tmp/kkkko",filename);
	//system(err_msg);
	//javascriptUpdate(1);
	printf("<CENTER><FORM><TABLE border=0><TBODY> <TR><TD class=h1 id=t_title><script>dw(MM_firmware)</script></TD></TR><TR><TD class=blue></TD></TR><TR><TD class=h2 id=t_restart><script>dw(HC_reboot_now)</script></TD></TR><TR><TD class=h2 id = \"t_upgrade\"><script>dw(HC_upgrade_ok)</script></TD></TR><TR><TD class=h2 id=t_complete style=\"DISPLAY: none\"><script>dw(HC_completed)</script></TD></TR> <TR><TD style=\"PADDING-LEFT: 5%%\" align=middle><TABLE class=space height=10 cellSpacing=0 cellPadding=0 width=\"40%%\" align=center border=0><TBODY><TR><TD><DIV id=percent></DIV></TD><TD style=\"WIDTH: 98\%%\"><TABLE class=guage id=td1 style=\"WIDTH: 0px\" height=10 cellSpacing=0 cellPadding=0 border=0><TBODY><TR><TD></TD></TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR><TR> <TD class=h2 id=t_notice style=\"DISPLAY: none\"><script>dw(HC_completed_info)</script></TD></TR><TR><TD class=blue></TD></TR></TBODY></TABLE></FORM></CENTER>");
	webFoot();
    free(boundary);
//#if defined (UPLOAD_BOOTLOADER_SUPPORT) || defined (CONFIG_RT2880_DRAM_8M)
    //system("sleep 3 && reboot &");
//#endif
    exit(0);

err:
    webFoot();
    free(boundary);
    exit(-1);
}

