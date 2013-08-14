#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>

struct mtd_info_user {
	u_char type;
	u_int32_t flags;
	u_int32_t size;
	u_int32_t erasesize;
	u_int32_t oobblock;
	u_int32_t oobsize;
	u_int32_t ecctype;
	u_int32_t eccsize;
};

struct erase_info_user {
	u_int32_t start;
	u_int32_t length;
};

#define MEMGETINFO	_IOR('M', 1, struct mtd_info_user)
#define MEMERASE	_IOW('M', 2, struct erase_info_user)
#define AR9331 1

int mtd_open(const char *name, int flags)
{
	FILE *fp;
	char dev[80];
	int i, ret;

	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, name)) {
				snprintf(dev, sizeof(dev), "/dev/mtd/%d", i);
				if ((ret = open(dev, flags)) < 0) {
					snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
					ret = open(dev, flags);
				}
				fclose(fp);
				return ret;
			}
		}
		fclose(fp);
	}
	return -1;
}

int flash_read_mac(char *buf)
{
	int fd, ret;

	if (!buf)
		return -1;
	fd = mtd_open("Factory", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
#if ! defined (NO_WIFI_SOC)
	lseek(fd, 0x2E, SEEK_SET);
#else
	lseek(fd, 0xE006, SEEK_SET);
#endif
	ret = read(fd, buf, 6);
	close(fd);
	return ret;
}

int flash_read_wlan_mac(char *buf) //by luotao for read wlan mac from flash
{
	int fd, ret;
	unsigned char bufftemp[200]={0};
	unsigned int tempi=0;
	if (!buf)
		return -1;
	fd = mtd_open("Factory", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
	//read(fd, bufftemp, 200);
	//printf("\r\neprombuf=");
	//for(tempi=0;tempi<200;tempi++)
	//{
		//printf("%02x-",bufftemp[tempi]);
	//}
	//printf("\r\n");
	lseek(fd, 0x04, SEEK_SET);
	ret = read(fd, buf, 6);
	close(fd);
	return ret;
}

int flash_read_all_eeprom_ar9331(char *buf) //by luotao for read wan mac from flash
{
	int fd, ret;
	unsigned char bufftemp[200]={0};
	unsigned int tempi=0;
	if (!buf)
		return -1;
	fd = mtd_open("ART-2411", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
	lseek(fd, 0x0, SEEK_SET);
	ret = read(fd, buf, 64*1024);
	close(fd);
	return ret;
}
int flash_read_size_eeprom_ar9331(char *buf,int size) //by luotao for read wan mac from flash
{
	int fd, ret;
	unsigned char bufftemp[200]={0};
	unsigned int tempi=0;
	if (!buf)
		return -1;
	fd = mtd_open("ART-2411", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
	lseek(fd, 0x0, SEEK_SET);
	ret = read(fd, buf, size);
	close(fd);
	return ret;
}
int flash_read_wlan_eeprom_ar9331(char *buf,int size) //by luotao for read wan mac from flash
{
	int fd, ret;
	unsigned char bufftemp[200]={0};
	unsigned int tempi=0;
	if (!buf)
		return -1;
	fd = mtd_open("ART-2411", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
	lseek(fd, 0x1000, SEEK_SET);
	ret = read(fd, buf, size);
	close(fd);
	return ret;
}
int flash_read_wlan_mac_ar9331(char *buf) //by luotao for read wlan mac from flash
{
	int fd, ret;
	unsigned char bufftemp[200]={0};
	unsigned int tempi=0;
	if (!buf)
		return -1;
	fd = mtd_open("ART-2411", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
	lseek(fd, 0x1002, SEEK_SET);
	ret = read(fd, buf, 6);
	close(fd);
	return ret;
}
int flash_read_wan_mac_ar9331(char *buf) //by luotao for read wan mac from flash
{
	int fd, ret;
	unsigned char bufftemp[200]={0};
	unsigned int tempi=0;
	if (!buf)
		return -1;
	fd = mtd_open("ART-2411", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
	lseek(fd, 0x0, SEEK_SET);
	ret = read(fd, buf, 6);
	close(fd);
	return ret;
}
int flash_read_lan_mac_ar9331(char *buf) //by luotao for read lan mac from flash
{
	int fd, ret;
	unsigned char bufftemp[200]={0};
	unsigned int tempi=0;
	if (!buf)
		return -1;
	fd = mtd_open("ART-2411", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
	lseek(fd, 0x06, SEEK_SET);
	ret = read(fd, buf, 6);
	close(fd);
	return ret;
}
int flash_write_wan_mac_ar9331(char *buf) //by luotao for read wan mac from flash
{
	int fd, ret;
	unsigned char bufftemp[200]={0};
	unsigned int tempi=0;
	flash_write_ar9331_eeprom( buf, 0, 0x1000);
	return 1;
}
int flash_write_all_mac_ar9331(char *buf) //by luotao for read wan mac from flash
{
	int fd, ret;
	unsigned char bufftemp[200]={0};
	unsigned int tempi=0;
	flash_write_ar9331_eeprom( buf, 0, 0x2000);
	return 1;
}
int flash_write_wlan_mac_ar9331(char *buf) //by luotao for read wan mac from flash
{
	int fd, ret;
	unsigned char bufftemp[200]={0};
	unsigned int tempi=0;
	flash_write_ar9331_eeprom( buf, 0x1000, 0x1000);
	return 1;
}
int flash_read_NicConf(char *buf)
{
	int fd, ret;

	if (!buf)
		return -1;
	fd = mtd_open("Factory", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}
	lseek(fd, 0x34, SEEK_SET);
	ret = read(fd, buf, 6);
	close(fd);
	return ret;
}

int flash_read(char *buf, off_t from, size_t len)
{
	int fd, ret;
	struct mtd_info_user info;

	fd = mtd_open("NVRAM", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes - %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	lseek(fd, from, SEEK_SET);
	ret = read(fd, buf, len);
	if (ret == -1) {
		fprintf(stderr, "Reading from mtd failed\n");
		close(fd);
		return -1;
	}

	close(fd);
	return ret;
}

#define min(x,y) ({ typeof(x) _x = (x); typeof(y) _y = (y); (void) (&_x == &_y); _x < _y ? _x : _y; })

int flash_write(char *buf, off_t to, size_t len)
{
	int fd, ret = 0;
	char *bak = NULL;
	struct mtd_info_user info;
	struct erase_info_user ei;

	fd = mtd_open("NVRAM", O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes: %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	while (len > 0) {
		if ((len & (info.erasesize-1)) || (len < info.erasesize)) {
			int piece_size;
			unsigned int piece, bakaddr;

			bak = (char *)malloc(info.erasesize);
			if (bak == NULL) {
				fprintf(stderr, "Not enough memory\n");
				close(fd);
				return -1;
			}

			bakaddr = to & ~(info.erasesize - 1);
			lseek(fd, bakaddr, SEEK_SET);

			ret = read(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Reading from mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			piece = to & (info.erasesize - 1);
			piece_size = min(len, info.erasesize - piece);
			memcpy(bak + piece, buf, piece_size);

			ei.start = bakaddr;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			lseek(fd, bakaddr, SEEK_SET);
			ret = write(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			free(bak);
			buf += piece_size;
			to += piece_size;
			len -= piece_size;
		}
		else {
			ei.start = to;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				return -1;
			}

			ret = write(fd, buf, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			buf += info.erasesize;
			to += info.erasesize;
			len -= info.erasesize;
		}
	}

	close(fd);
	return ret;
}

int flash_write_ar9331_eeprom(char *buf, off_t to, size_t len)
{
	int fd, ret = 0;
	char *bak = NULL;
	struct mtd_info_user info;
	struct erase_info_user ei;

	fd = mtd_open("ART-2411", O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes: %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	while (len > 0) {
		if ((len & (info.erasesize-1)) || (len < info.erasesize)) {
			int piece_size;
			unsigned int piece, bakaddr;

			bak = (char *)malloc(info.erasesize);
			if (bak == NULL) {
				fprintf(stderr, "Not enough memory\n");
				close(fd);
				return -1;
			}

			bakaddr = to & ~(info.erasesize - 1);
			lseek(fd, bakaddr, SEEK_SET);

			ret = read(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Reading from mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			piece = to & (info.erasesize - 1);
			piece_size = min(len, info.erasesize - piece);
			memcpy(bak + piece, buf, piece_size);

			ei.start = bakaddr;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			lseek(fd, bakaddr, SEEK_SET);
			ret = write(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			free(bak);
			buf += piece_size;
			to += piece_size;
			len -= piece_size;
		}
		else {
			ei.start = to;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				return -1;
			}

			ret = write(fd, buf, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			buf += info.erasesize;
			to += info.erasesize;
			len -= info.erasesize;
		}
	}

	close(fd);
	return ret;
}

