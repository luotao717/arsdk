/*
		head file
		lktos init config module for all platform
		
		v1.0 support mt7620 & ra5350  by luot  20130211
		
*/

#ifndef _LKTOS_INITCONFIG_H
#define _LKTOS_INITCONFIG_H 	1


typedef enum lktos_initconfig_platform_type
{
	UNDEFINED=0, //Ϊ�˼�����,Ŀǰ�ݲ�ʹ��
	RALINK3052_STD,//ǰ��Ϊ��˾��оƬ�ͺţ�_����Ϊ�������ã�STD��ʾ��׼
	RALINK3050_STD,
	RALINK5350_STD,
	MTK7620_STD,//
	AR9331_HEXICOM,
	UNKNOW//�����Դ�������δ֪�ͺ�
}T_LKTOS_INITCONFIG_PLATFORM_TYPE_,*T_LKTOS_INITCONFIG_PLATFORM_TYPE_P;


int lktos_networkconfig_init_lan(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg); //��һ������Ϊƽ̨���ͣ��ɹ�����1��ʧ�ܷ���0������ԭ��װ��
int lktos_networkconfig_init_wan(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg); //��һ������Ϊƽ̨���ͣ��ɹ�����1��ʧ�ܷ���0������ԭ��װ��
int lktos_networkconfig_gen_lanDhcpdConfig(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg); //��һ������Ϊƽ̨���ͣ��ɹ�����1��ʧ�ܷ���0������ԭ��װ��
int lktos_wificonfig_initall_by_filemode(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg); //��һ������Ϊƽ̨���ͣ��ɹ�����1��ʧ�ܷ���0������ԭ��װ��
int lktos_initconfig_init_all(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* errormsg);
int lktos_networkconfig_init_wan_ar9331HC(T_LKTOS_INITCONFIG_PLATFORM_TYPE_ platform,unsigned char* wanName,unsigned char vlanEn,unsigned int vlanId,unsigned char* errormsg); //��һ������Ϊƽ̨���ͣ��ɹ�����1��ʧ�ܷ���0������ԭ��װ��




#endif
