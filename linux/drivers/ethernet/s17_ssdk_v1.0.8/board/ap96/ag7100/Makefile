#
# Makefile for Ar7100 gigebit MAC and Phys
#
obj-$(CONFIG_AG7100)			+=	ag7100_mod.o

obj-phy-$(CONFIG_VITESSE_PHY)	=	vsc_phy.o
obj-phy-$(CONFIG_ICPLUS_PHY)	=	ipPhy.o
obj-phy-$(CONFIG_REALTEK_PHY)	=	rtPhy.o
obj-phy-$(CONFIG_ADM6996FC_PHY)	=	adm_phy.o
ifeq ($(CONFIG_AP81_CARRIER_CUS109), 1)
obj-phy-$(CONFIG_ATHRF1_PHY) 	=	athrf1_phy.o
else
obj-phy-$(CONFIG_ATHR_PHY) 	=	athrf1_phy.o
endif
obj-phy-$(CONFIG_ATHRS26_PHY) 	=	athrs26_phy.o

#default isis_nat_helper is needn't
#obj-phy-$(CONFIG_ATHRS16_PHY) 	=	athrs16_phy.o isis_nat_helper.o ssdk_ks_km.a
obj-phy-$(CONFIG_ATHRS16_PHY) 	=	athrs16_phy.o ssdk_ks_km.a
obj-phy-$(CONFIG_AG7100_GENERIC_PHY)    = generic_phy.o
obj-phy-$(CONFIG_VITESSE_8601_PHY)      = vsc8601_phy.o
obj-phy-$(CONFIG_VITESSE_8601_7395_PHY) = vsc8601_phy.o generic_i2c.o generic_spi.o vsc73xx.o

ag7100_mod-objs			:=	$(obj-phy-y) ag7100.o

PRJ_PATH=/disk/home/raoc/switch_isis_yao/board/ssdk_header

CFLAGS += -I$(PRJ_PATH)/include \
                   -I$(PRJ_PATH)/include/common \
                   -I$(PRJ_PATH)/include/fal \
                   -I$(PRJ_PATH)/include/hsl \
                   -I$(PRJ_PATH)/include/hsl/isis \
                   -I$(PRJ_PATH)/include/sal/os \
                   -I$(PRJ_PATH)/include/sal/sd \
                   -I$(PRJ_PATH)/include/sal/sd/linux/hydra_howl \
                   -I$(PRJ_PATH)/include/sal/sd/linux/uk_interface \
                   -I$(PRJ_PATH)/include/init

ifeq ($(CONFIG_AP81_CARRIER_CUS109), 1)
# There are two F1e PHY on the CUS109 board.
# One PHY connect to MAC 0, the other one connects to MAC 1.
# Define this symbol to indicate two PHY connect to AR913X.
CFLAGS_athrf1_phy.o += -DCONFIG_CUS109_F1E_PHY
CFLAGS_ag7100.o += -DCONFIG_DUAL_F1E_PHY
endif
ifeq ($(CONFIG_DUAL_F1E_PHY), 1)
CFLAGS_ag7100.o += -DCONFIG_DUAL_F1E_PHY
endif
