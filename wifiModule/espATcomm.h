/*! 
@file 
@brief 
@details 

@author Hamza Naeem Kakakhel
@copyright Taraz Technologies Pvt. Ltd.
 */
#ifndef espATcomm_h
#define espATcomm_h
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_common.h"
#include "fsl_uart.h"
#include "stdlib.h"
#include "math.h"
/*******************************************************************************
 * Defines
 ******************************************************************************/
#define UART_ESP UART1
/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structs
 ******************************************************************************/

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif
	
	/* rudimentary support functions */
	//	uint8_t* IntToString(int number);
	int StringToInt(char* inpStr, int size);
	void IntToString(int number, uint8_t* str);
	bool ReceiveReady();
	void ReceiveOk();
	void SearchForOk();
	void SearchForString(uint8_t* string,int length);
	void RecQuery(char* recBuf);
	void RecTillOk(char* recBuf);
	void SendbufferUart(char* buffer, int size);
	void ReceiveByteUart(uint8_t* recByte);
	
  /* General functions for ESP266*/
	bool SanityCheck();
	void Reset();
	void Echo(bool state);
	void DeepSleep(int timeMs);
	void UartCurConfigSet(unsigned int baudrate, unsigned int dataBits, unsigned int stopBits, unsigned int parity, unsigned int flowControl);
	void UartDefConfigSet(unsigned int baudrate, unsigned int dataBits, unsigned int stopBits, unsigned int parity, unsigned int flowControl);
	void UartCurQuery(int* baudRate,int* dataBits, int* stopBits, int* parity, int* flowControl );
	void UartDefQuery(int* baudRate,int* dataBits, int* stopBits, int* parity, int* flowControl );
	void SleepModeSet(int mode);
	void SleepModeQuery(int* mode);
	void WakeGpioSet(int enable, int trigGpio, int trigLevel);
	void SetCurSysMsg(int mode);
	void SetDefSysMsg(int mode);
	void CheckRam(int* space);
	
	/* WIFI functions for ESP266*/
	void SetCurWifiMode(int mode);
	void SetDefWifiMode(int mode);
	void WifiModeCurQuey(int* mode);
	void WifiModeDefQuey(int* mode);
	void ConnectToWifi(char* ssid, char* pass);
	void ConnectToWifiDef( char* ssid, char* pass);
	bool ConnWifiQuery(char* ssid, char* bssid, int* chnl, int* rssi);
	void ConnectToWifiViaMac(char* ssid, char* pass,char* mac);
	void ConnectToWifiViaMacDef(char* ssid, char* pass,char* mac);
	void ApListOpt(int sortRSSI, int mask);
	void ListNetworks();
	void DisconnectFrmNetwork();
	void ConfigSoftApCur(char* ssid, char* pass, int chnl, int encr, int maxCon, int ssidHide);
	void ConfigSoftApDef(char* ssid, char* pass, int chnl, int encr, int maxCon, int ssidHide);
	void ConfigSoftApCurQuery(char* ssid, char* pass, int* chnl, int* encr, int* maxCon, int* ssidHide);
	void ConfigSoftApDefQuery(char* ssid, char* pass, int* chnl, int* encr, int* maxCon, int* ssidHide);
	void DHCPconfigCur(int mode, int DHCP);
	void DHCPconfigDef(int mode, int DHCP);
	void DHCPConfigCurQuery(int* mode);
	void DHCPConfigDefQuery(int* mode);
	void ConnectedDeviceIp();//check with multiple devices
	void SetMacAddStationCur(char* mac);
	void SetMacAddStationDef(char* mac);
	void MacAddStaCurQuery(char* mac);
	void MacAddStaDefQuery(char* mac);
	void SetMacAddSoftAPCur(char* mac);
	void SetMacAddSoftAPDef(char* mac);
	void MacAddApCurQuery(char* mac);
	void MacAddApDefQuery(char* mac);
	void IpAddStaQuery(char* ip, char* gateway, char* netmask);
	void IpAddApCurQuery(char* ip, char* gateway, char* netmask);
	void IpAddApDefQuery(char* ip, char* gateway, char* netmask);
	void SetIPAddAPCur(char* ip, char* gateway, char* netmask);
	void SetIPAddAPDef(char* ip, char* gateway, char* netmask);
	void SetHostName(char* hostname);
	
	/* TCP/IP functions for ESP266*/
	void GetConnStatus();// look into it further, only responds with a single number
	void DnsFunc(char* domainName);
	void SetSSLBufSize(int size);
	void SetSSLClientConfig(int mode);
	void SSLClientConfigQuery(int* mode);
	void RstSegIDCnt(bool multConn, int linkID);
	void SetMultConn(bool enable);
	bool MultConnQuery();
	void CreateDelTCPServer(bool mode, int port);
	void SetMaxConnServer(int max);
	void ServerMaxConnQuery(int* max);
	void SetTransMode(int mode);
	void TransModeQuery(int* mode);
	void SetTcpServerTimeout(int time);//make sure server already created 
	void ServerTimoutQuery(int* time);
	bool PIng(int* time, char* hostIP);
	void OTAUpdate();//show responses
	void IPnPortInIPD(int mode);
	void SetTCPReceiveMode(int mode);
	void TCPRecModeQuery(int* mode);
	void SetCurDNSServer(bool enable, char* server0, char* server1);//user defined
	void SetDefDNSServer(bool enable, char* server0, char* server1);//user defined
	void DNSServerCurQuery(char* dnsServer0, char* dnsServer1);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

#if defined(__cplusplus)
}
#endif
#endif
/* EOF */