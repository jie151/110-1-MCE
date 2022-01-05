#include "C8051F040.h"
#include "LCD.h"

char LCD_status;
void LCDPortConfig()
{
    //turn down watch-dog timer
    WDTCN = 0xde;
    WDTCN = 0xad;

    //initialize SFR setup page
    SFRPAGE = CONFIG_PAGE;

    //setup the cross-bar and configure the I/O ports
	XBR2 = 0xc0;
    //P1MDIN = 0xff;
	P3MDOUT = 0x3f;

    //set to normal mode
    SFRPAGE = LEGACY_PAGE;
}

void LCD_Init ()
{
    P3=0x00;  
    P3=0x02;
    LCD_SendCommand(0x01); //clear display, cursor to home
    LCD_SendCommand(0x28); //display function(2 rows for 4-bit data; small)
    LCD_SendCommand(0x0e); //display and cursor on, cursor blink off
    //LCD_SendCommand(0x10); //cursor shift left
    //LCD_SendCommand(0x06); //cursor increment, shift off

}
void LCD_Status_SetRS ()
{
	LCD_status = LCD_status | 1;
}
void LCD_Status_ClearRS ()
{
	LCD_status = LCD_status & 0xfe; //0: command, 1:data
}
void LCD_Status_SetWord (char word)
{
	word = word & 0x0f;
	LCD_status = LCD_status & 0x03;
	LCD_status = LCD_status | (word<<2);
}
void LCD_Status_SetEnable ()
{
	LCD_status = LCD_status | 0x02;
}
void LCD_Status_ClearEnable ()
{
	LCD_status = LCD_status & 0xfd;
}
void LCD_Delay ()
{
	int i;
	for (i=0;i<18000;i++);
}
void  LCD_SendCommand(char cmd)
{
    LCD_Status_ClearRS (); //RS = 0 (command)

    LCD_Status_SetWord((cmd >> 4) & 0x0f); //higher-half
    LCD_Status_SetEnable(); //E = 1
    P3 = LCD_status; 
    LCD_Delay();
    LCD_Status_ClearEnable(); //E = 0
    P3 = LCD_status; 
    LCD_Delay();

    LCD_Status_SetWord(cmd & 0x0f); //lower-half
    LCD_Status_SetEnable(); //E = 1
    P3 = LCD_status; 
    LCD_Delay();
    LCD_Status_ClearEnable(); //E = 0
    P3 = LCD_status; 
    LCD_Delay();
}

void  LCD_SendData(char dat)
{
    LCD_Status_SetRS(); //RS = 1 (data)

    LCD_Status_SetWord((dat >> 4) & 0x0f); //higher-half
    LCD_Status_SetEnable(); //E = 1
    P3 = LCD_status; 
    LCD_Delay();
    LCD_Status_ClearEnable(); //E = 0
    P3 = LCD_status; 
    LCD_Delay();  

    LCD_Status_SetWord(dat & 0x0f); //lower-half
    LCD_Status_SetEnable(); //E = 1
    P3 = LCD_status; 
    LCD_Delay();
    LCD_Status_ClearEnable(); //E = 0
    P3 = LCD_status; 
    LCD_Delay();
}

void LCD_Delay_down ()
{
	int i;
	for (i=0;i<100;i++);
}
void  LCD_SendCommand_down(char cmd)
{
    LCD_Status_ClearRS (); //RS = 0 (command)

    LCD_Status_SetWord((cmd >> 4) & 0x0f); //higher-half
    LCD_Status_SetEnable(); //E = 1
    P3 = LCD_status; 
    LCD_Delay_down();
    LCD_Status_ClearEnable(); //E = 0
    P3 = LCD_status; 
    LCD_Delay_down();

    LCD_Status_SetWord(cmd & 0x0f); //lower-half
    LCD_Status_SetEnable(); //E = 1
    P3 = LCD_status; 
    LCD_Delay_down();
    LCD_Status_ClearEnable(); //E = 0
    P3 = LCD_status; 
    LCD_Delay_down();
}

int main()
{
    char record[2][40];
    int current;
    int line;
    int i;

    LCDPortConfig();
    LCD_Init();

    current = 0;
    line = 0;

    for (i = 0; 40 > i; i++)
    {
        record[0][i] = ' ';
        record[1][i] = ' ';
    }
    while(1)
    {
        if (P1 > 0x00 && P1 < 0x05) // 0 2 4
        {      
            if (P1 == 0x01)
            {
                LCD_SendData('A');
                record[line][current] = 'A';
                current++;
            }
            else if (P1 == 0x02)
            {
                LCD_SendData('B');
                record[line][current] = 'B';
                current++;
            }
            else if (P1 == 0x04)
            {
                LCD_SendData('C');
                record[line][current] = 'C';
                current++;
            }
            if(current == 16)
            {
                if (line == 0)
                {
                    LCD_SendCommand(0xC0);
                    //move cursor to begining of second row
                    current = 0;
                    line = 1;
                }
                else 
                {
                    LCD_SendCommand(0x0002);
                    //cursor to home
                    current = 0;
                    line = 0;
                }  
            }
        }

        else if (P1 == 0x08) //enter 
        {
            if (line == 0)
            {
                LCD_SendCommand(0x0001);
                //clear display, cursor to home
                for(i = 0; 16 > i; i++)
                {
                    LCD_SendData(record[line][i]);
                    record[1][i] = ' ';
                }
                LCD_SendCommand(0x00C0);
                //move cursor to beginning of second row
                line = 1;
            }
            else 
            { //line = 1
                
                LCD_SendCommand(0x0001);
                //clear display, cursor to home
                for(i = 0; 16 > i; i++)
                {

                    LCD_SendData(' ');
                }
                LCD_SendCommand(0x00C0);
                //move cursor to beginning of second row
                for(i = 0; 16 > i; i++)
                {

                    LCD_SendData(' ');
                }
                LCD_SendCommand(0x0001);
                //clear display, cursor to home
                for(i = 0; 16 > i; i++)
                {
                    
                    LCD_SendData(record[1][i]); 
                    
                    record[0][i] = record[1][i];
                    record[1][i] = ' ';
                }
                LCD_SendCommand(0x00C0);
                //move cursor to beginning of second row
                line = 1;
            }
        }
        else if(P1 > 15 && P1 < 129) //4~8 
        {
            if (P1 == 16) //right(2^4)
            {
                if(current != 15)
                {      
                    LCD_SendCommand(0x0014);
                    //cursor shift right
                    current++;
                }
            }
            else if (P1 == 32) //left(2^5)
            {
                if (current%16 != 0)
                {
                    LCD_SendCommand(0x0010);
                    //cursor shift left
                    current--; //left
                }
            } 
            else if (P1 == 64) //down (2^6)
            {
                if(line == 0)
                {
                    line = 1;
                    LCD_SendCommand(0x00C0);
                    //move cursor to beginning of second row
                    for(i =0; current > i; i++)
                    {
                        LCD_SendCommand_down(0x000C);
                        //display on, cursor and cursor blink off
                        LCD_SendCommand_down(0x0014);
                        //cursor shift right 
                    }
                    LCD_SendCommand_down(0x000e);
                    //display and cursor on, cursor blink off
                }
            }
            else //key =8, up (2^7) 
            {
                if(line == 1)
                {
                    LCD_SendCommand(0x0002);
                    //cursor to home
                    for(i = 0; current > i; i++)
                    {
                        LCD_SendCommand_down(0x000C);
                        //display on, cursor and cursor blink off
                        LCD_SendCommand_down(0x0014);
                        //cursor shift right
                    }
                    line = 0;
                    LCD_SendCommand_down(0x000e);
                    //display and cursor on, cursor blink off
                }
            }
        }
    }
	return -1;
}