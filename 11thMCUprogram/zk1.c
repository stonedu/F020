///////////////mian()////////////////////////
////////simple ad/da translation function////
/////////////////////////////////////////////
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include "init.h"

//
typedef unsigned char uchar; 
typedef unsigned int  uint;
typedef unsigned long ulong;

//18B20 
#define  DataPort_Set1  (P4|=0x01)
#define  DataPort_Set0  (P4&=0xFE)
#define  DataPort_Status (P4&0x01)

//ADDR Setting
#define A0  (P5&0x01)  //P5��Ϊ���뿪��
#define A1  (P5&0x02)
#define A2  (P5&0x04)
#define A3  (P5&0x08)
#define baud_mode (P5&0xf0)  //����ģʽ

#define ENQ 0x05  //ѯ��
#define ACK 0x06  //ȷ��
#define NAK 0x15  //����
#define EOT 0x04  //���ͽ���
#define ETX 0x03  //Ӧ�����
#define DB  0x01  //�ֽ�
#define DW  0x02  //��
#define DF  0x03  //����
#define BufMax 30 //�����ݸ���
#define time0  -100000

#define P1_ADDR 	0  //P1����̬���ĵ�ַ
#define P2_ADDR 	2  //P2�ڶ�Ӧ����̬���ĵ�ַ
#define DA0_ADDR 	20 //DA0��Ӧ����̬���ĵ�ַ
#define DA1_ADDR 	22
#define AD_ADDR 	4  //AD�ڶ�Ӧ����̬���еĵ�ַ
#define LIGHT_ADDR 	24 //DI�ڶ�Ӧ����̬���еĵ�ַ
#define Start 0x40
#define End 0x0d
#define Read 0x00
#define Write 0x01
#define labStart 0x80
#define labEnd 0xf0
////////////////matlab//////////////////////
#define BufferLength 32 
#define matStart 0x46   //f
#define matEnd1 0x46   //f
#define matEnd2 0x45   //e
//#define matRead 0x01
//#define matWrite 0x02
uchar xdata matRbuf[BufferLength]={0}; 
uchar xdata matTbuf[BufferLength]={0}; 
uchar matrecok=0;
uchar matbuf0;
uchar matbuf1;
uchar matwei=0;
////////////////labview//////////////////////
uchar xdata labRbuf[BufferLength]={0}; 
uchar xdata labTbuf[BufferLength]={0};
uchar labrecok=0;
uchar labbuf0;
uchar labwei=0;
//uchar aaaaa;
////////////////////////ASC////////////////////////////////////
uchar xdata ceshibuf[BufMax];
uchar xdata recbuf[BufMax];
uchar xdata sendbuf[BufMax];
uchar xdata Answer[BufMax];
uchar dat[10]=(1,2,3,4,5,6,7,8);
bit recok=0;
bit  Startrec=0;
uchar count=0;
uchar flag1=0;
bit ceshi;
uchar former1,former2;
void Write_byte(void);
bit check_CRC(void);
void Read_byte(void);
void write_inform(uchar dat);
uchar Dat_trans(uchar hight_v,uchar low_v);
uchar read_write_flag(void);
uchar HEXASC_high(uchar tempv);
uchar HEXASC_low(uchar tempv);
uchar MyAddr1,MyAddr2;
uchar matjishu;
//////////////////////////////modbus///////////////////////////////////////
uchar xdata modRbuf[BufMax];
uchar xdata modTbuf[BufMax];
uchar modrecok=0;
uchar modwei=0;
uchar matrecflag=0;
uchar timeoutcnt=0;
////////////////////////////////////////////////////////////
uchar RecvOk,CRCok;   //��־λ

uchar Rptr,Tptr,Tnum;
uchar MyAddr=0x00,DelayMs,last,ReadMe;
uchar  xdata Rbuf[BufMax],Tbuf[BufMax];

uchar WorkMode;   

uchar flag=0;    //���ʶ���־λ


uchar TH,TL;

uchar rd_data[8];     //����rd_data[8]�д�Ŵ�DS18B20�ж���������
//................//


sfr16   ADC0VAL=0xbe;


//Initialization function for device
//void Init_Device(void);  

//ADC0ת������
void ADC_Start(uchar source); 

//UARTͨѶ����
void UART_Send(uchar sdata);

//18B20 ����0-���豸����1-���豸����
bit RstDS1820(void);

//18B20 ��18B20����
unsigned char ReadDS1820(void);	

//18B20 д18B20����
void WriteDS1820(unsigned char ch);

//18B20 ���������������кŵĲ�������:0xcc
void SkipROMCode(void);

//18B20 ��ʼ��
void Init18b20(void);

//18B20 ����18B20ת��
void StartDS(void);

//18B20 ��ȡ�¶�ֵ�ĳ���
unsigned int GetTempValue(void);

//��ȡ�����ַ
void GetAddr(void);

//��ȡ���������ò���
void SetBaud(void);

void CheckCRC();

//UARTͨѶ�������ݰ�
void UART0_SendBuf(uchar* buf, uchar len);

void time1();    
uint crc16(uchar*str,uint num);/////////////////crc16
uint erheyi(uchar high,uchar low);///////��λ�͵�λ�ϳ�

//18B20 �й���ʱ
void Delay1us(unsigned char us);

void Delay15us(void);

void Delay10us(void);

void sleep_ms(unsigned int count);      //��ʱ



	

//main()
void main()
{
    uchar tmpCommand=0;  //0 is default,1 is da ;2 is ad
	uchar tmpAddr = 0x00;
	uchar value1,value2;
    uchar P1_Status,P2_Status,P6_Status;
	uchar temp,ctmp1,ctmp2,xordat,send_temp,send_temp1,send_temp2;/////////tian
	uchar x,tempw,tempw1,tempw2,dizhiw,read_write,dizhir,ko;//////tian
//	uint v1,v2,v3,v;
	uint w,crct,crcr;
	
//	uchar kan1,kan2,kan3,kan4;
//	uint zhuan;
	uchar n,i,*p,crc,k,num;
  	uint ptr;
  	Tptr=0; Tnum=0;  Rptr=0; 
  	ReadMe=0;	RecvOk=0; MyAddr=0;
	
	matrecok=0;///////////
	labrecok=0;
	modrecok=0;
    //////////////////////////////////////////////////////
	ceshibuf[0]=0x00;
	ceshibuf[1]=0x03;
	ceshibuf[2]=0x00;
	ceshibuf[3]=0x00;
	ceshibuf[4]=0x00;
	ceshibuf[5]=0x01;

	crct=crc16(ceshibuf,6);
	config();

	GetAddr();

	SetBaud();

	if(baud_mode==0x80) //0001 VB
	{
		EA=0;
		ES0=0; 
		SCON0 = 0x50;   //��У��
		WorkMode = 1;//115200,vb 
		Init18b20();//��ʼ��18b20
    	w = GetTempValue();            //�����¶�ֵ��ԭʼ���ݴ���W
	}
	else if(baud_mode==0x00) //0000 ��̬��hex
	{
		EA=1;
		ES0=1; 
		SCON0 = 0xD0; //��У��
		Init18b20();//��ʼ��18b20
		WorkMode = 0;//9600,��̬��
	}
	else if(baud_mode==0x40)   //0010 ��̬��ASC
	{	
		EA=1;
		ES0=1;
		SCON0=0xD0;//��У��
	
		Init18b20();//��ʼ��18b20
	//	SM2=1;
		WorkMode = 2;//9600,��̬��ASC
	}
	else if(baud_mode==0xc0) //0011 MATLAB
	{
		EA=1;
		ES0=1;
		SCON0=0x50;//��У��
	
		Init18b20();//��ʼ��18b20
		WorkMode = 3;

		//////////////////////////////
		TMOD=0x10;//��ʱ��1 16λ    4ms
		TH1=0xf0;//9600��������3.5���ֽڵ�ʱ���ԼΪ3.6MS��������������Ϊ4ms
		TL1=0x5f;
	//	TL0=TH0=0xFF;
		ET1=1;
	//	TR1=1;
		PS=1; //�ж�����
	}
	else if(baud_mode==0x20) //0100 LABVIEW
	{
		EA=1;
		ES0=1;
		SCON0=0x50;//��У��
	
		Init18b20();//��ʼ��18b20
		WorkMode = 4;
	}
	else if(baud_mode==0xa0) //0101 �л�modbus
	{
		EA=1;
		ES0=1;
		SCON0=0x50;//��У��
	
		Init18b20();//��ʼ��18b20
		WorkMode = 5;
		
		TMOD=0x10;//��ʱ��1 16λ    4ms
		TH1=0xf0;//9600��������3.5���ֽڵ�ʱ���ԼΪ3.6MS��������������Ϊ4ms
		TL1=0x5f;
	//	TL0=TH0=0xFF;
		ET1=1;
	//	TR1=1;
		PS=1; //�ж�����
	}
	else if(baud_mode==0xf0) //1111 VB���Կ�
	{
		EA=0;
		ES0=0; 
		SCON0 = 0x50;   //��У��
		WorkMode = 6;//115200,vb 
		Init18b20();//��ʼ��18b20
    	w = GetTempValue();            //�����¶�ֵ��ԭʼ���ݴ���W
	}
	else if(baud_mode==0x70) //1110  matlab simulink
	{
		EA=0;
		ES0=0; 
		SCON0 = 0x50;   //��У��
		WorkMode = 7;//115200,matlab simulink
	//	Init18b20();//��ʼ��18b20
    //	w = GetTempValue();            //�����¶�ֵ��ԭʼ���ݴ���W
	}
	else if(baud_mode==0x60) //0110 Modbus RTU��DAAD��
	{
		EA=1;
		ES0=1;
		SCON0=0x50;//��У��
	
		Init18b20();//��ʼ��18b20
		WorkMode = 8;
		
		TMOD=0x10;//��ʱ��1 16λ    4ms
		TH1=0xf0;//9600��������3.5���ֽڵ�ʱ���ԼΪ3.6MS��������������Ϊ4ms
		TL1=0x5f;
	//	TL0=TH0=0xFF;
		ET1=1;
	//	TR1=1;
		PS=1; //�ж�����
	}
	/////////////////////////////////////////////////
//    Init_Device();	     //initialization device
	DAC0L=0x00;
	DAC0H=0x08;          //��ʼ��da���ֵ
	DAC1L=0x00;
	DAC1H=0x08;          //��ʼ��da���ֵ
	P6 = 0xFF;
	//SCON0 = SCON0 | 0x20;



	
	/***************************   VB   *************************************/
	while(WorkMode==1)  //VBģʽ
	{
		while(RI0==0) {}
		RI0=0;
		tmpAddr=SBUF0; //���������ַ
		if(tmpAddr == MyAddr)
		{
		     while(RI0==0){}
			 RI0=0;
			 tmpCommand=SBUF0; //����������
			 if(0<=tmpCommand)
			 {
			     while(RI0==0){}
				 RI0=0;
				 value1=SBUF0;
				 while(RI0==0){}
				 RI0=0;
				 value2=SBUF0;
				 if(tmpCommand==0)		//daת�� 0ͨ��
				 {
				     DAC0L=value2;		//��λ
					 DAC0H=value1;      //��λ
                     UART_Send(0x55);
					 UART_Send(0x55);
				 }
				 else if(tmpCommand==1)  //daת�� 1ͨ��
				 {
				     DAC1L=value2;		//��λ
					 DAC1H=value1;		//��λ
					 UART_Send(0x55);
					 UART_Send(0x55);
				 }
				 else if(tmpCommand==2)	 //adת��		 
				 {
				     ADC_Start(value2);
					// v1=ADC0VAL;
				    //ADC_Start(value2);
					 //v2=ADC0VAL;
				     //ADC_Start(value2);
					// v3=ADC0VAL;
					// v=(v1+v2+v3)/3;
					 UART_Send(ADC0H);
					 UART_Send(ADC0L);
				 }
				 else if(tmpCommand==3)	 //DO
				 {
				 	P6 = value2;
					UART_Send(0x55);
					UART_Send(0x55);
				 }
				 else if(tmpCommand==4)	 //DI P1������������
				 {
				 	P1_Status = P1;
                    UART_Send(0);
					UART_Send(P1_Status);
				 }
				 else if(tmpCommand==5)	 //DI P2������������
				 {
				 	P2_Status = P2;
                    UART_Send(0);
					UART_Send(P2_Status);
				 }
				 else if(tmpCommand==6)	 //DI P6������������
				 {
				 	P6_Status = P6;
                    UART_Send(0);
					UART_Send(P6_Status);
				 }
				 else if(tmpCommand==0x30)  //��ȡ18B20������
				 {
				 	w = GetTempValue();            //�����¶�ֵ��ԭʼ���ݴ���W
					UART_Send(TH);
					UART_Send(TL);
				 }
				 else if(tmpCommand==0x40)  //TFT��ʾˢ��
				 {
				 	UART_Send(0x55);
					UART_Send(0x55);
				 }
				 else if(tmpCommand==0xFF)	 //�ڵ����
				 {
				 	if(value1==0xFF && value2==0xFF)
					{
						UART_Send(0xAA);
						UART_Send(0xAA);
					}
				
				 }
				 
			 }
		 }
		 else
		 {
		 	while(RI0==0){}
			 RI0=0;
			 tmpCommand=SBUF0; 
			 while(RI0==0){}
			 RI0=0;
			 value1=SBUF0;
			 while(RI0==0){}
			 RI0=0;
			 value2=SBUF0;
		 }
	}
	/*************************** HEX ��̬�� *************************************/
	while(WorkMode==0) //��̬��ģʽ  hex
	{
		if(RecvOk)
		{	
			RecvOk=0;
		//	SCON0 = SCON0 | 0x20;
			CheckCRC();
 	   		if(Rbuf[0]==MyAddr && CRCok==1)  //��PC�Ĳ�ѯ�������Ӧ��
	   		{
	      		Tbuf[0]=ACK; Tbuf[1]=MyAddr; Tbuf[2]=ETX; 
          		crc=0;  
				for(i=0;i<3;i++) crc^=Tbuf[i];
		  		Tbuf[3]=crc; 
		  		Tptr=0; Tnum=4;
				UART0_SendBuf(Tbuf, Tnum);
		  		//DelayMs=10; ReadMe=1; 
	   		}
	   		else if(CRCok==1 && flag==1)     //��Ӧ���������ݴ��Ϳ���
	   		{	  							//���Ƕ���д֡ʱ����Ҫ�ж�һ����һ֡�е��豸��ַ�Ƿ����Լ����豸������������ݣ����򲻷���	
				flag=0;
          		ReadMe=2;
		      	n=Rbuf[4];
			  	ptr=Rbuf[3]<<8 | Rbuf[2];  
		      	if(Rbuf[0]=='R')
	      		{
		     		switch(Rbuf[1])
		     		{
			   			case DB:num=n; 
	         
							Tbuf[3] = ~P1;
							Tbuf[4] = ~P2;
							if(Rbuf[2]==0)
							{Tbuf[3] = ~P6;}
			           		break;
			   			case DW:num=n/2; 	
			           		for(i=0;i<num;i++)
					   		{
					     		k=i*4;   //6.55��6.53������
								ADC_Start(i);
					     		Tbuf[3+k]=ADC0L;
					     		Tbuf[4+k]=ADC0H;
					   		}
							if(Rbuf[2]==30)//������¶�����������ִ���������
                            {
				            	w = GetTempValue();            //�����¶�ֵ��ԭʼ���ݴ���W
								//Tbuf[3]=0x6F;
								//Tbuf[4]=0xFE;
								Tbuf[3]=TL;
								Tbuf[4]=TH;								
								Tbuf[5]=0;
								Tbuf[6]=0;
							}
			           		break;
			   			case DF:num=n/4; 
			           		
			           		for(i=0;i<num;i++)
					   		{
					     		k=4*i;
					     		Tbuf[6+k]=p[k];
					     		Tbuf[5+k]=p[k+1];
					     		Tbuf[4+k]=p[k+2];
					     		Tbuf[3+k]=p[k+3];
					   		}
			           		break;
			 		}
	         		Tbuf[0]=ACK; Tbuf[1]=n&0xff; Tbuf[2]=n>>8; 
			 		Tbuf[3+n]=ETX; 
             		crc=0;  
					for(i=0;i<4+n;i++) crc^=Tbuf[i];
			 		Tbuf[4+n]=crc;
			 		Tptr=0; Tnum=n+5; 
					UART0_SendBuf(Tbuf, Tnum);
		  		}
		  		else if(Rbuf[0]=='W')
		  		{
             		n=Rptr-6;
		     		switch(Rbuf[1])//�ж�Ҫд����������
		     		{
			   			case DB: 
						    //if(Rbuf[2]==1)
			           		{P6 = (~Rbuf[4]);}
			           		break;
			   			case DW:
						    if(Rbuf[2]==DA0_ADDR)
			           		{
								//zhuan=<<8 | 
								//zhuan=zhuan*400+6147;
								DAC0L=Rbuf[4];//zhuan & 0xff;
								DAC0H=Rbuf[5];//zhuan >> 8;
							}
 							if(Rbuf[2]==DA1_ADDR)
			           		{
								//zhuan=Rbuf[5]<<8 | Rbuf[4];
								//zhuan=zhuan*400+6147;								
								DAC1L=Rbuf[4];//zhuan & 0xff;
								DAC1H=Rbuf[5];//zhuan >> 8;
							}

			           		break;
			   			case DF:num=n/4;
                       		
			           		for(i=0;i<num;i++)
					   		{	
					     		k=4*i;
					     		p[k+3]=Rbuf[k+4];
					     		p[k+2]=Rbuf[k+5];
					     		p[k+1]=Rbuf[k+6];
					     		p[k]=Rbuf[k+7];
					   		}
			           		break;
			 		}
	         		Tbuf[0]=ACK; Tbuf[1]=0; Tbuf[2]=ETX; 
             		crc=0;  
					for(i=0;i<3;i++) crc^=Tbuf[i];
			 		Tbuf[3]=crc;
			 		Tptr=0; Tnum=4; 
					UART0_SendBuf(Tbuf, Tnum);
		  		}
	   		}
		}
	
	}
	/*************************** ASC ��̬�� *************************************/	
	while(WorkMode==2)   //   ASC ��̬��
	{
		
			if(recok)
			{
			
	//			SM2=0;
			//	recok=0;
				temp= Dat_trans(recbuf[1],recbuf[2]);
			if(temp==MyAddr)//&&check_CRC()
				{	
			
					read_write=read_write_flag()&0x01;
				if(read_write==Read) 
				{
					n=Dat_trans(recbuf[9],recbuf[10]);//ȡ�ֽ���
					x=read_write_flag()&0x0c;    //��д���������ֽڻ��ַ
					switch(x)
					{
						case 0x00:num=n;
								if(Dat_trans(recbuf[7],recbuf[8])==0x18) //dizhi
							{
								send_temp1 = ~P1;
							 	sendbuf[5]=HEXASC_high(send_temp1);
								sendbuf[6]=HEXASC_low(send_temp1);
							}
							if(Dat_trans(recbuf[7],recbuf[8])==0x19)
							{
								send_temp1 = ~P2;
							 	sendbuf[5]=HEXASC_high(send_temp1);
								sendbuf[6]=HEXASC_low(send_temp1);
							}

							if(Dat_trans(recbuf[7],recbuf[8])==0)
							{
								send_temp1 = ~P6;
							 	sendbuf[5]=HEXASC_high(send_temp1);
								sendbuf[6]=HEXASC_low(send_temp1);
							}
								break;
						case 0x04:num=n/2;   //��   ����ADC0L��ADC0H��һ���ֽھ�������
							//	n=2*n;
								for(i=0;i<num;i++)
								{
								  dizhir=Dat_trans(recbuf[7],recbuf[8]);//��ַ
									if(dizhir==0x02) {ko=0;}
									if(dizhir==0x04) {ko=1;}
									if(dizhir==0x06) {ko=2;}
									if(dizhir==0x08) {ko=3;}
									if(dizhir==0x0a) {ko=4;}
									if(dizhir==0x0c) {ko=5;}
									if(dizhir==0x0e) {ko=6;}
									if(dizhir==0x10) {ko=7;}
									k=i*4;
									ADC_Start(ko);
									send_temp1=ADC0H;
					     			send_temp2=ADC0L;//asc��λ��ǰ��λ�ں�HEX��λ��ǰ����λ�ں�
									sendbuf[5+k]=HEXASC_high(send_temp1);//
									sendbuf[6+k]=HEXASC_low(send_temp1);
									sendbuf[7+k]=HEXASC_high(send_temp2);
									sendbuf[8+k]=HEXASC_low(send_temp2);
							
								//	kan1=sendbuf[5];
								}
					
								if(Dat_trans(recbuf[7],recbuf[8])==0x1e)//������¶�����������ִ���������,30��ʮ������Ϊ0x1e
                            	{
								for(i=0;i<num;i++)
								{
									k=i*4;
				            		w = GetTempValue();            //�����¶�ֵ��ԭʼ���ݴ���W
									//Tbuf[3]=0x6F;
									//Tbuf[4]=0xFE;
									send_temp1=TH;
					     			send_temp2=TL;//asc��λ��ǰ��λ�ں�HEX��λ��ǰ����λ�ں�
									sendbuf[5+k]=HEXASC_high(send_temp1);
									sendbuf[6+k]=HEXASC_low(send_temp1);
									sendbuf[7+k]=HEXASC_high(send_temp2);
									sendbuf[8+k]=HEXASC_low(send_temp2);
								
							
								}

								}
								break;
						case 0x08:num=n/4;
								{
									for(i=0;i<num;i++)
								{
									k=i*8;
									sendbuf[5+k]=0x00;
									sendbuf[6+k]=0x00;
									sendbuf[7+k]=0x00;
									sendbuf[8+k]=0x00;
									sendbuf[9+k]=0x00;
									sendbuf[10+k]=0x00;
									sendbuf[11+k]=0x00;
									sendbuf[12+k]=0x00;
								}
								}
								break;
					}
						
					/***********************��01234��ֵ***********************/
						sendbuf[0]=0x40;
						sendbuf[1]=recbuf[1];
						sendbuf[2]=recbuf[2];
						sendbuf[3]=recbuf[9];
						sendbuf[4]=recbuf[10];
						sendbuf[2*n+7]=0x0d;
						
						xordat=0;//��ż���λ�g��n+8
						for(i=1;i<(2*n+5);i++)xordat^=sendbuf[i];
						send_temp=xordat;
						ctmp1=(send_temp>>4)&0x0f;
						if(ctmp1>9)sendbuf[2*n+5]=ctmp1%9+0x40;
						else sendbuf[2*n+5]=ctmp1+0x30;
						ctmp2=send_temp&0x0f;
						if(ctmp2>9)sendbuf[2*n+6]=ctmp2%9+0x40;
						else sendbuf[2*n+6]=ctmp2+0x30;
						
				
						UART0_SendBuf(sendbuf,2*n+8);
				
						
				}
		else
				{                     //д
					n=Dat_trans(recbuf[9],recbuf[10]);//ȡ�ֽ���
					x=read_write_flag()&0x0c;    //��д���������ֽڻ��ַ
				switch(x)
					{
					case 0x00:num=n;         //�ֽ�
						tempw=Dat_trans(recbuf[11],recbuf[12]);
						P6 = (~tempw);
			      		break;
					case 0x04:num=n/2;									//�֣�û�з�λ�����������ֽ�
						dizhiw=Dat_trans(recbuf[7],recbuf[8]);
						tempw1=Dat_trans(recbuf[11],recbuf[12]);
						tempw2=Dat_trans(recbuf[13],recbuf[14]);
							if(dizhiw==0x14) //DA0_ADDR��Ϊ==20���ܼ�������20��ʮ������Ϊ���Ը�Ϊ0x14����ʵ��DA0_ADDR
							{
								
								DAC0H=tempw1;//asc��λ��ǰ��λ�ں�HEX��λ��ǰ����λ�ں�
								DAC0L=tempw2;
								DAC0H=tempw1;//51bug,�����DA��ֵ���Σ�һ�β���׼
								DAC0L=tempw2;//tempw2;
												
							}
							else if(dizhiw==0x16) //DA1_ADDRͬ��
			        		{
								DAC1H=tempw1;//tempw1;
								DAC1L=tempw2;//tempw2;
								DAC1H=tempw1;//51bug,�����DA��ֵ���Σ�һ�β���׼
								DAC1L=tempw2;//tempw2;
								
							}  //else if
						 break;
					  }//swi
					write_inform('#');
					               //Write_byte();          //break;
				}//else
			 }
			 
			else  write_inform('*' );
//				 SM2=1;
			recok=0;
			}
	
		}
	/************************matlab********************************/
		while(WorkMode==3) 
		{
		
			if(matrecok==1)
			{
			
	//			SM2=0;
			//	recok=0;
				temp= Dat_trans(matRbuf[1],matRbuf[2]);
			if(temp==MyAddr)//&&check_CRC()
				{	
			
					read_write=Dat_trans(matRbuf[3],matRbuf[4]);  //��д��־λ
				if(read_write==Read)   //00
				{
					n=Dat_trans(matRbuf[9],matRbuf[10]);//ȡ�ֽ���
					x=Dat_trans(matRbuf[5],matRbuf[6]);    //��д���������ֽڻ��ַ
					switch(x)
					{
						case 0x00:num=n;
								if(Dat_trans(matRbuf[7],matRbuf[8])==0x18) //dizhi
							{
								send_temp1 = ~P1;
							 	matTbuf[5]=HEXASC_high(send_temp1);
								matTbuf[6]=HEXASC_low(send_temp1);
							}
							if(Dat_trans(matRbuf[7],matRbuf[8])==0x19)
							{
								send_temp1 = ~P2;
							 	matTbuf[5]=HEXASC_high(send_temp1);
								matTbuf[6]=HEXASC_low(send_temp1);
							}

							if(Dat_trans(matRbuf[7],matRbuf[8])==0)
							{
								send_temp1 = ~P6;
							 	matTbuf[5]=HEXASC_high(send_temp1);
								matTbuf[6]=HEXASC_low(send_temp1);
							}
								break;
						case 0x01:num=n/2;   //��   ����ADC0L��ADC0H��һ���ֽھ�������
							//	n=2*n;
								for(i=0;i<num;i++)
								{
								  dizhir=Dat_trans(matRbuf[7],matRbuf[8]);//��ַ
									if(dizhir==0x02) {ko=0;}
									if(dizhir==0x04) {ko=1;}
									if(dizhir==0x06) {ko=2;}
									if(dizhir==0x08) {ko=3;}
									if(dizhir==0x0a) {ko=4;}
									if(dizhir==0x0c) {ko=5;}
									if(dizhir==0x0e) {ko=6;}
									if(dizhir==0x10) {ko=7;}
									k=i*4;
									ADC_Start(ko);
									send_temp1=ADC0H;
					     			send_temp2=ADC0L;//asc��λ��ǰ��λ�ں�HEX��λ��ǰ����λ�ں�
									matTbuf[5+k]=HEXASC_high(send_temp1);//
									matTbuf[6+k]=HEXASC_low(send_temp1);
									matTbuf[7+k]=HEXASC_high(send_temp2);
									matTbuf[8+k]=HEXASC_low(send_temp2);
							
								//	kan1=sendbuf[5];
								}
					
								if(Dat_trans(matRbuf[7],matRbuf[8])==0x1e)//������¶�����������ִ���������,30��ʮ������Ϊ0x1e
                            	{
								for(i=0;i<num;i++)
								{
									k=i*4;
				            		w = GetTempValue();            //�����¶�ֵ��ԭʼ���ݴ���W
									//Tbuf[3]=0x6F;
									//Tbuf[4]=0xFE;
									send_temp1=TH;
					     			send_temp2=TL;//asc��λ��ǰ��λ�ں�HEX��λ��ǰ����λ�ں�
									matTbuf[5+k]=HEXASC_high(send_temp1);
									matTbuf[6+k]=HEXASC_low(send_temp1);
									matTbuf[7+k]=HEXASC_high(send_temp2);
									matTbuf[8+k]=HEXASC_low(send_temp2);
								
							
								}

								}
								break;
						case 0x10:num=n/4;
								{
									for(i=0;i<num;i++)
								{
									k=i*8;
									matTbuf[5+k]=0x00;
									matTbuf[6+k]=0x00;
									matTbuf[7+k]=0x00;
									matTbuf[8+k]=0x00;
									matTbuf[9+k]=0x00;
									matTbuf[10+k]=0x00;
									matTbuf[11+k]=0x00;
									matTbuf[12+k]=0x00;
								}
								}
								break;
					}
						
					/***********************��01234��ֵ***********************/
						matTbuf[0]=0x46;
						matTbuf[1]=matRbuf[1];
						matTbuf[2]=matRbuf[2];
						matTbuf[3]=matRbuf[9];
						matTbuf[4]=matRbuf[10];
						matTbuf[2*n+5]=matEnd1;
						matTbuf[2*n+6]=matEnd2;
						
						
						
				
					//	UART0_SendBuf(matTbuf,2*n+7);
						UART0_SendBuf(matTbuf,13);
				
						
				}
		else
				{                     //д
					n=Dat_trans(matRbuf[9],matRbuf[10]);   //ȡ�ֽ���
					x=Dat_trans(matRbuf[5],matRbuf[6]);   //��д���������ֽڻ��ַ
				switch(x)
					{
					case 0x00:num=n;         //�ֽ�
						tempw=Dat_trans(matRbuf[11],matRbuf[12]);
						P6 = (~tempw);
			      		break;
					case 0x01:num=n/2;									//�֣�û�з�λ�����������ֽ�
						dizhiw=Dat_trans(matRbuf[7],matRbuf[8]);
						tempw1=Dat_trans(matRbuf[11],matRbuf[12]);
						tempw2=Dat_trans(matRbuf[13],matRbuf[14]);
							if(dizhiw==0x14) //DA0_ADDR��Ϊ==20���ܼ�������20��ʮ������Ϊ���Ը�Ϊ0x14����ʵ��DA0_ADDR
							{
								
								DAC0H=tempw1;//asc��λ��ǰ��λ�ں�HEX��λ��ǰ����λ�ں�
								DAC0L=tempw2;
								DAC0H=tempw1;//51bug,�����DA��ֵ���Σ�һ�β���׼
								DAC0L=tempw2;//tempw2;
												
							}
							else if(dizhiw==0x16) //DA1_ADDRͬ��
			        		{
								DAC1H=tempw1;//tempw1;
								DAC1L=tempw2;//tempw2;
								DAC1H=tempw1;//51bug,�����DA��ֵ���Σ�һ�β���׼
								DAC1L=tempw2;//tempw2;
								
							}  //else if
						 break;
					  }//swi
				        // 	write_inform('#');
						matTbuf[0]=0x46;
						matTbuf[1]=matRbuf[1];
						matTbuf[2]=matRbuf[2];
						matTbuf[3]=0x00;
						matTbuf[4]=0x01;
						matTbuf[5]=matEnd1;
						matTbuf[6]=matEnd2;
						matTbuf[7]=0x00;
						matTbuf[8]=0x00;
						matTbuf[9]=0x00;
						matTbuf[10]=0x00;
						matTbuf[11]=0x00;
						matTbuf[12]=0x00;
					

						//UART0_SendBuf(matTbuf,7);
						UART0_SendBuf(matTbuf,13);
					              
				}//else
			 }
			 
			else  //write_inform('*' );
			{
			    matTbuf[0]=0x46;
				matTbuf[1]=matRbuf[1];
				matTbuf[2]=matRbuf[2];
				matTbuf[3]=0x01;
				matTbuf[4]=0x01;
				matTbuf[5]=0x00;
				matTbuf[6]=0x00;
				matTbuf[7]=0x00;
				matTbuf[8]=0x00;
				matTbuf[9]=0x00;
				matTbuf[10]=0x00;
				matTbuf[11]=0x00;
				matTbuf[12]=0x00;
				
				

			//	UART0_SendBuf(matTbuf,7);
				UART0_SendBuf(matTbuf,13);
			}

			matrecok=0;
			}
		
		}
	/************************labview********************************/
		while(WorkMode==4)
		{
			if(labrecok==1)    
			{	
				if(labRbuf[1]==MyAddr) //dizhi
				{
					if(labRbuf[2]==Read)   //read
					{	
						n=labRbuf[5];
						switch(labRbuf[3])
						{
							case 0x00:num=n;
									  //if(labRbuf[4]==0x18)
									 labTbuf[3] = ~P1;
									 labTbuf[4] = ~P2;
									 if(labRbuf[4]==0x00)
									 {labTbuf[3]=~P6;}
									 break;
							case 0x01:num=n/2;
									  for(i=0;i<num;i++)
										{
								  			dizhir=labRbuf[4];//��ַ
											if(dizhir==0x02) {ko=0;}
											if(dizhir==0x04) {ko=1;}
											if(dizhir==0x06) {ko=2;}
											if(dizhir==0x08) {ko=3;}
											if(dizhir==0x0a) {ko=4;}
											if(dizhir==0x0c) {ko=5;}
											if(dizhir==0x0e) {ko=6;}
											if(dizhir==0x10) {ko=7;}
											k=i*4;
											ADC_Start(ko);
											labTbuf[3]=ADC0H;
					     					labTbuf[4]=ADC0L;//asc��λ��ǰ��λ�ں�HEX��λ��ǰ����λ�ں�
											
										}
									 	if(labRbuf[4]==0x1e)//������¶�����������ִ���������,30��ʮ������Ϊ0x1e
                            			{
							 			 for(i=0;i<num;i++)
										{
											k=i*4;
				            				w = GetTempValue();            //�����¶�ֵ��ԭʼ���ݴ���W
											//labTbuf[3]=0xfe;
					     				//	labTbuf[4]=0X6f;
											labTbuf[3]=TH;
					     					labTbuf[4]=TL;//asc��λ��ǰ��λ�ں�HEX��λ��ǰ����λ�ں�
										
										}
                                        }
										break;
									case 0x10:num=n/4;
										for(i=0;i<num;i++)
					   					{
					     					k=4*i;
					     					labTbuf[6+k]=0;
					     					labTbuf[5+k]=0;
					     					labTbuf[4+k]=0;
					     					labTbuf[3+k]=0;
					   					}
			           					break;
						}//switch
							
						labTbuf[0]=labStart;
						labTbuf[1]=labRbuf[1];
						labTbuf[2]=labRbuf[2];
						labTbuf[5]=labEnd;
						UART0_SendBuf(labTbuf,6);
						labrecok=0;
					}//read
				else  //write
					{
		     			switch(labRbuf[3])//�ж�Ҫд����������
		     			{
			   				case 0x00: 
						   		 //if(labRbuf[4]==0x00)
			           			{P6 = (~labRbuf[6]);}
			           				break;
			   				case 0x01:
						    	if(labRbuf[4]==0x14)  //DA0
			           			{
								
									DAC0H=labRbuf[6];
									DAC0L=labRbuf[7];
									DAC0H=labRbuf[6];
									DAC0L=labRbuf[7];
								}
 								if(labRbuf[4]==0x16)  //da1
			           			{
													
									DAC1H=labRbuf[6];
									DAC1L=labRbuf[7];
									DAC1H=labRbuf[6];
									DAC1L=labRbuf[7];
								}

			           			break;
			   				case 0x10:num=n/4;
                       			break;
			           		
			           	}//switch		
			 		
	         		labTbuf[0]=labStart; 
					labTbuf[1]=labRbuf[1];
             	
					labTbuf[2]=0x00;
					labTbuf[3]=0x00;
					labTbuf[4]=0x00;
					labTbuf[5]=labEnd;
				
					UART0_SendBuf(labTbuf,6);
					labrecok=0;
					}


				}//addr
			
			}//rec
		}//4

	/**************************�л�modbus***********************************/
		while(WorkMode==5)
		{
			if(modrecok==1)
			{
				if(modRbuf[0]==MyAddr && modRbuf[1]==0x03) //dizhi 
				{
					
					crcr=erheyi(modRbuf[7],modRbuf[6]);/////////////////crc16
					
					if(crc16(modRbuf,6)==crcr)
					{

						if(modRbuf[3]+modRbuf[5]>8 || modRbuf[2]>0 || modRbuf[4]>0)
						{
							modTbuf[0]=modRbuf[0];
							modTbuf[1]=modRbuf[1]|0x80;
							modTbuf[2]=0;
							crct=crc16(modTbuf,3);//����Ҫ��
							modTbuf[3]=crct&0x00ff;//bufλ�ø�
							modTbuf[4]=crct>>8;	
							//UART0_SendBuf(modTbuf,5);						
						}
						else
						{
							for(i=0;i<modRbuf[5];i++)
							{
								ADC_Start(modRbuf[3]+i);
								modTbuf[3+i*2]=(ADC0H<<4)+(ADC0L>>4);//12wei >>16wei
						     	modTbuf[4+i*2]=ADC0L<<4;
							}
							modTbuf[0]=modRbuf[0];
							modTbuf[1]=modRbuf[1];
							modTbuf[2]=modRbuf[5]*2;
							crct=crc16(modTbuf,3+modRbuf[5]*2);//����Ҫ��
							modTbuf[3+modRbuf[5]*2]=crct&0x00ff;//bufλ�ø�
							modTbuf[4+modRbuf[5]*2]=crct>>8;
							UART0_SendBuf(modTbuf,5+modRbuf[5]*2);//���͸���Ҫ��
						}
					}//crc
				}//addr
				modrecok=0;
			}//recok
	
		}//5
	/************************VB���Կ�*******************************/
		while(WorkMode==6)
		{
		 while(RI0==0){}
		 RI0=0;
		 tmpCommand=SBUF0; //����������
		 if(0<=tmpCommand<3)
		 {
		     while(RI0==0){}
			 RI0=0;
			 value1=SBUF0;
			 while(RI0==0){}
			 RI0=0;
			 value2=SBUF0;
			 if(tmpCommand==0)		//daת��
			 {
			     DAC0L=value2;		//��λ
				 DAC0H=value1;		//��λ
				 DAC0L=value2;		//��λ
				 DAC0H=value1;		//��λ
			 }
			 else if(tmpCommand==1)
			 {
			     DAC1L=value2;		//��λ
				 DAC1H=value1;		//��λ
				 DAC1L=value2;		//��λ
				 DAC1H=value1;		//��λ
			 }
			 else			 
			 {
			     ADC_Start(value2);
				// v1=ADC0VAL;
			     ADC_Start(value2);
				 //v2=ADC0VAL;
			     //ADC_Start(value2);
				// v3=ADC0VAL;
				// v=(v1+v2+v3)/3;
				 UART_Send(ADC0H);
				 UART_Send(ADC0L);
			 }
		 }
		}


	/************************Matlab Simulink*******************************/
	while(WorkMode==7)  //Matlab Simulinkģʽ
	{
		while(RI0==0) {}
		RI0=0;
		tmpAddr=SBUF0; //���������ַ
		if(tmpAddr == MyAddr)
		{
		     while(RI0==0){}
			 RI0=0;
			 tmpCommand=SBUF0; //����������
			 if(0<=tmpCommand)
			 {
			     while(RI0==0){}
				 RI0=0;
				 value1=SBUF0;
				 while(RI0==0){}
				 RI0=0;
				 value2=SBUF0;
				 if(tmpCommand==0)		//daת�� 0ͨ��
				 {
				     DAC0L=value2;		//��λ
					 DAC0H=value1;      //��λ
                     UART_Send(0x55);
					 UART_Send(0x55);
					 UART_Send(0x55);
					 UART_Send(0x55);
				 }
				 else if(tmpCommand==1)  //daת�� 1ͨ��
				 {
				     DAC1L=value2;		//��λ
					 DAC1H=value1;		//��λ
					 UART_Send(0x55);
					 UART_Send(0x55);
					 UART_Send(0x55);
					 UART_Send(0x55);   //����4��0x55
				 }
				 else if(tmpCommand==2)	 //adת��		 
				 {
				     ADC_Start(value2);
					 UART_Send(tmpAddr);  //���͵�ַ
					 UART_Send(value2);   //����ͨ����
					 UART_Send(ADC0H);
					 UART_Send(ADC0L);     //����ADת������
				 }				 
			 }
		 }
		 else
		 {
		 	while(RI0==0){}
			 RI0=0;
			 tmpCommand=SBUF0; 
			 while(RI0==0){}
			 RI0=0;
			 value1=SBUF0;
			 while(RI0==0){}
			 RI0=0;
			 value2=SBUF0;
		 }
	}
	/**************************Modbus RTU��AD+DA+DO��***********************************/
		while(WorkMode==8)
		{
			if(modrecok==1)
			{
				if(modRbuf[0]==MyAddr && modRbuf[1]==0x03) //dizhi+AD 
				{
					
					crcr=erheyi(modRbuf[7],modRbuf[6]);/////////////////crc16
					
					if(crc16(modRbuf,6)==crcr)
					{

						if(modRbuf[3]+modRbuf[5]>8 || modRbuf[2]>0 || modRbuf[4]>0) //��������ж�
						{
							modTbuf[0]=modRbuf[0];
							modTbuf[1]=modRbuf[1]|0x80;
							modTbuf[2]=0;
							crct=crc16(modTbuf,3);//����Ҫ��
							modTbuf[3]=crct&0x00ff;//bufλ�ø�
							modTbuf[4]=crct>>8;	
							//UART0_SendBuf(modTbuf,5);						
						}
						else
						{
					//	UART_Send(0x05);
							for(i=0;i<modRbuf[5];i++)
							{
								ADC_Start(modRbuf[3]+i);
								modTbuf[3+i*2]=ADC0H;
						     	modTbuf[4+i*2]=ADC0L;
							}
							modTbuf[0]=modRbuf[0];
							modTbuf[1]=modRbuf[1];
							modTbuf[2]=modRbuf[5]*2;
							crct=crc16(modTbuf,3+modRbuf[5]*2);//����Ҫ��
							modTbuf[3+modRbuf[5]*2]=crct&0x00ff;//bufλ�ø�
							modTbuf[4+modRbuf[5]*2]=crct>>8;
							UART0_SendBuf(modTbuf,5+modRbuf[5]*2);//���͸���Ҫ��
						}
					}
				}
				else if(modRbuf[0]==MyAddr && modRbuf[1]==0x06)//dizhi+DA+DO
				{
					crcr=erheyi(modRbuf[7],modRbuf[6]);
					if(crc16(modRbuf,6)==crcr)
					{
						if(modRbuf[2]>0 || modRbuf[3]<0x08 || modRbuf[3]>0x0a)
						{
							modTbuf[0]=modRbuf[0];
							modTbuf[1]=modRbuf[1]|0x80;
							modTbuf[2]=0;
							crct=crc16(modTbuf,3);//����Ҫ��
							modTbuf[3]=crct&0x00ff;//bufλ�ø�
							modTbuf[4]=crct>>8;	
							//UART0_SendBuf(modTbuf,5);						
						}
						else 
						{	
							if(modRbuf[3]==0x08)  //DA0ת��
							{
				     			DAC0L=modRbuf[5];		//��λ
					 			DAC0H=modRbuf[4];      //��λ
							}
							if(modRbuf[3]==0x09)  //DA1ת��
							{
				     			DAC1L=modRbuf[5];		//��λ
					 			DAC1H=modRbuf[4];      //��λ
							}
							if(modRbuf[3]==0x0a)  //DA0ת��
							{
				     			P6=~modRbuf[5];		//��λ
							}

							for(i=0;i<8;i++)
								modTbuf[i]=modRbuf[i];

							UART0_SendBuf(modTbuf,8);
						}
					}
				}
				modrecok=0;
			}//recok
	
		}//5
}

//ADC0ת������
void ADC_Start(uchar source)
{
    AMX0SL = source;	  //ѡ��ͨ��
	AD0INT = 0;
	AD0BUSY = 1;          //����ADת��
	while (!AD0INT){}	  //�ȴ�ת�����
	AD0INT =0;
							//����ת��
	AD0BUSY = 1;          //����ADת��
	while (!AD0INT){}	  //�ȴ�ת�����
	AD0INT =0;
} 

//UARTͨѶ����
void UART_Send(uchar sdata)
{
	SBUF0=sdata;
	while(TI0==0){;}
	TI0=0;
}

//UARTͨѶ�������ݰ�
void UART0_SendBuf(uchar* buf, uchar len)
{
	uchar num;
	
	for(num=0;num<len;num++)
	{
		
		SBUF0=buf[num];
		while(TI0==0){;}
		TI0=0;
	}
	
}

bit RstDS1820(void)		//����0-���豸����1-���豸����
{
	unsigned char i;
	bit RstFlag;
	RstFlag=1;	
	DataPort_Set0;
	for (i=0;i<40;i++)      	//480us ��ʱ
		Delay15us(); 
	DataPort_Set1;
	for(i=0;i<4;i++)			//15us-60us ��ʱ
	{
		Delay15us();
	}
	for (i=0;i<16;i++)			//60us-240us ��ʱ
	{
		Delay15us();
		if (DataPort_Status==0) RstFlag=0;
	}
	for (i=0;i<16;i++)			//240us
	{
		Delay15us();
	}
	return RstFlag;	
}

void Delay1us(unsigned char us)  //��ʱ
{
	while (us)
	{
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		--us;
	}
}
void Delay15us(void)  
{
	Delay1us(15);                  //��ʱ15��
}
void Delay10us(void)
{	
	Delay1us(10);
}

void WriteDS1820(unsigned char ch)
{
unsigned char i;
	DataPort_Set1;
	Delay1us(1);
	for (i=0;i<8;i++)
	{
		EA=0;
		DataPort_Set0;
		Delay15us();
		P4 |= ch&0x1;
		EA=1;
		Delay15us();Delay15us();Delay15us();
		DataPort_Set1;
		ch=ch>>1;
		Delay1us(1);
	}
}
unsigned char ReadDS1820(void)
{
	unsigned char i,ch;
	ch=0;
	DataPort_Set1;
	Delay1us(1);
	for (i=0;i<8;i++)
	{
		EA=0;
		DataPort_Set0;
		Delay10us();
		DataPort_Set1;
		Delay1us(2);
		ch=ch>>1;
		if (P4&0x01==1)
		{
			ch=ch+0x80;
		}
		EA=1;
		Delay15us();Delay15us();Delay15us();
	}	
	return ch;
}
void SkipROMCode(void)       //���������������кŵĲ�������:0xcc
{
	RstDS1820();
	WriteDS1820(0xcc);
}

void StartDS(void)
{
	if(RstDS1820()) UART_Send(0x80);;			//��λ
	WriteDS1820(0xcc);		//�㲥
	WriteDS1820(0x44);		//�����¶�ת��ת�� 12bit700ms
}
unsigned int GetTempValue(void)//��ȡ�¶�ֵ�ĳ���
{
	
	unsigned int T;

	sleep_ms(250);
	sleep_ms(250);
	sleep_ms(250);
	SkipROMCode();
	WriteDS1820(0xBE);  //������ȡ�¶�ֵ������(0xBE)
	
	TL=ReadDS1820();
	TH=ReadDS1820();

	StartDS();        //
	
	T=TL+TH*256;
	/*
	if (T==0xffff) return 0xffff;
	if (T>0x8000)                      //�¶�Ϊ����
	{
		T=-T;
		return(0x8000+T*5/8);          //�����¶�ֵ
	}
	else                               //�¶�Ϊ����
		return(T*5/8);
		*/
	return T;
}

void Init18b20(void)
{
	SkipROMCode();                        //���������������кŵĲ�������:0xcc
	WriteDS1820(0x44);                    //�����¶�ת��
}

void GetAddr(void)  //�õ���Ƭ����ַ
{
	if(A0) MyAddr+=1;     
	if(A1) MyAddr+=2;
	if(A2) MyAddr+=4;
	if(A3) MyAddr+=8;
	MyAddr1=HEXASC_high(MyAddr);  //ASC��ַ
	MyAddr2=HEXASC_low(MyAddr);

}

void SetBaud(void)  
{
	if(baud_mode == 0 || baud_mode == 0x40 || baud_mode == 0xc0 || baud_mode==0x20 || baud_mode==0xa0 || baud_mode==0x60) 
	{
		//9600
		RCAP2H = 0xFF;  // Timer 2 Capture Register High Byte
    	RCAP2L = 0xDC;  // Timer 2 Capture Register Low Byte
	}

	

}

void sleep_ms(unsigned int count)      //��ʱ
{
	unsigned char ii,jj;
	for(ii=0;ii<count;ii++)
	{
		for(jj=0;jj<250;jj++)
		_nop_();
	}
}
//////////////////��ʱ��1///////////////////////
void time1() interrupt 3
{	
//	modrecok=1;//	
//	modwei=0;

	matrecok=1;
	matwei=0;


    TR1=0;
    TH1=0xf0;
	TL1=0x5f;
//	timeoutcnt=1;
//	TR1=1;
//	timeoutcnt=0;

	
	
}
/////////////////////////////////////////////
void uart0_ISR(void) interrupt 4  //�����жϷ������
{
	uchar m; uchar sbuf; uchar matbuf;
	uchar labbuf;
	uchar modbuf;
	bit flag1;
	bit flagmat1;
	bit flaglab1;
//	uchar matwei;
//	uchar imat;
	if(RI0) //ENQ.......EOT,CRC
	{
	  	
		ceshi=1;
		matjishu++;

		RI0=0;m=SBUF0;	sbuf=m;  
		matbuf=sbuf;
		labbuf=matbuf;
		modbuf=labbuf;
		if(baud_mode == 0x00)
		{
			if(last==ENQ &&(m==MyAddr ||m=='R' || m=='W'))  Rptr=0;  //����ʼ��λ������Ҫ�����HEX  
			if(Rptr<BufMax) 
			{  
			  Rbuf[Rptr]=m; Rptr++; 
			}
		    if(last==EOT)   RecvOk=1; 
			if(Rptr==3 )
			   {
				  if(Rbuf[0]==MyAddr)   //��¼��λ���Ƿ���ѯ�ʱ�����־λ�����������1
				  {
			        flag=1; //	SCON0 = SCON0 | 0x20;//�����ж��ǵ�ַ��������������
		      	   }
				}
			last=m;
		}
		   //////////////////ASC///////////////////////
	   if(baud_mode == 0x40)
	 {
	   if(former2==Start &&(former1==MyAddr1) &&(sbuf==MyAddr2))
		{
		flag1=1,count=2;
		
		}
		if(flag1==1)
		{
			recbuf[0]=Start;
			recbuf[1]=MyAddr1;
			recbuf[count]=sbuf;
      		count++;
     	
		
		 if(sbuf==End)
			{
			recok=1;
			count=0;
			flag1=0;
					
			}
		}
		former2=former1;
		former1=sbuf;
	}
		//////////////////////////////matlab//////////
/*		if(baud_mode == 0xc0)
	{

		if(matbuf1==matStart && matbuf0==MyAddr1 && matbuf==MyAddr2) // &&  MyAddr 
		{
	//	matrecok=1;
			flagmat1=1;
			matwei=2;
		}
		if(flagmat1==1)
		{
			matRbuf[0]=matStart;
			matRbuf[1]=MyAddr1;
			matRbuf[matwei]=matbuf;
			matwei++;
			if(matbuf0==matEnd1 && matbuf==matEnd2)
			{
			matrecok=1;
			matwei=0;
			flagmat1=0;
			}
		}
		matbuf1=matbuf0;
		matbuf0=matbuf;
	} */
	if(baud_mode == 0xc0)
	{
		    TH1=0xf0;
			TL1=0x5f;
			TR1=1;
			matRbuf[matwei]=matbuf;
			matwei++;
	}
		//////////////////////////////labview/////////////////
		if(baud_mode == 0x20)
		{	
	//		aaaaa=1;
			if(labbuf0==labStart && labbuf==MyAddr)
			{
				flaglab1=1;//��ͷ��־
				labwei=1;
				
			}
			if(flaglab1==1)
			{
				labRbuf[0]=labStart;
				labRbuf[labwei]=labbuf;
				labwei++;
				if(labbuf==labEnd)
				{
				labrecok=1;
				labwei=0;
				flaglab1=0;
				}
			}
			labbuf0=labbuf;
		 }
		 //////////////////////�л�modbus////////////////////////
		 if(baud_mode == 0xa0)
		 {
		//	if(modrecok==1)
		//	{modwei=0;}
			
			TH1=0xf0;
			TL1=0x5f;
			TR1=1;
			modRbuf[modwei]=modbuf;
			modwei++;
			if(modwei == 8)
			{
				modrecok=1;//	
				modwei=0;
			}
		 }
		////////////////////////////////////
		 if(baud_mode == 0x60)
		 {
		//	if(modrecok==1)
		//	{modwei=0;}
			
			TH1=0xf0;
			TL1=0x5f;
			TR1=1;
			modRbuf[modwei]=modbuf;
			modwei++;
			if(modwei == 8)
			{
				modrecok=1;//	
				modwei=0;
			}
		 }
  
		
	
	}//RI0
 
}




void CheckCRC()
{
  uchar i,n,crc;
  n=Rptr;
  crc=ENQ;
  for(i=0;i<n;i++) crc^=Rbuf[i];
  if(crc==0) CRCok=1;
  else CRCok=0;
}

/************************************************************
*******************������ASC�ĵײ�***************************
*************************����****************************
********************************************************/



/******************��ȡ���������*******************************/
uchar read_write_flag()
{
uchar temp;
if(recbuf[4]>0x40)temp=(recbuf[4]-0x37)&0x0f;
      else   temp=(recbuf[4]-0x30)&0x0f;
return temp;
}
/******************����ת��ASC>>hex�*******************************/
uchar Dat_trans(uchar hight_v,uchar low_v)
{
      uchar value,hight,low;
      if (hight_v>0x40)hight=hight_v-0x37;
          else hight=hight_v-0x30;
      if (low_v>0x40)low=low_v-0x37;
            else low=low_v-0x30;
      value=(hight&0x0f)<<4;
      value+=(low&0x0f);
        return value;
}
/***********************HEX>>ASC***********************************************/
uchar HEXASC_high(uchar tempv)
     { 
			 uchar hight,ctmp1;
			 ctmp1=(tempv>>4);
      if(ctmp1>9) hight=0x40+ctmp1%9;
      else hight=ctmp1+0x30;
			return hight;
     }
uchar HEXASC_low(uchar tempv)
     { 
			 uchar low,ctmp2;
			 ctmp2=tempv&0x0f;
      if(ctmp2>9) low=0x40+ctmp2%9;
      else low=ctmp2+0x30;
			return low;
     }

/******************��д����Ӧ��356*******************************/
void write_inform(uchar dat)
{
      uchar xordat,i,ctmp1,ctmp2,send_temp;

      Answer[3]=dat; Answer[4]=dat;
      xordat=0;
      for(i=1; i<5; i++)
				{xordat^=Answer[i];}
      send_temp=xordat;
    ctmp1=(send_temp>>4)&0x0f;    
			if  (ctmp1>9)
				Answer[5]=ctmp1%9+0x40;
      else 
				Answer[5]=ctmp1+0x30;
      ctmp2=send_temp&0x0f;     
			if (ctmp2>9)
				Answer[6]=ctmp2%9+0x40;
      else 
				Answer[6]=ctmp2+0x30;
     // uartsends(Answer,8);
			Answer[0]=0x40;
			Answer[1]=recbuf[1];
			Answer[2]=recbuf[2];
			Answer[7]=0x0d;
			UART0_SendBuf(Answer,8);
}
/*****************������Ӧ�𣬷�������*******************************/
void Read_byte()
{
uchar ctmp1,ctmp2,xordat,i,send_temp;
send_temp=dat[Dat_trans(recbuf[7],recbuf[8])];
ctmp1=(send_temp>>4);
if(ctmp1>9) sendbuf[5]=0x40+ctmp1%9;
      else sendbuf[5]=ctmp1+0x30;
ctmp2=send_temp&0x0f;
if(ctmp2>9) sendbuf[6]=0x40+ctmp2%9;
else sendbuf[6]=ctmp2+0x30;
 xordat=0;
for(i=1;i<7;i++)xordat^=sendbuf[i];
send_temp=xordat;
ctmp1=(send_temp>>4)&0x0f;
if(ctmp1>9)sendbuf[7]=ctmp1%9+0x40;
      else sendbuf[7]=ctmp1+0x30;
ctmp2=send_temp&0x0f;
if(ctmp2>9)sendbuf[8]=ctmp2%9+0x40;
      else sendbuf[8]=ctmp2+0x30;
	//uartsends(sendbuf,10);
UART0_SendBuf(sendbuf, 10);
}
/***************����ż�*���У��*******************************/
bit check_CRC()
{
uchar xordat,i,ctmp1,ctmp2,temp;
if(((read_write_flag())&0x01)==Read)temp=11;
else if(((read_write_flag())&0x01)==Write)
        temp=13;
xordat=recbuf[1];
for(i=2;i<temp;i++)xordat^=recbuf[i];
ctmp1=xordat&0x0f;
ctmp1>>=4;
ctmp1+=0x30;
ctmp2=xordat&0x0f;
ctmp2+=0x30;
if((ctmp1==recbuf[temp])&&(ctmp2==recbuf[temp+1]))
return 1;
else return 0;
}
/*****************д���ݵ�Ƭ��Ӧ���*******************************/
void Write_byte()
{
uchar temp;
temp=Dat_trans(recbuf[11],recbuf[12]);
dat[Dat_trans(recbuf[7],recbuf[8])]=temp;
write_inform('#');
}


/******************************CRC16***********************************
********************************У��********************************/
uint crc16(uchar*str,uint num)
{
	uint i,j,c,crc;
	crc=0xffff;
	for(i=0;i<num;i++)
	{
		c=str[i]&0x00ff;
		crc^=c;
		for(j=0;j<8;j++)
		{
			if(crc&0x0001)
			{
			   crc>>=1;
			   crc^=0xa001;
			}
			else
			{crc>>=1;}
		}//for2
	}//for1
	return(crc);
}//int

uint erheyi(uchar high,uchar low)
{
	uint he;
	he=(high<<8)|low;
	return he;
}