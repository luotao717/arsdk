/* Simple version of _eval() (no timeout and wait for child termination) */
#define eval(cmd, args...) ({ \
    char *argv[] = { cmd, ## args, NULL }; \
    _eval(argv, ">/dev/console", 0, NULL); \
})

#define DBG_PRINT printf
#define DBG_PRINTBUF(buff, len) if(log) print_buff(buff, len)

#define HOMEPLUG_MTYPE 0x88E1
#define HL2MP_OP_DISCOVER  0xA000
#define HL2MP_OP_DISCOVER_RESPONSE  0xA001
#define HL2MP_OP_GET_PROTO_VERSION 0xA002
#define HL2MP_OP_GET_PROTO_VERSION_RESPONSE 0xA003
#define HL2MP_OP_SET_AGENT_MODE 0xA010
#define HL2MP_OP_SET_AGENT_MODE_RESPONSE 0xA011

#define HL2MP_OP_REMOTE_UPDATE 0xA012
#define HL2MP_OP_REMOTE_UPDATE_RESPONSE 0xA013

#define HL2MP_OP_GET_IF_STATUS  0xA020
#define HL2MP_OP_GET_IF_STATUS_RESPONSE  0xA021
#define HL2MP_OP_IF_CONFIG  0xA022
#define HL2MP_OP_IF_CONFIG_RESPONSE  0xA023
#define HL2MP_OP_WAN_CONFIG  0xA030
#define HL2MP_OP_WAN_CONFIG_RESPONSE  0xA031
//#define HL2MP_OP_SSID_CONFIG  0xB024
//#define HL2MP_OP_SSID_CONFIG_RESPONSE 0xB025
#define HL2MP_OP_WIFI_CONFIG 0xA024
#define HL2MP_OP_WIFI_CONFIG_RESPONSE 0xA025

#define PHYRD 0x01
#define PHYWR 0x02
#define VLANCONFIG 0x03



#define VLAN_MME_HEADER_OFF 16 //6+6+4 带tagvlan报文的mme偏移
#define MME_HEADER_OFF 12 //6+6带tagvlan报文的mme偏移

#define VLAN_MAX 12 //最多需要12个vlan
#define VLAN_MAX_V 12 //最多需要12个vlan
#define PORT_NUM 4 //有线端口数目
//#define PORT_WNUM 4 //无线端口数目
#define PORT_WNUM 2 //无线端口数目

//#define WAN_MAXNUM 8  //PORT_NUM + PORT_WNUM
#define WAN_MAXNUM 6  //PORT_NUM + PORT_WNUM

#define ROUTER_MAXNUM 6 //路由的最大数量，路由需要建立linux网桥br

#define WAN_PORT_MASK 0x3f


#define foreach(word, wordlist, next) \
        for (next = &wordlist[strspn(wordlist, " ")], \
             strncpy(word, next, sizeof(word)), \
             word[strcspn(word, " ")] = '\0', \
             word[sizeof(word) - 1] = '\0', \
             next = strchr(next, ' '); \
             strlen(word); \
             next = next ? &next[strspn(next, " ")] : "", \
             strncpy(word, next, sizeof(word)), \
             word[strcspn(word, " ")] = '\0', \
             word[sizeof(word) - 1] = '\0', \
             next = strchr(next, ' '))



