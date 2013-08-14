
EXEC = cvimg

CC=gcc

all: $(EXEC)


cvimg: cvimg.o

	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

cvimg.o: apmib.h

clean:
	rm -f *.o $(EXEC)

romfs:
	for i in $(EXEC) ; do $(ROMFSINST) /bin/$$i ; done
	

