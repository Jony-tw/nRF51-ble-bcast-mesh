#ifndef _RBC_MESH_COMMON_H__
#define _RBC_MESH_COMMON_H__
#include <stdint.h>


/******************************************************************************
* Debug related defines
******************************************************************************/
#ifdef BOARD_PCA10000
    #define TICK_PIN(x) 
    #define SET_PIN(x) 
    #define CLEAR_PIN(x) 
#else
    #ifdef RBC_MESH_DEBUG
        #define TICK_PIN(x) NRF_GPIO->OUTSET = (1 << (x)); \
                                                        __nop();\
                                                        __nop();\
                                                        NRF_GPIO->OUTCLR = (1 << (x))
         
        #define SET_PIN(x) NRF_GPIO->OUTSET = (1 << (x))
        #define CLEAR_PIN(x) NRF_GPIO->OUTCLR = (1 << (x))
    #else
        #define TICK_PIN(x) 
        #define SET_PIN(x) 
        #define CLEAR_PIN(x) 
    #endif
#endif

#define PIN_MESH_TX         (0)
#define PIN_SEARCHING       (1)
#define PIN_CPU_IN_USE      (2)
#define PIN_CONSISTENT      (3)
#define PIN_INCONSISTENT    (4)
#define PIN_RX              (5)
#define PIN_BUTTON          (6)
#define PIN_ABORTED         (7)

#define PIN_INT0            (25)
#define PIN_INT1            (26)
#define PIN_TX0             (27)
#define PIN_TX1             (28)
#define PIN_SYNC_TIME       (29)

#define PIN_RADIO_SIGNAL    (3)
#define PIN_TIMER_SIGNAL    (4)
#define PIN_IN_TIMESLOT     (6)

#define PIN_BIT_H           (25)
#define PIN_BIT_L           (28)

#ifdef RBC_MESH_DEBUG
    #define PIN_OUT(val,bitcount)      for (uint8_t i = 0; i < (bitcount); ++i){ if (((val) >> ((bitcount) - 1 - i) & 0x01)) { TICK_PIN(PIN_BIT_H); } else { TICK_PIN(PIN_BIT_L); } }
#else
    #define PIN_OUT(val,bitcount)   
#endif

#endif /* _RBC_MESH_COMMON_H__ */
