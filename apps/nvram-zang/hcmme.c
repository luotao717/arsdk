#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <nvram.h>
#include <linux/reboot.h>
#include "hexicom.h"
#include "../lktos_app/pctools/apmib.h"





#define htonl_mme(val) ((((val) >> 24) & 0x000000FF)|(((val) >> 8) & 0x0000FF00)|(((val) << 8) & 0x00FF0000)|(((val) << 24) & 0xFF000000))
#define htons_mme(val) ((((val) >> 8) & 0x00FF)|(((val) << 8) & 0xFF00))

#define KERNEL_IMA 1
#define ROOTFS 2

#define nvram_get(name) nvram_bufget(RT2860_NVRAM, name)
#define nvram_set(name, value) nvram_bufset(RT2860_NVRAM, name, value)

static unsigned char reboot_flag = 0;  
static unsigned char nvram_save = 0;
static unsigned char restart_flag = 0;
static int discovered = 0;
static int master_registed = 0;
static struct timeval tv_master={0}, tv_recv_blank={0};

static int sock_mme, log =0;
static char plcmac[6];
static int flag_kernelimage = 0;
static int flag_rootfs = 0;




#ifndef __packed
#define	__packed	__attribute__((__packed__))
#endif

typedef struct __packed {
        unsigned short MTYPE;
        unsigned char MMV;
        unsigned short MMTYPE;
        unsigned char OUI[3];
        unsigned short OPCODE;
        union
        {
        unsigned char SET;
        unsigned char STS;
        unsigned char req;
        };   
        unsigned char NUM;
} generic_mme_t;



typedef struct __packed {
        unsigned short OPCODE;
        unsigned char STS;
        unsigned char NUM;
        }response_header;


typedef struct __packed{
	unsigned char plcmac[6];
}hl2mp_macdiscover_t;

typedef struct __packed{
	unsigned char type;
    unsigned char proto;
    unsigned char wanid;
    unsigned char ip[4];
    unsigned char port[2];
    unsigned char user[16];
    unsigned char pwd[16];
    unsigned char path[128];
}hl2mp_remoteupdate_t;


typedef struct __packed{
	unsigned char VMA;       //default 00
	unsigned char VMI;       //default 02
}hl2mp_getversion_t;     //version 


typedef struct __packed{
	unsigned char type;
	unsigned char pid;
	unsigned char link;
	unsigned char speed;
	unsigned char duplex;
}hl2mp_port_status_t;  // port status


typedef struct __packed {
	unsigned char type;   //0:eth  1:wifi
	unsigned char pid;    //port id
	unsigned char enable;
	unsigned char mode;   //0:ap  1:router
	unsigned char auto_neg;  //auto_negotiate
	unsigned char speed;
	unsigned char duplex;
	unsigned char flowctrl;   //flow control
	unsigned char priority;
	unsigned char ingress_rate; //
	unsigned char egress_rate;  //
	unsigned short pvid;        //positive vlan id
	unsigned char tagged;
}hl2mp_port_config_t;    


typedef struct __packed {
	unsigned char WANID;
	unsigned char OPCODE;
	unsigned char WANDEV;
	unsigned char WANCD;
	unsigned char WANCN;
	unsigned char STATUS;
	unsigned char WANMODE;
	unsigned char CONNTYPE;
	unsigned char NATENABLE;
	unsigned char RESERVED0;//mo ren 0
	unsigned short  MTU;
	unsigned char RESERVED1[8];//mo ren 0
	unsigned short VLANID;
    unsigned char PRI;
	unsigned char RESERVED2;//mo ren 0
    unsigned char PORTMASK[4];
	unsigned char PPPEXT[8];
    unsigned char PPPUSR[32];
	unsigned char PPPPWD[16];
    unsigned char IPADDR[4];
	unsigned char NETMASK[4];
    unsigned char GATEWAY[4];
	unsigned char DNS[4];
    unsigned char SDNS[4];
}hl2mp_wan_config_t;


typedef struct __packed {
	unsigned char WIFIID;
    unsigned char ENABLE;
    unsigned char STATUS;
	unsigned char SSID[32];
    unsigned char SSIDCTRL;
    unsigned char MODE;
    unsigned char CHANNEL;
    unsigned char BAND;
    unsigned char SLOPE;
    unsigned char SLOPECTRL;
    unsigned char CODE[2];
    unsigned char AUTH;
    unsigned char ALGORITHM;
    unsigned char KEYLEN;
    unsigned char KEY[16];
    unsigned char WEPCTRL;
    unsigned char RADIUSURL[64];
    unsigned char RADIUSPORT[2];
    unsigned char RADIUSPWD[63];
}hl2mp_wifi_config_t;


typedef struct __packed {
	unsigned char PID;
	unsigned char SSID[32];
}hl2mp_ssid_config_t;


typedef struct __packed
{
    unsigned char TYPE[2];
    unsigned short VLANID;
}vlan_head_t;


void print_buff(void* buff_p, int len)
{
    int loop;
    printf("\n");
    for(loop=0;loop<len;loop++)
   {
    printf("%x ",*((unsigned char*)buff_p+loop));
    }
    printf("\n");
}

//todo
/*
void heximme_ioctl(int cmd, int portaddr, int reg, int * val_p)
{
	int vecarg[5];
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, "eth0");

    switch(cmd)
    {
        case PHYRD:
        {
            vecarg[0] = portaddr << 16;;
            vecarg[0] |= reg & 0xffff;

            ifr.ifr_data = (void *) vecarg;
            DBG_PRINT("heximme_ioctl PHYRD portaddr%d  reg%d %x \n",portaddr, reg);
            if (ioctl(sock_mme,  SIOCGETCPHYRD2, (caddr_t)&ifr) < 0)
            {
                DBG_PRINT("heximme_ioctl PHYRD error \n");
                break;
            }
        	*val_p = vecarg[1];
            DBG_PRINT("heximme_ioctl PHYRD val_p %x \n", *val_p);
            break;
        }
        case PHYWR:
        {
    		vecarg[0] = portaddr << 16;;
    		vecarg[0] |= reg & 0xffff;
    		vecarg[1] = *val_p;

    		ifr.ifr_data = (void *) vecarg;
            DBG_PRINT("heximme_ioctl PHYWR %d %d %x \n",portaddr, reg, *val_p);
    		if (ioctl(sock_mme, SIOCSETCPHYWR2, (void *)&ifr) < 0)
            {
                DBG_PRINT("heximme_ioctl PHYWR error \n");
            }
            break;
        }
    }

}*/



static int getEthPortStatus( hl2mp_port_status_t *ethPortStatus,unsigned char port)
{   
	ethPortStatus->pid=port;
    if( port == 5)
   {
	ethPortStatus->type=1;
   }
   else
   {
    ethPortStatus->type=0;
   }
	ethPortStatus->link=1;
	ethPortStatus->speed=1;
	ethPortStatus->duplex=1;
}

static int getWifiPortStatus( hl2mp_port_status_t *wifiPortStatus,unsigned char port)
{
	wifiPortStatus->pid=port+ 1;
	wifiPortStatus->type=1;
	wifiPortStatus->link=1;
	wifiPortStatus->speed=0xff;
	wifiPortStatus->duplex=0xff;
}

static int getEthPortConfig( hl2mp_port_config_t *ethPortConfig,unsigned char port)
{
    unsigned char nvram_buff_name[50] = {0};
    int reg_val;
   //ethPortConfig->pid = port ;
	ethPortConfig->pid=port;
	ethPortConfig->type=0;
    if(port > 4)

        
	ethPortConfig->enable=1;
	ethPortConfig->auto_neg=1;
	ethPortConfig->speed=1;
	ethPortConfig->duplex=1;
	ethPortConfig->flowctrl=0;
	ethPortConfig->priority=0x0;
	ethPortConfig->ingress_rate=0;
	ethPortConfig->egress_rate=0;
	ethPortConfig->tagged=0;
    ethPortConfig->pvid = 0;

    if(port < 5)
    {
    //todo  
        /*sprintf(nvram_buff_name, "port%dpvid", port);
        if(nvram_get(nvram_buff_name) != NULL)
       {   
           ethPortConfig->pvid = (unsigned short)atoi(nvram_get(nvram_buff_name));
           DBG_PRINT("ethPortConfig->pvid %d \n",ethPortConfig->pvid);
       }
       heximme_ioctl(PHYRD,(int)port - 1, 25, &reg_val);
       DBG_PRINT("getEthPortConfig PHYRD %x  \n", reg_val);
       ethPortConfig->duplex = (reg_val & 0x0001) ? 1 : 0;
       ethPortConfig->auto_neg = (reg_val & 0x0002) ? 1 : 0;
       ethPortConfig->enable = (reg_val & 0x0004) ? 1 : 0;
       ethPortConfig->speed = (reg_val & 0x0008) ? 1 : 0;  */
    }
    else
    {
       ethPortConfig->pid = port-3;
	   ethPortConfig->type=1;

    }
    
    return 0;

}

static int getWifiPortConfig( hl2mp_port_config_t *wifiPortConfig,unsigned char port)
{
	wifiPortConfig->pid=port+ 1;
	wifiPortConfig->type=1;
	wifiPortConfig->enable=1;
	wifiPortConfig->mode=0;
	wifiPortConfig->auto_neg=0xff;
	wifiPortConfig->speed=0xff;
	wifiPortConfig->duplex=0xff;
	wifiPortConfig->flowctrl=0xff;
	wifiPortConfig->priority=0xff;
	wifiPortConfig->ingress_rate=0xff;
	wifiPortConfig->egress_rate=0xff;
	wifiPortConfig->pvid=0xffff;
	wifiPortConfig->tagged=0xff;
}

static int getwanconfig(hl2mp_wan_config_t* hl2mp_wan_config, unsigned char wannum )
{
    unsigned char buff_name[50];
    unsigned char tmpBuf[64]={0};
    unsigned char *p;
    int intVal=0;
    
    sprintf(buff_name,"wan%dvid",wannum);
    if( nvram_get(buff_name)!= NULL )
   {    
        hl2mp_wan_config->WANID = wannum;
        hl2mp_wan_config->VLANID = htons_mme((unsigned short)atoi(nvram_get(buff_name)));
        sprintf(buff_name,"wan%dports",wannum);
        *(unsigned int *)(hl2mp_wan_config->PORTMASK)=htonl_mme((atoi(nvram_get(buff_name)) & 0x0f) | ((atoi(nvram_get(buff_name)) & 0x0f0) << 4));
        sprintf(buff_name, "wan%dmode", wannum);
        hl2mp_wan_config->WANMODE = (unsigned char)atoi(nvram_get(buff_name));
  
   
        if(hl2mp_wan_config->WANMODE == 1)
        {   
           
            if(!strncmp(nvram_bufget(RT2860_NVRAM, "wanConnectionMode"),"STATIC",6))
            	hl2mp_wan_config->CONNTYPE = 0;
            if(!strncmp(nvram_bufget(RT2860_NVRAM, "wanConnectionMode"),"DHCP",4))
                hl2mp_wan_config->CONNTYPE = 1;    
            if(!strncmp(nvram_bufget(RT2860_NVRAM, "wanConnectionMode"),"PPPOE",5))
                hl2mp_wan_config->CONNTYPE = 2; 
 

            snprintf(buff_name, 34, "%s", nvram_bufget(RT2860_NVRAM,"wan_primary_dns"));
           
	        *(unsigned int*)hl2mp_wan_config->DNS = inet_addr(buff_name);
            snprintf(buff_name, 34, "%s", nvram_bufget(RT2860_NVRAM,"wan_secondary_dns"));
	        *(unsigned int*)hl2mp_wan_config->SDNS = inet_addr(buff_name);
            *(unsigned int*)hl2mp_wan_config->IPADDR = inet_addr(nvram_bufget(RT2860_NVRAM,"wan_ipaddr"));
            *(unsigned int*)hl2mp_wan_config->NETMASK = inet_addr(nvram_bufget(RT2860_NVRAM,"wan_netmask"));
            *(unsigned int*)hl2mp_wan_config->GATEWAY = inet_addr(nvram_bufget(RT2860_NVRAM,"wan_gateway"));

            snprintf(hl2mp_wan_config->PPPEXT, 8, "%s", nvram_bufget(RT2860_NVRAM,"wan_pppoe_service"));
            snprintf(hl2mp_wan_config->PPPPWD, 16, "%s", nvram_bufget(RT2860_NVRAM,"wan_pppoe_pass"));
            snprintf(hl2mp_wan_config->PPPUSR, 32, "%s", nvram_bufget(RT2860_NVRAM,"wan_pppoe_user"));
           
            if(nvram_bufget(RT2860_NVRAM,"wan_pppoe_mtu") != NULL)
                hl2mp_wan_config->MTU = (unsigned short)atoi(nvram_bufget(RT2860_NVRAM,"wan_pppoe_mtu"));
        }      
     	}
    DBG_PRINT("getwanconfig end\n");
        return 0;
}


static int getwifiConfig(hl2mp_wifi_config_t *hl2mp_wifi_config, unsigned char port)
{   
    //todo
    /*
    char nvram_name[50]={0};
    if(port == 1)
    {
        memcpy(hl2mp_wifi_config->SSID, nvram_get("wl0_ssid"),32);
    }
    else
    {
        sprintf(nvram_name, "wl0.%d_ssid", port - 1);
        memcpy(hl2mp_wifi_config->SSID, nvram_get(nvram_name),32);
    }
    */
    unsigned char *wifissid;
    unsigned char tmpbuf[64]={0};

   sprintf(tmpbuf,"SSID%d",port);
   wifissid=nvram_bufget(RT2860_NVRAM,tmpbuf);
   memcpy(hl2mp_wifi_config->SSID, wifissid,32);
   
   hl2mp_wifi_config->WIFIID = port;
   
    return 0;
}



static int setEthPortConfig( hl2mp_port_config_t *ethPortConfig, int *flag ,unsigned int count)
{
	/*ethPortConfig->pid=count;
	ethPortConfig->type=0;
	ethPortConfig->enable=1;
	ethPortConfig->mode=0;
	ethPortConfig->auto_neg=0;
	ethPortConfig->speed=0;
	ethPortConfig->duplex=0;
	ethPortConfig->flowctrl=1;
	ethPortConfig->priority=0xff;
	ethPortConfig->ingress_rate=0;
	ethPortConfig->egress_rate=0;
	ethPortConfig->pvid=1;
	ethPortConfig->tagged=0;*/
    
	DBG_PRINT("\r\n set Eth port config:\r\n");
	DBG_PRINT("\r\n ipid=%u,type=%u,enable=%u,mode=%u,autoneg=%u,speed=%u,duplex=%u,fc=%u,pri=%u,ingress=%u,engress=%u,pvid=%u,tagged=%u:\r\n",ethPortConfig->pid,ethPortConfig->type,ethPortConfig->enable,ethPortConfig->mode,ethPortConfig->auto_neg,ethPortConfig->speed,ethPortConfig->duplex,ethPortConfig->flowctrl,ethPortConfig->priority,ethPortConfig->ingress_rate,ethPortConfig->egress_rate,htons(ethPortConfig->pvid),ethPortConfig->tagged);	

    //todo
    /*
    unsigned char nvram_buff_name[20] = {0};
    unsigned char nvram_buff_value[36] = {0};
    signed int reg_val = 0;
    if(ethPortConfig->pid < 5)
    {
        if(ethPortConfig->auto_neg == 1)
        {
            reg_val = 0x1000;
            if(ethPortConfig->speed != 0)
            reg_val |=  0x2000;
        }
        else
        {
            if(ethPortConfig->speed == 1)
            reg_val |= 0x2000;
            if(ethPortConfig->duplex == 1)
            reg_val |= 0x0010;
        }
        heximme_ioctl(PHYWR, ethPortConfig->pid - 1, 0, &reg_val);
        sprintf(nvram_buff_name, "port%dconfig", ethPortConfig->pid - 1);
        sprintf(nvram_buff_value, "%d", reg_val);
        nvram_set(nvram_buff_name, nvram_buff_value);
		nvram_save = 1;
    }*/

	return 1;
}

static int setWifiPortConfig( hl2mp_port_config_t *wifiPortConfig,int *flag)
{
	DBG_PRINT("\r\n set wifi port config:\r\n");
	DBG_PRINT("\r\n ipid=%u,type=%u,enable=%u,mode=%u,autoneg=%u,speed=%u,duplex=%u,fc=%u,pri=%u,ingress=%u,engress=%u,pvid=%u,tagged=%u:\r\n",wifiPortConfig->pid,wifiPortConfig->type,wifiPortConfig->enable,wifiPortConfig->mode,wifiPortConfig->auto_neg,wifiPortConfig->speed,wifiPortConfig->duplex,wifiPortConfig->flowctrl,wifiPortConfig->priority,wifiPortConfig->ingress_rate,wifiPortConfig->egress_rate,htons(wifiPortConfig->pvid),wifiPortConfig->tagged);
	/*
	wifiPortConfig->pid=port;
	wifiPortConfig->type=1;
	wifiPortConfig->enable=1;
	wifiPortConfig->mode=0;
	wifiPortConfig->auto_neg=0xff;
	wifiPortConfig->speed=0xff;
	wifiPortConfig->duplex=0xff;
	wifiPortConfig->flowctrl=0xff;
	wifiPortConfig->priority=0xff;
	wifiPortConfig->ingress_rate=0xff;
	wifiPortConfig->egress_rate=0xff;
	wifiPortConfig->pvid=0xffff;
	wifiPortConfig->tagged=0xffff;
	*/
	return 1;
}

void wan_routerconfig(hl2mp_wan_config_t* hl2mp_wan_config)//配置wan口
{   
    struct in_addr lanaddr;
	struct in_addr lanmaskaddr;
	struct in_addr wanmaskaddr;
    unsigned char buff_value[50];
	unsigned long netmask = 0;
	
    inet_aton(nvram_bufget(RT2860_NVRAM,"lan_ipaddr"), &lanaddr);
    inet_aton(nvram_bufget(RT2860_NVRAM,"wan_netmask"), &wanmaskaddr);
    inet_aton(nvram_bufget(RT2860_NVRAM,"lan_netmask"), &lanmaskaddr);	
	
	netmask = wanmaskaddr.s_addr & lanmaskaddr.s_addr;
	
    switch(hl2mp_wan_config->CONNTYPE)
        {
        case 0:
			if(((unsigned long)(lanaddr.s_addr) & netmask) != ((unsigned long)(hl2mp_wan_config->IPADDR) & netmask))
			{
	            nvram_bufset(RT2860_NVRAM,"wanConnectionMode", "STATIC");
	            nvram_bufset(RT2860_NVRAM,"wan_ipaddr", inet_ntoa(*(struct in_addr*)(hl2mp_wan_config->IPADDR)));
	            nvram_bufset(RT2860_NVRAM,"wan_netmask", inet_ntoa(*(struct in_addr*)(hl2mp_wan_config->NETMASK)));
	            nvram_bufset(RT2860_NVRAM,"wan_gateway", inet_ntoa(*(struct in_addr*)(hl2mp_wan_config->GATEWAY)));
	            nvram_bufset(RT2860_NVRAM,"wan_primary_dns", inet_ntoa(*(struct in_addr*)(hl2mp_wan_config->DNS)));
			}
            break;
        case 1:
            nvram_bufset(RT2860_NVRAM,"wanConnectionMode", "DHCP");
            break;
        case 2:
            nvram_bufset(RT2860_NVRAM,"wanConnectionMode", "PPPOE");
            snprintf(buff_value, 32, "%s", hl2mp_wan_config->PPPUSR);
            nvram_bufset(RT2860_NVRAM,"wan_pppoe_user", buff_value);
            snprintf(buff_value, 32, "%s", hl2mp_wan_config->PPPPWD);
            nvram_bufset(RT2860_NVRAM,"wan_pppoe_pass", buff_value);
          //  if(hl2mp_wan_config->MTU > 0)
           //{
               // sprintf(buff_value, "%d", hl2mp_wan_config->MTU);
                //nvram_set("wan_pppoe_mtu", buff_value);
           // }
            //snprintf(buff_value, 8, "%s", hl2mp_wan_config->PPPEXT);
           // nvram_set("wan_pppoe_service", buff_value);

            break;
        default:
            break;
        }
        //stop_wan(); //reboot, not needed
        //start_wan();
}




static int setwanconfig(hl2mp_wan_config_t* hl2mp_wan_config, unsigned char num, int *flag)
{  
    DBG_PRINT("setWANConfig    num %d!!!!!!\n",num);
    unsigned char buff_name[50] = {0};
    unsigned char buff_value[100] = {0};
    unsigned char buff_name2[50] = {0};
    unsigned char buff_value2[100] = {0};
    unsigned short wanvid;
    unsigned char wanmode;
    unsigned char lq_flag =0;
    unsigned char wanvid_utag[6]={0xff,0xff,0xff,0xff,0xff,0xff};
    unsigned int wanports,wanvid_utag_1=0;
    unsigned int i, j, k, l, lq, m, r = 0, s = 0, r_c = 0;//k为循环变量；i为vlan_v数目；j为vlan数目；m为路由数目
                                    //lq 为无线桥接口建立的br数r为内部lan vlanid

                                    
                                    
    typedef struct __packed vlan_v
    {
    unsigned short vlan_vid;
    unsigned char portmem_v;//每一位表示包含端口 
    unsigned char type_v;//是否属于路由lan口
    } vlan_v_t;
    
    vlan_v_t hexi_vlan_v[VLAN_MAX_V] = {0}; 

    //计算r
    for(r=271; r<271 + WAN_MAXNUM*WAN_MAXNUM; r++)
    {
        for(k=0; k<WAN_MAXNUM; k++)
        {  
            for(l=0;l<WAN_MAXNUM+1;l++)//连续7个不同，为局端管理保留一个
           {
                if((r+l) == (hl2mp_wan_config+k)->VLANID)
                    break;
           }
            if(l<WAN_MAXNUM)
                break;
        }
        DBG_PRINT("r is %d \n",r);
        if(k >= WAN_MAXNUM)
            break;
    } 

    for(k=0, l=0, r_c=0; k<num; k++)
    {DBG_PRINT("k is %d ,l is %d  PORTMASK %x  vid %d\n", k,l,(hl2mp_wan_config+k)->PORTMASK, (hl2mp_wan_config+k)->VLANID );
        if((htons_mme((hl2mp_wan_config+k)->VLANID) >=0) &&(htonl_mme(*(unsigned int*)((hl2mp_wan_config+k)->PORTMASK))&& WAN_PORT_MASK != 0) && ((hl2mp_wan_config+k)->OPCODE == 0))
        {
            wanvid_utag[l]=k;
            l++;

            if((hl2mp_wan_config+k)->WANMODE == 1)
            {
               r_c++; 
            }
        }
    }

    if(l == 0 )
    {
        DBG_PRINT("blank setting!! \n");
        return 0;
    }
    if(r_c > 1)
    {
        DBG_PRINT("surpport only one router!! \n");
        return 0;
    }

 for(k=0; k < WAN_MAXNUM; k++)
    {   
        sprintf(buff_name,"wan%dports",k+1);
        nvram_unset(buff_name);
        sprintf(buff_name,"wan%dvid",k+1);
        nvram_unset(buff_name);
        DBG_PRINT("nvram_unset is %s\n",buff_name);
        sprintf(buff_name,"wan%dmode",k+1);
        nvram_unset(buff_name);
    }

    for(k=0, i=0, m=1; k < num; k++)
    {   
        if(wanvid_utag[k] < WAN_MAXNUM)//正确的一路设置
       {
            wanports = ((htonl_mme(*(unsigned int*)((hl2mp_wan_config+wanvid_utag[k])->PORTMASK)) &0xff) | ((htonl_mme(*(unsigned int*)((hl2mp_wan_config+wanvid_utag[k])->PORTMASK))&0xff00) >> 4));
       }
        else
        {
            continue;
        }
          
        DBG_PRINT("k is %d  \n", k);
        DBG_PRINT("wanports is %x\n", wanports);
        
        wanvid = htons_mme((hl2mp_wan_config+wanvid_utag[k])->VLANID);
        DBG_PRINT("wanvid_utag[k] %d %d",wanvid_utag[k],wanvid);

        sprintf(buff_name,"wan%dvid",wanvid_utag[k]+1);
        sprintf(buff_value,"%d",wanvid);
        nvram_set(buff_name, buff_value);
        
        sprintf(buff_name,"wan%dports",wanvid_utag[k]+1);
        sprintf(buff_value,"%d",wanports);
        nvram_set(buff_name, buff_value);
        
        sprintf(buff_name,"wan%dmode",wanvid_utag[k]+1);
        wanmode=(hl2mp_wan_config+wanvid_utag[k])->WANMODE;
        sprintf(buff_value, "%d", wanmode);
        nvram_set(buff_name, buff_value);
        if(wanmode == 1)//router                           
       {    
            wan_routerconfig(hl2mp_wan_config+wanvid_utag[k]);//配置wan口
            if(wanports & 0x0f)//建立有线lan的vlan
            {   
                hexi_vlan_v[i].vlan_vid = r+m-1;//因为m从1开始
                hexi_vlan_v[i].portmem_v = wanports & 0x31;//两个无线口和第一个有线口
                hexi_vlan_v[i].type_v=m&0x0f;            
            }
  
            
            if(wanports & 0x30)//建立无线lan 的vlan
            {
                hexi_vlan_v[i].vlan_vid = r+m-1;//因为m从1开始
                hexi_vlan_v[i].portmem_v |= wanports & 0x31;//两个无线口和第一个有线口
                hexi_vlan_v[i].type_v=(m&0x0f);//低四位表示路由lan的几路
            }
                i++;
            //建立wan端vlan

            hexi_vlan_v[i].vlan_vid = wanvid;
            hexi_vlan_v[i].portmem_v = 0;//wan 只有一个口，本身
            hexi_vlan_v[i].type_v=(m&0x0f)<<4;//高 四位表示路由wan的几路
            
            i++;
            m++;
            DBG_PRINT("hexi_vlan_v[i].type_v %d \n", hexi_vlan_v[i-1].type_v);
         }

        if(wanmode == 0)//ap
       {
            if(wanports & 0x31)//两个无线口和第一个有线口
            {
               hexi_vlan_v[i].vlan_vid = wanvid;
               hexi_vlan_v[i].portmem_v = wanports & 0x31;
               hexi_vlan_v[i].type_v = 0;//置0表示桥接
               i++;//
            }
           
       }

    }


    for(k= 0; k < WAN_MAXNUM+PORT_WNUM; k++)//将全部的lanifname设置为空      
    {                             
        sprintf(buff_name, "lan%d_ifname", k);            
        nvram_unset(buff_name);            
        sprintf(buff_name, "lan%d_ifnames", k);            
        nvram_unset(buff_name);            
           
        sprintf(buff_name, "wan%d_ifname",k);            
        nvram_unset(buff_name);       
    }

    for(k= 0; k < i; k++)
    {   
        DBG_PRINT(" hexi_vlan_v[k].type_v %d hexi_vlan_v[k].vlan_vid %d\n", hexi_vlan_v[k].type_v,hexi_vlan_v[k].vlan_vid);
    
        lq=ROUTER_MAXNUM;        
        if(hexi_vlan_v[k].vlan_vid != 0)        
        {               
            sprintf(buff_name, "vlan_v%did", k);//vconfig  需要建立的wan端eth0上的vlan
            sprintf(buff_value, "%d", hexi_vlan_v[k].vlan_vid);      
            nvram_set(buff_name, buff_value);


            sprintf(buff_value, "%s", "");
            if(hexi_vlan_v[k].portmem_v & 0x01)
            {
               sprintf(buff_value, "%s", "eth1");
            }

            for(l=0; l < PORT_WNUM; l++)
            {
                if(hexi_vlan_v[k].portmem_v & (0x10 << l))
               {
                    sprintf(buff_value, "%s ath%d", buff_value, l);
                }
            }


            if(hexi_vlan_v[k].type_v == 0)
            {
                 sprintf(buff_value, "%s eth0.%d", buff_value, hexi_vlan_v[k].vlan_vid);//
                 sprintf(buff_name, "lan%d_ifnames", lq);
                 nvram_set(buff_name, buff_value);  
                 
                 sprintf(buff_name, "lan%d_ifname", lq);
                 sprintf(buff_value, "br%d", lq);
                 nvram_set(buff_name, buff_value);

                 lq++;

            }

            if((hexi_vlan_v[k].type_v) & 0x0f > 0)
            {
                 DBG_PRINT("hexi_vlan_v[k].type_v %d\n",hexi_vlan_v[k].type_v&0x0f);

                 sprintf(buff_name, "lan%d_ifnames", hexi_vlan_v[k].type_v & 0x0f-1);
                 sprintf(buff_value, "%s eth0.%d", buff_value, hexi_vlan_v[k].vlan_vid);
                 nvram_set(buff_name, buff_value);

                 sprintf(buff_name, "lan%d_ifname", hexi_vlan_v[k].type_v & 0x0f-1);
                 sprintf(buff_value, "br%d", hexi_vlan_v[k].type_v & 0x0f-1);
                 nvram_set(buff_name, buff_value);
                 
                 DBG_PRINT("buff_name %s-------buff_value %s\n",buff_name,buff_value);
            }
            else if((hexi_vlan_v[k].type_v & 0xf0) > 0)
            {   
                DBG_PRINT("%d %d %d =====/n",hexi_vlan_v[k].type_v&0xf0,(hexi_vlan_v[k].type_v & 0xf0) >> 4,((hexi_vlan_v[k].type_v&0xf0) >> 4)-1);
                sprintf(buff_name, "wan%d_ifname", ((hexi_vlan_v[k].type_v&0xf0) >> 4)-1);
                sprintf(buff_value, "eth0.%d", hexi_vlan_v[k].vlan_vid);
                nvram_set(buff_name, buff_value);
    
                DBG_PRINT("wan is %s  %s !!! \n", buff_name,buff_value);         

            }         
        }
    }

    for(k = i; k < WAN_MAXNUM; k++)//将i到WAN_MAXNUM的vlan_v设置为空
    {
        DBG_PRINT("i     %d\n", k);
        sprintf(buff_name, "vlan_v%did", k);
        nvram_unset(buff_name);
    }   
    
    nvram_save = 1;
    reboot_flag = 1;

    return 0;  
}




static int setwifiConfig(hl2mp_wifi_config_t *hl2mp_wifi_config, int *lag)
{
    char ssid_buff[32];
    char nvram_name[50]={0};
    snprintf(ssid_buff, 32, "%s", hl2mp_wifi_config->SSID);
    DBG_PRINT(" hl2mp_wifi_config %x ssid_buff is %s %d %x =====\n",hl2mp_wifi_config, ssid_buff, hl2mp_wifi_config->WIFIID, hl2mp_wifi_config->WIFIID);

  //todo
  /*
    if(ssid_buff != NULL && *ssid_buff != "")
   {
        if(hl2mp_wifi_config->WIFIID == 1)
        {
        nvram_set("wl_ssid", ssid_buff);
        nvram_set("wl0_ssid", ssid_buff);
        }
        else
        {
        sprintf(nvram_name, "wl0.%d_ssid", hl2mp_wifi_config->WIFIID - 1);
        nvram_set(nvram_name, ssid_buff);
        }
   }*/
    
    if(ssid_buff != NULL && *ssid_buff != '\0')
   {
        if(hl2mp_wifi_config->WIFIID == 1)
        {
        nvram_bufset(RT2860_NVRAM,"SSID1", ssid_buff);
        }
        else if(hl2mp_wifi_config->WIFIID == 2)
        {
        nvram_bufset(RT2860_NVRAM,"SSID2", ssid_buff);
        }
	 else if(hl2mp_wifi_config->WIFIID == 3)
        {
        nvram_bufset(RT2860_NVRAM,"SSID3", ssid_buff);
        }
	  else if(hl2mp_wifi_config->WIFIID == 4)
        {
        nvram_bufset(RT2860_NVRAM,"SSID4", ssid_buff);
        }
        nvram_save = 1;
        restart_flag = 1;
   }
    return 0;
}                    



/**************************************************
 *函数功能：获取网卡的ifr_ifindex
 *************************************************/
int ifindex_get(int fd, struct ifreq *ifr_p)
{
    if (ioctl(fd, SIOCGIFINDEX, ifr_p) == -1)
    {
    DBG_PRINT("ioctl error !!\n");
    return -1;
    }
    DBG_PRINT("ifr.ifr_ifindex %x \n", ifr_p->ifr_ifindex);
    return 0;
}
/**************************************************
 *函数功能：获取网卡的mac地址
 *************************************************/
int hwaddr_get(int fd, struct ifreq *ifr_p)
{
    if (ioctl(fd, SIOCGIFHWADDR, ifr_p) == -1)
    {
    DBG_PRINT("ioctl error !\n");
    return -1;
    }
    return 0;
}
int time_up(struct timeval tv, int second)
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    if((tv_now.tv_sec - tv.tv_sec) > second)
        return 1;
    else
        return 0;
}      

/*********************************************************
 *函数功能：处理接收到的报文，判断接收到报文的源地址是否准确，
 *准确则将接收到报文的源地址作为目标地址，调用mme_fill函数
 *填充报文后发送回复。
 *********************************************************/
int recv_proc(int sock,struct sockaddr_ll *recv_sll)
{
	unsigned int ret = -1, resultFlag, tempi;//ret 接收的长度
	unsigned char buff[2000] = {0x0};
    unsigned char buff_eocmac[500] = {0x0};
    unsigned char req;
    unsigned char buff_path[49];
    struct in_addr addr;
	struct ethhdr *ethHeader;
    generic_mme_t *generic_mme_header;
	int vlanPacketFlag = 0;
    char broadCast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    char master[6] = {0x40, 0xc2, 0x45, 0x0, 0x71, 0x48};
    response_header response_h;
    hl2mp_macdiscover_t *hl2mp_macdiscover_info;
    hl2mp_getversion_t *hl2mp_getversion;
    hl2mp_remoteupdate_t* hl2mp_remoteupdate;
   // hl2mp_port_status_t (*hl2mp_port_status)[WAN_MAXNUM];
    hl2mp_port_status_t *hl2mp_port_status;
    hl2mp_port_config_t *hl2mp_port_config_info;
    hl2mp_wan_config_t *hl2mp_wan_config_info;
    hl2mp_wifi_config_t *hl2mp_wifi_config_info;
    generic_mme_t generic_mme_h;
    unsigned char *send_buff_proc = NULL;
    unsigned char *send_buff = NULL;
    unsigned int send_buff_proc_len = 0;
    unsigned int send_buff_len = 0;
    
    vlan_head_t vlan_head = {{0x81,0x00},0};

	do
	{
	    ret = recv(sock, buff, 2000, 0);
	    if(ret == -1){
	        DBG_PRINT("%s\n", strerror(errno));
	        if(errno == EAGAIN | errno == EWOULDBLOCK){
	            continue;
	            }

	        return -1;
	        }
	}while(ret == -1);

	if(ret == 0)
	{
		DBG_PRINT("disconnected! \n");
		return -1;
	}
    DBG_PRINT("\n recv: ");
    DBG_PRINTBUF(buff, ret);



	ethHeader = (struct ethhdr*)buff;
    if(htons(ethHeader->h_proto) == 0x8100)
	{
		generic_mme_header = (char*)buff + VLAN_MME_HEADER_OFF;
          //9331 发送报文默认不带tag
		vlanPacketFlag=1;
        vlan_head.VLANID = htons_mme((buff[14] & 0x0f) | buff[15]<<8);//注意字节序
	}
    else
	{
	    generic_mme_header = (char*)buff + MME_HEADER_OFF;
		vlanPacketFlag=0;
    }

        if(htons(generic_mme_header->MTYPE) == HOMEPLUG_MTYPE)
        {
         DBG_PRINT("\n recv: ");
         DBG_PRINTBUF(buff, ret);
        }

    if((!memcmp(recv_sll->sll_addr,ethHeader->h_dest,6)) && (*(buff+20) == 0 || *(buff+24) ==0 )&& ((*(unsigned short*)(buff+19) == htons_mme(0xa001)) ||(*(unsigned short*)(buff+15) == htons_mme(0xa001))))
     {
          discovered = 1;
          DBG_PRINT("verygood! local eoc discovered !!!!!############\n");  
          memcpy(plcmac, &buff[6], ETHER_ADDR_LEN);
          return 0;
      }
        
    if((htons(generic_mme_header->MTYPE) == HOMEPLUG_MTYPE) && (!memcmp(recv_sll->sll_addr,ethHeader->h_dest,6) ||(!memcmp(broadCast,ethHeader->h_dest,6))))
    {	printf("OPCODE is %x",generic_mme_header->OPCODE);
		switch(htons_mme(generic_mme_header->OPCODE))
        {       
		    case HL2MP_OP_GET_PROTO_VERSION:
                   {
                    hl2mp_getversion = (void*)malloc(sizeof(hl2mp_getversion_t));
                    memset(hl2mp_getversion, 0, sizeof(hl2mp_getversion_t));
                    response_h.OPCODE = htons_mme(HL2MP_OP_GET_PROTO_VERSION_RESPONSE);
                    response_h.STS = 0;
                    response_h.NUM= 0;
                    hl2mp_getversion->VMA = 0;
                    hl2mp_getversion->VMI = 3;

                    send_buff_proc = (void*)hl2mp_getversion;
                    send_buff_proc_len = sizeof(hl2mp_getversion_t);
		            printf("send_buff_proc_len %d \n",send_buff_proc_len);
					break;
                   }
            case HL2MP_OP_SET_AGENT_MODE:
               {    
                    req = generic_mme_header->req;
                    if(req & 0x01 == 1)
                   {
                   //to do 恢复出厂设置
                   /*     nvram_set("restore_defaults","1");
                        nvram_commit();*/
				    //nvram_save = 1;
                    system("ralink_init clear 2860");
				    system("ralink_init renew 2860 /sbin/RT2860_default_vlan");
                    }
                    
                     if(req & 0x80 > 1)
                   {
                        reboot_flag = 1;
                   } 
                     response_h.STS = 0;
                     
                     if(req & 0x02 == 1)
                   {
                        //sprintf(buff_path, "%s", nvram_get("update_path"));//mtd_write 判空
                        //todo  romote update
                        //if(mtd_write(buff_path, "linux") != 0)                          
                        response_h.STS = 1;
                   } 
                    response_h.OPCODE = htons_mme(HL2MP_OP_SET_AGENT_MODE_RESPONSE);
                    response_h.NUM = 0;
                    
                    send_buff_proc = NULL;
                    send_buff_proc_len = 0;
                    break;
                }
            case HL2MP_OP_REMOTE_UPDATE:
                {   
                    hl2mp_remoteupdate=(hl2mp_remoteupdate_t*)((char*)generic_mme_header + sizeof(generic_mme_t)-1);
                    if( hl2mp_remoteupdate->type == 0 && hl2mp_remoteupdate->proto== 2 )
                   {
                    if(*(unsigned short*)(hl2mp_remoteupdate->port) > 0 )
                        sprintf(buff_path, "http://%s:%d", inet_ntoa(*(struct in_addr*)(hl2mp_remoteupdate->ip)), ntohs(*(unsigned short*)(hl2mp_remoteupdate->port)));
                    if(*(hl2mp_remoteupdate->path) == '/'){
                        sprintf(buff_path, "%s%s", buff_path, hl2mp_remoteupdate->path);
                   }
                    else{
                        sprintf(buff_path, "%s/%s", buff_path, hl2mp_remoteupdate->path);        
                        }
                    }
                    nvram_set("update_path", buff_path);

                    response_h.STS=0;
                    if(eval("wget", "-c", "-q", "-O", "/var/update", buff_path) != 0)
                   {
                        response_h.STS = 1;
                    }

                    nvram_save = 1;
                    
                    response_h.OPCODE = HL2MP_OP_REMOTE_UPDATE_RESPONSE;
                    response_h.NUM=0;//pad 0
                    
                    send_buff_proc = NULL;
                    send_buff_proc_len = 0;
                    
                    break;
                }
			case HL2MP_OP_GET_IF_STATUS:
				{   
                    hl2mp_port_status = (void *)malloc(sizeof(hl2mp_port_status_t) * WAN_MAXNUM);
                    memset(hl2mp_port_status, 0, sizeof(hl2mp_port_status_t) * WAN_MAXNUM);
				    for(tempi=1;tempi<=WAN_MAXNUM;tempi++)
					{
						getEthPortStatus(hl2mp_port_status+tempi-1, tempi);
					}
						
					response_h.OPCODE = htons_mme(HL2MP_OP_GET_IF_STATUS_RESPONSE);
                    response_h.STS = 0;
                    response_h.NUM = WAN_MAXNUM;

                    send_buff_proc = (void*)hl2mp_port_status;
                    send_buff_proc_len = sizeof(hl2mp_port_status_t) * WAN_MAXNUM;
					break;
		        }

			  case HL2MP_OP_IF_CONFIG:
					{   
                        hl2mp_port_config_info = (hl2mp_port_config_t*)((char*)generic_mme_header +  
								sizeof(generic_mme_t));
                       
						if(generic_mme_header->SET)
						{
                            if((generic_mme_header->NUM > 0) && (generic_mme_header->NUM < 8))
                            {
                                for(tempi=0;(tempi<generic_mme_header->NUM) && (hl2mp_port_config_info+1 <= (buff+ret));tempi++)
                                {
                                    setEthPortConfig(hl2mp_port_config_info++, &resultFlag, tempi);
                                }
                            }
						}
 
						    send_buff_proc_len = sizeof(hl2mp_port_config_t) * WAN_MAXNUM;
						    hl2mp_port_config_info = (void *)malloc(send_buff_proc_len);
                            memset(hl2mp_port_config_info, 0, send_buff_proc_len);
                            
							for(tempi = 1;tempi <= WAN_MAXNUM;tempi++)
							{
							    DBG_PRINT("tempi is %d!!!\n", tempi);
								getEthPortConfig(hl2mp_port_config_info+tempi - 1, tempi);
                                DBG_PRINT("type %d, pid %d, enable %d, speed %d, auto_neg %d\n",(hl2mp_port_config_info+tempi - 1)->type,(hl2mp_port_config_info+tempi - 1)->pid,(hl2mp_port_config_info+tempi - 1)->enable,(hl2mp_port_config_info+tempi - 1)->speed,(hl2mp_port_config_info+tempi - 1)->auto_neg);
							}
                            response_h.NUM= WAN_MAXNUM;

                        send_buff_proc = (void*)hl2mp_port_config_info;
                        response_h.OPCODE = htons_mme(HL2MP_OP_IF_CONFIG_RESPONSE);
                        response_h.STS = 0;
					break;
					}
                    
                case  HL2MP_OP_WAN_CONFIG:
					{   
         
                        hl2mp_wan_config_info = (hl2mp_wan_config_t*)((char*)generic_mme_header + sizeof(generic_mme_t));
                  
                        DBG_PRINT("generic_mme_header->NUM %d  ...\n",generic_mme_header->NUM);
                        
						if(generic_mme_header->SET)
						{ 
                           if((generic_mme_header->NUM > 0) && (generic_mme_header->NUM <= WAN_MAXNUM)&&((hl2mp_wan_config_info+generic_mme_header->NUM)<=(buff+ret)))
                           {
								setwanconfig(hl2mp_wan_config_info,generic_mme_header->NUM, &resultFlag);
							}
                         }
                            send_buff_proc_len = sizeof(hl2mp_wan_config_t) * WAN_MAXNUM;    
						    hl2mp_wan_config_info = (void *)malloc(send_buff_proc_len);
                            memset(hl2mp_wan_config_info, 0, sizeof(hl2mp_wan_config_t) * WAN_MAXNUM);
                      
                            
                   
							for(tempi = 1;tempi <= WAN_MAXNUM;tempi++)
							{   DBG_PRINT("HL2MP_OP_WAN_CONFIG read 1... tempi %d\n", tempi);
								getwanconfig(hl2mp_wan_config_info + (tempi -1), tempi);
                                //DBG_PRINT("eth port status =%u--%u--%u--%u--%u\n",req->STATUS[tempi].type,req->STATUS[tempi].pid,htons(req->STATUS[tempi].pvid),req->STATUS[tempi].speed,req->STATUS[tempi].duplex);
							}
                            response_h.NUM= WAN_MAXNUM;



                        send_buff_proc = (void *)hl2mp_wan_config_info;
			   DBG_PRINTBUF((char *)hl2mp_wan_config_info, 104);
                        response_h.OPCODE = htons_mme(HL2MP_OP_WAN_CONFIG_RESPONSE);
         
                        response_h.STS = 0;
            
					break;
					}  
                    
                case  HL2MP_OP_WIFI_CONFIG:
					{   
                        hl2mp_wifi_config_info = (hl2mp_wifi_config_t*)((char*)generic_mme_header + sizeof(generic_mme_t));
                        
						if(generic_mme_header->SET)
						{
							if((generic_mme_header->NUM > 0) && (generic_mme_header->NUM < 8))
                            {   
                                for(tempi=0;tempi<generic_mme_header->NUM && (hl2mp_wifi_config_info+1 <= (buff+ret));tempi++)
								{   
								    setwifiConfig(hl2mp_wifi_config_info++, &resultFlag);
						        }
                            }	
						}
						    hl2mp_wifi_config_info = (void *)malloc(sizeof(hl2mp_wifi_config_t) * PORT_WNUM);
                            memset(hl2mp_wifi_config_info, 0, sizeof(hl2mp_wifi_config_t) * PORT_WNUM);
                            send_buff_proc_len = sizeof(hl2mp_wifi_config_t) * PORT_WNUM;
							for(tempi = 1;tempi <= PORT_WNUM;tempi++)
							{
								getwifiConfig(hl2mp_wifi_config_info+(tempi -1), tempi);
							}
                            response_h.NUM= PORT_WNUM;

                                
                        response_h.OPCODE = htons_mme(HL2MP_OP_WIFI_CONFIG_RESPONSE);
                        response_h.STS = 0;
                        send_buff_proc = (void*)hl2mp_wifi_config_info;
                                              
					break;
					}  
                case HL2MP_OP_DISCOVER_RESPONSE:
                    {
                        DBG_PRINT("verygood connected!!!!!!############\n");
                        master_registed = 1;
                        gettimeofday(&tv_master, NULL);
                        break;
                    }  
                    
				default:
                    return 0;
					//break;
			
		}


        if(vlanPacketFlag == 1)
       {
        send_buff_len = sizeof(generic_mme_t) + send_buff_proc_len + 16;
       }
        else
       {
        send_buff_len = sizeof(generic_mme_t) + send_buff_proc_len + 12;
       }
        send_buff = (void *)malloc(send_buff_len);
        
        memset(send_buff, 0, send_buff_len);
        
        generic_mme_h.MTYPE = htons(HOMEPLUG_MTYPE);
        generic_mme_h.MMV = 0x0;
        generic_mme_h.MMTYPE = htons(0xffff); //todo
        generic_mme_h.OUI[0] = 0;
        generic_mme_h.OUI[1] = 0xb0;
        generic_mme_h.OUI[2] = 0x52;

        generic_mme_h.STS = response_h.STS;
        generic_mme_h.OPCODE = response_h.OPCODE;
        generic_mme_h.NUM = response_h.NUM;

        DBG_PRINTBUF(ethHeader->h_source,6);
        memcpy(send_buff, ethHeader->h_source, ETHER_ADDR_LEN);
        DBG_PRINTBUF(recv_sll->sll_addr,6);
        memcpy(send_buff+6, recv_sll->sll_addr, ETHER_ADDR_LEN);
        
        if(vlanPacketFlag == 1)
       {
            memcpy(send_buff+12, &vlan_head, 4);  
            memcpy(send_buff+16, &generic_mme_h, sizeof(generic_mme_t));
            memcpy(send_buff+16+sizeof(generic_mme_t), send_buff_proc, send_buff_proc_len);
       }
        else
       {
        memcpy(send_buff+12, &generic_mme_h, sizeof(generic_mme_t));
        memcpy(send_buff+12+sizeof(generic_mme_t), send_buff_proc, send_buff_proc_len);
       }

       DBG_PRINTBUF(&generic_mme_h, sizeof(generic_mme_t));
       DBG_PRINTBUF(send_buff_proc, send_buff_proc_len);
       DBG_PRINT("\n send");
       DBG_PRINTBUF(send_buff, send_buff_len);
        ret = sendto(sock, send_buff, send_buff_len, 0, (struct sockaddr *)recv_sll, sizeof(struct sockaddr_ll));
				if(ret == -1)
				{
					DBG_PRINT("errno is %d, %s\n", errno, strerror(errno));
					return -1;
				}
        if(send_buff_proc != NULL){
        free(send_buff_proc);
        }
        if(send_buff != NULL){
        free(send_buff);
        }

        gettimeofday(&tv_recv_blank, NULL);
        
    }
   
    return 0;
}


int update_process()
{
    int file_begin = 0, file_end = 0;
    char* line;
    IMG_HEADER_Tp pHeader;
    int checkOk=0;
    int firmwareType=0;
    char *filename = "/var/update";
    int ret = 0;

    file_begin = 0;
    
    line = getMemInFile(filename, file_begin, 16);


    if(line != NULL)
    {
      pHeader = (IMG_HEADER_Tp)line;

      if(pHeader->signature[0] == 'k' && pHeader->signature[1] == 'f')
      {   
          firmwareType = KERNEL_IMA;

          if(flag_kernelimage == 1)
          {
              firmwareType = 0;
          }
          
          if(pHeader->len > 851960)
          {
              firmwareType = 0;
          }
          
      }
      else if(pHeader->signature[0] == 'r' && pHeader->signature[1] == 'f')
      {
          firmwareType = ROOTFS;

          if(flag_rootfs == 1)
          {
              firmwareType = 0;
          }
            
          if(pHeader->len > 2883580)
          {
              firmwareType = 0;
          }
          
      }
      else
      {
          firmwareType = 0;
          printf("not valid firetype");
      }
      //printf("head-flag=%c%c%c%c--len=%d--%0x--%d",pHeader->signature[0],pHeader->signature[1],pHeader->signature[2],pHeader->signature[3],pHeader->len,pHeader->len,file_end - file_begin);
      file_begin+=16;
      free(line);
      line = getMemInFile(filename, file_begin, pHeader->len);

      if(line != NULL && firmwareType > 0)
      {
          checkOk=fwChecksumOk(line, pHeader->len);
          free(line);
          printf("checkOk %x \n", checkOk);
          //printf("checksum=%d--size=%d",checkOk,file_end - file_begin);
          if(!checkOk)
          {
              printf("checksum error");
          }

          if(firmwareType == ROOTFS)
              flag_rootfs = 1;

          if(firmwareType == KERNEL_IMA)
              flag_kernelimage = 1;

     /*     
           printf("filename %s \n", filename);
           printf("file_begin %x \n", file_begin);
           printf("pHeader->len - 2 %x \n", pHeader->len - 2);
           printf("firmwareType %x \n", firmwareType);
           printf("/sbin/mtd_write -o %d -l %d write %s rootfs", file_begin, pHeader->len - 2, filename);*/
              system("echo 884 > /proc/simple_config/simple_config_led");
              if( mtd_write_firmware(filename, file_begin, pHeader->len - 2, firmwareType) != -1)
             {
               //  reboot_flag = 1;
                
              }else
              {       
                    DBG_PRINT("firmwareType is %d,mtd_write_firmware failed! \n", firmwareType);
              }
          
      }
     // mtd_write_bootloader(filename, file_begin, pHeader->len);
    }
}



/**************************************************
 *函数功能：建立AF_PACKETsocket，循环等待接收报文
 *接收到报文后调用recv_proc(sock_mme, &eth_sll)处理
 *************************************************/
main()
{
	struct ifreq ifr;
	struct sockaddr_ll eth_sll;
	struct timeval t_val={0}, tv_discover={0}, tv_rand={0};
	fd_set fds;
	int ret = -1, buff_eocmac_ready = 0;
    unsigned short port4pvid = 0;

    //unsigned char buff_name[20] = {0x0};
    unsigned char buff_eocmac[500] = {0x0};
    hl2mp_macdiscover_t *hl2mp_macdiscover_info;
    generic_mme_t generic_mme_h;
    unsigned char *send_buff_proc = NULL;
    unsigned int send_buff_len, send_buff_proc_len = 0;
     
    char eoc_dst[6] = {0x0, 0xb0, 0x52, 0x0, 0x0, 0x01};
    char eoc_broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    //char eoc_broadcast[6] = {00,0x0c,0x29,0x5b,0x19,0xe5};
    vlan_head_t vlan_head = {{0x81,0x00},0};

    
    #if 0
    int i, reg_val;

    for(i=0; i<5; i++)
    {
        sprintf(buff_name, "port%dconfig", i);
        if(nvram_get(buff_name) != NULL)
        {
            reg_val = atoi(nvram_get(buff_name));
            heximme_ioctl(PHYWR, i, 0, &reg_val);
        }
    }
    #endif

    if(nvram_get("wan1ports") == NULL || *nvram_get("wan1ports") == '\0')
    {
        printf("wan1ports is NULL !\n");
        system("ralink_init clear 2860");
        system("ralink_init renew 2860 /sbin/RT2860_default_vlan");
        reboot_flag = 1;
        
    }
                
    gettimeofday(&tv_rand, NULL);
    srand(tv_rand.tv_usec);
	sleep(rand() >> 28);
    if(nvram_get("mme_log") != NULL)
        log = atoi(nvram_get("mme_log"));
        log=1;
	sock_mme = socket(AF_PACKET, SOCK_RAW, htons(HOMEPLUG_MTYPE));
	if(-1 == sock_mme)
	{
		DBG_PRINT("sock\n");
	}
	memset(&eth_sll, 0, sizeof(struct sockaddr_ll));
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, "eth0");
	ifindex_get(sock_mme, &ifr);
	eth_sll.sll_ifindex = ifr.ifr_ifindex; 
	hwaddr_get(sock_mme, &ifr);
	memcpy(eth_sll.sll_addr, ifr.ifr_hwaddr.sa_data, 6);
	eth_sll.sll_protocol = htons(ETH_P_ALL);
	eth_sll.sll_family = AF_PACKET;
	DBG_PRINT("ifr.ifr_ifindex %x \n", ifr.ifr_ifindex);
	DBG_PRINT("mac: %x %x %x %x %x %x \n", eth_sll.sll_addr[0], eth_sll.sll_addr[1], eth_sll.sll_addr[2], eth_sll.sll_addr[3], eth_sll.sll_addr [4], eth_sll.sll_addr[5]);
	eth_sll.sll_halen = htons(6);

	ret = bind(sock_mme, (const struct sockaddr *)&eth_sll, sizeof(struct sockaddr_ll));
	if(-1 == ret)
	{
		DBG_PRINT("errno is %d, %s", errno, strerror(errno));
	}


	 t_val.tv_sec = 2;
	 t_val.tv_usec = 1000;

     generic_mme_h.MTYPE = htons(HOMEPLUG_MTYPE);
     generic_mme_h.MMV = 0x0;
     generic_mme_h.MMTYPE = htons_mme(0xa000);
     generic_mme_h.OUI[0] = 0;
     generic_mme_h.OUI[1] = 0xb0;
     generic_mme_h.OUI[2] = 0x52;

     //9331 默认port4pvid=0
     //port4pvid=atoi(nvram_get("port4pvid")); 
     
     DBG_PRINT("port4pvid is %d \n", port4pvid);
     memset(buff_eocmac, 0 ,500);
     if(port4pvid > 0 )
     {
         vlan_head.VLANID=htons_mme(port4pvid);
         memcpy(buff_eocmac, eoc_dst, ETHER_ADDR_LEN);
         memcpy(buff_eocmac+6, eth_sll.sll_addr, ETHER_ADDR_LEN);
     
         memcpy(buff_eocmac+12, &vlan_head, 4);  
         memcpy(buff_eocmac+16, &generic_mme_h, sizeof(generic_mme_t));
     }
     else
     {
         memcpy(buff_eocmac, eoc_dst, ETHER_ADDR_LEN);
         memcpy(buff_eocmac+6, eth_sll.sll_addr, ETHER_ADDR_LEN);
         memcpy(buff_eocmac+12, &generic_mme_h, sizeof(generic_mme_t));
     }

     while(1) 
     {   
        /*if(proto_pppoe == 1)
        {
            if(time_up(tv_pppoe, 90))
            {
            gettimeofday(&tv_pppoe, NULL);
            stop_wan();
            if (waitpid(-1, NULL, 0) == -1)
                printf("waitpid error !!!\n");            
            if (waitpid(-1, NULL, 0) == -1)
                printf("waitpid error !!!\n");
            start_wan();
            }
        }*/
      //  eval("wget", "-c", "-q", "-O", "/var/update", "http://192.168.1.29:8080/v22.bin");
        if(discovered == 0)
        {   
            if(time_up(tv_discover, 5))
            {
                if(-1 == sendto(sock_mme, buff_eocmac, 500, 0, (struct sockaddr *)&eth_sll, sizeof(struct sockaddr_ll)))
                {
                    DBG_PRINT("errno is %d, %s\n", errno, strerror(errno));
                    continue;
                }
                    DBG_PRINT("send discover\n");
                gettimeofday(&tv_discover, NULL);
            }
       }
        else if(master_registed == 0)
        {    
             if(buff_eocmac_ready == 0)
            {
                 hl2mp_macdiscover_info = (void *)malloc(sizeof(hl2mp_macdiscover_t));
                 memset(hl2mp_macdiscover_info, 0, sizeof(hl2mp_macdiscover_t));
                 memcpy(hl2mp_macdiscover_info->plcmac, plcmac, ETHER_ADDR_LEN);
                 
                 send_buff_proc = (void*)hl2mp_macdiscover_info;
                 send_buff_proc_len = sizeof(hl2mp_macdiscover_t);
    
                 if(port4pvid > 0)
                {
                    send_buff_len = sizeof(generic_mme_t) -2 + send_buff_proc_len + 16;
                }
                 else
                {
                    send_buff_len = sizeof(generic_mme_t) - 2 + send_buff_proc_len + 12;
                }
                
                 memset(buff_eocmac, 0, 500);

                 generic_mme_h.MMTYPE = htons_mme(0xffff);
                 generic_mme_h.OPCODE = htons_mme(HL2MP_OP_DISCOVER);
                 
                 memcpy(buff_eocmac, eoc_broadcast, ETHER_ADDR_LEN);
                 memcpy(buff_eocmac+6, eth_sll.sll_addr, ETHER_ADDR_LEN);
                 if(port4pvid > 0)
                {
                     memcpy(buff_eocmac+12, &vlan_head, 4);  
                     memcpy(buff_eocmac+16, &generic_mme_h, sizeof(generic_mme_h) - 2);
                     memcpy(buff_eocmac+16+sizeof(generic_mme_t) - 2, send_buff_proc, send_buff_proc_len);
                }
                 else
                {
                     memcpy(buff_eocmac+12, &generic_mme_h, sizeof(generic_mme_h)-2);
                     memcpy(buff_eocmac+12+sizeof(generic_mme_t) - 2, send_buff_proc, send_buff_proc_len);
                }
                free(send_buff_proc);  
                buff_eocmac_ready = 1;
            }
            if(time_up(tv_discover, 5))
            {   
                DBG_PRINT(" \n send:");
                DBG_PRINTBUF(buff_eocmac,sizeof(buff_eocmac));
                ret = sendto(sock_mme, buff_eocmac, send_buff_len, 0, (struct sockaddr *)&eth_sll, sizeof(struct sockaddr_ll));
                if(ret == -1)
                {
                    DBG_PRINT("errno is %d, %s\n", errno, strerror(errno));
                    continue;
                }
                gettimeofday(&tv_discover, NULL);
            }
        }
        else if(time_up(tv_master, 60))
       {
           master_registed = 0;
       }
                             
	    t_val.tv_sec = 2;
	    t_val.tv_usec = 1000;
	    FD_SET(sock_mme, &fds);
	    ret = select(sock_mme+1, &fds, NULL, NULL, &t_val);
	    if(ret == -1)
	    {
	        DBG_PRINT("select error\n!");
	        continue;
	    }
	    if(ret == 0)
	    {
	        DBG_PRINT("timeout!\n");
	        continue;
	    }
	    if(ret > 0)
	    {
	        recv_proc(sock_mme, &eth_sll);
	    }
        
        if(time_up(tv_recv_blank, 3))
        {   
     
            update_process();
   
            if( nvram_save == 1)
            {
                DBG_PRINT("nvram_commit...\n");
                nvram_commit(RT2860_NVRAM);
            }
            if(reboot_flag == 1)
            {
                DBG_PRINT("sleep 1...\n");
                system("echo 882 > /proc/simple_config/simple_config_led");
                //reboot(LINUX_REBOOT_CMD_RESTART);
            }
            if(restart_flag == 1)
                {
                 DBG_PRINT("sleep 1...\n");
                 sleep(1);
           //      system("rc restart");
                }
            nvram_save = 0;
            reboot_flag = 0;
            restart_flag = 0;

        }

	}
    return 0;
}

