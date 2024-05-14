#include<reg51.h>

#define LEDData P0

unsigned char code LEDCode[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

unsigned char minit,second,count,count1;        	//分，秒，计数器
sbit add2=P1^0;						//在未开始比赛时为加时间分
sbit add3=P1^1;						//在未开始比赛时为减时间分
sbit secondpoint=P0^7;				//秒闪动点
//----依次点亮数码管的位------
sbit led1=P2^7;
sbit led2=P2^6;
sbit led3=P2^5;
sbit led4=P2^4;
sbit led5=P2^3;
sbit led6=P2^2;
sbit led7=P2^1;
sbit led8=P2^0;
sbit led9=P3^7;
sbit led10=P3^6;
sbit led11=P3^5;

sbit alam=P1^7;						//报警
sbit onoff=P3^2;
bit  playon=0;						//比赛进行标志位，为1时表示比赛开始，计时开启
bit  timeover=0;					//比赛结束标志位，为1时表示时间已经完
bit  halfsecond=0;					//半秒标志位

unsigned int scoreA,flat=1,pan=0;				//得分

		

void Delay5ms(void)
{
	unsigned int i;
	for(i=100;i>0;i--);
}
void display(void)
{
//-----------显示时间分--------------
	LEDData=LEDCode[minit/10];
	led1=0;
	Delay5ms();
	led1=1;
	LEDData=LEDCode[minit%10];
	led2=0;
	Delay5ms();
	led2=1;
//-------------秒点闪动------------
	if(halfsecond==1)
		LEDData=0x80;
	else
		LEDData=0x00;
	led2=0;
	Delay5ms();
	led2=1;
	secondpoint=0;
//-----------显示时间秒------------
	LEDData=LEDCode[second/10];
	led3=0;
	Delay5ms();
	led3=1;
	LEDData=LEDCode[second%10];
	led4=0;
	Delay5ms();
	led4=1;
//-----------显示1组的分数百位-------
	LEDData=LEDCode[scoreA/100];
	led5=0;
	Delay5ms();
	led5=1;
//---------------显示1组分数的十位-----------
	LEDData=LEDCode[(scoreA%100)/10];
	led6=0;
	Delay5ms();
	led6=1;

//---------------显示1组分数的个位-----------
	LEDData=LEDCode[scoreA%10];
	led7=0;
	Delay5ms();
	led7=1;
}

//=========================按键检测程序================================================
void keyscan(void)
{
	if(onoff==0)
	{
		display();
		if(onoff==0)
		{
			alam=0;
			Delay5ms();Delay5ms();
			alam=1;
			TR1=0;
			timeover=0;
			playon=!playon;				//开始标志位
			TR0=playon;					//开启计时
			do
				display();
			while(onoff==0);
		}
	}
	if(playon==0)
	{
		if(add2==0)
		{
			display();
			if(add2==0);
			{
				alam=0;
				Delay5ms();Delay5ms();
				alam=1;
				if(minit<99)
					minit++;
				else
					minit=99;
			}
			do
				display();
			while(add2==0);
		}

		if(add3==0)
		{
			display();
			if(add3==0);
			{
				alam=0;
				Delay5ms();Delay5ms();
				alam=1;
				if(minit>0)
					minit--;
				else
					minit=0;
			}
			do
				display();
			while(add3==0);
		}
	}
	else
	{
		if(playon==1&&add2==1&&add3==0){pan=1;}
		if(add2==0)
		{
			display();
			if(add2==0);
			{
				alam=0;
				Delay5ms();Delay5ms();
				alam=1;
				if(scoreA<999)
						scoreA=scoreA+2;
					else
						scoreA=999;
			}
			do
				display();
			while(add2==0);
		}

		if(add3==0)
		{
			display();
			if(add3==0);
			{
				alam=0;
				Delay5ms();Delay5ms();
				alam=1;
				if(scoreA<999)
						scoreA=scoreA+3;
					else
						scoreA=999;
			}
			do
				display();
			while(add3==0);
		}
	}
}
	


//******************************主函数*************************************************
void main(void)
{
	TMOD=0x11;
 	TL0=0xb0;
 	TH0=0x3c;
 	TL1=0xb0;
 	TH1=0x3c;
	minit=1;				//初始值为1：00
	second=0;
 	EA=1;
	ET0=1;
	ET1=1;
 	TR0=0;
	TR1=0;
	P1=0xFF;
	P3=0xFF;
  
 	while(1)
 	{
		keyscan();
		display();
		if(pan==1){
		if(flat==1&&(scoreA==30||scoreA>30)){
		minit=0;
			second=40;
			scoreA=0;
			flat++;
		}
		if(flat==2&&(scoreA==30||scoreA>30)){
		minit=0;
			second=30;
			scoreA=0;
			flat++;	
		}
		if(flat==3&&(scoreA==20||scoreA>20)){
		  minit=1;
			second=0;
			scoreA=0;
			flat=1;
		  playon=0;
			keyscan();
			if(playon==0)
			{
					break;
			}
		}
		pan=0;
		}
  }
}


//*********************************中断服务函数**************************************
void  time0_int(void) interrupt 1 
{
 	TL0=0xb0;
 	TH0=0x3c;
 	TR0=1;
  	count++;
	if(count==20)
	{
		halfsecond=0;
	}

   	if(count==30)
   	{
		count=0;
		halfsecond=1;
	
    	if(second==0)
	 	{
			if(minit>0)
			{
				second=59;
				minit--;
			}
			else
			{
				timeover=1;
				playon=0;
				TR0=0;
				TR1=1;
			}
		}
		else
			second--;
   	}
}


//*********************************中断服务函数**************************************
void  time1_int(void) interrupt 3
{
	TL1=0xb0;
 	TH1=0x3c;
 	TR1=1;
  	count1++;
	if(count1==10)
	{
		alam=0;
	}

   	if(count1==20)
   	{
		count1=0;
		alam=1;
	}
} 
