EXEC = mtd_write
OBJS = mtd.o
//CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)

CC :=mips-linux-uclibc-gcc

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

