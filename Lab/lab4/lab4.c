#include "C8051F040.h"

int status, pattern = 0, timerCount=0;

void portConfiguration()
{
    XBR2 = 0xc0;
    P1MDIN = 0xff;
    P2MDOUT = 0xff;
}
void TimerConfig()
{
    TMOD = 0x01;  //mode 1
    TCON = 0x010; //TR0(bit4) = 1
    CKCON = 0x010;
    IE = 0x082; //EA(bit7)=1, ET0(bit1) =1
    TL0 = 0x0C6; //#C6h (1734= 0x6C6)
    TH0 = 0x06; //#6h
}
void defaultConfig()
{   //turn down watch-dog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    TimerConfig();
    
    //initialize SFR setup page
    SFRPAGE = CONFIG_PAGE;

    portConfiguration();

    //set to normal mode
    SFRPAGE = LEGACY_PAGE;
}

void timer0ISR() interrupt 1 //interrupt number 1: TIMER/COUNTER 0
{
    timerCount++;
    if (timerCount == 4)
    {
        timerCount = 0;
        if (pattern)  //pattern = 1, shift right
        {
            status = status  >> 1;
            
            if (status == 0) status = 128; //if status == 0, status = 2^7
            P2 = status; 
        }
        else //patern = 0, shift left
        {
            status = status << 1; 
            P2 = status;
            if (status == 256) //if status == 256(2^8), status = 1(2^0) 
            {
                status = 1;
                P2 = status;
            }   
        }
    }
}

void  waitButtonPressed()
{
    char key_hold;
    int key_release, N = 7,count;

    do{
        key_hold = P1;
    }while(!key_hold);

    //stage 2: wait for key released
    key_release = 0;
    count = N;

    while(!key_release)
    {
        key_hold = P1;
        if (key_hold)
        {
            count = N;
        }
        else 
        {
            count--;
            if(count == 0) key_release = 1;
        }
    }
}

int main()
{
	defaultConfig();

	status = 1;
	P2 = status;

    while(1)
    {
		waitButtonPressed();
		P2 = status;
        pattern = (pattern + 1) % 2;
    }

	return -1;
}
