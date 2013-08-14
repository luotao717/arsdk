/*
		head file
		lktos init config module for all platform
		
		v1.0 support mt7620 & ra5350  by luot  20130211
		
*/

#ifndef _LKTOS_INITCONFIG_H
#define _LKTOS_INITCONFIG_H 	1


typedef enum lktos_initconfig_platform_type
{
	UNDEFINED=0, //为了兼容性,目前暂不使用
	RALINK3052_STD,//前面为公司加芯片型号，_后面为特殊配置，STD表示标准
	RALINK3050_STD,
	RALINK5350_STD,
	MTK7620_STD,//
	AR9331_HEXICOM,
	UNKNOW//兼容性处理，处理未知型号
}T_LKTOS_INITCONFIG_PLATFORM_TYPE_,*T_LKTOS_INITCONFIG_PLATFORM_TYPE_P;


int lktos_networkconfig_init_lan(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg); //第一个参数为平台类型，成功返回1，失败返回0，错误原因装在
int lktos_networkconfig_init_wan(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg); //第一个参数为平台类型，成功返回1，失败返回0，错误原因装在
int lktos_networkconfig_gen_lanDhcpdConfig(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg); //第一个参数为平台类型，成功返回1，失败返回0，错误原因装在
int lktos_wificonfig_initall_by_filemode(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg); //第一个参数为平台类型，成功返回1，失败返回0，错误原因装在
int lktos_initconfig_init_all(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg);
int lktos_networkconfig_init_wan_ar9331HC(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* wanName,unsigned char vlanEn,unsigned int vlanId,unsigned char* errormsg); //第一个参数为平台类型，成功返回1，失败返回0，错误原因装在




#endif
