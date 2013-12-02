NVDIR = ../../lib/libnvram
CFLAGS += -I$(NVDIR) -I../../linux/kernels/mips-linux-2.6.31/drivers/char -I../../include
LDFLAGS += -L../../lib/libnvram
#LIBNVRAM = $(NVDIR)/libnvram-0.9.28.so
LIBNVRAM = $(NVDIR)/libnvram.a
LDLIBS += $(NVDIR)/libnvram.a
EXEC = nvram_daemon ralink_init upgrade ftpup reload ppp_daemon hcmme
#CONF_H	= $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
#UCONF_H	= $(ROOTDIR)/config/autoconf.h

CC :=mips-linux-uclibc-gcc

vpath %.c ../shared/

all: $(EXEC)

nvram_daemon: nvram_daemon.c $(LIBNVRAM)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ralink_init: ralink_init.c $(LIBNVRAM)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	 
upgrade: upgrade.c $(LIBNVRAM)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ftpup: ftpup.c $(LIBNVRAM)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

reload: reload.c $(LIBNVRAM)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	
ppp_daemon: ppp_daemon.c $(LIBNVRAM)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

hcmme: hcmme.c $(LIBNVRAM) shutils.c upgrade_common.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o $(EXEC)

romfs:
	for i in $(EXEC) ; do $(ROMFSINST) /bin/$$i ; done
	ln -sf ralink_init nvram_get
	ln -sf ralink_init nvram_set
	mv -f nvram_get $(ROMFSDIR)/bin
	mv -f nvram_set $(ROMFSDIR)/bin
	cp -f nart.out $(ROMFSDIR)/bin
	cp -f art.ko $(ROMFSDIR)/bin
	
