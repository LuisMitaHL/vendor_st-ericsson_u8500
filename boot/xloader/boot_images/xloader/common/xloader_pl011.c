/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*  The output is hardwired to UART2_B, 115200, 8, N, 1                      */

#include "xloader_pl011.h"
#include "ab8500.h"
#include "uart.h" /* for API enum */
/*
 * Ux500 UART registers base for 3 serial devices
 */
#define CFG_UART0_BASE          0x80120000
#define CFG_UART1_BASE          0x80121000
#define CFG_UART2_BASE          0x80007000
#define CFG_SERIAL0         CFG_UART0_BASE
#define CFG_SERIAL1         CFG_UART1_BASE
#define CFG_SERIAL2         CFG_UART2_BASE
#define CONFIG_CONS_INDEX       2
#define CONFIG_PL01x_PORTS      { (void *) (CFG_SERIAL0),   \
                    (void *) (CFG_SERIAL1),     \
                    (void *) (CFG_SERIAL2) }

#define CFG_DEBUG_UART_BASE CFG_UART2_BASE

/* define in uart.h for each project : t_uart_baudrate;
   Baud Rate Divisor = BDR = UARTCLK/(16 x Baud Rate) * 64 = BRDI*64 + BRDF*64
			/16 because OVSFACT = 0b
			64 for have 6bit DIVFRAC (0x0-0x3F)
			BRDI = INT(BDR/64)
			BRDF = INT(BDR-BDRI*64+0.5) 0.5 added for rounding

UARTCLK=26 MHz
==================
baudRATE    DIVINT  (BRDI)          DIVFRAC BRDF
110         14772   (0x39B4)        47      (0x2F)  0x2F39B4
1200        1354    (0x054A)        11      (0x0B)  0x0B054A
2400        677     (0x02A5)        5       (0x05)  0x0502A5
9600        169     (0x00A9)        17      (0x11)  0x1100A9
38400       42      (0x002A)        20      (0x14)  0x14002A
115200      14      (0x000E)        7       (0x07)  0x07000E
460800      3       (0x0003)        34      (0x22)  0x220003
921600      1       (0x0001)        49      (0x31)  0x310001
1843200     0       (0x0000)        56      (0x38)  0x380000
3000000     0       (0x0000)        35      (0x23)  0x230000

UARTCLK=38.4 MHz
====================
baudRATE    DIVINT  (BRDI)          DIVFRAC BRDF
110         21818   (0x553A)        12      (0x0C)  0x0C553A
1200        2000    (0x07D0)        0       (0x00)  0x0007D0
2400        1000    (0x03E8)        0       (0x00)  0x0003E8
9600        250     (0x00FA)        0       (0x00)  0x0000FA
38400       62      (0x003E)        32      (0x20)  0x20003E
115200      20      (0x0014)        53      (0x35)  0x350014
460800      5       (0x0005)        13      (0x0D)  0x0D0005
921600      2       (0x0002)        39      (0x27)  0x270002
1843200     1       (0x0001)        19      (0x13)  0x130001
3000000     0       (0x0000)        51      (0x33)  0x330000
*/

#define writew(addr, val) (*(volatile unsigned long *)(addr) = (val))
#define readw(addr) (*(volatile unsigned long *)(addr))

#define CONSOLE_PORT CONFIG_CONS_INDEX
static volatile unsigned char *const port[] = CONFIG_PL01x_PORTS;
#define NUM_PORTS (sizeof(port)/sizeof(port[0]))

#define BAUDRATE    BR115200BAUD
#define PARITYBIT   NOPARITY_BIT
#define STOPBITS    ONE_STOPBIT
#define DATABITS    DATABITS_8


#define UART_FR_TXFE    0x80    /* Transmit FIFO empty */
#define UART_FR_TXFF    0x20    /* Transmit FIFO full */

volatile t_uart_registers *p_uartRegs;

t_bool pl011_is_tx_fifo_empty(volatile t_uart_registers *p_Regs)
{
    return p_Regs->FR.TransmitFifoEmpty  ?
        TRUE : FALSE;
}

void pl011_putc(char c)
{
    /* int portnum = CONSOLE_PORT; */

    while (p_uartRegs->FR.TransmitFifoFull)
        ;

    p_uartRegs->DATA.Data = c;

}

#if defined(UART_OVER_ULPI) || defined(UART_OVER_USB)

#if defined(UART_OVER_ULPI) && defined(UART_OVER_USB)
#error "Both UART_OVER_ULPI and UART_OVER_USB defined"
#endif

/* Some basic time suport: delay in usec */
#define PER4_BASE               0x80150000
#define PRCMU_BASE             (PER4_BASE + 0x07000)
#define PRCM_TIMER_4_DOWNCOUNT  0x454

void delay_uwait(unsigned long us)
{
    int now;
    int start;
    int count = us / 31;

    now = readw(PRCMU_BASE + PRCM_TIMER_4_DOWNCOUNT);
    start = now;
    while (start - now  < count+2)
        now = readw(PRCMU_BASE + PRCM_TIMER_4_DOWNCOUNT);
}

extern t_uint8 read_hw_i2c(t_uint16 base_addr);
extern void write_hw_i2c(t_uint16 base_addr, t_uint8 value);

int usbswitch_uart_cable(void)
{
    t_uint8 val, usbotgctrl;
    int uart_cable = 0;

    /* check if USB cable already detected */
    val = read_hw_i2c(AB8500_ITSOURCE2) & 0x80;
    if (val != 0)
        return 0;   /* VBUS: it is not UART cable! */

    val = (read_hw_i2c(AB9540_USBLINKSTATUS) >> 3) & 0x1F;
    if (val != 0) {
        if ((val == 0x19) || (val == 0x1A))
            return 1;   /* UART cable detected */
        return 0;   /* cable detected, but not a UART cable! */
    }

    /* if no vbus, test cable resistance */
    usbotgctrl = read_hw_i2c(AB8500_USBOTGCTRL);
    write_hw_i2c(AB8500_USBOTGCTRL, usbotgctrl & (~0x23));
    val = read_hw_i2c(AB8500_REGIDDETCTRL3);
    write_hw_i2c(AB8500_REGIDDETCTRL3, val | 0x80);

    val = read_hw_i2c(AB8500_REGIDDETCTRL2);
    write_hw_i2c(AB8500_REGIDDETCTRL2, val | 0x20);     /* ena pullup 1uA on ID */
    val = read_hw_i2c(AB8500_REGIDDETCTRL1);
    write_hw_i2c(AB8500_REGIDDETCTRL1, val | 0x08);     /* ena ID plug detect comparator */
    if ((read_hw_i2c(AB8500_REGIDDETSTATE) & 0x08) == 0)
        goto detection_done;

    write_hw_i2c(AB8500_REGIDDETCTRL2, 0x87);   /* enable detection supplies */
    write_hw_i2c(AB8500_REGIDDETVTH, 0x40);     /* 619kOhm thresh. */
    delay_uwait(10000);
    if ((read_hw_i2c(AB8500_REGIDDETSTATE) & 0x60) == 0x20) {
        uart_cable = 1;
        goto detection_done;
    }
    write_hw_i2c(AB8500_REGIDDETVTH, 0x20);     /* 523kOhm thresh. */
    delay_uwait(10000);
    if ((read_hw_i2c(AB8500_REGIDDETSTATE) & 0x60) == 0x20)
        uart_cable = 1;

detection_done:
    write_hw_i2c(AB8500_REGIDDETCTRL1, 0x00);   /* disable all ID pullups */
    write_hw_i2c(AB8500_REGIDDETCTRL2, 0x00);   /* disable ID comparator */
    val = read_hw_i2c(AB8500_REGIDDETCTRL3);
    write_hw_i2c(AB8500_REGIDDETCTRL3, val & (~0x80));
    write_hw_i2c(AB8500_USBOTGCTRL, usbotgctrl);

    return uart_cable;
}

void setup_uart_on_device(void)
{
    volatile t_gpio_registers *p_gpioRegs;
    t_uint8 val;

    // USB PHY off
    write_hw_i2c(AB8500_USBPHYCTRL, 0);
    delay_uwait(50000);
    // enable and kick watchdog
    write_hw_i2c(AB8500_MAINWDOGCTRL, 1);
    delay_uwait(50000);
    write_hw_i2c(AB8500_MAINWDOGCTRL, 3);
    delay_uwait(50000);
    write_hw_i2c(AB8500_MAINWDOGCTRL, 0); // works only on non-fused samples
    delay_uwait(50000);
    // mux AP9540 pads GPIO266/267: UART2 to AB9540 GPIO13/50
    p_gpioRegs = (t_gpio_registers *) GPIO8_REG_START_ADDR;
    p_gpioRegs->AFSLA = p_gpioRegs->AFSLA & ~0xFFF;
    p_gpioRegs->AFSLB = (p_gpioRegs->AFSLB & ~0x3FF) | 0xC00;
    p_gpioRegs->PDIS  = (p_gpioRegs->PDIS & ~0x3FF) | 0xC00;
    p_gpioRegs->DATC  = 0xFFD;
    p_gpioRegs->DATS  = 0x002;
#ifdef UART_OVER_USB
    // ABB: set UART-on-USB from AB8505[GPIO13/50]
    val = read_hw_i2c(AB8505_ALTERNATFUNCTION);
    write_hw_i2c(AB8505_ALTERNATFUNCTION, (val & ~0x1C) | 0x0C); // UART from GPIO13/50 pads
    val = read_hw_i2c(AB8500_REGIDDETCTRL4);
    write_hw_i2c(AB8500_REGIDDETCTRL4, val | 0x5); // USB in UART mode from GPIO pads
#elif defined UART_OVER_ULPI
    // ABB: set UART-on-ULPI
    val = read_hw_i2c(AB8500_REGIDDETCTRL4);
    write_hw_i2c(AB8500_REGIDDETCTRL4, (val & ~1) | 4);
#endif
    // USB PHY on
    write_hw_i2c(AB8500_USBPHYCTRL, 2);
    delay_uwait(50000);
#ifdef UART_OVER_USB
    // ABB: GPIO13/50 in Alt mode
    val = read_hw_i2c(AB8505_GPIOSEL2);
    write_hw_i2c(AB8505_GPIOSEL2, val & ~0x10); // ABB: GPIO13 in Alternate mode
    val = read_hw_i2c(AB8505_GPIOSEL7);
    write_hw_i2c(AB8505_GPIOSEL7, val & ~0x2); // ABB: GPIO50 in Alternate mode
    delay_uwait(5000);
#endif
}

#endif


int pl011_init(void)
{
    volatile t_gpio_registers *p_gpioRegs;
    volatile t_prcc_registers *p_prccRegs;
    volatile t_prcmu_hw_registers *p_prcmuRegs;

    /* !
     *  ! !       WARNING ! ! !
     *  If UART2-b config is used you need to
     *  - Init GPIO   in periph 1
     *  - init UART 2 in periph 3
     */

    /* Enable UARTCLK with PRCMU */
    p_prcmuRegs = (t_prcmu_hw_registers *)PRCMU_REG_START_ADDR;

    /* CLK38 SRC = SYSCLK */
    p_prcmuRegs->prcm_uartclk_mgt.Bit.CLK38SRC = 0;

    /* SYSCLK 38,4 Mhz feeds UARTCLK */
    p_prcmuRegs->prcm_uartclk_mgt.Bit.CLK38    = 1;

    /* Enable */
    p_prcmuRegs->prcm_uartclk_mgt.Bit.CLKEN    = 1;

    /* Enable UART clocks with PRCC */
    p_prccRegs = (t_prcc_registers *)CLK_RST3_REG_START_ADDR;

    /*PRCC_PCKEN:  bit6 CLK_P3_UART2*/
    p_prccRegs->pcken = p_prccRegs->pcksr | 0x40;

    /*PRCC_KCKEN: bit6 CLK_UART2 */
    p_prccRegs->kcken = p_prccRegs->pkcksr | 0x40;

    /* Warning: GPIO are in periph 1 ! not in periph 3 */
    p_prccRegs = (t_prcc_registers *)CLK_RST1_REG_START_ADDR;

    /*PRCC_PCKEN: bit9 CLK_P1_GPIO_CTRL */
    p_prccRegs->pcken = p_prccRegs->pcksr | 0x200;

    p_uartRegs = (t_uart_registers *)port[CONSOLE_PORT];

    p_uartRegs->CR.Reg = 0;
    p_uartRegs->RSR = 0;
    p_uartRegs->CR.Bit.UartEnable = 1;
    p_uartRegs->IBRD = (t_uint32) (BAUDRATE & MASK_ALL16);
    p_uartRegs->FBRD = (t_uint32) (BAUDRATE >> 16);

    p_uartRegs->LCRH_TX.Reg = 0;
    p_uartRegs->LCRH_TX.Bit.SendBreak = 0;
    p_uartRegs->LCRH_TX.Bit.FifoEnable = 1;
    p_uartRegs->LCRH_TX.Bit.WordLength = DATABITS;
    p_uartRegs->LCRH_TX.Bit.TwoStopBitsSelect = STOPBITS;

    p_uartRegs->LCRH_TX.Bit.ParityEnable = 0;
    p_uartRegs->LCRH_TX.Bit.StickParitySelect = 0;
    p_uartRegs->LCRH_TX.Bit.EvenParitySelect = 0;


    /** Enable GPIO U_RXD, U_TXD for the corresponding UART. */
#ifdef MACH_L8540
    /* UART2-B GPIO: GPIO 120, 121 Alternate A */
    p_gpioRegs = (t_gpio_registers *)GPIO3_REG_START_ADDR;
    p_gpioRegs->AFSLA |= 0x03000000; /* Set bits 24&25 */
    p_gpioRegs->AFSLB &= 0xFCFFFFFF; /* Reset bits 24&25*/
#else
#if defined(UART_OVER_ULPI) || defined(UART_OVER_USB)
    if (usbswitch_uart_cable() == 1)
        setup_uart_on_device();
#endif
    /* UART2-B GPIO: GPIO 29, 30 Alternate C */
    p_gpioRegs = (t_gpio_registers *)GPIO0_REG_START_ADDR;

    p_gpioRegs->AFSLA |= 0x60000000; /* Set bits */
    p_gpioRegs->AFSLB |= 0x60000000; /* Set bits */
#endif

    /*
     * Set to 8 characters the interrupt FIFO
     * level for transmit and receive
     */
    p_uartRegs->IFLS.TxIntFifoLevel = 3;
    p_uartRegs->IFLS.RxIntFifoLevel = 3;

    /* Clear all interrupts */
    p_uartRegs->ICR.Reg = 0x1FFF;

    p_uartRegs->IMSC.Bit.OverrunError = 1;
    p_uartRegs->IMSC.Bit.BreakError = 1;
    p_uartRegs->IMSC.Bit.ParityError = 1;
    p_uartRegs->IMSC.Bit.FramingError = 1;

    p_uartRegs->CR.Bit.TransmitEnable = 1;

    while (!pl011_is_tx_fifo_empty(p_uartRegs)) ; /* Flush the Tx FIFO */

    return 0;
}
