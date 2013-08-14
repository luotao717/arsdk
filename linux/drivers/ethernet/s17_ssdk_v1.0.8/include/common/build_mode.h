#ifndef _BUILD_MODE_H_
#define _BUILD_MODE_H_

/*****************************************************
  This file is automatically generated by config2h.pl.
  Except macro MODULE_TYPE_KSLIB and MODULE_TYPE_USLIB
  Please DO NOT EDIT other items!

  when build kernel space parts you can define
  #define MODULE_TYPE_KSLIB
  when build user space parts you can define
  #define MODULE_TYPE_USLIB
******************************************************/

#define MODULE_TYPE_KSLIB
//#define MODULE_TYPE_USLIB




#ifdef MODULE_TYPE_KSLIB
    #define KERNEL_MODULE
#endif

#define KVER26
#define KERNEL_MODE
#define HSL_STANDALONG
#define ISIS
#define UK_IF
#define UK_IOCTL
#define UK_MINOR_DEV 254
#define IN_ACL
#define IN_FDB
#define IN_IGMP
#define IN_LEAKY
#define IN_LED
#define IN_MIB
#define IN_MIRROR
#define IN_MISC
#define IN_PORTCONTROL
#define IN_PORTVLAN
#define IN_QOS
#define IN_RATE
#define IN_STP
#define IN_VLAN
#define IN_COSMAP
#define IN_SEC

#endif
