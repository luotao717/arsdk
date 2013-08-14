NVDIR = ../../lib/libnvram
CFLAGS += -I$(NVDIR) -I../../linux/kernels/mips-linux-2.6.31/drivers/char
LDFLAGS += -L../../lib/libnvram
#LIBNVRAM = $(NVDIR)/libnvram-0.9.28.so
LIBNVRAM = $(NVDIR)/libnvram.a
LDLIBS += $(NVDIR)/libnvram.a
EXEC = nvram_daemon ralink_init upgrade ftpup reload ppp_daemon hcmme
#CONF_H	= $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
#UCONF_H	= $(ROOTDIR)/config/autoconf.h

CC :=mips-linux-uclibc-gcc

all: $(EXEC)

nvram_daemon: nvram_daemon.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ralink_init: ralink_init.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	
upgrade: upgrade.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ftpup: ftpup.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

reload: reload.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	
ppp_daemon: ppp_daemon.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

hcmme: hcmme.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

ralink_init.o: $(LIBNVRAM)

nvram_daemon.o: $(LIBNVRAM)
upgrade.o: $(LIBNVRAM)
ftpup.o: $(LIBNVRAM)
reload.o: $(LIBNVRAM)
ppp_daemon.o: $(LIBNVRAM)
hcmme.o: $(LIBNVRAM)

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
	
