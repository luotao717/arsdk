SCRIPTS = $(notdir $(wildcard *.sh))

all:

clean:

romfs:
	sed -e '/^#/d' ${ROOTDIR}/${CONFIG_LINUXDIR}/.config > .config
	chmod 755 .config
	$(ROMFSINST) .config /sbin/config.sh
	for i in $(SCRIPTS) ; do \
		$(ROMFSINST) $$i /sbin/$$i ; \
	done
ifeq ("$(CONFIG_USER_STORAGE)", "")	
	rm -rf $(ROMFSDIR)/sbin/storage.sh
endif
