#include <avr/io.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/pgmspace.h>   /* required by usbdrv.h */

#include "peri.h"
#include "usbdrv.h"

#define RQ_SET_LED         0
#define RQ_SET_BUZZER      1
#define RQ_GET_SWITCH      2
#define RQ_GET_LIGHT       3

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */
usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
    usbRequest_t *rq = (void *)data;

    /* declared as static so they stay valid when usbFunctionSetup returns */
    static uint8_t switch_state;
    static uint16_t light_value;

    if (rq->bRequest == RQ_SET_LED)
    {
        uint8_t led_state = rq->wValue.bytes[0];
        uint8_t led_no  = rq->wIndex.bytes[0];
        set_led(led_no, led_state);
        return 0;
    }
    else if (rq->bRequest == RQ_SET_BUZZER) {
        uint8_t buz_state = rq->wValue.bytes[0];
        set_buzzer(buz_state);
        return 0;
    }
    else if (rq->bRequest == RQ_GET_SWITCH)
    {
        switch_state = SWITCH_PRESSED();

        /* point usbMsgPtr to the data to be returned to host */
        usbMsgPtr = &switch_state;

        /* return the number of bytes of data to be returned to host */
        return 1;
    }
    else if (rq->bRequest == RQ_GET_LIGHT) {
        light_value = read_adc(PC1);
        usbMsgPtr = &light_value;
        return 2;
    }

    /* default for not implemented requests: return no data back to host */
    return 0;
}

/* ------------------------------------------------------------------------- */
int main(void)
{
    init_peri();

    usbInit();

    /* enforce re-enumeration, do this while interrupts are disabled! */
    usbDeviceDisconnect();
    _delay_ms(300);
    usbDeviceConnect();

    /* enable global interrupts */
    sei();

    /* main event loop */
    for(;;)
    {
        usbPoll();
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

