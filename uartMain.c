//proper dumpster fire of a script

#include "msp430.h"

const char RXData[] = {"guy\r\n"};
unsigned int i; // this is my counter

void initUART()
{
    UCA1CTLW0 = UCSWRST;
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BR0 = 52; //from data sheet table 30-5. 9600 baud rate at 8MHz
    UCA1BR1 = 0x00;
    UCA1MCTLW |= UCOS16 | UCBRF_1;
    UCA1CTLW0 &= ~UCSWRST; //intialize the eUSCI
    UCA1IE |= UCRXIE; //recieve interrupt enabled
}

void initDCO()
{
    //FRCTL0 = FRCTLPW | NWAITS_5;
    CSCTL0_H = CSKEY >> 8; //unlock clock regisets
    CSCTL1 = DCOFSEL_3 | DCORSEL; //set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK; //set SMCLK & MCLK to the DCO (digital controlled oscillator)
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; //divde clock by 1
    CSCTL0_H = 0; //lock registers
}

void initGPIO()
{
    PM5CTL0 &=  ~LOCKLPM5; // disable the GPIO power-on Default
    //configure GPIO for uart
    P2SEL1 |= BIT5 | BIT6; //2.5 is TX and 2.6 is RX in e_USCI_UART Mode (A1)
    P2SEL0 &= ~(BIT5 | BIT6);
    P2OUT &= ~BIT1;
    P2DIR |= BIT1;
    //P2OUT &= ~BIT2;
    //P2DIR |= BIT2;

    //Configure pins for 32.7680 clock (Low Frequency)
    //PJSEL0 |= BIT4 | BIT5;
}

int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;                 // Stop Watchdog
  initGPIO();
  initUART();
  initDCO();
  __bis_SR_register(LPM3_bits | GIE);       // Enter LPM3, interrupts enabled
  __no_operation();                         // For debugger
}


#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)

{
  switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        if (UCA1RXBUF == 'a')
        {
            while(!(UCA1IFG&UCTXIFG));
            P2OUT |= BIT1;
            UCA1TXBUF = 't';

        }
        else if (UCA1RXBUF == 'b')
        {
            while(!(UCA1IFG&UCTXIFG));
            P2OUT &= ~BIT1;
            UCA1TXBUF = 'x';
        }
      __no_operation();
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}


