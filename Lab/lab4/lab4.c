#include "C8051F040.h"

void portConfiguration()
{
    XBR2 = 0xc0;
    P1MDIN = 0xff;
    P2MDOUT = 0xff;
}
void defaultConfig()
{   //turn down watch-dog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    //initialize SFR setup page
    SFRPAGE = CONFIG_PAGE;

    portConfiguration();

    //set to normal mode
    SFRPAGE = LEGACY_PAGE;
}

void timer0ISR() interrupt 1
{
    timerCount++;
    if (timerCount == 4)
    {
        timerCount = 0;
        if (pattern)
        {
            status = status >> 1;
        }
        else 
        {
            status = status << 1;
        }
    }
}

void  waitButtonPressed()
{
    char key_hold;
    int key_release, N = 7;

    do{
        key_hold = P1;
    }while(!key_hold);

    //stage 2: wait for key released
    key_release = 0;
    int count = N;

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
	int status, pattern = 0, btnCount = 0, timerCount=0;
	
	defaultConfig();

	status = 1;
	P2 = status;

    while(1)
    {
		waitButtonPressed();
		btnCount++;
		P2 = status;
        pattern = (btnCount) % 2;
    }

	return -1;
}


