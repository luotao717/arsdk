/* vi: set sw=4 ts=4 sts=4: */
/*
 *	usb.c -- USB Application Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: //WIFI_SOC/release/SDK_4_1_0_0/source/user/goahead/src/usb.c#1 $
 */

#include	<stdlib.h>
#include	<dirent.h>
#include	<arpa/inet.h>

#include	"webs.h"
#include	"nvram.h"
#include	"usb.h"
#include 	"utils.h"
#include 	"internet.h"
#include	"linux/autoconf.h"  //kernel config
#include	"config/autoconf.h" //user config

#if defined CONFIG_RALINKAPP_MPLAYER
#include	"media.h"
#endif

static void storageAdm(webs_t wp, char_t *path, char_t *query);
static void StorageAddUser(webs_t wp, char_t *path, char_t *query);
static void StorageEditUser(webs_t wp, char_t *path, char_t *query);
static void storageDiskAdm(webs_t wp, char_t *path, char_t *query);
static void storageDiskPart(webs_t wp, char_t *path, char_t *query);
#if defined CONFIG_USER_PROFTPD && (defined CONFIG_USB || defined CONFIG_MMC)
static void storageFtpSrv(webs_t wp, char_t *path, char_t *query);
#endif
static int GetNthFree(char *entry);
static int ShowPartition(int eid, webs_t wp, int argc, char_t **argv);
static int ShowAllDir(int eid, webs_t wp, int argc, char_t **argv);
static int ShowPartitionLuo(int eid, webs_t wp, int argc, char_t **argv); //by luotao
static int ShowPartitionLuoSingle(int eid, webs_t wp, int argc, char_t **argv); //by luotao
static int ShowLeftItem(int eid, webs_t wp, int argc, char_t **argv); //by luotao

static int ShowAllDirFile1(int eid, webs_t wp, int argc, char_t **argv);
static int ShowAllDirFile2(int eid, webs_t wp, int argc, char_t **argv); //by luotao
static int ShowDirFile1(int eid, webs_t wp, int argc, char_t **argv); //by luotao
static int GetXml(int eid, webs_t wp, int argc, char_t **argv); //by luotao
static void storageGetFirmwarePath(webs_t wp, char_t *path, char_t *query);
static int getCount(int eid, webs_t wp, int argc, char_t **argv);
#if defined CONFIG_USER_STORAGE && (defined CONFIG_USB || defined CONFIG_MMC)
static void setFirstPart(void);
#endif
static int getMaxVol(int eid, webs_t wp, int argc, char_t **argv);
#if CONFIG_USER_SAMBA && (defined CONFIG_USB || defined CONFIG_MMC)
static int ShowSmbDir(int eid, webs_t wp, int argc, char_t **argv);
static void storageSmbSrv(webs_t wp, char_t *path, char_t *query);
static void SmbDirAdd(webs_t wp, char_t *path, char_t *query);
static void SmbDirEdit(webs_t wp, char_t *path, char_t *query);
static void SetSambaSrv();
static int fetchSmbConfig(void);
#endif
#if defined CONFIG_USB && defined CONFIG_USER_MINIDLNA
static void DelDir(webs_t wp, char_t *path, char_t *query);
static void AddDir(webs_t wp, char_t *path, char_t *query);
static int fetchdlnaConfig(void);
static void DLNA_mini(webs_t wp, char_t *path, char_t *query);
static int ShowDlnaDir(int eid, webs_t wp, int argc, char_t **argv);
static int Dlna_Dir(int eid, webs_t wp, int argc, char_t **argv);
#endif
#if defined CONFIG_USB && defined CONFIG_USER_USHARE
static void storageMediaSrv(webs_t wp, char_t *path, char_t *query);
static void MediaDirAdd(webs_t wp, char_t *path, char_t *query);
static int ShowMediaDir(int eid, webs_t wp, int argc, char_t **argv);
static void fetchMediaConfig(void);
static void RunMediaSrv();
#endif
#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
static void webcamra(webs_t wp, char_t *path, char_t *query);
#endif
#if defined CONFIG_USB && defined CONFIG_USER_P910ND
static void printersrv(webs_t wp, char_t *path, char_t *query);
#endif
#if defined CONFIG_USER_MTDAAPD 
static void iTunesSrv(webs_t wp, char_t *path, char_t *query);
#endif

#define	LSDIR_INFO		"/tmp/lsdir"
#define	MOUNT_INFO		"/proc/mounts"

#define STORAGE_PATH    "/media"
#define USB_STORAGE_SIGN    "/media/sd"
#define SD_STORAGE_SIGN    "/media/mmc"
#define MIN_SPACE_FOR_FIRMWARE			(1024 * 1024 * 8)// minimum space for firmware upload
#define MIN_FIRMWARE_SIZE				(1048576)		// minium firmware size(1MB)
#define IH_MAGIC			0x27051956					// firmware magic number in header

#define DEBUG(x) do{fprintf(stderr, #x); \
					fprintf(stderr, ": %s\n", x); \
				 }while(0)

char firmware_path[1024] = {0};

void formDefineUSB(void) {
	websAspDefine(T("ShowPartition"), ShowPartition);
	websAspDefine(T("ShowAllDir"), ShowAllDir);
	websAspDefine(T("ShowPartitionLuo"), ShowPartitionLuo);//by luotao
	websAspDefine(T("ShowPartitionLuoSingle"), ShowPartitionLuoSingle);//by luotao
	websAspDefine(T("ShowLeftItem"), ShowLeftItem);//by luotao
	websAspDefine(T("ShowAllDirFile1"), ShowAllDirFile1);
	websAspDefine(T("ShowAllDirFile2"), ShowAllDirFile2); //by luotao
	websAspDefine(T("ShowDirFile1"), ShowDirFile1);
	websAspDefine(T("GetXml"), GetXml);
	websAspDefine(T("getCount"), getCount);
	websAspDefine(T("getMaxVol"), getMaxVol);
#if defined CONFIG_USER_SAMBA  && (defined CONFIG_USB || defined CONFIG_MMC)
	websAspDefine(T("ShowSmbDir"), ShowSmbDir);
#endif
#if defined CONFIG_USB && defined CONFIG_USER_USHARE 
	websAspDefine(T("ShowMediaDir"), ShowMediaDir);
#endif

	websFormDefine(T("storageAdm"), storageAdm);
	websFormDefine(T("StorageAddUser"), StorageAddUser);
	websFormDefine(T("StorageEditUser"), StorageEditUser);
	websFormDefine(T("storageDiskAdm"), storageDiskAdm);
	websFormDefine(T("storageDiskPart"), storageDiskPart);
	websFormDefine(T("storageGetFirmwarePath"), storageGetFirmwarePath);
#if defined CONFIG_USER_PROFTPD && (defined CONFIG_USB || defined CONFIG_MMC)
	websFormDefine(T("storageFtpSrv"), storageFtpSrv);
#endif
#if defined CONFIG_USER_SAMBA && (defined CONFIG_USB || defined CONFIG_MMC)
	websFormDefine(T("storageSmbSrv"), storageSmbSrv);
	websFormDefine(T("SmbDirAdd"), SmbDirAdd);
	websFormDefine(T("SmbDirEdit"), SmbDirEdit);
#endif
#if defined CONFIG_USB && defined CONFIG_USER_MINIDLNA
	websFormDefine(T("DelDir"), DelDir);
	websFormDefine(T("AddDir"), AddDir);
	websFormDefine(T("DLNA_mini"), DLNA_mini);
	websAspDefine(T("ShowDlnaDir"),ShowDlnaDir);
	websAspDefine(T("Dlna_Dir"),Dlna_Dir);
#endif
#if defined CONFIG_USB && defined CONFIG_USER_USHARE
	websFormDefine(T("storageMediaSrv"), storageMediaSrv);
	websFormDefine(T("MediaDirAdd"), MediaDirAdd);
#endif
#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
	websFormDefine(T("webcamra"), webcamra);
#endif
#if defined CONFIG_USB && defined CONFIG_USER_P910ND
	websFormDefine(T("printersrv"), printersrv);
#endif
#if defined CONFIG_USER_MTDAAPD 
	websFormDefine(T("iTunesSrv"), iTunesSrv);
#endif
}

static int dir_count;
static int part_count;
static int media_dir_count;
static char first_part[13];

#if defined CONFIG_USER_SAMBA && (defined CONFIG_USB || defined CONFIG_MMC)

#define		DIREXIST(bitmap,index)		bitmap[index/32] & 1<<(index%32)?1:0

struct smb_dir {
	char path[40];
	char name[20];
	char permit[1024];
};

struct smb_dir_config {
	int count;
	int bitmap[3];
	struct smb_dir dir_list[100];
};

static struct smb_dir_config smb_conf;
#endif

struct media_config {
	char path[40];
};

#if defined CONFIG_USB && defined CONFIG_USER_MINIDLNA
struct dlna_config {
	char path[128];
};

#endif
/* goform/storageAdm */
static void storageAdm(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *submit;

	submit = websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "delete"))
	{
		char_t *user_select;
		char feild[20];

		feild[0] = '\0';
		user_select = websGetVar(wp, T("storage_user_select"), T(""));
		sprintf(feild, "User%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		doSystem("rm -rf \"%s/home/%s\"", first_part, nvram_bufget(RT2860_NVRAM, feild));
		sprintf(feild, "User%sPasswd", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		sprintf(feild, "FtpUser%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		sprintf(feild, "SmbUser%s", user_select);
		nvram_bufset(RT2860_NVRAM, feild, "");
		nvram_commit(RT2860_NVRAM);
		doSystem("storage.sh admin");
		//doSystem("storage.sh ftp");
		//doSystem("storage.sh samba");
		//SetSambaSrv();
	}

	
	websRedirect(wp, submitUrl);
}

/* goform/StorageAddUser */
static void StorageAddUser(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *name, *password;
	char_t *user_ftp_enable, *user_smb_enable;
	char mode[6], feild[20];
	int index; 

	mode[0] = '\0';
	feild[0] = '\0';

	// fetch from web input
	name = websGetVar(wp, T("adduser_name"), T(""));
	password = websGetVar(wp, T("adduser_pw"), T(""));
	user_ftp_enable = websGetVar(wp, T("adduser_ftp"), T(""));
	user_smb_enable = websGetVar(wp, T("adduser_smb"), T(""));
	/*
	DEBUG(name);
	DEBUG(password);
	DEBUG(user_ftp_enable);
	DEBUG(user_smb_enable);
	*/
	// get null user feild form nvram
	index = GetNthFree("User");
	// fprintf(stderr, "index: %d\n", index);

	// set to nvram
	if (0 != index) {
		sprintf(feild, "User%d", index);
		nvram_bufset(RT2860_NVRAM, feild, name);
		sprintf(feild, "User%dPasswd", index);
		nvram_bufset(RT2860_NVRAM, feild, password);
		sprintf(feild, "FtpUser%d", index);
		nvram_bufset(RT2860_NVRAM, feild, user_ftp_enable);
		sprintf(feild, "SmbUser%d", index);
		nvram_bufset(RT2860_NVRAM, feild, user_smb_enable);
		nvram_commit(RT2860_NVRAM);
		doSystem("storage.sh admin");
		//doSystem("storage.sh ftp");
		//SetSambaSrv();
		websRedirect(wp, submitUrl);
	} else {
		websHeader(wp);
		websWrite(wp, T("<h2><script>dw(JS_msg46)</script></h2><br>\n"));
		websFooter(wp);
		websDone(wp, 200);
	}
}

/* goform/StorageEditUser */
static void StorageEditUser(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *index, *password; 
	char_t *user_ftp_enable, *user_smb_enable;
	char feild[20];

	feild[0] = '\0';
	// fetch from web input
	index = websGetVar(wp, T("hiddenIndex"), T(""));
	password = websGetVar(wp, T("edituser_pw"), T(""));
	user_ftp_enable = websGetVar(wp, T("edituser_ftp"), T(""));
	user_smb_enable = websGetVar(wp, T("edituser_smb"), T(""));
	/*
	DEBUG(index);
	DEBUG(password);
	DEBUG(user_ftp_enable);
	DEBUG(user_smb_enable);
	*/

	// set to nvram
	sprintf(feild, "User%sPasswd", index);
	nvram_bufset(RT2860_NVRAM, feild, password);
	sprintf(feild, "FtpUser%s", index);
	// DEBUG(feild);
	nvram_bufset(RT2860_NVRAM, feild, user_ftp_enable);
	sprintf(feild, "SmbUser%s", index);
	// DEBUG(feild);
	nvram_bufset(RT2860_NVRAM, feild, user_smb_enable);
	nvram_commit(RT2860_NVRAM);
	doSystem("storage.sh admin");
		
	websRedirect(wp, submitUrl);
}

static void FormatPart(char *part, char *path)
{
		FILE *fp = NULL;
		char temp[30];

		if (0 == strcmp(part, "") && 0 < strcmp(path, "")) {
			if (NULL == (fp = fopen("/proc/mounts", "r"))) {
				perror(__FUNCTION__);
				return;
			}
			while(EOF != fscanf(fp, "%s %s %*s %*s %*s %*s\n", part, temp))
			{
				if (0 == strcmp(temp, path)) {
					break;
				}
				strcpy(part, "");
			}
			fclose(fp);
		}
		if (0 == strcmp(part, "")) {
				perror(__FUNCTION__);
				return;
		} else {
			char blk_size[30];
			if (NULL == (fp = fopen("/proc/partitions", "r"))) {
				perror(__FUNCTION__);
				return;
			}
			while(EOF != fscanf(fp, "%*s %*s %s %s\n", blk_size, temp)) {
				char temp_part[30];
				sprintf(temp_part, "/dev/%s", temp);
				if (0 == strcmp(part, temp_part)) {
					doSystem("storage.sh format %s %s", part, blk_size);
					break;
				}
			}
			fclose(fp);
		}
#if defined CONFIG_USER_SAMBA && (defined CONFIG_USB || defined CONFIG_MMC)
		if (0 == strcmp(path, first_part))
			memset(&smb_conf, 0, sizeof(struct smb_dir_config));
#endif	
		return;
}

static void storageDiskAdm(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *submit;

	submit = websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "delete"))
	{
		char_t *dir_path = websGetVar(wp, T("dir_path"), T(""));

#if defined CONFIG_USER_SAMBA && (defined CONFIG_USB || defined CONFIG_MMC)
		FILE *fp = NULL; 
		char smb_config_file[25];
		int exist, index = 0;

		sprintf(smb_config_file, "%s/.smb_config", first_part);
		if (NULL == (fp = fopen(smb_config_file, "w")))
		{
			perror(__FUNCTION__);
			return;
		}
		while(96 > index)
		{
			exist = DIREXIST(smb_conf.bitmap, index);
			if ((exist) && 
				(0 == strcmp(dir_path, smb_conf.dir_list[index].path)))
			{
				// fprintf(stderr, "before set bitmap: %x%x%x\n", smb_conf.bitmap[2], smb_conf.bitmap[1], smb_conf.bitmap[0]);
				smb_conf.bitmap[index/32] &= ~(1<<(index%32));
				// fprintf(stderr, "after set bitmap: %x%x%x\n", smb_conf.bitmap[2], smb_conf.bitmap[1], smb_conf.bitmap[0]);
				smb_conf.count--;
				// fprintf(stderr, "smb dir count: %d\n", smb_conf.count);
				break;
			}
			index++;
		}
		fwrite(&smb_conf, sizeof(struct smb_dir_config), 1, fp);
		fclose(fp);
#endif	
		doSystem("rm -rf \"%s\"", dir_path);
		websRedirect(wp, submitUrl);
	}
	else if (0 == strcmp(submit, "add"))
	{
		char_t *dir_name, *disk_part;

		dir_name = websGetVar(wp, T("adddir_name"), T(""));
		disk_part = websGetVar(wp, T("disk_part"), T(""));
		// DEBUG(dir_name);
		// DEBUG(disk_part);
		doSystem("mkdir -p \"%s/%s\"", disk_part, dir_name);	
		doSystem("chmod 777 \"%s/%s\"", disk_part, dir_name);	
	}
	else if (0 == strcmp(submit, "format"))
	{
		char_t *disk_part = websGetVar(wp, T("disk_part"), T(""));
		char part[30] = "";
		FormatPart(part, disk_part);
		sleep(5);
		doSystem("storage.sh restart");
		sleep(5);

		websRedirect(wp, submitUrl);
	}
	else if (0 == strcmp(submit, "remove"))
	{
		FILE *fp_mount = NULL;
		char part[30];
		
		if (NULL == (fp_mount = fopen("/proc/mounts", "r")))
		{
			perror(__FUNCTION__);
			return;
		}
		while(EOF != fscanf(fp_mount, "%s %*s %*s %*s %*s %*s\n", part))
		{
			if (NULL != strstr(part, "/dev/sd") || NULL != strstr(part, "/dev/mmc"))
				doSystem("umount -l %s", part);
		}
		fclose(fp_mount);
	//#ifdef CONFIG_LKTOS_PRIVATE_OEM_NAME_CDRKING
		//system("gpio c 13 1");
	//#else
		//system("gpio u 0");
	//#endif
		websRedirect(wp, submitUrl);
	}
}

static void storageDiskPart(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *part1_vol, *part2_vol, *part3_vol;
	int max_part = atoi(websGetVar(wp, T("max_part"), T("")));
	FILE *fp = NULL;
	char part[30];

	if (NULL == (fp = fopen("/proc/mounts", "r")))
	{
		perror(__FUNCTION__);
		return;
	}
	while(EOF != fscanf(fp, "%s %*s %*s %*s %*s %*s\n", part))
	{
		if (NULL != strstr(part, "/dev/sd") || NULL != strstr(part, "/dev/mmc"))
			doSystem("umount -l %s", part);
	}
	fclose(fp);
	part1_vol = websGetVar(wp, T("part1_vol"), T(""));
	part2_vol = websGetVar(wp, T("part2_vol"), T(""));
	part3_vol = websGetVar(wp, T("part3_vol"), T(""));
	doSystem("storage.sh reparted %s %s %s %d", 
			  part1_vol, part2_vol, part3_vol, max_part);
	sleep(50);
	memset(part, 0, 30);
	do {
		sprintf(part, "/dev/sda%d", max_part+4);
		FormatPart(part, "");
		sleep(5);
	} while (max_part--);
	doSystem("storage.sh restart");
	sleep(5);
	websRedirect(wp, submitUrl);
}

#if defined CONFIG_USER_PROFTPD && (defined CONFIG_USB || defined CONFIG_MMC)
/* goform/storageFtpSrv */
static void storageFtpSrv(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *ftp, *name, *anonymous, *port, *max_sessions;
	char_t *adddir, *rename, *remove, *read, *write, *download, *upload;

	// fetch from web input
	ftp = websGetVar(wp, T("ftp_enabled"), T("0"));
	name = websGetVar(wp, T("ftp_name"), T("0"));
	anonymous = websGetVar(wp, T("ftp_anonymous"), T("0"));
	port = websGetVar(wp, T("ftp_port"), T(""));
	max_sessions = websGetVar(wp, T("ftp_max_sessions"), T(""));
	adddir = websGetVar(wp, T("ftp_adddir"), T("0"));
	rename = websGetVar(wp, T("ftp_rename"), T("0"));
	remove = websGetVar(wp, T("ftp_remove"), T("0"));
	read = websGetVar(wp, T("ftp_read"), T("0"));
	write = websGetVar(wp, T("ftp_write"), T("0"));
	download = websGetVar(wp, T("ftp_download"), T("0"));
	upload = websGetVar(wp, T("ftp_upload"), T("0"));

	// set to nvram
	nvram_bufset(RT2860_NVRAM, "FtpEnabled", ftp);
	nvram_bufset(RT2860_NVRAM, "FtpName", name);
	nvram_bufset(RT2860_NVRAM, "FtpAnonymous", anonymous);
	nvram_bufset(RT2860_NVRAM, "FtpPort", port);
	nvram_bufset(RT2860_NVRAM, "FtpMaxSessions", max_sessions);
	nvram_bufset(RT2860_NVRAM, "FtpAddDir", adddir);
	nvram_bufset(RT2860_NVRAM, "FtpRename", rename);
	nvram_bufset(RT2860_NVRAM, "FtpRemove", remove);
	nvram_bufset(RT2860_NVRAM, "FtpRead", read);
	nvram_bufset(RT2860_NVRAM, "FtpWrite", write);
	nvram_bufset(RT2860_NVRAM, "FtpDownload", download);
	nvram_bufset(RT2860_NVRAM, "FtpUpload", upload);
	nvram_commit(RT2860_NVRAM);

	// setup device
	doSystem("storage.sh ftp");

	websRedirect(wp, submitUrl);
}
#endif

#if defined CONFIG_USER_SAMBA && (defined CONFIG_USB || defined CONFIG_MMC)
/* goform/storageSmbSrv */
static void storageSmbSrv(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *submit;

	submit =  websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "delete"))
	{
		int index;
		FILE *fp;
		char smb_config_file[25];

		// strcpy(smb_config_file, "/var/.smb_config");
		sprintf(smb_config_file, "%s/.smb_config", first_part);
		fp = fopen(smb_config_file, "w");

		if (NULL == fp) {
			perror(__FUNCTION__);
			return;
		}

		index = atoi(websGetVar(wp, T("selectIndex"), T("")));
		// fprintf(stderr, "before set bitmap: %x%x%x\n", smb_conf.bitmap[2], smb_conf.bitmap[1], smb_conf.bitmap[0]);
		smb_conf.bitmap[index/32] &= ~(1<<(index%32));
		// fprintf(stderr, "after set bitmap: %x%x%x\n", smb_conf.bitmap[2], smb_conf.bitmap[1], smb_conf.bitmap[0]);
		smb_conf.count--;
		// fprintf(stderr, "smb dir count: %d\n", smb_conf.count);
		fwrite(&smb_conf, sizeof(struct smb_dir_config), 1, fp);
		fclose(fp);
		doSystem("killall -q nmbd");
		doSystem("killall -q smbd");
		SetSambaSrv();
		websRedirect(wp, submitUrl);
	} 
	else if (0 == strcmp(submit, "apply"))
	{
		char_t *smb_enable, *wg, *netbios;
		
		// fetch from web input
		smb_enable = websGetVar(wp, T("smb_enabled"), T(""));
		wg = websGetVar(wp, T("smb_workgroup"), T(""));
		netbios = websGetVar(wp, T("smb_netbios"), T(""));

		// set to nvram
		nvram_bufset(RT2860_NVRAM, "SmbEnabled", smb_enable);
		nvram_bufset(RT2860_NVRAM, "HostName", wg);
		nvram_bufset(RT2860_NVRAM, "SmbNetBIOS", netbios);
		nvram_commit(RT2860_NVRAM);

		// setup device
		SetSambaSrv();

		websRedirect(wp, submitUrl);
	}
	
}

/* goform/SmbDirAdd */
static void SmbDirAdd(webs_t wp, char_t *path, char_t *query)
{
	char_t *dir_name, *dir_path, *allow_users;
	FILE *fp;
	int exist, index = 0;
	char smb_config_file[25];

	// strcpy(smb_config, "/var/.smb_config");
	sprintf(smb_config_file, "%s/.smb_config", first_part);
	fp = fopen(smb_config_file, "w");

	if (NULL == fp) {
		perror(__FUNCTION__);
		return;
	}

	// fetch from web input
	dir_name = websGetVar(wp, T("dir_name"), T(""));
	dir_path = websGetVar(wp, T("dir_path"), T(""));
	allow_users = websGetVar(wp, T("allow_user"), T(""));
	/*
	DEBUG(dir_name);
	DEBUG(dir_path);
	DEBUG(allow_users);
	*/

	while(96 > index)
	{
		exist = DIREXIST(smb_conf.bitmap, index);
		if (exist && 0 == strcmp(dir_name, smb_conf.dir_list[index].name))
		{
				fprintf(stderr, "Existed Samba Shared Dir: %s\n", dir_name);
				fwrite(&smb_conf, sizeof(struct smb_dir_config), 1, fp);
				fclose(fp);
				return;
		}
		index++;
	}
	index = 0;
	// fprintf(stderr, "before set bitmap: %x%x%x\n", smb_conf.bitmap[2], smb_conf.bitmap[1], smb_conf.bitmap[0]);
	while(96 > index)
	{
		exist = DIREXIST(smb_conf.bitmap, index);
		if (!exist)
		{
			strcpy(smb_conf.dir_list[index].path, dir_path);
			strcpy(smb_conf.dir_list[index].name, dir_name);
			strcpy(smb_conf.dir_list[index].permit, allow_users);
			smb_conf.bitmap[index/32] |= 1<<(index%32);
			smb_conf.count++;
			break;
		}
		index++;
	}
	/*
	fprintf(stderr, "after set bitmap: %x%x%x\n", smb_conf.bitmap[2], smb_conf.bitmap[1], smb_conf.bitmap[0]);
	DEBUG(smb_conf.dir_list[index].path);
	DEBUG(smb_conf.dir_list[index].name);
	DEBUG(smb_conf.dir_list[index].permit);
	*/
	fwrite(&smb_conf, sizeof(struct smb_dir_config), 1, fp);
	fclose(fp);
	doSystem("killall -q nmbd");
	doSystem("killall -q smbd");
	SetSambaSrv();
}

/* goform/SmbDirEdit */
static void SmbDirEdit(webs_t wp, char_t *path, char_t *query)
{
	char_t *allow_user;
	int index = 0;
	FILE *fp;
	char smb_config_file[25];

	// strcpy(smb_config, "/var/.smb_config");
	sprintf(smb_config_file, "%s/.smb_config", first_part);
	fp = fopen(smb_config_file, "w");

	if (NULL == fp) {
		perror(__FUNCTION__);
		return;
	}

	// fetch from web input
	index = atoi(websGetVar(wp, T("hidden_index"), T("")));
	allow_user = websGetVar(wp, T("allow_user"), T(""));

	strcpy(smb_conf.dir_list[index].permit, allow_user);
	fwrite(&smb_conf, sizeof(struct smb_dir_config), 1, fp);
	fclose(fp);
}

static int fetchSmbConfig(void)
{
	FILE *fp = NULL;
	char smb_config_file[25];

	memset(&smb_conf, 0, sizeof(struct smb_dir_config));
	sprintf(smb_config_file, "%s/.smb_config", first_part);
	if (NULL == (fp = fopen(smb_config_file, "r")))
	{
		perror(__FUNCTION__);
		return -1;
	}
	fread(&smb_conf, sizeof(struct smb_dir_config), 1, fp);
	fclose(fp);

	return 0;
}

static int ShowSmbDir(int eid, webs_t wp, int argc, char_t **argv)
{
	int exist, i = 0, count = 0;

	//if (-1 == fetchSmbConfig())
	//{
	//	return 0;
	//}

	websWrite(wp, T("<tr align=center>"));
	websWrite(wp, T("<td>--</td>"));
	websWrite(wp, T("<td>Home</td>"));
	websWrite(wp, T("<td>%s</td>"), first_part);
	websWrite(wp, T("<td>admin</td>"));
	websWrite(wp, T("</tr>"));
	
	while(smb_conf.count > 0 && i < 96)
	{
		exist = DIREXIST(smb_conf.bitmap, i);
		if (exist)
		{
			websWrite(wp, T("<tr align=center>"));
			websWrite(wp, T("<td><input type=\"radio\" name=\"smb_dir\" value=\"%s\"></td>"), 
					  smb_conf.dir_list[i].name);
			websWrite(wp, T("<td>%s</td>"), smb_conf.dir_list[i].name);
			websWrite(wp, T("<input type=\"hidden\" name=\"smb_dir_path\" value=\"%s\">"), 
					  smb_conf.dir_list[i].path);
			websWrite(wp, T("<td>%s</td>"), smb_conf.dir_list[i].path);
			websWrite(wp, T("<input type=\"hidden\" name=\"smb_dir_permit\" value=\"%s\">"), 
					  smb_conf.dir_list[i].permit);
			if (0 == strlen(smb_conf.dir_list[i].permit))
				websWrite(wp, T("<td>%s</td>"), "<br />");
			else
				websWrite(wp, T("<td>%s</td>"), smb_conf.dir_list[i].permit);
			websWrite(wp, T("</tr>"));
			count++;
		}
		i++;
	}
	smb_conf.count = count;
	// fprintf(stderr, "smb_conf.count: %d\n", smb_conf.count);

	return 0;
}

static void SetSambaSrv()
{
	int i;
	const char *admin_id = nvram_bufget(RT2860_NVRAM, "Login");

	doSystem("storage.sh samba");
	if (1 == atoi(nvram_bufget(RT2860_NVRAM, "SmbEnabled")) &&
		strlen(first_part) > 7) {
		if (fetchSmbConfig() == 0) {
			for(i=0;i<smb_conf.count;i++)
			{
				doSystem("samba_add_dir.sh \"%s\" \"%s\" \"%s\" \"%s\"", 
						smb_conf.dir_list[i].name, 
						smb_conf.dir_list[i].path, 
						admin_id,
						smb_conf.dir_list[i].permit);
			}
		}
		doSystem("nmbd");
		doSystem("smbd");
	}
}
#endif
#define Tsize (1024.00*1024.00*1024.00)  
 #define Gsize (1024.00*1024.00)  
 #define Msize 1024.00

int chform_size_usb(unsigned long long ac_size,char *ch_size)
	{
	 
	   float c_size;
	   memset(ch_size,0,15);
	   if(ac_size>=Gsize)
			{
				c_size=ac_size/Gsize;
               sprintf(ch_size,"%.2f %c",c_size,'G');	
			   return 0;
				
		    }
			else if(ac_size>=Msize)
				{
				   c_size=ac_size/Msize;
				   sprintf(ch_size,"%.2f %c",c_size,'M');
				   return 0;
				}
				else 
					{
					  
					   strcpy(ch_size,"< 1M");
					   return 0;
					   
				    }
					
					
		}
static void getusbInfo(int eid, webs_t wp, int argc, char_t **argv)
{	
	FILE *fp;
	char usbbuf_info[128]={0};
	if(strlen(first_part)==0)
		{
		websWrite(wp, T("%s"),"0?0?0?0?0?");
		return;
		}
	fp=fopen("/var/usbinfo","r");
	fgets(usbbuf_info,sizeof(usbbuf_info),fp);
	websWrite(wp, T("%s"),usbbuf_info);
	return;
}
#if defined CONFIG_USB && defined CONFIG_USER_MINIDLNA
static struct dlna_config dlna_conf[4];
#define DLNA_CONFIG_FILE "/etc_ro/minidlna.conf" 
static void DLNA_mini(webs_t wp, char_t *path, char_t *query)
{	
	int i;
	char_t *DLNAEnabled,*DLNAPort,*friend_name,*rescan_h;
	char_t tmpbuf[256]={0};
	int fh;
	doSystem("killall minidlna;rm -f /etc_ro/minidlna.conf");
 	DLNAEnabled=websGetVar(wp, T("DLNAEnabled"), T(""));
	if(gatoi(DLNAEnabled))
		{
		
		DLNAPort=websGetVar(wp, T("DLNAPort"), T(""));
		friend_name=websGetVar(wp, T("friend_name"), T(""));
		rescan_h=websGetVar(wp, T("rescan_h"), T(""));
		nvram_bufset(RT2860_NVRAM, "DlnaPort", DLNAPort);
		nvram_bufset(RT2860_NVRAM, "DlnaName", friend_name);
		nvram_bufset(RT2860_NVRAM, "DlnaScan",rescan_h);
		
		fh = open(DLNA_CONFIG_FILE, O_RDWR|O_CREAT|O_TRUNC);
		sprintf(tmpbuf,"port=%s\nenable_tivo=no\nstrict_dlna=no\nnetwork_interface=br0\n",DLNAPort); 
		write(fh, tmpbuf, strlen(tmpbuf));
		sprintf(tmpbuf,"presentation_url=http://%s:%s/usb/minidlna.asp\n",nvram_bufget(RT2860_NVRAM,"lan_ipaddr"),"80");
		write(fh, tmpbuf, strlen(tmpbuf));
		sprintf(tmpbuf,"db_dir=%s/.db\n",first_part);
		write(fh, tmpbuf, strlen(tmpbuf));
		sprintf(tmpbuf,"friendly_name=%s\n",friend_name);
		write(fh, tmpbuf, strlen(tmpbuf));
		strcpy(tmpbuf,"album_art_names=Cover.jpg/cover.jpg/AlbumArtSmall.jpg/albumartsmall.jpg/AlbumArt.jpg/albumart.jpg/Album.jpg/album.jpg/Folder.jpg/folder.jpg/Thumb.jpg/thumb.jpg\n");
		write(fh, tmpbuf, strlen(tmpbuf));
		strcpy(tmpbuf,"inotify=no\nnotify_interval=900\nserial=12345678\nmodel_number=1\n");
		write(fh, tmpbuf, strlen(tmpbuf));
		if (fetchdlnaConfig() == 0) {
			for(i=0;i<4;i++)
			{
			if(strlen(dlna_conf[i].path)){
				memset(tmpbuf,'\0',sizeof(tmpbuf));
				sprintf(tmpbuf,"media_dir=%s\n",dlna_conf[i].path);
				write(fh,tmpbuf,strlen(tmpbuf));}
			}
		}
		close(fh);
		if(gatoi(rescan_h))
		doSystem("minidlna -f /etc_ro/minidlna.conf -R");
		else
		doSystem("minidlna -f /etc_ro/minidlna.conf");
	}
	
	nvram_bufset(RT2860_NVRAM, "DlnaEnabled", DLNAEnabled);
	nvram_commit(RT2860_NVRAM);
	websRedirect(wp, "/usb/minidlna.asp");
	//return 0;
	 
}
Dlna_Setup()
{	int fh,i;
	char tmpbuf[256];
	fh = open(DLNA_CONFIG_FILE, O_RDWR|O_CREAT|O_TRUNC);
			sprintf(tmpbuf,"port=%s\nenable_tivo=no\nstrict_dlna=no\nnetwork_interface=br0\n",nvram_bufget(RT2860_NVRAM,"DlnaPort")); 
			write(fh, tmpbuf, strlen(tmpbuf));
			sprintf(tmpbuf,"presentation_url=http://%s:%s/usb/minidlna.asp\n",nvram_bufget(RT2860_NVRAM,"lan_ipaddr"),"80");
			write(fh, tmpbuf, strlen(tmpbuf));
			sprintf(tmpbuf,"db_dir=%s/.db\n",first_part);
			write(fh, tmpbuf, strlen(tmpbuf));
			sprintf(tmpbuf,"friendly_name=%s\n",nvram_bufget(RT2860_NVRAM,"DlnaName"));
			write(fh, tmpbuf, strlen(tmpbuf));
			strcpy(tmpbuf,"album_art_names=Cover.jpg/cover.jpg/AlbumArtSmall.jpg/albumartsmall.jpg/AlbumArt.jpg/albumart.jpg/Album.jpg/album.jpg/Folder.jpg/folder.jpg/Thumb.jpg/thumb.jpg\n");
			write(fh, tmpbuf, strlen(tmpbuf));
			strcpy(tmpbuf,"inotify=no\nnotify_interval=900\nserial=12345678\nmodel_number=1\n");
			write(fh, tmpbuf, strlen(tmpbuf));
			
				for(i=0;i<4;i++)
				{
				if(strlen(dlna_conf[i].path)){
					memset(tmpbuf,'\0',sizeof(tmpbuf));
					sprintf(tmpbuf,"media_dir=%s\n",dlna_conf[i].path);
					write(fh,tmpbuf,strlen(tmpbuf));}
				}
			
			close(fh);
			
			if(gatoi(nvram_bufget(RT2860_NVRAM,"DlnaScan")))
			doSystem("minidlna -f /etc_ro/minidlna.conf -R");
			else
			doSystem("minidlna -f /etc_ro/minidlna.conf");
		}




void sort_dlna()
	{
//	struct dlna_config tmp;
	int i,j;
	for(i=0;i<4;i++){
		if(strlen(dlna_conf[i].path)==0){
			for(j=i+1;j<4;j++)
				if(strlen(dlna_conf[j].path)!=0)
					{
					strncpy(dlna_conf[i].path,dlna_conf[j].path,sizeof(dlna_conf[i].path));
					memset(dlna_conf[j].path,0,sizeof(dlna_conf[j].path));
					}
			}
		}
	}

static void DelDir(webs_t wp, char_t *path, char_t *query)
{
			int index,i;
			FILE *fp;
			char *en,dlna_config_file[25];
			char dir_all[10];
			sprintf(dlna_config_file, "%s/.dlna_config", first_part);
			fp = fopen(dlna_config_file, "w");
	
			if (NULL == fp) {
				perror(__FUNCTION__);
				return;
			}
			sort_dlna();
			for(i=0;i<4;i++){
			sprintf(dir_all,"del_en_%d",i);
			en=websGetVar(wp, T(dir_all), T(""));
			if(!strcmp(en,"on")){
				
				memset(&dlna_conf[i].path, 0, sizeof(dlna_conf[i].path));	
				
				}
				}
			fwrite(dlna_conf, sizeof(dlna_conf), 1, fp);
			fclose(fp);
			websRedirect(wp, "/usb/minidlna.asp");

}
static void AddDir(webs_t wp, char_t *path, char_t *query)
{
	char_t *dir_path,*media_type;
	FILE *fp;
	char dlna_config_file[25];
	int index = 0;
	char type;
	char dir_name[128];
	sprintf(dlna_config_file, "%s/.dlna_config", first_part);
	fp = fopen(dlna_config_file, "w");

	if (NULL == fp) {
		perror(__FUNCTION__);
		return;
	}

	// fetch from web input
	dir_path = websGetVar(wp, T("direct"), T(""));
	media_type = websGetVar(wp, T("media_type"), T(""));
	//printf("###%s\n",media_type);
	if(!strncmp(media_type,"all",3))
		{
		sprintf(dir_name,"%s",dir_path);
		}
		else {
			if(!strncmp(media_type,"audio",5))
				type='A';
			else if(!strncmp(media_type,"video",5))
				type='V';
			else
				type='P';
			sprintf(dir_name,"%c,%s",type,dir_path);
			}	

	while (4 > index)
	{
		if (0 == strlen(dlna_conf[index].path))
		{
			strcpy(dlna_conf[index].path, dir_name);
			break;
		}
		index++;
	}
	if (index == 5)
	{
		perror("Media Server Shared Dirs exceed 4");
		fclose(fp);
		return;
	}
	fwrite(dlna_conf, sizeof(dlna_conf), 1, fp);
	fclose(fp);
	websRedirect(wp, "/usb/minidlna.asp");
}


static int ShowDlnaDir(int eid, webs_t wp, int argc, char_t **argv)

{
	
	char dir_name[30];
	int dir_len = 0;
	
		if (NULL == first_part) {
			perror(__FUNCTION__);
			return -1;
		}
			dir_count = 0;
			DIR *dp;
			struct dirent *dirp;
			struct stat statbuf;
			if (NULL == (dp = opendir(first_part)))
			{
				fprintf(stderr, "open %s error\n", first_part);
				return -1;
			}
			chdir(first_part);
			while(NULL != (dirp = readdir(dp)))
			{
				lstat(dirp->d_name, &statbuf);
				if(S_ISDIR(statbuf.st_mode))
				{
					if (0 == strncmp(dirp->d_name, ".", 1) ||
						0 == strcmp(dirp->d_name, "home"))
						continue;
					strcpy(dir_name, dirp->d_name);
					dir_len = strlen(dir_name);
					if (dir_len < 30 && dir_len > 0)
					{
						websWrite(wp, T("<tr><td><script>function SelectPath_%d(){"),dir_count );
						websWrite(wp, T("parent.opener.self.document.AddDir.direct.value=\"%s/%s\";self.close();}</script>"), first_part, dir_name);
						websWrite(wp, T("<a href=\"#\" id=\"underline\" onclick=\"SelectPath_%d();\">%s/%s</a></td></tr>"),dir_count, first_part,dir_name);
						dir_count++;
					}
				}
			}
			chdir("/");
			closedir(dp);	
		return 0;

}

static int Dlna_Dir(int eid, webs_t wp, int argc, char_t **argv)
{
	char dir[128]={0};
	char dlna_dir[512]={0};
	//char dlna_config_file[25];
	int index=0;
	for(;index<4;index++)
		{
		if(strlen(dlna_conf[index].path))
			{
			
			sprintf(dir,"%s;",dlna_conf[index].path);
			strcat(dlna_dir,dir);
			}

		}
//	printf("%s\n",dlna_dir);
	websWrite(wp,"%s",dlna_dir);
	return 0;
}

static int fetchdlnaConfig(void)
{
	FILE *fp = NULL;
	char dlna_config_file[25];

	memset(dlna_conf, 0, sizeof(dlna_conf));
	sprintf(dlna_config_file, "%s/.dlna_config", first_part);
	if (NULL == (fp = fopen(dlna_config_file, "r")))
	{
		perror(__FUNCTION__);
		return;
	}
	fread(dlna_conf, sizeof(dlna_conf), 1, fp);
	fclose(fp);
}
#endif

#if defined CONFIG_USB && defined CONFIG_USER_USHARE
static struct media_config media_conf[4];
static void storageMediaSrv(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *submit;

	submit =  websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "delete"))
	{
		int index;
		FILE *fp;
		char media_config_file[25];

		// strcpy(smb_config, "/var/.smb_config");
		sprintf(media_config_file, "%s/.media_config", first_part);
		fp = fopen(media_config_file, "w");

		if (NULL == fp) {
			perror(__FUNCTION__);
			return;
		}

		index = atoi(websGetVar(wp, T("media_dir"), T("")));
		memset(&media_conf[index].path, 0, sizeof(media_conf[index].path));	
		fwrite(media_conf, sizeof(media_conf), 1, fp);
		fclose(fp);
		websRedirect(wp, submitUrl);
	} 
	else if (0 == strcmp(submit, "apply"))
	{
		char_t *media_enabled, *media_name;
		int i;
		
		// fetch from web input
		media_enabled = websGetVar(wp, T("media_enabled"), T(""));
		media_name = websGetVar(wp, T("media_name"), T(""));

		// set to nvram
		nvram_bufset(RT2860_NVRAM, "mediaSrvEnabled", media_enabled);
		nvram_bufset(RT2860_NVRAM, "mediaSrvName", media_name);
		nvram_commit(RT2860_NVRAM);

		// setup device
		if (0 == strcmp(media_enabled, "0"))
			memset(media_conf, 0, sizeof(media_conf));
		RunMediaSrv();

		websRedirect(wp, submitUrl);
	}
}

static void MediaDirAdd(webs_t wp, char_t *path, char_t *query)
{
	char_t *dir_path;
	FILE *fp;
	char media_config_file[25];
	int index = 0;

	sprintf(media_config_file, "%s/.media_config", first_part);
	fp = fopen(media_config_file, "w");

	if (NULL == fp) {
		perror(__FUNCTION__);
		return;
	}

	// fetch from web input
	dir_path = websGetVar(wp, T("dir_path"), T(""));
	/*
	DEBUG(dir_path);
	*/
	while (4 > index)
	{
		if (0 == strcmp(dir_path, media_conf[index].path))
		{
			fprintf(stderr, "Existed Media Shared Dir: %s\n", dir_path);
			fwrite(media_conf, sizeof(media_conf), 1, fp);
			fclose(fp);
			return;
		}
		index++;
	}
	index = 0;
	while (4 > index)
	{
		if (0 == strlen(media_conf[index].path))
		{
			strcpy(media_conf[index].path, dir_path);
			break;
		}
		index++;
	}
	if (index == 5)
	{
		perror("Media Server Shared Dirs exceed 4");
		fclose(fp);
		return;
	}
	fwrite(media_conf, sizeof(media_conf), 1, fp);
	fclose(fp);
}

static int ShowMediaDir(int eid, webs_t wp, int argc, char_t **argv)
{
	int index;

	fetchMediaConfig();
	media_dir_count = 0;
	for(index=0;index<4;index++)
	{
		if (0 != strlen(media_conf[index].path)) 
		{
			websWrite(wp, T("<tr align=\"center\">"));
			websWrite(wp, T("<td><input type=\"radio\" name=\"media_dir\" value=\"%d\"></td>"), 
					  index);
			websWrite(wp, T("<td>%s</td>"), media_conf[index].path);
			websWrite(wp, T("</tr>"));
			media_dir_count++;
		}
	}

	return 0;
}

static void fetchMediaConfig(void)
{
	FILE *fp = NULL;
	char media_config_file[25];

	memset(media_conf, 0, sizeof(media_conf));
	sprintf(media_config_file, "%s/.media_config", first_part);
	if (NULL == (fp = fopen(media_config_file, "r")))
	{
		perror(__FUNCTION__);
		return;
	}
	fread(media_conf, sizeof(media_conf), 1, fp);
	fclose(fp);
}

static void RunMediaSrv()
{
	char mediasrv_dir[160];
	int i;

	memset(mediasrv_dir, 0, sizeof(mediasrv_dir));
	for(i=0;i<4;i++)
		if (0 != strlen(media_conf[i].path))
			sprintf(mediasrv_dir, "%s %s", mediasrv_dir, media_conf[i].path);

	doSystem("storage.sh media \"%s\"", mediasrv_dir);
}
#endif

#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
/* goform/webcamra */
static void webcamra(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *enable, *resolution, *fps, *port;

	// fetch from web input
	enable = websGetVar(wp, T("enabled"), T(""));
	resolution = websGetVar(wp, T("resolution"), T(""));
	fps = websGetVar(wp, T("fps"), T(""));
	port = websGetVar(wp, T("port"), T(""));

	// set to nvram
	nvram_bufset(RT2860_NVRAM, "WebCamEnabled", enable);
	nvram_bufset(RT2860_NVRAM, "WebCamResolution", resolution);
	nvram_bufset(RT2860_NVRAM, "WebCamFPS", fps);
	nvram_bufset(RT2860_NVRAM, "WebCamPort", port);
	nvram_commit(RT2860_NVRAM);

	// setup device
	doSystem("killall uvc_stream");
	if (0 == strcmp(enable, "1"))
	{
		doSystem("uvc_stream -r %s -f %s -p %s -b", resolution, fps, port);
	}

	websRedirect(wp, submitUrl);
}
#endif

#if defined CONFIG_USB && defined CONFIG_USER_P910ND
static void printersrv(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *enable;

	// fetch from web input
	enable = websGetVar(wp, T("enabled"), T(""));
	// set to nvram
	nvram_bufset(RT2860_NVRAM, "PrinterSrvEnabled", enable);
	nvram_commit(RT2860_NVRAM);

	// setup device
	doSystem("killall p910nd");
	if (0 == strcmp(enable, "1"))
	{
		doSystem("p910nd -b -f /dev/lp0");
	}

	websRedirect(wp, submitUrl);
}
#endif

int initUSB(void)
{
	printf("\n##### Hotplug device init #####\n");
	sleep(1);			// wait for sub-storage initiation
#if defined CONFIG_USER_STORAGE && (defined CONFIG_USB || defined CONFIG_MMC)
	doSystem("storage.sh admin");
#if defined CONFIG_USER_PROFTPD && (defined CONFIG_USB || defined CONFIG_MMC)
	doSystem("storage.sh ftp");
#endif
	setFirstPart();
#if defined CONFIG_USER_SAMBA && (defined CONFIG_USB || defined CONFIG_MMC)
	SetSambaSrv();
#endif
#endif
#if defined CONFIG_USB && defined CONFIG_USER_USHARE
	fetchMediaConfig();
	RunMediaSrv();
#endif
#if defined CONFIG_USB && defined CONFIG_USER_MINIDLNA
	printf("MINIDLNA init\n");
	const char *minidlna = nvram_bufget(RT2860_NVRAM, "DlnaEnabled");
	doSystem("killall minidlna");
	if ((0 == strcmp(minidlna, "1"))&&(strlen(first_part)!=0))
		{
	fetchdlnaConfig();
	Dlna_Setup();
		}
#endif
#if defined CONFIG_USB && defined CONFIG_USER_UVC_STREAM
	printf("UVC init\n");
	const char *webcamebl = nvram_bufget(RT2860_NVRAM, "WebCamEnabled");
	doSystem("killall uvc_stream");
	if (0 == strcmp(webcamebl, "1"))
	{
		printf("UVC start\n");
		doSystem("uvc_stream -r %s -f %s -p %s -b", 
				  nvram_bufget(RT2860_NVRAM, "WebCamResolution"), 
				  nvram_bufget(RT2860_NVRAM, "WebCamFPS"), 
				  nvram_bufget(RT2860_NVRAM, "WebCamPort"));
	}
#endif
#if defined CONFIG_USB && defined CONFIG_USER_P910ND
	printf("P910ND init\n");
	const char *printersrvebl = nvram_bufget(RT2860_NVRAM, "PrinterSrvEnabled");
	doSystem("killall p910nd");
	if (0 == strcmp(printersrvebl, "1"))
	{
		printf("P910ND start\n");
		doSystem("p910nd -b -f /dev/lp0");
	}
#endif
#if defined CONFIG_USB && defined CONFIG_USER_MTDAAPD 
	doSystem("killall mt-daapd; killall mDNSResponder");
	if (strcmp(nvram_bufget(RT2860_NVRAM, "iTunesEnable"), "1") == 0)
		doSystem("config-iTunes.sh \"%s\" \"%s\" \"%s\"", 
				 nvram_bufget(RT2860_NVRAM, "iTunesSrvName"),
				 nvram_bufget(RT2860_NVRAM, "Password"),
				 nvram_bufget(RT2860_NVRAM, "iTunesDir"));
#endif

	return 0;
}

static int getCount(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char count[3];

	if (2 > ejArgs(argc, argv, T("%d %s"), &type, &field)) 
	{
		return websWrite(wp, T("Insufficient args\n"));
	}

	if (0 == strcmp(field, "AllDir"))
	{
		sprintf(count, "%d", dir_count);
		// fprintf(stderr,"AllDir: %s\n", count);
	}
	else if (0 == strcmp(field, "AllPart"))
	{
		sprintf(count, "%d", part_count);
		// fprintf(stderr,"AllPart: %s\n", count);
	}
#if defined CONFIG_USER_SAMBA && (defined CONFIG_USB || defined CONFIG_MMC)
	else if (0 == strcmp(field, "AllSmbDir"))
	{
		sprintf(count, "%d", smb_conf.count);
		// fprintf(stderr,"AllSmbDir: %s\n", count);
	}
#endif
	else if (0 == strcmp(field, "AllMediaDir"))
	{
		sprintf(count, "%d", media_dir_count);
		// fprintf(stderr,"AllPart: %s\n", count);
	}

	if (1 == type) {
		if (!strcmp(count, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), count);
	}
	if (!strcmp(count, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, count);

	return 0;
}

#if defined CONFIG_USER_STORAGE && (defined CONFIG_USB || defined CONFIG_MMC)
static void setFirstPart(void)
{
	FILE *pp = popen("cat /proc/mounts | grep media", "r");
	char dir[13];
	int getPartitionFlag=0;

	memset(first_part, 0, sizeof(first_part));
	while(EOF != fscanf(pp, "%*s %s %*s %*s %*s %*s\n", dir))
	{
		if (!memcmp(dir, "/media/sda", 10))
		{
			fprintf(stderr, "get the first partition: %s\n", dir);
			getPartitionFlag=1;
			strcpy(first_part, dir);
			break;
		}

	}
	pclose(pp);
	if(getPartitionFlag)
	{
	//#ifdef CONFIG_LKTOS_PRIVATE_OEM_NAME_CDRKING
		//system("gpio c 13 0");
	//#else
		//system("gpio u 1");
	//#endif
	}
	else
	{
	//#ifdef CONFIG_LKTOS_PRIVATE_OEM_NAME_CDRKING
		//system("gpio c 13 1");
	//#else
		//system("gpio u 0");
	//#endif
	}
}
#endif

static int GetNthFree(char *entry)
{
	int index = 1;
	char feild[7];
	const char *user_name;

	feild[0] = '\0';
	do
	{
		sprintf(feild, "%s%d", entry, index);
		user_name = (const char *) nvram_bufget(RT2860_NVRAM, feild);
		if (strlen(user_name) == 0)
			return index;
		index++;
	} while (index <= 10);

	return 0;
}

static int ShowAllDir(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp_mount = fopen(MOUNT_INFO, "r");
	char part[30], path[50];
	char dir_name[30];
	int dir_len = 0;

	if (NULL == fp_mount) {
        perror(__FUNCTION__);
		return -1;
	}

	dir_count = 0;

	while(EOF != fscanf(fp_mount, "%s %s %*s %*s %*s %*s\n", part, path))
	{
		DIR *dp;
		struct dirent *dirp;
		struct stat statbuf;

		// if (strncmp(path, "/var", 4) != 0)
		if (0 != strncmp(path, "/media/sd", 9) && 0 != strncmp(path, "/media/mmc", 10))
		{
			continue;
		}
		
		if (NULL == (dp = opendir(path)))
		{
			fprintf(stderr, "open %s error\n", path);
			return -1;
		}
		chdir(path);
		while(NULL != (dirp = readdir(dp)))
		{
			lstat(dirp->d_name, &statbuf);
			if(S_ISDIR(statbuf.st_mode))
			{
				if (0 == strncmp(dirp->d_name, ".", 1) ||
					0 == strcmp(dirp->d_name, "home"))
					continue;
				strcpy(dir_name, dirp->d_name);
				dir_len = strlen(dir_name);
				if (dir_len < 30 && dir_len > 0)
				{
					websWrite(wp, T("<tr align=center><td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>"), path, dir_name);
					websWrite(wp, T("<td>%s/%s</td>"), path, dir_name);
					websWrite(wp, T("<input type=\"hidden\" name=\"dir_part\" value=\"%s\">"), part);
					websWrite(wp, T("<td>%s</td>"), part);
					websWrite(wp, T("</tr>"));
					dir_count++;
				}
			}
		}
		chdir("/");
		closedir(dp);
	}
	fclose(fp_mount);
	// fprintf(stderr, "dir_count: %d\n", dir_count);

	return 0;
}

static void List( char *path, int indent )//indent为输出时的缩进
{ 
     struct dirent* ent = NULL; 
     DIR     *pDir;
     char    dir[512];
     struct stat    statbuf;  
     if(  (pDir=opendir(path))==NULL  ) 
     {
         fprintf( stderr, "Cannot open directory:%s\n", path );
         return;
     }
     while(  (ent=readdir(pDir))!=NULL  )
     {
         //得到读取文件的绝对路径名
         snprintf( dir, 512,"%s/%s", path, ent->d_name );   
         //得到文件信息
         lstat(dir, &statbuf);
         //判断是目录还是文件<img src="/graphics/dir.gif
         if( S_ISDIR(statbuf.st_mode) )
         {
             //排除当前目录和上级目录
             if(strcmp( ".",ent->d_name) == 0 || 
                     strcmp( "..",ent->d_name) == 0)  continue;
             //如果是子目录,递归调用函数本身,实现子目录中文件遍历
             printf( "%*sDIR:%s/\n", indent, "", ent->d_name );
            
             //递归调用,遍历子目录中文件
             List( dir, indent+4 );               
         }
         else
         {
             printf( "%*sfile:%s\n", indent, "", ent->d_name );
         }       
     }//while
     closedir(pDir);
 }




static void listAllTxtXml( char *path, int flag,char *lanIp )//flag为判断生成XML文件时新生成还是追加
{ 
     struct dirent* ent = NULL; 
     DIR     *pDir;
     char    dir[512];
     char    fileHttpPath[512]={0};
     struct stat    statbuf;
     char *pTmp;
     FILE *xmlfp;
     if(  (pDir=opendir(path))==NULL  ) 
     {
         fprintf( stderr, "Cannot open directory:%s\n", path );
         return;
     }
     //if(flag == 0 || flag == 1)
     //{
       
     //}
     while(  (ent=readdir(pDir))!=NULL  )
     {
         //得到读取文件的绝对路径名
         snprintf( dir, 512,"%s/%s", path, ent->d_name );   
         //得到文件信息
         lstat(dir, &statbuf);
         //判断是目录还是文件<img src="/graphics/dir.gif
         if( S_ISDIR(statbuf.st_mode) )
         {
             //排除当前目录和上级目录
             if(strcmp( ".",ent->d_name) == 0 || 
                     strcmp( "..",ent->d_name) == 0)  continue;
             //如果是子目录,递归调用函数本身,实现子目录中文件遍历
             //printf( "%*sDIR:%s/\n", indent, "", ent->d_name );
            
             //递归调用,遍历子目录中文件
             listAllTxtXml(dir,flag+2,lanIp);               
         }
         else
         {
	     pTmp=strchr(ent->d_name,'.');
             if(pTmp != NULL)
             {
               if(!strcasecmp(pTmp+1,"txt") || !strcasecmp(pTmp+1,"doc") || !strcasecmp(pTmp+1,"docx") || !strcasecmp(pTmp+1,"ppt") || !strcasecmp(pTmp+1,"pptx") || !strcasecmp(pTmp+1,"pps") || !strcasecmp(pTmp+1,"xls") || !strcasecmp(pTmp+1,"xlsx") || !strcasecmp(pTmp+1,"pdf"))
               {
                   //printf("houzui3=%s\n",pTmp+1);
		     xmlfp=fopen("/etc_ro/web/usb/alltxt.xml","a+");
                   if(NULL != xmlfp)
                  {
                     //printf("houzui5=%s\n",pTmp+1);
                     fputs("<file><name>",xmlfp);
                     fputs(ent->d_name,xmlfp);
                     fputs("</name><type>doc</type><url>",xmlfp);
                     sprintf(fileHttpPath,"http://%s/%s/%s",lanIp,strstr(path,"htdocs/")+7,ent->d_name);
                      fputs(fileHttpPath,xmlfp);
                      fputs("</url></file>",xmlfp);
			 fclose(xmlfp);
                  }
                }
              }
             //printf( "file:%s--str=%s\n", ent->d_name,pTmp);
         }       
     }//while
     closedir(pDir);
 }

static void listAllImgXml( char *path, int flag,char *lanIp )//flag为判断生成XML文件时新生成还是追加
{ 
     struct dirent* ent = NULL; 
     DIR     *pDir;
     char    dir[512];
     char    fileHttpPath[512]={0};
     struct stat    statbuf;
     char *pTmp;
     FILE *xmlfp;
     if(  (pDir=opendir(path))==NULL  ) 
     {
         fprintf( stderr, "Cannot open directory:%s\n", path );
         return;
     }
     //if(flag == 0 || flag == 1)
     //{
       
     //}
     while(  (ent=readdir(pDir))!=NULL  )
     {
         //得到读取文件的绝对路径名
         snprintf( dir, 512,"%s/%s", path, ent->d_name );   
         //得到文件信息
         lstat(dir, &statbuf);
         //判断是目录还是文件<img src="/graphics/dir.gif
         if( S_ISDIR(statbuf.st_mode) )
         {
             //排除当前目录和上级目录
             if(strcmp( ".",ent->d_name) == 0 || 
                     strcmp( "..",ent->d_name) == 0)  continue;
             //如果是子目录,递归调用函数本身,实现子目录中文件遍历
             //printf( "%*sDIR:%s/\n", indent, "", ent->d_name );
            
             //递归调用,遍历子目录中文件
             listAllImgXml(dir,flag+2,lanIp);               
         }
         else
         {
	     pTmp=strchr(ent->d_name,'.');
             if(pTmp != NULL)
             {
               if(!strcasecmp(pTmp+1,"jpg") || !strcasecmp(pTmp+1,"gif") || !strcasecmp(pTmp+1,"bmp") || !strcasecmp(pTmp+1,"png") )
               {
                   //printf("houzui3=%s\n",pTmp+1);
		     xmlfp=fopen("/etc_ro/web/usb/allimg.xml","a+");
                   if(NULL != xmlfp)
                  {
                     //printf("houzui5=%s\n",pTmp+1);
                     fputs("<file><name>",xmlfp);
                     fputs(ent->d_name,xmlfp);
                     fputs("</name><type>pic</type><url>",xmlfp);
                     sprintf(fileHttpPath,"http://%s/%s/%s",lanIp,strstr(path,"htdocs/")+7,ent->d_name);
                      fputs(fileHttpPath,xmlfp);
                      fputs("</url></file>",xmlfp);
			 fclose(xmlfp);
                  }
                }
              }
             //printf( "file:%s--str=%s\n", ent->d_name,pTmp);
         }       
     }//while
     closedir(pDir);
 }

static void listAllAudioXml( char *path, int flag,char *lanIp )//flag为判断生成XML文件时新生成还是追加
{ 
     struct dirent* ent = NULL; 
     DIR     *pDir;
     char    dir[512];
     char    fileHttpPath[512]={0};
     struct stat    statbuf;
     char *pTmp;
     FILE *xmlfp;
     if(  (pDir=opendir(path))==NULL  ) 
     {
         fprintf( stderr, "Cannot open directory:%s\n", path );
         return;
     }
     //if(flag == 0 || flag == 1)
     //{
       
     //}
     while(  (ent=readdir(pDir))!=NULL  )
     {
         //得到读取文件的绝对路径名
         snprintf( dir, 512,"%s/%s", path, ent->d_name );   
         //得到文件信息
         lstat(dir, &statbuf);
         //判断是目录还是文件<img src="/graphics/dir.gif
         if( S_ISDIR(statbuf.st_mode) )
         {
             //排除当前目录和上级目录
             if(strcmp( ".",ent->d_name) == 0 || 
                     strcmp( "..",ent->d_name) == 0)  continue;
             //如果是子目录,递归调用函数本身,实现子目录中文件遍历
             //printf( "%*sDIR:%s/\n", indent, "", ent->d_name );
            
             //递归调用,遍历子目录中文件
             listAllAudioXml(dir,flag+2,lanIp);               
         }
         else
         {
	     pTmp=strchr(ent->d_name,'.');
             if(pTmp != NULL)
             {
               if(!strcasecmp(pTmp+1,"mp3") || !strcasecmp(pTmp+1,"wma") || !strcasecmp(pTmp+1,"flac") || !strcasecmp(pTmp+1,"ogg") || !strcasecmp(pTmp+1,"aac") || !strcasecmp(pTmp+1,"wav") || !strcasecmp(pTmp+1,"m4a") || !strcasecmp(pTmp+1,"m4k") )
               {
                   //printf("houzui3=%s\n",pTmp+1);
		     xmlfp=fopen("/etc_ro/web/usb/allaudio.xml","a+");
                   if(NULL != xmlfp)
                  {
                     //printf("houzui5=%s\n",pTmp+1);
                     fputs("<file><name>",xmlfp);
                     fputs(ent->d_name,xmlfp);
                     fputs("</name><type>audio</type><url>",xmlfp);
                     sprintf(fileHttpPath,"http://%s/%s/%s",lanIp,strstr(path,"htdocs/")+7,ent->d_name);
                      fputs(fileHttpPath,xmlfp);
                      fputs("</url></file>",xmlfp);
			 fclose(xmlfp);
                  }
                }
              }
             //printf( "file:%s--str=%s\n", ent->d_name,pTmp);
         }       
     }//while
     closedir(pDir);
 }

static void listAllVedioXml( char *path, int flag,char *lanIp )//flag为判断生成XML文件时新生成还是追加
{ 
     struct dirent* ent = NULL; 
     DIR     *pDir;
     char    dir[512];
     char    fileHttpPath[512]={0};
     struct stat    statbuf;
     char *pTmp;
     FILE *xmlfp;
     if(  (pDir=opendir(path))==NULL  ) 
     {
         fprintf( stderr, "Cannot open directory:%s\n", path );
         return;
     }
     //if(flag == 0 || flag == 1)
     //{
       
     //}
     while(  (ent=readdir(pDir))!=NULL  )
     {
         //得到读取文件的绝对路径名
         snprintf( dir, 512,"%s/%s", path, ent->d_name );   
         //得到文件信息
         lstat(dir, &statbuf);
         //判断是目录还是文件<img src="/graphics/dir.gif
         if( S_ISDIR(statbuf.st_mode) )
         {
             //排除当前目录和上级目录
             if(strcmp( ".",ent->d_name) == 0 || 
                     strcmp( "..",ent->d_name) == 0)  continue;
             //如果是子目录,递归调用函数本身,实现子目录中文件遍历
             //printf( "%*sDIR:%s/\n", indent, "", ent->d_name );
            
             //递归调用,遍历子目录中文件
             listAllVedioXml(dir,flag+2,lanIp);               
         }
         else
         {
	     pTmp=strchr(ent->d_name,'.');
             if(pTmp != NULL)
             {
               if(!strcasecmp(pTmp+1,"mp4") || !strcasecmp(pTmp+1,"mov") || !strcasecmp(pTmp+1,"wmv") || !strcasecmp(pTmp+1,"avi") || !strcasecmp(pTmp+1,"m4v")  || !strcasecmp(pTmp+1,"rm") || !strcasecmp(pTmp+1,"rmvb") || !strcasecmp(pTmp+1,"mpg") || !strcasecmp(pTmp+1,"3gp") || !strcasecmp(pTmp+1,"flv"))
               {
                   //printf("houzui3=%s\n",pTmp+1);
		     xmlfp=fopen("/etc_ro/web/usb/allvedio.xml","a+");
                   if(NULL != xmlfp)
                  {
                    // printf("houzui5=%s\n",pTmp+1);
                     fputs("<file><name>",xmlfp);
                     fputs(ent->d_name,xmlfp);
                     fputs("</name><type>vedio</type><url>",xmlfp);
                     sprintf(fileHttpPath,"http://%s/%s/%s",lanIp,strstr(path,"htdocs/")+7,ent->d_name);
                      fputs(fileHttpPath,xmlfp);
                      fputs("</url></file>",xmlfp);
			 fclose(xmlfp);
                  }
                }
              }
           //  printf( "file:%s--str=%s\n", ent->d_name,pTmp);
         }       
     }//while
     closedir(pDir);
 }

static int GetXml(int eid,webs_t wp,int argc,char_t **argv)
{
  FILE *fp_mount=fopen(MOUNT_INFO,"r");
  FILE *xmlfp=NULL;
  FILE *scsiFp = NULL;
  FILE *nvfp = NULL;
  char tfbuf[64]={0};
  char udiskbuf[64]={0};
  char scsiBuf[128]={0};
  char tmpBuf[128]={0};
  char checkbuf[32]={0};
  char webUdiskPath[64]={0};
  char webTfdiskPath[64]={0};
  char strBuf[50]={0};
  char *tmpStrP=NULL;
  char xmlTypeName[32]={0};
  char lanIpAddress[32]={0};
  char part[30]={0};
  char path[50]={0};
  char *nvramPtr=NULL;
	
  part_count = 0;
  
  if(NULL == fp_mount)
  {
    perror(__FUNCTION__);
    return -1;
  }

  nvfp = popen("nvram_get 2860 comcryptcheck", "r");
  if (nvfp==NULL)
  	return -1;
  if (NULL == fgets(checkbuf, sizeof(checkbuf),nvfp)) {
  	pclose(nvfp);
  	return -1;
  }
  pclose(nvfp);
  if(strncmp(checkbuf,"check=ok",8))
  		return -1;

  tmpStrP = websGetVar(wp,T("type"),T("txt"));
  strncpy(xmlTypeName,tmpStrP,31);

  while(EOF != fscanf(fp_mount,"%s %s %*s %*s %*s %*s\n",part,path))
  {
    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;

    if(0 != strncmp(path,"/media/sd",9))
    {
      continue;
    }
    if(NULL == (dp = opendir(path)))
    {
      fprintf(stderr,"open %s error\n",path);
      return -1; 
    }
    part_count++;

  if(part_count ==1)
  {
    	system("rm -f /tmp/apache/htdocs/udisk");
	sprintf(tmpBuf,"ln -sf %s /tmp/apache/htdocs/udisk",path);
	system(tmpBuf);
	strcpy(webUdiskPath,"/tmp/apache/htdocs/udisk");
    }
   if(part_count ==2)
   {
    	system("rm -f /tmp/apache/htdocs/tfdisk");
	sprintf(tmpBuf,"ln -sf %s /tmp/apache/htdocs/tfdisk",path);
	system(tmpBuf);
	strcpy(webTfdiskPath,"/tmp/apache/htdocs/tfdisk");
    }
   closedir(dp);	
  }
  fclose(fp_mount);
  strcpy(lanIpAddress,"192.168.1.1:8088");
  if(part_count == 1 || part_count == 2)
  {
      if(!strcmp(xmlTypeName,"txt"))
      {
	  xmlfp=fopen("/etc_ro/web/usb/alltxt.xml","w+");
         if(xmlfp != NULL)
         {
          	//websWrite(wp, T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
	   	fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>",xmlfp);
	   	//websWrite(wp, T("<fileslist>"));
         	fputs("<fileslist>",xmlfp);
          	fclose(xmlfp);
         }
	  listAllTxtXml(webUdiskPath,1,lanIpAddress);
	  if(part_count==2)
        	listAllTxtXml(webTfdiskPath,1,lanIpAddress);
     }
   else if(!strcmp(xmlTypeName,"img"))
   {
        xmlfp=fopen("/etc_ro/web/usb/allimg.xml","w+");
        if(xmlfp != NULL)
        {
	   fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>",xmlfp);
          fputs("<fileslist>",xmlfp);
          fclose(xmlfp);
        }
	 listAllImgXml(webUdiskPath,1,lanIpAddress);
	 if(part_count==2)
	{
       	 listAllImgXml(webTfdiskPath,1,lanIpAddress);
	}	
   }
   else if(!strcmp(xmlTypeName,"audio"))
   {
        xmlfp=fopen("/etc_ro/web/usb/allaudio.xml","w+");
        if(xmlfp != NULL)
        {
          fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>",xmlfp);
          fputs("<fileslist>",xmlfp);
          fclose(xmlfp);
        }
	listAllAudioXml(webUdiskPath,1,lanIpAddress);
	if(part_count==2)
	{
        	listAllAudioXml(webTfdiskPath,1,lanIpAddress);
	}	
   }
   else if(!strcmp(xmlTypeName,"vedio"))
   {
        xmlfp=fopen("/etc_ro/web/usb/allvedio.xml","w+");
        if(xmlfp != NULL)
        {
          fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>",xmlfp);
          fputs("<fileslist>",xmlfp);
          fclose(xmlfp);
        }
	listAllVedioXml(webUdiskPath,1,lanIpAddress);
	if(part_count==2)
	{
        	listAllVedioXml(webTfdiskPath,1,lanIpAddress);
	}	
   }
   else
   {
        xmlfp=fopen("/etc_ro/web/usb/alltxt.xml","w+");
        if(xmlfp != NULL)
        {
          fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>",xmlfp);
          fputs("<fileslist>",xmlfp);
          fclose(xmlfp);
        }
	listAllTxtXml(webUdiskPath,1,lanIpAddress);
	if(part_count==2)
	{
       	 listAllTxtXml(webTfdiskPath,1,lanIpAddress);
	} 	
   }

   if(!strcmp(xmlTypeName,"txt"))
  {
    xmlfp=fopen("/etc_ro/web/usb/alltxt.xml","a+");
    if(xmlfp != NULL)
    {
      fputs("</fileslist>",xmlfp);
      fclose(xmlfp);
     // websWrite(wp, T("</fileslist>"));
    }
   xmlfp=fopen("/etc_ro/web/usb/alltxt.xml","r");
    if(xmlfp != NULL)
    {
      while(NULL != fgets(tmpBuf,128,xmlfp))
      {
      	websWrite(wp, T("%s"),tmpBuf);
      }
      fclose(xmlfp);
    }
  }
  else if(!strcmp(xmlTypeName,"img"))
  {
    xmlfp=fopen("/etc_ro/web/usb/allimg.xml","a+");
    if(xmlfp != NULL)
    {
      fputs("</fileslist>",xmlfp);
      fclose(xmlfp);
    }
    xmlfp=fopen("/etc_ro/web/usb/allimg.xml","r");
    if(xmlfp != NULL)
    {
      while(NULL != fgets(tmpBuf,128,xmlfp))
      {
      	websWrite(wp, T("%s"),tmpBuf);
      }
      fclose(xmlfp);
    }
  }
  else if(!strcmp(xmlTypeName,"audio"))
  {
    xmlfp=fopen("/etc_ro/web/usb/allaudio.xml","a+");
    if(xmlfp != NULL)
    {
      fputs("</fileslist>",xmlfp);
      fclose(xmlfp);
    }
     xmlfp=fopen("/etc_ro/web/usb/allaudio.xml","r");
    if(xmlfp != NULL)
    {
      while(NULL != fgets(tmpBuf,128,xmlfp))
      {
      	websWrite(wp, T("%s"),tmpBuf);
      }
      fclose(xmlfp);
    }
  }
  else if(!strcmp(xmlTypeName,"vedio"))
  {
    xmlfp=fopen("/etc_ro/web/usb/allvedio.xml","a+");
    if(xmlfp != NULL)
    {
      fputs("</fileslist>",xmlfp);
      fclose(xmlfp);
    }
     xmlfp=fopen("/etc_ro/web/usb/allvedio.xml","r");
    if(xmlfp != NULL)
    {
      while(NULL != fgets(tmpBuf,128,xmlfp))
      {
      	websWrite(wp, T("%s"),tmpBuf);
      }
      fclose(xmlfp);
    }
  }
  else
  {
    xmlfp=fopen("/etc_ro/web/usb/alltxt.xml","a+");
    if(xmlfp != NULL)
    {
      fputs("</fileslist>",xmlfp);
      fclose(xmlfp);
    }
     xmlfp=fopen("/etc_ro/web/usb/alltxt.xml","r");
    if(xmlfp != NULL)
    {
      while(NULL != fgets(tmpBuf,128,xmlfp))
      {
      	websWrite(wp, T("%s"),tmpBuf);
      }
      fclose(xmlfp);
    }
  }
 }
  
return 0;
}




static int ShowAllDirFile1(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp_mount = fopen(MOUNT_INFO, "r");
	char tmpBuf[64]={0};
	char strBuf[50]={0};
	char part[30], path[50];
	char dir_name[30];
	int dir_len = 0;

	if (NULL == fp_mount) {
        perror(__FUNCTION__);
		return -1;
	}
	
	part_count = 0;
	dir_count = 0;

	while(EOF != fscanf(fp_mount, "%s %s %*s %*s %*s %*s\n", part, path))
	{
		DIR *dp;
		struct dirent *dirp;
		struct stat statbuf;

		// if (strncmp(path, "/var", 4) != 0)
		if (0 != strncmp(path, "/media/sd", 9))
		{
			continue;
		}
		if (NULL == (dp = opendir(path)))
		{
			fprintf(stderr, "open %s error\n", path);
			return -1;
		}
		part_count++;
		if(part_count>1)
		{
			closedir(dp);
			break;
		}
		strcpy(strBuf,strstr(path,"sd"));
		sprintf(tmpBuf,"ln -sf %s /etc_ro/web/usb/%s",path,strBuf);
		system(tmpBuf);
		chdir(path);
		while(NULL != (dirp = readdir(dp)))
		{
			lstat(dirp->d_name, &statbuf);
			if(S_ISDIR(statbuf.st_mode))
			{
				if (0 == strncmp(dirp->d_name, ".", 1) ||
					0 == strcmp(dirp->d_name, "home"))
					continue;
				strcpy(dir_name, dirp->d_name);
				dir_len = strlen(dir_name);
				if (dir_len < 30 && dir_len > 0)
				{
					websWrite(wp, T("<tr><td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>"), 
							  path, dir_name);
					websWrite(wp, T("<td><img src=/images/dir.gif><a href=\"showdir1.asp?dir=%s/%s\">%s/%s</a></td>"), path, dir_name,path, dir_name);
					websWrite(wp, T("<input type=\"hidden\" name=\"dir_part\" value=\"%s\">"), 
							  part);
					websWrite(wp, T("<td>%s</td>"), part);
					websWrite(wp, T("</tr>"));
					dir_count++;
				}
			}
			else
      {
      			 strcpy(dir_name, dirp->d_name);
						dir_len = strlen(dir_name);
      			 websWrite(wp, T("<tr><td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>"), 
							  path, dir_name);
					websWrite(wp, T("<td><img src=/images/text.gif><a href=\"%s/%s\">%s/%s</a></td>"),strBuf,dir_name, path, dir_name);
					websWrite(wp, T("<input type=\"hidden\" name=\"dir_part\" value=\"%s\">"), 
							  part);
					websWrite(wp, T("<td>%s</td>"), part);
					websWrite(wp, T("</tr>"));
             //printf( "%*sfile:%s\n", indent, "", ent->d_name );
      }     
		}
		chdir("/");
		closedir(dp);
	}
	fclose(fp_mount);
	// fprintf(stderr, "dir_count: %d\n", dir_count);

	return 0;
}

static int ShowAllDirFile2(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp_mount = fopen(MOUNT_INFO, "r");
	char part[30], path[50];
	char tmpBuf[64]={0};
	char strBuf[50]={0};
	char dir_name[30];
	int dir_len = 0;

	if (NULL == fp_mount) {
        perror(__FUNCTION__);
		return -1;
	}

	dir_count = 0;
	part_count = 0;
	while(EOF != fscanf(fp_mount, "%s %s %*s %*s %*s %*s\n", part, path))
	{
		DIR *dp;
		struct dirent *dirp;
		struct stat statbuf;

		// if (strncmp(path, "/var", 4) != 0)
		if (0 != strncmp(path, "/media/sd", 9))
		{
			continue;
		}
		if (NULL == (dp = opendir(path)))
		{
			fprintf(stderr, "open %s error\n", path);
			return -1;
		}
		part_count++;
		if(part_count == 1)
		{
			closedir(dp);
			continue;
		}
		strcpy(strBuf,strstr(path,"sd"));
		sprintf(tmpBuf,"ln -sf %s /etc_ro/web/usb/%s",path,strBuf);
		system(tmpBuf);
		chdir(path);
		chdir(path);
		while(NULL != (dirp = readdir(dp)))
		{
			lstat(dirp->d_name, &statbuf);
			if(S_ISDIR(statbuf.st_mode))
			{
				if (0 == strncmp(dirp->d_name, ".", 1) ||
					0 == strcmp(dirp->d_name, "home"))
					continue;
				strcpy(dir_name, dirp->d_name);
				dir_len = strlen(dir_name);
				if (dir_len < 30 && dir_len > 0)
				{
					websWrite(wp, T("<tr><td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>"), 
							  path, dir_name);
					websWrite(wp, T("<td><img src=/images/dir.gif><a href=\"showdir2.asp?dir=%s/%s\">%s/%s</a></td>"), path, dir_name,path, dir_name);
					websWrite(wp, T("<input type=\"hidden\" name=\"dir_part\" value=\"%s\">"), 
							  part);
					websWrite(wp, T("<td>%s</td>"), part);
					websWrite(wp, T("</tr>"));
					dir_count++;
				}
			}
			else
      {
      			 strcpy(dir_name, dirp->d_name);
						dir_len = strlen(dir_name);
      			 websWrite(wp, T("<tr><td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>"), 
							  path, dir_name);
					websWrite(wp, T("<td><img src=/images/text.gif><a href=\"%s/%s\">%s/%s</a></td>"),strBuf,dir_name, path, dir_name);
					websWrite(wp, T("<input type=\"hidden\" name=\"dir_part\" value=\"%s\">"), 
							  part);
					websWrite(wp, T("<td>%s</td>"), part);
					websWrite(wp, T("</tr>"));
             //printf( "%*sfile:%s\n", indent, "", ent->d_name );
      }     
		}
		chdir("/");
		closedir(dp);
	}
	fclose(fp_mount);
	// fprintf(stderr, "dir_count: %d\n", dir_count);

	return 0;
}


static int ShowDirFile1(int eid, webs_t wp, int argc, char_t **argv)
{
	char part[30], path[120];
	char dir_name[30];
	char tmpBuf[180]={0};
	char bufStr[128]={0};
	int dir_len = 0;
	char *dirName;
	DIR *dp;
	struct dirent *dirp;
	struct stat statbuf;
	part_count = 0;
	dir_count = 0;
	
	dirName = websGetVar(wp, T("dir"), T(""));
	strcpy(path,dirName);
	
	if (NULL == (dp = opendir(path)))
	{
		fprintf(stderr, "open %s error\n", path);
		return -1;
	}
	strcpy(bufStr,dirName+7);
	chdir(path);
	while(NULL != (dirp = readdir(dp)))
	{
		lstat(dirp->d_name, &statbuf);
		if(S_ISDIR(statbuf.st_mode))
		{
			if (0 == strncmp(dirp->d_name, ".", 1) ||
				0 == strcmp(dirp->d_name, "home"))
				continue;
			strcpy(dir_name, dirp->d_name);
			dir_len = strlen(dir_name);
			if (dir_len < 30 && dir_len > 0)
			{
				websWrite(wp, T("<tr><td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>"), 
							  path, dir_name);
				websWrite(wp, T("<td><img src=/images/dir.gif><a href=\"showdir1.asp?dir=%s/%s\">%s/%s</a></td>"), path, dir_name,path, dir_name);
				websWrite(wp, T("<input type=\"hidden\" name=\"dir_part\" value=\"%s\">"), 
							 part);
				websWrite(wp, T("<td>---</td>"));
				websWrite(wp, T("</tr>"));
				dir_count++;
			}
		}
		else
      		{
      			strcpy(dir_name, dirp->d_name);
			dir_len = strlen(dir_name);
      			websWrite(wp, T("<tr><td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>"), 
						path, dir_name);
			websWrite(wp, T("<td><img src=/images/text.gif><a href=\"%s/%s\">%s/%s</a></td>"), bufStr,dir_name,path, dir_name);
			websWrite(wp, T("<input type=\"hidden\" name=\"dir_part\" value=\"%s\">"), 
						part);
			websWrite(wp, T("<td>---</td>"));
			websWrite(wp, T("</tr>"));
             //printf( "%*sfile:%s\n", indent, "", ent->d_name );
     		 }     
	}
	chdir("/");
	closedir(dp);
	return 0;
}

static int ShowPartition(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp = fopen(MOUNT_INFO, "r");
	char part[30], path[50]; 
	if (NULL == fp) {
        perror(__FUNCTION__);
		return -1;
	}
	part_count = 0;

	while(EOF != fscanf(fp, "%s %s %*s %*s %*s %*s\n", part, path))
	{
		// if (strncmp(path, "/var", 4) != 0)
		if (0 != strncmp(path, "/media/sd", 9) && 0 != strncmp(path, "/media/mmc", 10))
		{
			continue;
		}
		websWrite(wp, T("<tr align=center>"));
		websWrite(wp, T("<td><input type=\"radio\" name=\"disk_part\" value=\"%s\"></td>"), path);
		websWrite(wp, T("<td>%s</td>"), part);
		websWrite(wp, T("<td>%s</td>"), path);
		websWrite(wp, T("</tr>"));
		part_count++;
	}
	fclose(fp);
	// fprintf(stderr, "part_count: %d\n", part_count);

	return 0;
}


static int ShowPartitionLuo(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp = fopen(MOUNT_INFO, "r");
	FILE *scsiFp = NULL;
	FILE *nvfp =NULL;
	char part[30], path[50]; 
	char tmpdirBuf[100]={0};
	char checkbuf[32]={0};
	char tfbuf[64]={0};
	char udiskbuf[64]={0};
	char scsiBuf[128]={0};
	if (NULL == fp) {
        perror(__FUNCTION__);
		return -1;
	}
	part_count = 0;
	
   	nvfp = popen("nvram_get 2860 comcryptcheck", "r");
	if (nvfp==NULL)
		return -1;
	if (NULL == fgets(checkbuf, sizeof(checkbuf),nvfp)) {
		pclose(nvfp);
		return -1;
	}
	//printf("\nrrr=%s\n",checkbuf);
	pclose(nvfp);
	if(strncmp(checkbuf,"check=ok",8))
		return -1;

	
	while(EOF != fscanf(fp, "%s %s %*s %*s %*s %*s\n", part, path))
	{
		// if (strncmp(path, "/var", 4) != 0)
		if(part_count==2)
			break;
		if (0 != strncmp(path, "/media/sd", 9))
		{
			continue;
		}
		
		if(part_count ==0)
			strcpy(udiskbuf,path);
		else if(part_count ==1)
			strcpy(tfbuf,path);
		else
		{
		}
		part_count++;
	}
	fclose(fp);
	// fprintf(stderr, "part_count: %d\n", part_count);
	if(udiskbuf[0] != 0)
	{
		system("rm -f /tmp/apache/htdocs/udisk");
		sprintf(tmpdirBuf,"ln -sf %s /tmp/apache/htdocs/udisk",udiskbuf);
		system(tmpdirBuf);
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<a href=\"http://192.168.1.1:8088/udisk/\"  target=\"_self\">"));
		websWrite(wp,T("<img src=\"images/luotao2_03.gif\" width=\"274\" height=\"412\" border=\"0\">"));
		websWrite(wp,T("</a></td>"));
		
	}
	else
	{
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<a>"));
		websWrite(wp,T("<img src=\"images/luotao2_03.gif\" width=\"274\" height=\"412\" border=\"0\">"));
		websWrite(wp,T("</a></td>"));
	}

	websWrite(wp, T("<td rowspan=\"2\"><img src=\"images/luotao2_04.gif\" width=\"194\" height=\"478\"></td>"));
	
	if(tfbuf[0] != 0)
	{
		system("rm -f /tmp/apache/htdocs/tfdisk");
		sprintf(tmpdirBuf,"ln -sf %s /tmp/apache/htdocs/tfdisk",tfbuf);
		system(tmpdirBuf);
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<a href=\"http://192.168.1.1:8088/tfdisk/\"  target=\"_self\">"));
		websWrite(wp,T("<img src=\"images/luotao2_05.gif\" width=\"274\" height=\"412\" border=\"0\">"));
		websWrite(wp,T("</a></td>"));
		
	}
	else
	{
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<a>"));
		websWrite(wp,T("<img src=\"images/luotao2_05.gif\" width=\"274\" height=\"412\" border=\"0\">"));
		websWrite(wp,T("</a></td>"));
	}
	return 0;
}


static int ShowPartitionLuoSingle(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp = fopen(MOUNT_INFO, "r");
	FILE *scsiFp = NULL;
	char part[30], path[50]; 
	char tmpdirBuf[100]={0};
	char udiskbuf[64]={0};
	if (NULL == fp) {
        perror(__FUNCTION__);
		return -1;
	}
	part_count = 0;

	while(EOF != fscanf(fp, "%s %s %*s %*s %*s %*s\n", part, path))
	{
		if(part_count==2)
			break;
		if (0 != strncmp(path, "/media/sd", 9))
		{
			continue;
		}
		strcpy(udiskbuf,path);
		part_count++;
	}
	fclose(fp);
	// fprintf(stderr, "part_count: %d\n", part_count);
	if(udiskbuf[0] != 0)
	{
		system("rm -f /tmp/apache/htdocs/WiDisk");
		sprintf(tmpdirBuf,"ln -sf %s /tmp/apache/htdocs/WiDisk",udiskbuf);
		system(tmpdirBuf);
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<a href=\"http://%s:8088/WiDisk/\"  target=\"_self\">"),nvram_bufget(RT2860_NVRAM,"lan_ipaddr"));
		websWrite(wp,T("<img src=\"images/luotao_03.jpg\" width=\"275\" height=\"408\" border=\"0\">"));
		websWrite(wp,T("</a></td>"));
		
	}
	else
	{
		websWrite(wp, T("<td>"));
		websWrite(wp, T("<a>"));
		websWrite(wp,T("<img src=\"images/luotao_03.jpg\" width=\"275\" height=\"408\" border=\"0\">"));
		websWrite(wp,T("</a></td>"));
	}

	
	return 0;
}

static int ShowLeftItem(int eid, webs_t wp, int argc, char_t **argv)
{
	char *strptr=NULL; 
	strptr=nvram_bufget(RT2860_NVRAM, "OperationMode");
	printf("\r\nddddd=%s----%c--%c",strptr,strptr[0],*strptr );
	if( !strcmp(strptr,"0"))
	{
		websWrite(wp, T("left2.asp"));	
	}
	else
	{
		websWrite(wp, T("left1.asp"));	
	}

	return 0;
}

static int getMaxVol(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *pp = popen("fdisk -l /dev/sda", "r");
	char maxvol[5], unit[5];
	unsigned int result;

	fscanf(pp, "%*s %*s %s %s %*s %*s\n", maxvol, unit);
	result = atoi(maxvol);
	if (NULL != strstr(unit, "GB,"))
		websWrite(wp, T("%d"), result*1000);
	else if (NULL != strstr(unit, "MB,"))
		websWrite(wp, T("%d"), result);
	pclose(pp);

	return 0;
}

static int isStorageExist(void)
{
	char buf[256];
	FILE *fp = fopen("/proc/mounts", "r");
	if(!fp){
		perror(__FUNCTION__);
		return 0;
	}

	while(fgets(buf, sizeof(buf), fp)){
		if(strstr(buf, USB_STORAGE_SIGN) || strstr(buf, SD_STORAGE_SIGN)){
			fclose(fp);
			return 1;
		}
	}

	fclose(fp);
	printf("no hotplug disk found\n.");
	return 0;
}

/*
 *  taken from "mkimage -l" with few modified....
 */
int checkIfFirmware(char *imagefile)
{
	unsigned int magic;
	FILE *fp = fopen(imagefile, "r");
	if(!fp){
		perror(imagefile);
		return 0;
	}
	if( fread(&magic, sizeof(magic), 1, fp) != 1){
		perror(imagefile);
		fclose(fp);
		return 0;
	}

	fclose(fp);

	if(magic == ntohl(IH_MAGIC))
		return 1;
	else
		return 0;
}


/*
 *  find the firmware recursively in usb disk.
 */
static void lookupFirmware(char *dir_path)
{
	struct dirent *dir;
	struct stat stat_buf;
	char whole_path[1024];

	DIR *d = opendir(dir_path);
	if(!d){
		perror(dir_path);
		return;			// give up to open the directory
	}

	while((dir = readdir(d))){
		if(!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
			continue;

		snprintf(whole_path, 1024, "%s/%s", dir_path, dir->d_name);
		if(stat( whole_path, &stat_buf) == -1){
			perror(dir_path);
			continue;
		}

		if( S_ISDIR( stat_buf.st_mode)){
			lookupFirmware(whole_path);	//recursive
			continue;
		}

		if(stat_buf.st_size < MIN_FIRMWARE_SIZE)
			continue;

		if(checkIfFirmware(whole_path)){
			if(!strlen(firmware_path))
				snprintf(firmware_path, sizeof(firmware_path), "%s", whole_path);
			else
				snprintf(firmware_path, sizeof(firmware_path), "%s\n%s", firmware_path, whole_path);
		}
	}
	closedir(d);
	return;
}

static int isSpaceEnough(char *part)
{
	// Write a big file to test the partition
	// TODO: I'm lazy to use fstatfs() to do the same thing.
	int count;
	char path[1024];
	snprintf(path, 1024, "%s/.delete_me", part);
	FILE *fp = fopen(path, "w");

	if(!fp){
        perror(__FUNCTION__);
		return 0;
	}
	for(count =0; count <MIN_SPACE_FOR_FIRMWARE; count += sizeof(path)){
		if( fwrite(path, sizeof(path), 1, fp) != 1){
			fclose(fp);
			unlink(path);
			sync();
			fprintf(stderr, __FILE__":no enough space left in %s.\n", part);
			return 0;
		}
	}
	fclose(fp);
	unlink(path);
	sync();
	return 1;
}

/*
 *  choose a suitable usb disk partition for the firmware upload.
 *  return NULL if not found.
 */
char *isStorageOK(void)
{
    char buf[256];
	char device_path[256];
	static char mount_path[256];
    FILE *fp = fopen(MOUNT_INFO, "r");
    if(!fp){
        perror(__FUNCTION__);
        return 0;
    }

    while(fgets(buf, sizeof(buf), fp)){
		sscanf(buf, "%s %s", device_path, mount_path);
		if(strstr(mount_path, USB_STORAGE_SIGN) || strstr(mount_path, SD_STORAGE_SIGN)){
			if(!isSpaceEnough(mount_path))
				continue;
			fclose(fp);
			fprintf(stderr, __FILE__":choose USB %s for firmrware space.\n", mount_path);
			return mount_path;
		}
    }

    fclose(fp);
	fprintf(stderr, "No suitable usb disk partition found \n.");
    return NULL;
}

void setFirmwarePath(void)
{
	firmware_path[0] = '\0';
	if(!isStorageExist())
		return;

	lookupFirmware(STORAGE_PATH);
	sync();
	printf("firmware found: %s\n", firmware_path);
}

static void storageGetFirmwarePath(webs_t wp, char_t *path, char_t *query)
{
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));

	websWrite(wp, "%s", firmware_path);
	websDone(wp, 200);
}

#if defined CONFIG_USER_MTDAAPD 
static void iTunesSrv(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl = websGetVar(wp, T("submit-url"), T(""));

	char_t *enable, *dir_path, *name;

	// fetch from web input
	enable = websGetVar(wp, T("enabled"), T("0"));
	dir_path = websGetVar(wp, T("dir_path"), T(""));
	name = websGetVar(wp, T("srv_name"), T(""));
	// set to nvram
	nvram_bufset(RT2860_NVRAM, "iTunesEnable", enable);
	nvram_bufset(RT2860_NVRAM, "iTunesDir", dir_path);
	nvram_bufset(RT2860_NVRAM, "iTunesSrvName", name);
	nvram_commit(RT2860_NVRAM);

	// setup device
	doSystem("killall mt-daapd; killall mDNSResponder");
	if (strcmp(enable, "1") == 0)
		doSystem("config-iTunes.sh \"%s\" \"%s\" \"%s\"", 
				 name,
				 nvram_bufget(RT2860_NVRAM, "Password"),
				 dir_path);

	websRedirect(wp, submitUrl);
}
#endif

/*
 *  Hotpluger signal handler
 */
void hotPluglerHandler(int signo)
{
#if defined CONFIG_RALINKAPP_MPLAYER
	cleanup_musiclist();
#endif
	initUSB();
}

