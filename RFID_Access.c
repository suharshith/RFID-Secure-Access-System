#include<AT89x52.h>
#include<string.h>

sbit Relay=P2^3;
sbit Rs=P2^0;
sbit Rw=P2^1;
sbit En=P2^2;
sbit Busy=P2^7;

#define Lcd_data P1

unsigned char Temp=0,i=0;
unsigned char Buff[18];

unsigned char Rx_char();

void Time_Delay(unsigned int Count)
{
    unsigned int i;
    for(i=0;i<Count;i++)
    {
        TH0 = 0xFC;        // LOAD THE TH0 VALUE
        TL0 = 0xFF;        // LOAD THE TL0 VALUE
        TR0 = 1;        // START THE TIMER
        while(TF0 == 0);    // WAIT HERE TILL THE TIMER OVERFLOWS
        TR0 = 0;        // STOP THE TIMER
        TF0 = 0;        // CLEAR THE OVERFLOW FLAG
         
    }
}
void Busy_check()
{
    Busy = 1;
    Rs = 0;                // RS =0 FOR COMMAND
    Rw = 1;            // RW =1 FOR READING
    while(Busy)
    {
        En=0;            // ENABLE IS LOW
        En=1;            // ENABLE IS HIGH
    }
}

void Lcd_Byte(unsigned char Dat, bit Mode)
{
    Busy_check();
    Lcd_data = Dat;
    Rs    =    Mode;      // RS =0 FOR COMMAND & RS =1 FOR DATA
    Rw    =    0;        // RW =0 FOR WRITING
    En    =    0;        // ENABLE IS LOW
    En    =    1;        // ENABLE IS HIGH
}

void Lcd_String(unsigned char *str)
{
    while(*str)
    {
        Lcd_Byte(*str,1);
        str++;
    }
}


void Lcd_Init()
{
    Lcd_Byte(0x38,0);        // SET THE LCD IN 2 LINE & 5X7 MATRIX MODE
    Lcd_Byte(0x0c,0);        // DISPLAY ON & CURSER OFF
    Lcd_Byte(0x01,0);        // CLEAR THE LCD
    Lcd_Byte(0x06,0);        // SHIFT THE CURSER TO RIGHT
    Lcd_Byte(0x80,0);        // CURSOR IS AT LCD  1ST LINE STARTING LINE
}



unsigned char Rx_Char()
{
    while(!RI);    
    return SBUF;
}
void Ser_Init()
{
    SCON = 0X50;    // ENABLE THE TRANSMIT & RECEIVE FLAGS
    TMOD = 0x21;    // SET THE TIMER 0 IN MODE1 & TIMER1 IN   MODE2
    TH1  = 0XE8;        // SET THE BAUD RATE
     TR1  = 1;        // START THE TIMER1
}
void main()
{
    bit k=0;
    Relay=0;
    Ser_Init();
    Lcd_Init();
    
while(1)
    {
        Temp=0;
        i=0;
    
        Lcd_Byte(0x01,0);
        Lcd_String("SWAP THE CARD");
        do
        {
            Temp = Rx_char();
            Buff[i++] = Temp;
        }while(i!=12);
            if(!strncmp(Buff,"380002B9D251",12))
            {
                if(k==0)
                {
                    Lcd_Byte(0x01,0);
                    Lcd_String("Authorised card");
                    Time_Delay(1000);
                    Lcd_String("LOAD ON");
                    Relay=1;
                    k=1;
                }
                else if(k==1)
                {
                    Lcd_Byte(0x01,0);
                    Lcd_String("LOAD OFF");
                    Time_Delay(1000);
                    Relay=0;
                    k=0;
            
    }
            }
            else
            {                
                Lcd_Byte(0x01,0);
                Lcd_String("Not Authorised");
                Time_Delay(1000);
            }
    }

