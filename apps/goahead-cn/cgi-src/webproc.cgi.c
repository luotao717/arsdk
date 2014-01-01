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

#define DEFAULT_WORKTYPE "1"
char *getWorkType(void)
{
	static char buf[64];
	char *nl;
	FILE *fp;

	memset(buf, 0, sizeof(buf));
	if( (fp = popen("nvram_get 2860 workModeOk", "r")) == NULL )
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
	fprintf(stderr, "warning, cant find default work type\n");
	return DEFAULT_WORKTYPE;
}

#define DEFAULT_UPGRADESTATUS "10"
char *getUpgradeStatus(void)
{
	static char buf[64];
	char *nl;
	FILE *fp;

	memset(buf, 0, sizeof(buf));
	if( (fp = popen("nvram_get 2860 upgradeStatus", "r")) == NULL )
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
	fprintf(stderr, "warning, cant find default upgrade status\n");
	return DEFAULT_UPGRADESTATUS;
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
	char tmp_buff[256]={0};
	char cmdBuf[256]={0};
	char *boundary; int boundary_len;
	IMG_HEADER_Tp pHeader;
	int i=0;
	int checkOk=0;
	int firewareType=0;
	int pid;
	int iscmts=0;
	char *serverIp=NULL;
	char *firewareName=NULL;
	char *cmdtype = getenv("reset");
	char *stbip =getenv("stbip");
	char *stbpath=getenv("stbpath");

	if(!strcmp(getWorkType(),"2"))
	{
		iscmts=1;
		//firewareName="stb_kf_cm.bin";
		firewareName="stb_kf.bin";
	}
	else
	{
		iscmts=0;
		//firewareName="stb_kf_eoc.bin";
		firewareName="stb_kf.bin";
	}
	if(!strncmp(cmdtype,"stbget",7))
	{
		sprintf(tmp_buff,"flag_version=%s:%s:%s&iscm=%d","0101","b001","f001",iscmts);
		printf("Server: %s\nPragma: no-cache\nContent-type: text/html\n",getenv("SERVER_SOFTWARE"));
		printf("\n%s\n",tmp_buff);
	}
	else if(!strncmp(cmdtype,"stbset",7))
	{
		if(stbip==NULL)
		{
			stbip="192.168.1.2";
		}
		if(stbpath!=NULL)
		{
			firewareName=stbpath;
		}
		sprintf(cmdBuf,"upgrade %s %s",stbip,firewareName);
		sprintf(tmp_buff,"%s","ok");
		printf("Server: %s\nPragma: no-cache\nContent-type: text/html\n",getenv("SERVER_SOFTWARE"));
		printf("\n%s\n",tmp_buff);
		fflush(stdout);
		pid=fork();
		if(0 == pid)
		{
	    		//sleep(3);
			//reboot(LINUX_REBOOT_CMD_RESTART);
			system(cmdBuf);
			exit(1);
		}
		
	}
	else if(!strncmp(cmdtype,"stbstatget",11))
	{
		sprintf(tmp_buff,"%s",getUpgradeStatus());
             printf("Server: %s\nPragma: no-cache\nContent-type: text/html\n",getenv("SERVER_SOFTWARE"));
		printf("\n%s\n",tmp_buff);
	}
	else
	{
		sprintf(tmp_buff,"%s","unknow command!");
	}
	exit(0);
}

