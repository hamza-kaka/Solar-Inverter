/*! 
@file 
@brief espwroom-2 at command driver functions
@details 

@author Hamza Naeem Kakakhel
@copyright Taraz Technologies Pvt. Ltd.
*/
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include"espATcomm.h"
/*******************************************************************************
 * Defines
 ******************************************************************************/

/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structs
 ******************************************************************************/

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
																																																		/************************************************************ TCP/IP ******************************************************/
 void GetConnStatus() // look into it, only responds with a single number 
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPSTATUS\r\n"; 
	char recBuffer[150] = {0};
	char colon = ':';
	char ok = 'O';
	char* add = recBuffer;
	int index = 0;
	int outcnt = 0;
//	char* outchar[3] = {ip, gateway, netmask};
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART_ESP->S1 & UART_S1_TC_MASK))
			{}
		(void)UART_ESP->S1;
				temp = UART_ESP->D;
				
		RecQuery(recBuffer);
 }
 
 
 void DnsFunc(char* domainName) // make sure wifi is connected 
 {

	char clearLine[] = "\r\n";
	char quoteMrk[] = "\"";
	char txBuf[60] = "AT+CIPDOMAIN=";

	strcat(txBuf,quoteMrk);
	strcat(txBuf,domainName);
	strcat(txBuf,quoteMrk);
	strcat(txBuf,clearLine);
	
	SendbufferUart(txBuf, strlen(txBuf));
				
	SearchForOk();			
 }
 
 
 void SetSSLBufSize(int size)
 {
	 
	if(size< 2048 || size >4096)
	{
	 return;  //give error 
	}	
	
  char clearLine[] = "\r\n";
  char tempBuf[5] = {0}; 
	char txBuf[20] = "AT+CIPSSLSIZE="; 

	IntToString(size,(uint8_t*)tempBuf);
  strcat(txBuf,tempBuf);
	strcat(txBuf,clearLine);

	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();	
 }
		
 void SetSSLClientConfig(int mode)
 {
	if(mode> 2)
	{
	 return;  //give error 
	}	
	
  char clearLine[] = "\r\n";
  char tempBuf ='0'; 
	char txBuf[18] = "AT+CIPSSLCCONF="; 

	IntToString(mode,(uint8_t*)&tempBuf);
  strcat(txBuf,&tempBuf);
	strcat(txBuf,clearLine);

	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();	
 }	 
 
 void SSLClientConfigQuery(int* mode)
 {

	uint8_t CR = 13;
	char txBuf[] = "AT+CIPSSLCCONF?\r\n"; 
	char recBuffer[15] = {0};
	char colon = ':';
	char* add = recBuffer;
	
	SendbufferUart(txBuf, strlen(txBuf));
	RecQuery(recBuffer);
		
	while(*add++ != colon)
	{	}
		
	*mode = StringToInt(add,1);
 }
 
 void RstSegIDCnt(bool multConn, int linkID) // check again when connection is established 
 {

  char clearLine[] = "\r\n";
  char tempBuf ='0'; 
	char txBuf[20] = "AT+CIPBUFRESET"; 
	char equal = '=';
	
	if(multConn)
	{
		if(linkID >4 )
			return;  //give error 
		
			IntToString(linkID,(uint8_t*)&tempBuf);
			strcat(txBuf,&equal);
			strcat(txBuf,&tempBuf);

	}	
	
	strcat(txBuf,clearLine);

	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();	
 }
 
 void SetMultConn(bool enable) //check transmission mode first
 {
	char tempBuf = '0';
	char clearLine[] = "\r\n";
	char txBuf[15] = "AT+CIPMUX="; 
	 
	 if(enable)
		tempBuf ='1';
	 
  strcat(txBuf,&tempBuf);
	strcat(txBuf,clearLine);

	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();
 }
 
 bool MultConnQuery()
 {
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPMUX?\r\n"; 
	char recBuffer[15] = {0};
	char colon = ':';
	char* add = recBuffer;
	
	SendbufferUart(txBuf, strlen(txBuf));
	RecQuery(recBuffer);
		
	while(*add++ != colon)
	{	}
		
	 if(StringToInt(add,1))
		 return true;
	 else 
		 return false;
 }
 
 void CreateDelTCPServer(bool mode, int port) // can be only be created when multiple connections are allowed
 {
	char tempBuf[] = {0};
	char comma = ',';
	char clearLine[] = "\r\n";
	char txBuf[25] = "AT+CIPSERVER="; 
	 
	 if(mode)
	 {
		 tempBuf[0] = '1';
		 strcat(txBuf,tempBuf);
		 strcat(txBuf,&comma);
		 IntToString(port,(uint8_t*)tempBuf);
		 strcat(txBuf,tempBuf);
	 }
	 else 
	 {
		tempBuf[0] = '0';
		strcat(txBuf,tempBuf);
	 }
	 
  strcat(txBuf,clearLine);
	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();
 }
 
 void SetMaxConnServer(int max)
 {
	if(max>5)
	{
	 return;  //give error 
	}	
	
  char clearLine[] = "\r\n";
  char tempBuf = '0'; 
	char txBuf[25] = "AT+CIPSERVERMAXCONN="; 

	IntToString(max,(uint8_t*)&tempBuf);
  strcat(txBuf,&tempBuf);
	strcat(txBuf,clearLine);

	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();	
 }
 
 void ServerMaxConnQuery(int* max)
 {
	 uint8_t CR = 13;
	char txBuf[] = "AT+CIPSERVERMAXCONN?\r\n"; 
	char recBuffer[25] = {0};
	char colon = ':';
	char* add = recBuffer;
	
	SendbufferUart(txBuf, strlen(txBuf));
	RecQuery(recBuffer);
		
	while(*add++ != colon)
	{	}
		
	*max = StringToInt(add,1);
 }
 
 void SetTransMode(int mode)
 {
	if(mode>1)
	{
	 return;  //give error 
	}	
	
  char clearLine[] = "\r\n";
  char tempBuf = '0'; 
	char txBuf[13] = "AT+CIPMODE="; 

	IntToString(mode,(uint8_t*)&tempBuf);
  strcat(txBuf,&tempBuf);
	strcat(txBuf,clearLine);

	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();
 }
 
 void TransModeQuery(int* mode)
 {
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPMODE?\r\n"; 
	char recBuffer[15] = {0};
	char colon = ':';
	char* add = recBuffer;
	
	SendbufferUart(txBuf, strlen(txBuf));
	RecQuery(recBuffer);
		
	while(*add++ != colon)
	{	}
		
	*mode = StringToInt(add,1);
 }
 
 void SetTcpServerTimeout(int time)
 {
	if(time >7200)
	{
	 return;  //give error 
	}	
	
	char txBuf[17] = "AT+CIPSTO="; 
	char tempBuf[5] = {0}; 
	char clearLine[] = "\r\n";
  
	
	IntToString(time,(uint8_t*)tempBuf);
	strcat(txBuf,tempBuf);
  strcat(txBuf,clearLine);
	
	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();
 }
 
 void ServerTimoutQuery(int* time)
 {
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPSTO?\r\n"; 
	char recBuffer[15] = {0};
	char colon = ':';
	char* add = recBuffer;
	int index = 0;
	
	SendbufferUart(txBuf, strlen(txBuf));
	RecQuery(recBuffer);
		
	while(*add++ != colon)
	{	}
	while (add[index++] != CR)
	{}
		
	*time = StringToInt(add,index-1);
 }
 
 bool PIng(int* time, char* hostIP)
 {
	char clearLine[] = "\r\n";
	char quoteMrk[] = "\"";
	char txBuf[50] = "AT+PING=";

	strcat(txBuf,quoteMrk);
	strcat(txBuf,hostIP);
	strcat(txBuf,quoteMrk);
	strcat(txBuf,clearLine);
	 
  uint8_t CR = 13;
	char recBuffer[15] = {0};
	char plus = '+';
	char* add = recBuffer;
	int index = 0;
	
	SendbufferUart(txBuf, strlen(txBuf));
	RecQuery(recBuffer);
		
	while(*add++ != plus)
	{	}
	while (add[index++] != CR)
	{}
		
	*time = StringToInt(add,index-1);
 }
 
 void OTAUpdate()
 {
	char txBuf[17] = "AT+CIPUPDATE\r\n"; 
	char tempBuf[5] = {0}; 
	
	SendbufferUart(txBuf, strlen(txBuf));
 }
 
 void IPnPortInIPD(mode)
 {
	if(mode>1)
	{
	 return;  //give error 
	}	
	
  char clearLine[] = "\r\n";
  char tempBuf = '0'; 
	char txBuf[13] = "AT+CIPDINFO="; 

	IntToString(mode,(uint8_t*)&tempBuf);
  strcat(txBuf,&tempBuf);
	strcat(txBuf,clearLine);

	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();
 }
 
 void SetTCPReceiveMode(int mode)
 {
	if(mode>1)
	{
	 return;  //give error 
	}	
	
  char clearLine[] = "\r\n";
  char tempBuf = '0'; 
	char txBuf[20] = "AT+CIPRECVMODE="; 

	IntToString(mode,(uint8_t*)&tempBuf);
  strcat(txBuf,&tempBuf);
	strcat(txBuf,clearLine);

	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();
 }
 
 void TCPRecModeQuery(int* mode)
 {
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPRECVMODE?\r\n"; 
	char recBuffer[18] = {0};
	char colon = ':';
	char* add = recBuffer;
	
	SendbufferUart(txBuf, strlen(txBuf));
	RecQuery(recBuffer);
		
	while(*add++ != colon)
	{}
		
	*mode = StringToInt(add,1);
 }
 
 void SetCurDNSServer(bool enable, char* server0, char* server1)
 {
	char tempBuf = '0';
	char clearLine[] = "\r\n";
	char comma[] = ",";
	char quoteMrk[] = "\"";
	char txBuf[60] = "AT+CIPDNS_CUR="; 
	if(enable)
		tempBuf ='1';
	 
	strcat(txBuf,&tempBuf);
	
	char* inputs[2] = {server0,server1};
	
 if(!enable)
	{
		goto send;
	}
	
	for(int a=0; a<2; a++)
	{
		strcat(txBuf,comma);
		strcat(txBuf,quoteMrk);
		strcat(txBuf,inputs[a]);
		strcat(txBuf,quoteMrk);
	}
	
	send:
	strcat(txBuf,clearLine);
	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();	
	
 }
 
 void SetDefDNSServer(bool enable, char* server0, char* server1)
 {
	char tempBuf = '0';
	char clearLine[] = "\r\n";
	char comma[] = ",";
	char quoteMrk[] = "\"";
	char txBuf[60] = "AT+CIPDNS_DEF="; 
	if(enable)
		tempBuf ='1';
	 
	strcat(txBuf,&tempBuf);
	
	char* inputs[2] = {server0,server1};
	
 if(!enable)
	{
		goto send;
	}
	
	for(int a=0; a<2; a++)
	{
		strcat(txBuf,comma);
		strcat(txBuf,quoteMrk);
		strcat(txBuf,inputs[a]);
		strcat(txBuf,quoteMrk);
	}
	
	send:
	strcat(txBuf,clearLine);
	SendbufferUart(txBuf, strlen(txBuf));
	ReceiveOk();	
	
 }
 
 void DNSServerCurQuery(char* dnsServer0, char* dnsServer1)
 {
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPDNS_CUR?\r\n"; 
	char recBuffer[80] = {0};
	char colon = ':';
	char ok = 'O';
	char* add = recBuffer;
	int index = 0;
	int outcnt = 0;
	char* outchar[2] = {dnsServer0, dnsServer1};
	
	SendbufferUart(txBuf, strlen(txBuf));
	
	RecTillOk(recBuffer);
				
    while(*add != ok) 
			{	
				while(*add++ != colon )//
				{}
				while (add[index++] != CR)
				{}
				outchar[outcnt] = strncpy(outchar[outcnt],add,index-1);
				add += index+1;
				index = 0;
				outcnt++;					
			}
		
 }
	
	 
																																															/******************************************************** WIFI ************************************************************/
 
  void IpAddApDefQuery(char* ip, char* gateway, char* netmask)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPAP_DEF?\r\n"; 
	char recBuffer[95] = {0};
	char colon = ':';
	char ok = 'O';
	char* add = recBuffer;
	int index = 0;
	int outcnt = 0;
	char* outchar[3] = {ip, gateway, netmask};
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecTillOk(recBuffer);
				
		for(int a=0; a<3; a++)		
			{
				for(int a=0; a<2;a++)				
				while(*add++ != colon)
				{}
				while (add[index++] != CR)
				{}
				outchar[outcnt] = strncpy(outchar[outcnt],add,index-1);
				add += index+1;
				index = 0;
				outcnt++;					
			}
 }
 
 void IpAddApCurQuery(char* ip, char* gateway, char* netmask)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPAP_CUR?\r\n"; 
	char recBuffer[95] = {0};
	char colon = ':';
	char ok = 'O';
	char* add = recBuffer;
	int index = 0;
	int outcnt = 0;
	char* outchar[3] = {ip, gateway, netmask};
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecTillOk(recBuffer);
				
		for(int a=0; a<3; a++)		
			{
				for(int a=0; a<2;a++)				
				while(*add++ != colon)
				{}
				while (add[index++] != CR)
				{}
				outchar[outcnt] = strncpy(outchar[outcnt],add,index-1);
				add += index+1;
				index = 0;
				outcnt++;					
			}

 }
 
 
 
 void IpAddStaQuery(char* ip, char* gateway, char* netmask)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPSTA_CUR?\r\n"; 
	char recBuffer[95] = {0};
	char colon = ':';
	char ok = 'O';
	char* add = recBuffer;
	int index = 0;
	int outcnt = 0;
	char* outchar[3] = {ip, gateway, netmask};
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecTillOk(recBuffer);
				
		for(int a=0; a<3; a++)		
			{
				for(int a=0; a<2;a++)				
				while(*add++ != colon)
				{}
				while (add[index++] != CR)
				{}
				outchar[outcnt] = strncpy(outchar[outcnt],add,index-1);
				add += index+1;
				index = 0;
				outcnt++;					
			}
 }
 
 void MacAddApDefQuery(char* mac)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPAPMAC_DEF?\r\n"; 
	char recBuffer[40] = {0};
	char colon = ':';
	char comma = ',';
	char* add = recBuffer;
	int index = 0;
	int outcnt = 0;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
			
		while(*add++ != colon)
		{}	
		while (add[index++] != CR)
		{}
		
		mac = strncpy(mac,add,index-1);
 }
 
 void MacAddApCurQuery(char* mac)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPAPMAC_CUR?\r\n"; 
	char recBuffer[40] = {0};
	char colon = ':';
	char comma = ',';
	char* add = recBuffer;
	int index = 0;
	int outcnt = 0;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
			
		while(*add++ != colon)
		{}	
		while (add[index++] != CR)
		{}
		
		mac = strncpy(mac,add,index-1);
 }
	 
 void MacAddStaDefQuery(char* mac)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPSTAMAC_DEF?\r\n"; 
	char recBuffer[40] = {0};
	char colon = ':';
	char comma = ',';
	char* add = recBuffer;
	int index = 0;
	int outcnt = 0;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
			
		while(*add++ != colon)
		{}	
		while (add[index++] != CR)
		{}
		
		mac = strncpy(mac,add,index-1);
 }
 
 void MacAddStaCurQuery(char* mac)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CIPSTAMAC_CUR?\r\n"; 
	char recBuffer[40] = {0};
	char colon = ':';
	char comma = ',';
	char* add = recBuffer;
	int index = 0;
	int outcnt = 0;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
			
		while(*add++ != colon)
		{}	
		while (add[index++] != CR)
		{}
		
		mac = strncpy(mac,add,index-1);
 }
 
  void DHCPConfigDefQuery(int* mode)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CWDHCP_DEF?\r\n"; 
	char recBuffer[15] = {0};
	char colon = ':';
	char* add = recBuffer;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
		
		while(*add++ != colon)
		{	}
		
		*mode = StringToInt(add,1);
 }
 
 void DHCPConfigCurQuery(int* mode)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CWDHCP_CUR?\r\n"; 
	char recBuffer[15] = {0};
	char colon = ':';
	char* add = recBuffer;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
		
		while(*add++ != colon)
		{	}
		
		*mode = StringToInt(add,1);
 }
 
 void ConnectedDeviceIp()//check with multiple devices
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char ok[] = "OK"; 
	char txBuf[] = "AT+CWLIF\r\n"; 
	char recBuffer[240] = {0};
	char comma = ',';
	char* add = recBuffer;
	int index = 0;
	int outcnt = 0;
	int noOfDevices = 0;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecTillOk(recBuffer);
	  
						while (add[index++] != *ok)
		{
//				if(add[index-1] == comma)
//				{
//					if(outcnt < 2)
//						outchar[outcnt] = strncpy(outchar[outcnt],add,index-1);
//					else
//						*outint[outcnt-2] = StringToInt(add,index-1);
//					add += index;
//					index = 0;
//					outcnt++;
//				}
				 if(add[index-1] == CR)
				{
					noOfDevices++;
				}
		}
			
			if(!noOfDevices)
			{}// do shit for no connection
 }
 
  void ConfigSoftApDefQuery(char* ssid, char* pass, int* chnl, int* encr, int* maxCon, int* ssidHide)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CWSAP_CUR?\r\n"; 
	char recBuffer[95] = {0};
	char colon = ':';
	char comma = ',';
	char* add = recBuffer;
	int* outint[4] = {chnl,encr,maxCon,ssidHide};
	char* outchar[2] = {ssid,pass};
	int index = 0;
	int outcnt = 0;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
			
		while(*add++ != colon)
		{}
		
			while (add[index++] != CR)
		{
				if(add[index-1] == comma)
				{
					if(outcnt < 2)
						outchar[outcnt] = strncpy(outchar[outcnt],add,index-1);
					else
						*outint[outcnt-2] = StringToInt(add,index-1);
					add += index;
					index = 0;
					outcnt++;
				}
		}
 }
 
 void ConfigSoftApCurQuery(char* ssid, char* pass, int* chnl, int* encr, int* maxCon, int* ssidHide)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CWSAP_CUR?\r\n"; 
	char recBuffer[95] = {0};
	char colon = ':';
	char comma = ',';
	char* add = recBuffer;
	int* outint[4] = {chnl,encr,maxCon,ssidHide};
	char* outchar[2] = {ssid,pass};
	int index = 0;
	int outcnt = 0;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
			
		while(*add++ != colon)
		{}
		
			while (add[index++] != CR)
		{
				if(add[index-1] == comma)
				{
					if(outcnt < 2)
						outchar[outcnt] = strncpy(outchar[outcnt],add,index-1);
					else
						*outint[outcnt-2] = StringToInt(add,index-1);
					add += index;
					index = 0;
					outcnt++;
				}
		}
 }
	 
 bool ConnWifiQuery(char* ssid, char* bssid, int* chnl, int* rssi)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char disconnected[] = "No AP"; 
	char txBuf[] = "AT+CWJAP_CUR?\r\n"; 
	char recBuffer[80] = {0};
	char colon = ':';
	char comma = ',';
	char* add = recBuffer;
	bool connected = false;
	int* outint[2] = {chnl,rssi};
	char* outchar[2] = {ssid,bssid};
	int index = 0;
	int outcnt = 0;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
				
			for(int a=0;a<5;a++)
			{
				if(recBuffer[a] == disconnected[a])
					connected = false;
				else 
				{
				  connected = true;
					break;
				}
			}		
			
			if(!connected)
			 return connected;
			
		while(*add++ != colon)
		{}
		
			while (add[index++] != CR)
		{
				if(add[index-1] == comma)
				{
					if(outcnt < 2)
						outchar[outcnt] = strncpy(outchar[outcnt],add,index-1);
					else
						*outint[outcnt-2] = StringToInt(add,index-1);
					add += index;
					index = 0;
					outcnt++;
				}
		}
		
 }
 
 void WifiModeDefQuey(int* mode)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CWMODE_DEF?\r\n"; 
	char recBuffer[20] = {0};
	char colon = ':';
	char* add = recBuffer;
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
		
		while(*add++ != colon)
		{	}
		
		*mode = StringToInt(add,1);
 }
 
 
 void WifiModeCurQuey(int* mode)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+CWMODE_CUR?\r\n"; 
	char recBuffer[20] = {0};
	char colon = ':';
	char* add = recBuffer;

	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
		
		while(*add++ != colon)
		{	}
		
		*mode = StringToInt(add,1);
 }
	 
 void CheckRam(int* space)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+SYSRAM?\r\n"; 
	char recBuffer[15] = {0};
	char colon = ':';
	char* add = recBuffer;
		
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
		
		while(*add++ != colon)
		{	}
		
		*space = StringToInt(add,5);
 }
 
 void SetHostName(char* hostname)
 {
	char clearLine[] = "\r\n";
	char quoteMrk[] = "\"";
	char temp = 0;
	char txBuf[60] = "AT+CWHOSTNAME=";
	

		strcat(txBuf,quoteMrk);
		strcat(txBuf,hostname);
		strcat(txBuf,quoteMrk);
	  strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
 }
 
  void SetIPAddAPDef(char* ip, char* gateway, char* netmask)
 {
	 
	char* inputs[3] = {ip,gateway,netmask};
	char clearLine[] = "\r\n";
	char comma[] = ",";
	char quoteMrk[] = "\"";
	uint8_t temp = 0;
	char txBuf[70] = "AT+CIPAP_DEF=";
		
	 if(!gateway || !netmask)
	{
	  strcat(txBuf,ip);
		strcat(txBuf,clearLine);
		goto send;
	}
	
	for(int a=0; a<3; a++)
	{
		strcat(txBuf,quoteMrk);
		strcat(txBuf,inputs[a]);
		strcat(txBuf,quoteMrk);
		if(a != 2)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);
	
	send:
	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
				
 }
 
 
 void SetIPAddAPCur(char* ip, char* gateway, char* netmask)
 {
	 
	char* inputs[3] = {ip,gateway,netmask};
	char clearLine[] = "\r\n";
	char comma[] = ",";
	char quoteMrk[] = "\"";
	uint8_t temp = 0;
	char txBuf[70] = "AT+CIPAP_CUR=";
		
	 if(!gateway || !netmask)
	{
	  strcat(txBuf,ip);
		strcat(txBuf,clearLine);
		goto send;
	}
	
	for(int a=0; a<3; a++)
	{
		strcat(txBuf,quoteMrk);
		strcat(txBuf,inputs[a]);
		strcat(txBuf,quoteMrk);
		if(a != 2)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);
	
	send:
	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
				
 }
 
   void SetMacAddSoftAPDef(char* mac) // mac for soft AP and station should be different 
 {
	 if(mac[1]-'0' > 9)
	 {
		 if((mac[1]-'a'+10) % 2)
			return; // error because bit 0 of mac cant be 1
	 }
	 else 
	 {
		if(mac[1]%2)
			return; // error because bit 0 of mac cant be 1
	 }
	char clearLine[] = "\r\n";
	char quoteMrk[] = "\"";
	char temp = 0;
	char txBuf[60] = "AT+CIPAPMAC_DEF=";
	

		strcat(txBuf,quoteMrk);
		strcat(txBuf,mac);
		strcat(txBuf,quoteMrk);
	  strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
 }
 
 
  void SetMacAddSoftAPCur(char* mac) // mac for soft AP and station should be different 
 {
	 if(mac[1]-'0' > 9)
	 {
		 if((mac[1]-'a'+10) % 2)
			return; // error because bit 0 of mac cant be 1
	 }
	 else 
	 {
		if(mac[1]%2)
			return; // error because bit 0 of mac cant be 1
	 }
	char clearLine[] = "\r\n";
	char quoteMrk[] = "\"";
	char temp = 0;
	char txBuf[60] = "AT+CIPAPMAC_CUR=";
	

		strcat(txBuf,quoteMrk);
		strcat(txBuf,mac);
		strcat(txBuf,quoteMrk);
	  strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
 }
 
 
 void SetMacAddStationDef(char* mac)
 {
	  if(mac[1]-'0' > 9)
	 {
		 if((mac[1]-'a'+10) % 2)
			return; // error because bit 0 of mac cant be 1
	 }
	 else 
	 {
		if(mac[1]%2)
			return; // error because bit 0 of mac cant be 1
	 }
	char clearLine[] = "\r\n";
	char quoteMrk[] = "\"";
	char temp = 0;
	char txBuf[60] = "AT+CIPSTAMAC_DEF=";
	

		strcat(txBuf,quoteMrk);
		strcat(txBuf,mac);
		strcat(txBuf,quoteMrk);
	  strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
 }
 
 void SetMacAddStationCur(char* mac) // mac for soft AP and station should be different 
 {
	 if(mac[1]-'0' > 9)
	 {
		 if((mac[1]-'a'+10) % 2)
			return; // error because bit 0 of mac cant be 1
	 }
	 else 
	 {
		if(mac[1]%2)
			return; // error because bit 0 of mac cant be 1
	 }
	char clearLine[] = "\r\n";
	char quoteMrk[] = "\"";
	char temp = 0;
	char txBuf[60] = "AT+CIPSTAMAC_CUR=";
	

		strcat(txBuf,quoteMrk);
		strcat(txBuf,mac);
		strcat(txBuf,quoteMrk);
	  strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
 }
 
  void DHCPconfigDef(int mode, int DHCP)
 {
	if(mode> 2 || DHCP >1)
	{
	 return;  //give error 
	}	
	
	int inputs[2] = {mode, DHCP};
  char clearLine[] = "\r\n";
	char comma[] = ",";
	char temp = 0;
  char tempBuf[10] = {0}; 
	char txBuf[20] = "AT+CWDHCP_DEF="; 
	
	for(int a =0; a<2; a++)
	{
	IntToString(inputs[a],(uint8_t*)tempBuf);
  strcat(txBuf,tempBuf);
	if(!a)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);

	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
				ReceiveOk();	
				
 }
 
 
 void DHCPconfigCur(int mode, int DHCP)
 {
	if(mode> 2 || DHCP >1)
	{
	 return;  //give error 
	}	
	
	int inputs[2] = {mode, DHCP};
  char clearLine[] = "\r\n";
	char comma[] = ",";
	char temp = 0;
  char tempBuf[10] = {0}; 
	char txBuf[20] = "AT+CWDHCP_CUR="; 
	
	for(int a =0; a<2; a++)
	{
	IntToString(inputs[a],(uint8_t*)tempBuf);
  strcat(txBuf,tempBuf);
	if(!a)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);

	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
				ReceiveOk();	
				
 }
 
 
 void ConfigSoftApDef(char* ssid, char* pass, int chnl, int encr, int maxCon, int ssidHide)
 {
		if(encr> 4 || encr ==1)
	{
	 return;  //give error 
	}	
	if(maxCon > 8)
	{
		 return;  //give error 
	}
	
	char* chrInput[2] = {ssid,pass};// sprcial characters in pass or ssid
	int intInput[4] = {chnl,encr,maxCon,ssidHide};
	char clearLine[] = "\r\n";
	char comma[] = ",";
	char quoteMrk[] = "\"";
	char tempBuf[10] = {0}; 
	char temp = 0;
	char txBuf[100] = "AT+CWSAP_DEF="; // ssid and password length should be considered here, and should preferably not be lengthy
	
	for(int a=0; a<2; a++)
	{
		strcat(txBuf,quoteMrk);
		strcat(txBuf,chrInput[a]);
		strcat(txBuf,quoteMrk);
		strcat(txBuf,comma);
	}
		for(int a=0; a<4; a++)
	{
		IntToString(intInput[a],(uint8_t*)tempBuf);
		strcat(txBuf,tempBuf);
		if(a!=3)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();
 }
 
 
 void ConfigSoftApCur(char* ssid, char* pass, int chnl, int encr, int maxCon, int ssidHide)
 {
	  	if(encr> 4 || encr ==1)
	{
	 return;  //give error 
	}	
	if(maxCon > 8)
	{
		 return;  //give error 
	}
	
	char* chrInput[2] = {ssid,pass};// sprcial characters in pass or ssid
	int intInput[4] = {chnl,encr,maxCon,ssidHide};
	char clearLine[] = "\r\n";
	char comma[] = ",";
	char quoteMrk[] = "\"";
	char tempBuf[10] = {0}; 
	char temp = 0;
	char txBuf[100] = "AT+CWSAP_CUR="; // ssid and password length should be considered here, and should preferably not be lengthy
	
	for(int a=0; a<2; a++)
	{
		strcat(txBuf,quoteMrk);
		strcat(txBuf,chrInput[a]);
		strcat(txBuf,quoteMrk);
		strcat(txBuf,comma);
	}
		for(int a=0; a<4; a++)
	{
		IntToString(intInput[a],(uint8_t*)tempBuf);
		strcat(txBuf,tempBuf);
		if(a!=3)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, (uint8_t*)txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();
 }
 
 
	 
 void DisconnectFrmNetwork()
 {
	uint8_t temp = 0;
	uint8_t clearLine[] = "\r\n";
	uint8_t txBuf[] = "AT+CWQAP";
	strcat(txBuf,clearLine);	
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		SearchForOk();	
 }
 
 	void ListNetworks() // extract network properties 
	{
	uint8_t temp = 0;
	uint8_t clearLine[] = "\r\n";
	uint8_t txBuf[] = "AT+CWLAP";
	strcat(txBuf,clearLine);	
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		SearchForOk();	
	}
	
 void ApListOpt(int sortRSSI, int mask)
 {
		if(mask> 0x7FF)
	{
	 return;  //give error 
	}	
	uint8_t inputs[2] = {sortRSSI,mask}; 
	uint8_t clearLine[] = "\r\n";
	uint8_t temp = 0;
	uint8_t comma[] = ",";
  uint8_t tempBuf[10] = {0}; 
	uint8_t txBuf[28] = "AT+CWLAPOPT=";
	
	for(int a=0; a<2; a++)
	{
		IntToString(inputs[a],tempBuf);
		strcat(txBuf,tempBuf);
		if(!a)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
 }
 
 
 
 void ConnectToWifi(char* ssid, char* pass)
 {
	uint8_t* inputs[2] = {ssid,pass};// sprcial characters in pass or ssid
	uint8_t clearLine[] = "\r\n";
	uint8_t comma[] = ",";
	uint8_t quoteMrk[] = "\"";
	uint8_t temp = 0;
	uint8_t txBuf[100] = "AT+CWJAP_CUR=";
	
	for(int a=0; a<2; a++)
	{
		strcat(txBuf,quoteMrk);
		strcat(txBuf,inputs[a]);
		strcat(txBuf,quoteMrk);
		if(!a)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		SearchForOk();	
				
 }
	 
 void ConnectToWifiDef(char* ssid, char* pass)
 {
	uint8_t* inputs[2] = {ssid,pass};// sprcial characters in pass or ssid
	uint8_t clearLine[] = "\r\n";
	uint8_t comma[] = ",";
	uint8_t quoteMrk[] = "\"";
	uint8_t temp = 0;
	uint8_t txBuf[100] = "AT+CWJAP_DEF=";
	
	for(int a=0; a<2; a++)
	{
		strcat(txBuf,quoteMrk);
		strcat(txBuf,inputs[a]);
		strcat(txBuf,quoteMrk);
		if(!a)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		SearchForOk();	
 }
	 
 void ConnectToWifiViaMac(char* ssid, char* pass,char* mac)
 {
	uint8_t* inputs[3] = {ssid,pass,mac};// sprcial characters in pass or ssid
	uint8_t clearLine[] = "\r\n";
	uint8_t comma[] = ",";
	uint8_t quoteMrk[] = "\"";
	uint8_t temp = 0;
	uint8_t txBuf[120] = "AT+CWJAP_CUR=";
	
	for(int a=0; a<3; a++)
	{
		strcat(txBuf,quoteMrk);
		strcat(txBuf,inputs[a]);
		strcat(txBuf,quoteMrk);
		if(a!=2)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		SearchForOk();	
				
 }
 
 void ConnectToWifiViaMacDef(char* ssid, char* pass,char* mac)
 {
	uint8_t* inputs[3] = {ssid,pass,mac};// sprcial characters in pass or ssid
	uint8_t clearLine[] = "\r\n";
	uint8_t comma[] = ",";
	uint8_t quoteMrk[] = "\"";
	uint8_t temp = 0;
	uint8_t txBuf[120] = "AT+CWJAP_DEF=";
	
	for(int a=0; a<3; a++)
	{
		strcat(txBuf,quoteMrk);
		strcat(txBuf,inputs[a]);
		strcat(txBuf,quoteMrk);
		if(a!=2)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		SearchForOk();	
				
 }
 
 void SetCurWifiMode(int mode)
 {
		 	if(mode> 3)
	{
	 return;  //give error 
	}	
	
	uint8_t clearLine[] = "\r\n";
	uint8_t temp = 0;
  uint8_t tempBuf[10] = {0}; 
	IntToString(mode,tempBuf);
	uint8_t txBuf[] = "AT+CWMODE_CUR=";
  strcat(txBuf,tempBuf);
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
 }
 
 
 void SetDefWifiMode(int mode)
 {
	 	if(mode> 3)
	{
	 return;  //give error 
	}	
	
	uint8_t clearLine[] = "\r\n";
	uint8_t temp = 0;
  uint8_t tempBuf[10] = {0}; 
	IntToString(mode,tempBuf);
	uint8_t txBuf[] = "AT+CWMODE_DEF=";
  strcat(txBuf,tempBuf);
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
 }
 
 
 
 void SetDefSysMsg(int mode)
 {
		 	if(mode> 3)
	{
	 return;  //give error 
	}	
	
	uint8_t clearLine[] = "\r\n";
	uint8_t temp = 0;
  uint8_t tempBuf[10] = {0}; 
	IntToString(mode,tempBuf);
	uint8_t txBuf[] = "AT+SYSMSG_DEF=";
  strcat(txBuf,tempBuf);
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
				
 }
 
 
 void SetCurSysMsg(int mode)
 {
		 	if(mode> 3)
	{
	 return;  //give error 
	}	
	
	uint8_t clearLine[] = "\r\n";
	uint8_t temp = 0;
  uint8_t tempBuf[10] = {0}; 
	IntToString(mode,tempBuf);
	uint8_t txBuf[] = "AT+SYSMSG_CUR=";
  strcat(txBuf,tempBuf);
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		ReceiveOk();	
				
 }
 void WakeGpioSet(int enable, int trigGpio, int trigLevel)
 {
		if ( ( trigLevel>1) || (enable>1) || (trigGpio >15) )
	{
	  return; // give error
	}
	
  uint8_t txBuf[25] = "AT+WAKEUPGPIO=";
	int inArr[3] = {enable,trigGpio,trigLevel};
  uint8_t clearLine[] = "\r\n";
	uint8_t comma[] = ",";
	uint8_t temp = 0;
  uint8_t tempBuf[10] = {0}; 
	
		if(!enable)
	{
	  strcat(txBuf,"0\r\n");
		goto send;
	}
	
	for(int a =0; a<3; a++)
	{
	IntToString(inArr[a],tempBuf);
  strcat(txBuf,tempBuf);
	if(a!=2)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);

	send:
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
				ReceiveOk();	
 }
 
 void SleepModeQuery(int* mode)
 {
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+SLEEP?\r\n"; 
	char recBuffer[15] = {0};
	char colon = ':';
	char* add = recBuffer;

	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
		
		while(*add++ != colon)
		{	}
		
		*mode = StringToInt(add,1);
 }
 
 void SleepModeSet(int mode)
 {
	 	if(mode> 2)
	{
	 return;  //give error 
	}	
	
	uint8_t clearLine[] = "\r\n";
	uint8_t temp = 0;
  uint8_t tempBuf[10] = {0}; 
	IntToString(mode,tempBuf);
	uint8_t txBuf[] = "AT+SLEEP=";
  strcat(txBuf,tempBuf);
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
				ReceiveOk();	
 }
	  
 void UartCurConfigSet(unsigned int baudrate, unsigned int dataBits, unsigned int stopBits, unsigned int parity, unsigned int flowControl)
 {
	if(baudrate> 115200*40)
	{
	 return;  //give error 
	}		
	if ((dataBits<5 && dataBits>8) || (stopBits<1 && stopBits>3) || (parity>2) || (flowControl >4) )
	{
	  return; // give error
	}
	int inArr[5] = {baudrate,dataBits,stopBits,parity,flowControl};
  uint8_t clearLine[] = "\r\n";
	uint8_t comma[] = ",";
	uint8_t temp = 0;
  uint8_t tempBuf[10] = {0}; 
	uint8_t txBuf[27] = "AT+UART_CUR="; // have to make a big array because the second array is used more than once and char replaced, so confusing for compiler, better practice to make a big array beforehand
	
	for(int a =0; a<5; a++)
	{
	IntToString(inArr[a],tempBuf);
  strcat(txBuf,tempBuf);
	if(a!=4)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);

	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
				ReceiveOk();	
 }
 
  void UartDefConfigSet(unsigned int baudrate, unsigned int dataBits, unsigned int stopBits, unsigned int parity, unsigned int flowControl)
 {
	if(baudrate> 115200*40)
	{
	 return;  //give error 
	}		
	if ((dataBits<5 && dataBits>8) || (stopBits<1 && stopBits>3) || (parity>2) || (flowControl >4) )
	{
	  return; // give error
	}
	int inArr[5] = {baudrate,dataBits,stopBits,parity,flowControl};
  uint8_t clearLine[] = "\r\n";
	uint8_t comma[] = ",";
	uint8_t temp = 0;
  uint8_t tempBuf[10] = {0}; 
	uint8_t txBuf[27] = "AT+UART_DEF="; // have to make a big array because the second array is used more than once and char replaced, so confusing for compiler, better practice to make a big array beforehand
	
	for(int a =0; a<5; a++)
	{
	IntToString(inArr[a],tempBuf);
  strcat(txBuf,tempBuf);
	if(a!=4)
		strcat(txBuf,comma);
	}
	strcat(txBuf,clearLine);

	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
				ReceiveOk();	
 }
 

void IntToString(int number,uint8_t* str)
{
	int tempnum = number, digits =0, length=0;
	if(!number)
	{
	 *str = 0+'0';
		*(str+1) = 0;
		return;
	}
	while(tempnum)
	{
		digits++;
		tempnum /= 10;
	}
	length = digits;
	while(number)
	{
		*(str+digits-1) = number%10+'0';
		number/=10;
		digits--; 
	}
	*(str+length) = 0;
}

void DeepSleep(int timeMs)
{
	uint8_t clearLine[] = "\r\n";
	uint8_t temp = 0;
  uint8_t tempBuf[10] = {0}; 
	IntToString(timeMs,tempBuf);
	uint8_t txBuf[] = "AT+GSLP=";
  strcat(txBuf,tempBuf);
	strcat(txBuf,clearLine);
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
				ReceiveOk();	
}


void Echo(bool state)
{
	uint8_t txBuf[] = "ATE0\r\n";
	if(state)		
		txBuf[3] = '1';
	uint8_t temp = 0;
	UART_WriteBlocking(UART_ESP, txBuf, sizeof(txBuf)-1);
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
				SearchForOk();	
}

bool SanityCheck()
{
	uint8_t txBuf[] = "AT\r\n";
	SendbufferUart(txBuf, sizeof(txBuf)-1);
	SearchForOk();	
}

void Reset()
{
	uint8_t txBuf[] = "AT+RST\r\n";
	SendbufferUart(txBuf, sizeof(txBuf)-1);
	ReceiveReady();
}

void SearchForOk()
{
	uint8_t checkBuf[] = "OK\r\n";
	SearchForString(checkBuf,sizeof(checkBuf)-1);
}

void ReceiveOk()
{
	uint8_t checkBuf[] = "\r\nOK\r\n";
	uint8_t errorBuf[] = "\r\nERROR";
	uint8_t recBuf[6] = {0};
	uint8_t dataError = 0;
  bool error = false;
	
		
		for(int a =0; a<6;a++)
		{
				ReceiveByteUart(&recBuf[a]);
				if(recBuf[a] != checkBuf[a])
					dataError++;
			}
		
			for(int a=0;a<6;a++)
			{
				if(recBuf[a] == errorBuf[a])
					error = true;
				else 
					break;	
			}
			
		if(dataError || error)
		{
			// do stuff for error
		}
	
}

bool ReceiveReady()
{
  uint8_t checkBuf[] = "ready\r\n";
//	uint8_t rxBuf[lenBuff] = {0}; 
//	uint8_t* address = rxBuf+6;
//	uint8_t checkIndex = lenBuff-1;
	bool ready = false;

	SearchForString(checkBuf,sizeof(checkBuf)-1);
	ready = true;
//	while(!ready)
//	{
//		while(!(UART1->S1 & UART_S1_RDRF_MASK))
//			{}
//				*(address) = UART1->D;
//				(void)UART1->S1;
//			 	
//				for (int a=0;a<lenBuff;a++)
//				{
//				  if(checkBuf[checkIndex] == rxBuf[checkIndex])
//						checkIndex--;
//					else
//					{
//					  checkIndex=lenBuff-1;
//						break;
//					}			
//					if(a==6)
//						ready = true;
//				}
//				
//				for(int a=0;a<6;a++)
//				{
//					rxBuf[a] = rxBuf[a+1] ;
//				}
//			}
	return ready;
}

void SearchForString(uint8_t* string,int length)
{ 
	uint8_t* add = (uint8_t*)calloc(length,sizeof(uint8_t));
	uint8_t* tempRecAdd = add+length-1;
	uint8_t* recAdd = tempRecAdd;
	uint8_t* tempOrigAdd = string+length-1;
	bool found = false;

  while(!found)
	{
//		while(!(UART1->S1 & UART_S1_RDRF_MASK))
//			{}
//				*(recAdd) = UART1->D;
//				(void)UART1->S1;
			 	ReceiveByteUart(recAdd);
		
				for (int a=0;a<length;a++)
				{
				  if(*tempOrigAdd == *tempRecAdd)
					{	
						tempOrigAdd--;	
						tempRecAdd--;
					}
					else
					{
					  tempOrigAdd = string+length-1;
						tempRecAdd = add+length-1;
						break;
					}			
					if(a==length-1)
						found = true;
				}		
				for(int a=0;a<length-1;a++)
				{
					*(add+a) = *(add+a+1);
				}
			}
	free(add);
}

int StringToInt(char* inpStr, int size)
{
	int retval = 0;
	int temp = retval;
	int mult= 1;
	
	if(*inpStr == 45)
	{
	  size--;
		inpStr++;
		mult = -1;
	}
	
	for(int a =0; a<size; a++)
	{
		temp = pow(10,a);
	  retval += (inpStr[size-a-1]-'0')*temp;
	}
	return retval*mult;
	
}

void UartDefQuery(int* baudRate,int* dataBits, int* stopBits, int* parity, int* flowControl)
{
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+UART_DEF?\r\n"; 
	char recBuffer[18] = {0};
	char* add = recBuffer;
	int index = 0;
	char comma = ',';
	int* output[5] = {baudRate,dataBits, stopBits, parity, flowControl};
	int outcnt = 0;
	char colon = ':';
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);
		
	 while(*add++ != colon)
		{	}
		
		while (add[index++] != CR)
		{
				if(add[index-1] == comma)
				{
					*output[outcnt++] = StringToInt(add,index-1);
					add += index;
					index = 0;
				}
		}
}


void UartCurQuery(int* baudRate,int* dataBits, int* stopBits, int* parity, int* flowControl)
{
	uint8_t temp = 0;
	uint8_t CR = 13;
	char txBuf[] = "AT+UART_CUR?\r\n"; 
	char recBuffer[30] = {0};
	char* add = recBuffer;
	int index = 0;
	char comma = ',';
	int* output[5] = {baudRate,dataBits, stopBits, parity, flowControl};
	int outcnt = 0;
	char colon = ':';
	
	UART_WriteBlocking(UART_ESP, txBuf, strlen(txBuf));
		while(!(UART1->S1 & UART_S1_TC_MASK))
			{}
		(void)UART1->S1;
				temp = UART1->D;
				
		RecQuery(recBuffer);

		while(*add++ != colon)
		{	}
		
		while (add[index++] != CR)
		{
				if(add[index-1] == comma)
				{
					*output[outcnt++] = StringToInt(add,index-1);
					add += index;
					index = 0;
				}
		}
}

void RecQuery(char* recBuf )
{
	char errorBuf[5] = "ERROR";
	uint8_t CR = 13;
	int index = 0;
  bool error = false;

		while(recBuf[index-1] != CR)
			ReceiveByteUart(&recBuf[index++]);
		
			for(int a=0;a<6;a++)
			{
				if(recBuf[a] == errorBuf[a])
					error = true;
				else 
					break;	
			}
			
		if( error)
		{
			// do stuff for error
		}
}

void RecTillOk(char* recBuf)
{
	char errorBuf[5] = "ERROR";
	char ok[] = "OK";
	uint8_t CR = 13;
	int index = 0;
  bool error = false;

		
		while(recBuf[index-1] != *ok)
			ReceiveByteUart(&recBuf[index++]);
		
			for(int a=0;a<6;a++)
			{
				if(recBuf[a] == errorBuf[a])
					error = true;
				else 
					break;	
			}
			
		if( error)
		{
			// do stuff for error
		}
}



void SendbufferUart(char* buffer, int size)
{
	uint8_t temp = 0;
	UART_WriteBlocking(UART_ESP, buffer, size);
		while(!(UART_ESP->S1 & UART_S1_TC_MASK))
			{}
		(void)UART_ESP->S1;
		temp = UART1->D;
}
	
void ReceiveByteUart(uint8_t* recByte)
{
		while(!(UART1->S1 & UART_S1_RDRF_MASK))
			{}
			*recByte = UART1->D;
			(void)UART_ESP->S1;
}
	
//uint8_t* IntToString(int number)
//{
//	int tempnum = number, digits =0, length=0;
//	while(tempnum)
//	{
//		digits++;
//		tempnum /= 10;
//	}
//		uint8_t* arrPoint = (uint8_t*)calloc(digits+1,sizeof(uint8_t));  // free this pointer ahead 
//	length = digits;
//	
//	while(number)
//	{
//		*(arrPoint+digits-1) = number%10+'0';
//		number/=10;
//		digits--; 
//	}
//  *(arrPoint+length) = 0;
//	return arrPoint;
//}

/* EOF */