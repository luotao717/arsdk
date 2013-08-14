NVDIR = ../../../lib/libnvram
CFLAGS += -I$(NVDIR) -I../../../include
LDFLAGS += -L../../../lib/libnvram
LIBNVRAM = ../../../lib/libnvram/libnvram.a
LDLIBS += $(NVDIR)/libnvram.a
EXEC = lktos_networkconfig lktos_wificonfig lktos_initconfig

CC=mips-linux-uclibc-gcc

all: $(EXEC)


lktos_networkconfig: lktNetworkConfig.o networkconfig_main.o

	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	
lktos_wificonfig: lktWifiConfig.o wificonfig_main.o

	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	
lktos_initconfig: lktInitConfig.o lktNetworkConfig.o lktWifiConfig.o initconfig_main.o

	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

lktNetworkConfig.o: $(LIBNVRAM)

networkconfig_main.o: $(LIBNVRAM)

lktWifiConfig.o: $(LIBNVRAM)

wificonfig_main.o: $(LIBNVRAM)

lktInitConfig.o: $(LIBNVRAM)

initconfig_main.o: $(LIBNVRAM)

clean:
	rm -f *.o $(EXEC)

romfs:
	for i in $(EXEC) ; do $(ROMFSINST) /bin/$$i ; done
	

