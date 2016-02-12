/* 
 * File:   main.c
 * Author: Admin
 *
 * Created on February 18, 2015, 4:02 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <p24FJ256DA210.h>
#include "GenericTypeDefs.h"

#define TX_TRIS   TRISFbits.TRISF3
#define RX_TRIS   TRISDbits.TRISD0
//#define CS_TRIS   TRISAbits.TRISA14
//#define CS_LAT    LATAbits.LATA14
#define SCK_TRIS  TRISDbits.TRISD8
#define SDO_TRIS  TRISBbits.TRISB1
#define SDI_TRIS  TRISBbits.TRISB0
#define SDI_ANS   ANSBbits.ANSB0
#define SDO_ANS   ANSBbits.ANSB1

_CONFIG1( WDTPS_PS32768 & FWPSA_PR128 & ALTVREF_ALTVREDIS & WINDIS_OFF & FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF )
_CONFIG2( POSCMOD_HS & IOL1WAY_OFF & OSCIOFNC_OFF & OSCIOFNC_OFF & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF )
_CONFIG3( WPFP_WPFP255 & SOSCSEL_EC & WUTSEL_LEG & ALTPMP_ALTPMPEN & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM )

void SPIPut(unsigned char data)
{
    while(SPI2STATbits.SPITBF);
    SPI2BUF = data;
    while(!SPI2STATbits.SPITBF);
    return;
}

BYTE SPIGet ()
{
    BYTE spiData = 0;
    while(!SPI2STATbits.SPIRBF);
    spiData = SPI2BUF;
    return spiData;
}

void SPIinit()
{
        SPI2STAT = 0;
        //SPI1CON1 = 0;
        SPI2CON1 = 0b0001000111111;
        SPI2CON2 = 0;
        SPI2STATbits.SPIEN = 1;
}

void UARTInit()
{
     TX_TRIS = 0;
     RX_TRIS = 1;
     OSCCON = 0x3302;    		// Enable secondary oscillator
     CLKDIV = 0x0020;    		// Set PLL prescaler (1:1), PLLEN = 1
    //#define BAUDRATE    115200ul    //38400
    //#define BRG_TEMP    10 * GetPeripheralClock() / 4 / BAUDRATE
    //U2BRG = 25;//38400                          BRG_TEMP / 10 - 1;
        /*Desired Baud Rate = FCY/(16 (BRGx + 1))
          Solving for BRGx Value:
          BRGx = ((FCY/Desired Baud Rate)/16) ? 1
          BRGx = ((4000000/9600)/16) ? 1
          BRGx = 25
          Calculated Baud Rate = 4000000/(16 (25 + 1))
          = 9615
          Error = (Calculated Baud Rate ? Desired Baud Rate)
          Desired Baud Rate
          = (9615 ? 9600)/9600
        */
        U2BRG = 24;
	U2MODEbits.UARTEN = 1;		// UART2 is Enabled
	U2MODEbits.USIDL = 0;		// Continue operation at Idlestate
	U2MODEbits.IREN = 0;		// IrDA En/Decoder is disabled
	U2MODEbits.RTSMD = 0; 		// flow control mode
	U2MODEbits.UEN = 0b00;//0	// UTX, RTX are enable and on use.
	U2MODEbits.WAKE = 1;		// Wake-up on start bit is enabled
	U2MODEbits.LPBACK = 0;		// Loop-back is disabled
	U2MODEbits.ABAUD = 0;		// auto baud is disabled
	U2MODEbits.RXINV = 0;		// No RX inversion  UxRX Idle state is ?1?
	U2MODEbits.BRGH = 0;		// low boud rate (Standard-Speed mode (16 BRG clock cycles per bit)
	U2MODEbits.PDSEL = 0b00; 	// 8bit no parity
	U2MODEbits.STSEL = 0;		// one stop bit

	U2STAbits.UTXINV = 0;
        U2STAbits.UTXISEL1 = 0;//1;
        U2STAbits.UTXISEL0 = 0;//1;
	U2STAbits.UTXBRK = 0;		// sync break tx is disabled
	U2STAbits.UTXEN = 0;//1;	// transmit  is enabled
	//U2STAbits.URXISEL = 0b11;	// no interrupt
	U2STAbits.URXISEL = 0;		// Bits 6,7 Int. on character recieved //added
        U2STAbits.ADDEN = 0;		// address detect mode is disabled

	IFS1bits.U2TXIF = 0;	// Clear the Transmit Interrupt Flag
	//IEC1bits.U2TXIE = 1;	// Enable Transmit Interrupts
	IFS1bits.U2RXIF = 0;	// Clear the Recieve Interrupt Flag
	IEC1bits.U2RXIE = 0;//1;	// Enable Recieve Interrupts

	U2MODEbits.UARTEN = 1;	// And turn the peripheral on
        U2STAbits.RIDLE = 0;
	U2STAbits.UTXEN = 1;
}

void UARTPutChar(BYTE ch)
{
    while(U2STAbits.UTXBF == 1);
    U2TXREG = ch;
}

void UARTPutString(char *str)
{
    while(*str)
    UARTPutChar(*str++);
}

BYTE UARTGetChar(void)
{
    BYTE temp;
    IEC1bits.U2RXIE = 1;
    while(!IFS1bits.U2RXIF);//did
    temp = U2RXREG;
    // Clear error flag
    U2STAbits.OERR = 0;
    IFS1bits.U2RXIF = 0;//did
    IEC1bits.U2RXIE = 0;
    return temp;
}

void InitializeBoard(void)
{
    ANSA = 0x0000;
    ANSB = 0x0020;		// RB5 as potentiometer input,
    ANSC = 0x0010;		// RC4 as touch screen X+, RC14 as Rx
    ANSD = 0x0000;
    ANSE = 0x0000;    // RE9 is S2
    ANSF = 0x0000;
    ANSG = 0x0080;		// RG7 as touch screen Y+, RG8 is S1

    SDI_ANS = 0;
    SDO_ANS = 0;
   
   __builtin_write_OSCCONL(OSCCON & 0xbf);
   _U2RXR = 11;				// Bring RX2 Input to RP11/RD0
   _RP16R = 5; 				// Bring TX2 output on RP16/RF3
   RPOR1bits.RP2R = 11;                // assign RP2 for SCK2
   RPOR0bits.RP1R = 10;                // assign RP1 for SDO2
   RPINR22bits.SDI2R = 0;              // assign RP0 for SDI2
   __builtin_write_OSCCONL(OSCCON | 0x40);
   
   SCK_TRIS = 0;
   SDO_TRIS = 0;
   SDI_TRIS = 1;
}

void DelayMs(long msval)
{
   long i;
   for( i = msval ; i > 0 ;)
   {
       i-- ;asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
       asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");
   }
}

void main(void)
{
   // Disable Watch Dog Timer
   RCONbits.SWDTEN = 0;
   InitializeBoard();
   DelayMs(100);
   UARTInit();
   DelayMs(100);
   SPIinit();
   
   while(1)
   {
       //UARTPutChar('U');
       //UARTPutString("\n\rHello\n\r");
       SPIPut(0x33);
       UARTPutString("0x55\n\r");
       DelayMs(5000);DelayMs(5000);DelayMs(5000);
       DelayMs(5000);DelayMs(5000);DelayMs(5000);
       DelayMs(5000);DelayMs(5000);DelayMs(5000);
       DelayMs(5000);DelayMs(5000);DelayMs(5000);

       SPIPut(0xcc);
       UARTPutString("0xaa\n\r");
       asm("NOP");asm("NOP");

       DelayMs(5000);DelayMs(5000);DelayMs(5000);
       DelayMs(5000);DelayMs(5000);DelayMs(5000);
       DelayMs(5000);DelayMs(5000);DelayMs(5000);
       DelayMs(5000);DelayMs(5000);DelayMs(5000);
   
   }
}
