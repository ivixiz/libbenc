/** @file           : libbenc.hpp
  * @brief          : Encoder-Button control library 
  * @note           : Single file library
  
  * //| us(MHz) | digitalWrite | gio_write | gio_toggle | digitalRead | gio_read   | pinMode      | gio_mode   |
  * //|---------|--------------|-----------|------------|-------------|------------|--------------|------------|
  * //| AVR     | 1.47 (0.68)  | 0.06 (16) | 0.06 (16)  | 3.23 (0.31) | 0.13 (8)   | 3.33 (0.3)   | 0.13 (8)   |
  * //| ESP8266 | 1.56 (0.64)  | 0.08 (12) | 0.5 (2)    | 0.56 (1.8)  | 0.21 (4.8) | 1.43 (0.7)   | 0.67 (1.5) |
  * //| ESP32   | 0.33 (3)     | 0.04 (24) | 0.13 (8)   | 1.67 (0.6)  | 0.08 (12)  | 16.67 (0.06) | 0.08 (12)  |
  * //| ESP32C3 | 0.92 (1)     | 0.06 (16) | 0.19 (5.2) | 0.25 (4)    | 0.04 (24)  | 20 (0.05)    | 0.04 (24)  |
  * //| STM32Fx |      ?       |     ?     |     ?      |      ?      |     ?      |       ?      |     ?      |
  * //| STM32Gx |      ?       |     ?     |     ?      |      ?      |     ?      |       ?      |     ?      | 
  * //| STM32Hx |      ?       |     ?     |     ?      |      ?      |     ?      |       ?      |     ?      |
  */

/*//===USAGE======USAGE======USAGE======USAGE======USAGE===
    //FOR STM32 Platform: declaration  (STM32G474 tested) ##########################
    EncButtonT<// using template
        Pin{GPIOB_BASE, GPIO_PIN_4}, 
        Pin{GPIOB_BASE, GPIO_PIN_5}, 
        Pin{GPIOB_BASE, GPIO_PIN_10}, 
        PinMode{GPIO_PULLUP, GPIO_MODE_IT_RISING_FALLING}, 
        PinMode{GPIO_PULLDOWN, GPIO_MODE_IT_RISING_FALLING}
    > enc;

    EncButton enc; // using constructor

    int main(){
        //after MX_GPIO_Init(); !!!
        enc.init(); //if template

        enc.init({GPIOB, GPIO_PIN_4}, //if contructor
                {GPIOB, GPIO_PIN_5}, 
                {GPIOB, GPIO_PIN_10},
                {GPIO_PULLUP, GPIO_MODE_IT_RISING_FALLING}, 
                {GPIO_PULLDOWN, GPIO_MODE_IT_RISING_FALLING});
        int i = 0;
        enc.setEncISR(true);
        while (1){
            enc.tick();//usage
            if(enc.left()){i += enc.step(i); }else if(enc.right()){ i-= enc.step(i);} 

        }
    }
    //somewhere below... 
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
        if(GPIO_Pin == GPIO_PIN_4 || GPIO_Pin == GPIO_PIN_5 || GPIO_Pin == GPIO_PIN_10){
            enc.tickISR();
        }
    }

    FOR ARDUINO AVR (Atmega328 tested) ####################################################
    #define FASTSTEPS {1, 10, 20, 50, 100} //define your own multipliers of fast turn steps
    #include <libbenc.hpp>
    EncButton eb(EN_CLK,EN_DAT,EN_SW,INPUT,INPUT);// constructor

    EncButtonT<EN_CLK,EN_DAT,EN_SW> eb; //template

    ISR(PCINT0_vect) {
    eb.tickISR();
    }
    void isr(){
    eb.tickISR();
    }
    void setup(){
        ...
        attachInterrupt(1, isr, CHANGE);
        PCICR |= (1 << PCIE0);
        PCMSK0 |= (1 << PCINT2);  // pin 10
        PCMSK0 |= (1 << PCINT4);  // pin 12
        PCMSK0 |= (1 << PCINT0);
        ...
    }

    void loop(){
    eb.tick();//usage
    ...
    }


*/

#pragma once
//1728kB
//========================================== OPTIONS =======================================================
#define EB_NO_FUNCTIONAL   //uncomment to disable std::function<void()> ActionHandler; in order to save ~5kB
#define GIO_WITH_SHIFT_HPP

#if defined(STM32WB)//detect platform
    #define STM32
    #include "stm32wbxx_hal.h"
#elif defined(STM32F0)
    #define STM32
    #include "stm32f0xx_hal.h"
#elif defined(STM32F1)
    #define STM32
    #include "stm32f1xx_hal.h"
#elif defined(STM32F4)
    #define STM32
    #include "stm32f4xx_hal.h"
#elif defined(STM32L0)
    #define STM32
    #include "stm32l0xx_hal.h"
#elif defined(STM32L1)
    #define STM32
    #include "stm32l1xx_hal.h"
#elif defined(STM32L4)
    #define STM32
    #include "stm32l4xx_hal.h"
#elif defined(STM32L5)
    #define STM32
    #include "stm32l5xx_hal.h"
#elif defined(STM32F3)
    #define STM32
    #include "stm32f3xx_hal.h"
#elif defined(STM32H7)
    #define STM32
    #include "stm32h7xx_hal.h"
#elif defined(STM32F7)
    #define STM32
    #include "stm32f7xx_hal.h"
#elif defined(STM32G0)
    #define STM32
    #include "stm32g0xx_hal.h"
#elif defined(STM32G4)
    #define STM32
    #include "stm32g4xx_hal.h"
#elif defined(STM32C0)
    #define STM32
    #include "stm32c0xx_hal.h"
#elif defined(STM32U5)
    #define STM32
    #include "stm32u5xx_hal.h"
#endif

#ifndef LIBBENC_HPP
#define LIBBENC_HPP
    #define _GIO_INLINE static inline __attribute__((always_inline))
    #define AUTO           2

    #ifndef FASTSTEPS
        #define FASTSTEPS {1, 10, 100, 1000, 10000, 50000}
    #endif
    
    #ifndef __AVR__
        #undef abs //abs is defined as macro and conflicts with abs() function
        #ifndef EB_NO_FUNCTIONAL
            #include <functional>
        #endif
    #endif
   
    #ifdef STM32
        #define LOW            0
        #define HIGH           1
        #define INPUT          GPIO_NOPULL
        #define INPUT_PULLUP   GPIO_PULLUP
        #define INPUT_PULLDOWN GPIO_PULLDOWN
        
        _GIO_INLINE void     delay(uint32_t ms){ HAL_Delay(ms); }
        _GIO_INLINE uint32_t millis(void)      { return HAL_GetTick(); }
        typedef struct Pin {
            std::uintptr_t port{};
            uint16_t n{};

            constexpr Pin() noexcept = default;

            // для template / constexpr
            constexpr Pin(std::uintptr_t baseAddr, uint16_t pin) noexcept
                : port(baseAddr), n(pin) {}
            // для runtime init({GPIOB, GPIO_PIN_4}, ...)
            Pin(GPIO_TypeDef* p, uint16_t pin) noexcept
                : port(reinterpret_cast<std::uintptr_t>(p)), n(pin) {}
            constexpr bool operator==(const Pin&) const = default;
            GPIO_TypeDef* gpio() const noexcept {
                return reinterpret_cast<GPIO_TypeDef*>(port);
            }
            constexpr bool isValid() const noexcept {
                    return port != 0 && n && ((n & (n - 1)) == 0);
            }
        } Pin;

        typedef struct PinMode { 
            uint32_t io; //INPUT / OUTPUT / INPUT_PULLUP 
            uint32_t it; //GPIO_MODE_XXXX
        } PinMode;
    #else  
        #include "Arduino.h"
        template<typename Reg, typename Mask> inline bool greg_read(Reg* reg, Mask mask) { return (*reg & mask) != 0; }
        template<typename Reg, typename Mask> inline void greg_set(Reg* reg, Mask mask) { *reg |= mask; }
        template<typename Reg, typename Mask> inline void greg_clr(Reg* reg, Mask mask) { *reg &= ~mask;}
        template<typename Reg, typename Mask> inline void greg_write(Reg* reg, Mask mask, bool val) {
            if (val) *reg |= mask;
            else     *reg &= ~mask;
        }
        typedef uint8_t Pin;
        typedef uint8_t PinMode;
    #endif


#ifndef GIO_LIB_HPP
#define GIO_LIB_HPP
    namespace gio {
    #if defined(__AVR__) || defined(ARDUINO_ARCH_AVR)
        // --- Arduino Mega and ATmega128x/256x based boards ---
        #if (defined(ARDUINO_AVR_MEGA) ||     \
            defined(ARDUINO_AVR_MEGA1280) || \
            defined(ARDUINO_AVR_MEGA2560) || \
            defined(__AVR_ATmega1280__) ||   \
            defined(__AVR_ATmega1281__) ||   \
            defined(__AVR_ATmega2560__) ||   \
            defined(__AVR_ATmega2561__))
        #define __avr_pin_to_port(P) \
            (((P) >= 22 && (P) <= 29) ? &PORTA : ((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &PORTB : (((P) >= 30 && (P) <= 37) ? &PORTC : ((((P) >= 18 && (P) <= 21) || (P) == 38) ? &PORTD : ((((P) <= 3) || (P) == 5) ? &PORTE : (((P) >= 54 && (P) <= 61) ? &PORTF : ((((P) >= 39 && (P) <= 41) || (P) == 4) ? &PORTG : ((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &PORTH : (((P) == 14 || (P) == 15) ? &PORTJ : (((P) >= 62 && (P) <= 69) ? &PORTK : &PORTL))))))))))
        #define __avr_pin_to_ddr(P) \
            (((P) >= 22 && (P) <= 29) ? &DDRA : ((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &DDRB : (((P) >= 30 && (P) <= 37) ? &DDRC : ((((P) >= 18 && (P) <= 21) || (P) == 38) ? &DDRD : ((((P) <= 3) || (P) == 5) ? &DDRE : (((P) >= 54 && (P) <= 61) ? &DDRF : ((((P) >= 39 && (P) <= 41) || (P) == 4) ? &DDRG : ((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &DDRH : (((P) == 14 || (P) == 15) ? &DDRJ : (((P) >= 62 && (P) <= 69) ? &DDRK : &DDRL))))))))))
        #define __avr_pin_to_pin(P) \
            (((P) >= 22 && (P) <= 29) ? &PINA : ((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &PINB : (((P) >= 30 && (P) <= 37) ? &PINC : ((((P) >= 18 && (P) <= 21) || (P) == 38) ? &PIND : ((((P) <= 3) || (P) == 5) ? &PINE : (((P) >= 54 && (P) <= 61) ? &PINF : ((((P) >= 39 && (P) <= 41) || (P) == 4) ? &PING : ((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &PINH : (((P) == 14 || (P) == 15) ? &PINJ : (((P) >= 62 && (P) <= 69) ? &PINK : &PINL))))))))))
        #define __avr_pin_to_bit(P) \
            (((P) >= 7 && (P) <= 9) ? (P)-3 : (((P) >= 10 && (P) <= 13) ? (P)-6 : (((P) >= 22 && (P) <= 29) ? (P)-22 : (((P) >= 30 && (P) <= 37) ? 37 - (P) : (((P) >= 39 && (P) <= 41) ? 41 - (P) : (((P) >= 42 && (P) <= 49) ? 49 - (P) : (((P) >= 50 && (P) <= 53) ? 53 - (P) : (((P) >= 54 && (P) <= 61) ? (P)-54 : (((P) >= 62 && (P) <= 69) ? (P)-62 : (((P) == 0 || (P) == 15 || (P) == 17 || (P) == 21) ? 0 : (((P) == 1 || (P) == 14 || (P) == 16 || (P) == 20) ? 1 : (((P) == 19) ? 2 : (((P) == 5 || (P) == 6 || (P) == 18) ? 3 : (((P) == 2) ? 4 : (((P) == 3 || (P) == 4) ? 5 : 7)))))))))))))))
        // --- Arduino MightyCore standard pinout ---
        #elif (defined(__AVR_ATmega1284P__) || \
            defined(__AVR_ATmega1284__)     || \
            defined(__AVR_ATmega644P__)     || \
            defined(__AVR_ATmega644A__)     || \
            defined(__AVR_ATmega644__)      || \
            defined(__AVR_ATmega324PB__)    || \
            defined(__AVR_ATmega324PA__)    || \
            defined(__AVR_ATmega324P__)     || \
            defined(__AVR_ATmega324A__)     || \
            defined(__AVR_ATmega164P__)     || \
            defined(__AVR_ATmega164A__)     || \
            defined(__AVR_ATmega32__)       || \
            defined(__AVR_ATmega16__)       || \
            defined(__AVR_ATmega8535__))    && \
            !defined(BOBUINO_PINOUT)
        #if defined(__AVR_ATmega324PB__)
        #define __avr_pin_to_port(P) \
            (((P) <= 7) ? &PORTB : (((P) >= 8 && (P) <= 15) ? &PORTD : (((P) >= 16 && (P) <= 23) ? &PORTC : (((P) >= 24 && (P) <= 31) ? &PORTA : &PORTE))))
        #define __avr_pin_to_ddr(P) \
            (((P) <= 7) ? &DDRB : (((P) >= 8 && (P) <= 15) ? &DDRD : (((P) >= 16 && (P) <= 23) ? &DDRC : (((P) >= 24 && (P) <= 31) ? &DDRA : &DDRE))))
        #define __avr_pin_to_pin(P) \
            (((P) <= 7) ? &PINB : (((P) >= 8 && (P) <= 15) ? &PIND : (((P) >= 16 && (P) <= 23) ? &PINC : (((P) >= 24 && (P) <= 31) ? &PINA : &PINE))))
        #define __avr_pin_to_bit(P) \
            (((P) <= 7) ? (P) : (((P) >= 8 && (P) <= 15) ? (P)-8 : (((P) >= 16 && (P) <= 23) ? (P)-16 : (((P) >= 16 && (P) <= 23) ? (P)-24 : (P)-32))))
        #elif defined(PORTA)
        #define __avr_pin_to_port(P) \
            (((P) <= 7) ? &PORTB : (((P) >= 8 && (P) <= 15) ? &PORTD : (((P) >= 16 && (P) <= 23) ? &PORTC : &PORTA)))
        #define __avr_pin_to_ddr(P) \
            (((P) <= 7) ? &DDRB : (((P) >= 8 && (P) <= 15) ? &DDRD : (((P) >= 16 && (P) <= 23) ? &DDRC : &DDRA)))
        #define __avr_pin_to_pin(P) \
            (((P) <= 7) ? &PINB : (((P) >= 8 && (P) <= 15) ? &PIND : (((P) >= 16 && (P) <= 23) ? &PINC : &PINA)))
        #define __avr_pin_to_bit(P) \
            (((P) <= 7) ? (P) : (((P) >= 8 && (P) <= 15) ? (P)-8 : (((P) >= 16 && (P) <= 23) ? (P)-16 : (P)-24)))
        #else
        #define __avr_pin_to_port(P) \
            (((P) <= 7) ? &PORTB : (((P) >= 8 && (P) <= 15) ? &PORTD : &PORTC))
        #define __avr_pin_to_ddr(P) \
            (((P) <= 7) ? &DDRB : (((P) >= 8 && (P) <= 15) ? &DDRD : &DDRC))
        #define __avr_pin_to_pin(P) \
            (((P) <= 7) ? &PINB : (((P) >= 8 && (P) <= 15) ? &PIND : &PINC))
        #define __avr_pin_to_bit(P) \
            (((P) <= 7) ? (P) : (((P) >= 8 && (P) <= 15) ? (P)-8 : (((P) >= 16 && (P) <= 23) ? (P)-16 : (P)-24)))
        #endif
        // --- Arduino Leonardo and ATmega16U4/32U4 based boards ---
        #elif (defined(ARDUINO_AVR_LEONARDO) || \
            defined(__AVR_ATmega16U4__) ||   \
            defined(__AVR_ATmega32U4__))
        #if defined(TEENSYDUINO)
        #define __avr_pin_to_port(P)                                                                                                     \
            ((((P) <= 4) || ((P) >= 13 && (P) <= 15)) ? &PORTB : (((P) == 9 || (P) == 10) ? &PORTC : (((P) >= 16 && (P) <= 21)) ? &PORTF \
                                                                                                                                : &PORTD))
        #define __avr_pin_to_ddr(P)                                                                                                   \
            ((((P) <= 4) || ((P) >= 13 && (P) <= 15)) ? &DDRB : (((P) == 9 || (P) == 10) ? &DDRC : (((P) >= 16 && (P) <= 21)) ? &DDRF \
                                                                                                                            : &DDRD))
        #define __avr_pin_to_pin(P)                                                                                                   \
            ((((P) <= 4) || ((P) >= 13 && (P) <= 15)) ? &PINB : (((P) == 9 || (P) == 10) ? &PINC : (((P) >= 16 && (P) <= 21)) ? &PINF \
                                                                                                                            : &PIND))
        #define __avr_pin_to_bit(P) \
            (((P) <= 3) ? (P) : (((P) == 4 || (P) == 12) ? 7 : (((P) <= 8) ? (P)-5 : (((P) <= 10) ? (P)-3 : (((P) == 11) ? 6 : (((P) <= 15) ? (P)-9 : (((P) <= 19) ? 23 - (P) : (((P) <= 21) ? 21 - (P) : (P)-18))))))))
        #else
        #define __avr_pin_to_port(P)                                                                                                                                         \
            ((((P) <= 4) || (P) == 6 || (P) == 12 || (P) == 24 || (P) == 25 || (P) == 29) ? &PORTD : (((P) == 5 || (P) == 13) ? &PORTC : (((P) >= 18 && (P) <= 23)) ? &PORTF \
                                                                                                                                                                    : (((P) == 7) ? &PORTE : &PORTB)))
        #define __avr_pin_to_ddr(P)                                                                                                                                       \
            ((((P) <= 4) || (P) == 6 || (P) == 12 || (P) == 24 || (P) == 25 || (P) == 29) ? &DDRD : (((P) == 5 || (P) == 13) ? &DDRC : (((P) >= 18 && (P) <= 23)) ? &DDRF \
                                                                                                                                                                : (((P) == 7) ? &DDRE : &DDRB)))
        #define __avr_pin_to_pin(P)                                                                                                                                       \
            ((((P) <= 4) || (P) == 6 || (P) == 12 || (P) == 24 || (P) == 25 || (P) == 29) ? &PIND : (((P) == 5 || (P) == 13) ? &PINC : (((P) >= 18 && (P) <= 23)) ? &PINF \
                                                                                                                                                                : (((P) == 7) ? &PINE : &PINB)))
        #define __avr_pin_to_bit(P) \
            (((P) >= 8 && (P) <= 11) ? (P)-4 : (((P) >= 18 && (P) <= 21) ? 25 - (P) : (((P) == 0) ? 2 : (((P) == 1) ? 3 : (((P) == 2) ? 1 : (((P) == 3) ? 0 : (((P) == 4) ? 4 : (((P) == 6) ? 7 : (((P) == 13) ? 7 : (((P) == 14) ? 3 : (((P) == 15) ? 1 : (((P) == 16) ? 2 : (((P) == 17) ? 0 : (((P) == 22) ? 1 : (((P) == 23) ? 0 : (((P) == 24) ? 4 : (((P) == 25) ? 7 : (((P) == 26) ? 4 : (((P) == 27) ? 5 : 6)))))))))))))))))))
        #endif
        // --- Arduino Uno and ATmega168/328 based boards ---
        #elif (defined(ARDUINO_AVR_UNO)      || \
            defined(ARDUINO_AVR_DUEMILANOVE) || \
            defined(__AVR_ATmega8__)         || \
            defined(__AVR_ATmega48__)        || \
            defined(__AVR_ATmega48P__)       || \
            defined(__AVR_ATmega48PB__)      || \
            defined(__AVR_ATmega88P__)       || \
            defined(__AVR_ATmega88PB__)      || \
            defined(__AVR_ATmega168__)       || \
            defined(__AVR_ATmega168PA__)     || \
            defined(__AVR_ATmega168PB__)     || \
            defined(__AVR_ATmega328__)       || \
            defined(__AVR_ATmega328P__)      || \
            defined(__AVR_ATmega328PB__))
        #if defined(__AVR_ATmega48PB__) || defined(__AVR_ATmega88PB__) || defined(__AVR_ATmega168PB__) || defined(__AVR_ATmega328PB__)
        #define __avr_pin_to_port(P) \
            (((P) <= 7) ? &PORTD : (((P) >= 8 && (P) <= 13) ? &PORTB : (((P) >= 14 && (P) <= 19) ? &PORTC : &PORTE)))
        #define __avr_pin_to_ddr(P) \
            (((P) <= 7) ? &DDRD : (((P) >= 8 && (P) <= 13) ? &DDRB : (((P) >= 14 && (P) <= 19) ? &DDRC : &DDRE)))
        #define __avr_pin_to_pin(P) \
            (((P) <= 7) ? &PIND : (((P) >= 8 && (P) <= 13) ? &PINB : (((P) >= 14 && (P) <= 19) ? &PINC : &PINE)))
        #define __avr_pin_to_bit(P) \
            (((P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P)-8 : (((P) >= 14 && (P) <= 19) ? (P)-14 : (((P) >= 20 && (P) <= 21) ? (P)-18 : (P)-22))))
        #else
        #define __avr_pin_to_port(P) \
            (((P) <= 7) ? &PORTD : (((P) >= 8 && (P) <= 13) ? &PORTB : &PORTC))
        #define __avr_pin_to_ddr(P) \
            (((P) <= 7) ? &DDRD : (((P) >= 8 && (P) <= 13) ? &DDRB : &DDRC))
        #define __avr_pin_to_pin(P) \
            (((P) <= 7) ? &PIND : (((P) >= 8 && (P) <= 13) ? &PINB : &PINC))
        #define __avr_pin_to_bit(P) \
            (((P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P)-8 : (P)-14))
        #endif
        #elif defined(__AVR_ATmega4809__) // --- Arduino Uno WiFi Rev 2, Nano Every ---
        #define __avr_pin_to_port(P)                                                                                 \
            (((P) == 2 || (P) == 7) ? &VPORTA.OUT : ((P) == 5 || (P) == 9 || (P) == 10)                ? &VPORTB.OUT \
                                                : ((P) == 4)                                           ? &VPORTC.OUT \
                                                : (((P) >= 14 && (P) <= 17) || (P) == 20 || (P) == 21) ? &VPORTD.OUT \
                                                : ((P) == 8 || (P) == 11 || (P) == 12 || (P) == 13)    ? &VPORTE.OUT \
                                                                                                    : &VPORTF.OUT)
        #define __avr_pin_to_ddr(P)                                                                                  \
            (((P) == 2 || (P) == 7) ? &VPORTA.DIR : ((P) == 5 || (P) == 9 || (P) == 10)                ? &VPORTB.DIR \
                                                : ((P) == 4)                                           ? &VPORTC.DIR \
                                                : (((P) >= 14 && (P) <= 17) || (P) == 20 || (P) == 21) ? &VPORTD.DIR \
                                                : ((P) == 8 || (P) == 11 || (P) == 12 || (P) == 13)    ? &VPORTE.DIR \
                                                                                                    : &VPORTF.DIR)
        #define __avr_pin_to_pin(P)                                                                                \
            (((P) == 2 || (P) == 7) ? &VPORTA.IN : ((P) == 5 || (P) == 9 || (P) == 10)                ? &VPORTB.IN \
                                            : ((P) == 4)                                           ? &VPORTC.IN \
                                            : (((P) >= 14 && (P) <= 17) || (P) == 20 || (P) == 21) ? &VPORTD.IN \
                                            : ((P) == 8 || (P) == 11 || (P) == 12 || (P) == 13)    ? &VPORTE.IN \
                                                                                                    : &VPORTF.IN)
        #define __avr_pin_to_bit(P)                                                                                       \
            (((P) == 2 || (P) == 9 || (P) == 11 || (P) == 17) ? 0 : ((P) == 7 || (P) == 10 || (P) == 12 || (P) == 16) ? 1 \
                                                                : ((P) == 5 || (P) == 13 || (P) == 15 || (P) == 18)   ? 2 \
                                                                : ((P) == 9 || (P) == 14 || (P) == 19)                ? 3 \
                                                                : ((P) == 6 || (P) == 20)                             ? 4 \
                                                                : ((P) == 3 || (P) == 21)                             ? 5 \
                                                                                                                    : 6)
        // TinyCore
        // https://raw.githubusercontent.com/xukangmin/TinyCore/master/avr/package/package_tinycore_index.json
        // https://docs.tinycore.dev/en/latest/
        #elif defined(__AVR_ATtiny1616__) || defined(__AVR_ATtiny3216__) || defined(__AVR_ATtiny3217__)
        #define __avr_pin_to_port(P) ((P) <= 5 ? &VPORTB.OUT : ((P) <= 9 ? &VPORTC.OUT : ((P) <= 16 ? &VPORTA.OUT : ((P) <= 18 ? &VPORTB.OUT : &VPORTC.OUT))))
        #define __avr_pin_to_ddr(P) ((P) <= 5 ? &VPORTB.DIR : ((P) <= 9 ? &VPORTC.DIR : ((P) <= 16 ? &VPORTA.DIR : ((P) <= 18 ? &VPORTB.DIR : &VPORTC.DIR))))
        #define __avr_pin_to_pin(P) ((P) <= 5 ? &VPORTB.IN : ((P) <= 9 ? &VPORTC.IN : ((P) <= 16 ? &VPORTA.IN : ((P) <= 18 ? &VPORTB.IN : &VPORTC.IN))))
        #define __avr_pin_to_bit(P) ((P) <= 3 ? (3 - P) : ((P) <= 5 ? (P) : ((P) <= 9 ? (P - 6) : ((P) <= 16 ? ((P)-9) : ((P) <= 18 ? ((P)-11) : ((P)-15))))))
        #elif defined(__AVR_ATtiny1614__)
        #define __avr_pin_to_port(P) ((P) <= 3 ? &VPORTA.OUT : ((P) <= 7 ? &VPORTB.OUT : &VPORTA.OUT))
        #define __avr_pin_to_ddr(P) ((P) <= 3 ? &VPORTA.DIR : ((P) <= 7 ? &VPORTB.DIR : &VPORTA.DIR))
        #define __avr_pin_to_pin(P) ((P) <= 3 ? &VPORTA.IN : ((P) <= 7 ? &VPORTB.IN : &VPORTA.IN))
        #define __avr_pin_to_bit(P) ((P) <= 3 ? (P + 4) : ((P) <= 7 ? (7 - P) : ((P) <= 10 ? (P - 7) : (P)-11)))
        #elif defined(__AVR_ATtiny816__)
        // https://github.com/Arduino-IRremote/Arduino-IRremote/discussions/1029
        #define __avr_pin_to_port(P) ((P) <= 3 ? &VPORTA.OUT : ((P) <= 9 ? &VPORTB.OUT : ((P) <= 13 ? &VPORTC.OUT : ((P) <= 17 ? &VPORTA.OUT : &VPORTC.OUT))))
        #define __avr_pin_to_ddr(P) ((P) <= 3 ? &VPORTA.DIR : ((P) <= 9 ? &VPORTB.DIR : ((P) <= 13 ? &VPORTC.DIR : ((P) <= 17 ? &VPORTA.DIR : &VPORTC.DIR))))
        #define __avr_pin_to_pin(P) ((P) <= 3 ? &VPORTA.IN : ((P) <= 9 ? &VPORTB.IN : ((P) <= 13 ? &VPORTC.IN : ((P) <= 17 ? &VPORTA.IN : &VPORTC.IN))))
        #define __avr_pin_to_bit(P) ((P) <= 3 ? (P + 4) : ((P) <= 9 ? (9 - P) : ((P) <= 13 ? (P - 10) : ((P) <= 16 ? (P - 13) : ((P)-17)))))
        // --- ATtinyX5 ---
        #elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
        // we have only PORTB
        #define __avr_pin_to_port(P) (&PORTB)
        #define __avr_pin_to_ddr(P) (&DDRB)
        #define __avr_pin_to_pin(P) (&PINB)
        #define __avr_pin_to_bit(P) (((P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P)-8 : (P)-14))
        // --- ATtiny88 ---
        #elif defined(__AVR_ATtiny88__)
        #if defined(ARDUINO_AVR_DIGISPARKPRO)
        #define __avr_pin_to_port(P) ((P) <= 7 ? &PORTD : ((P) <= 14 ? &PORTB : ((P) <= 18 ? &PORTA : &PORTC)))
        #define __avr_pin_to_ddr(P) ((P) <= 7 ? &DDRD : ((P) <= 14 ? &DDRB : ((P) <= 18 ? &DDRA : &DDRC)))
        #define __avr_pin_to_pin(P) ((P) <= 7 ? &PIND : ((P) <= 14 ? &PINB : ((P) <= 18 ? &PINA : &PINC)))
        #define __avr_pin_to_bit(P) ((P) <= 7 ? (P) : ((P) <= 13 ? ((P)-8) : ((P) == 14 ? 7 : ((P) <= 16 ? ((P)-14) : ((P) <= 18 ? ((P)-17) : ((P) == 25 ? 7 : ((P)-19)))))))
        #else
        #define __avr_pin_to_port(P) ((P) <= 7 ? &PORTD : ((P) <= 15 ? &PORTB : ((P) <= 22 ? &PORTC : ((P) <= 26 ? &PORTA : &PORTC))))
        #define __avr_pin_to_ddr(P) ((P) <= 7 ? &DDRD : ((P) <= 15 ? &DDRB : ((P) <= 22 ? &DDRC : ((P) <= 26 ? &DDRA : &DDRC))))
        #define __avr_pin_to_pin(P) ((P) <= 7 ? &PIND : ((P) <= 15 ? &PINB : ((P) <= 22 ? &PINC : ((P) <= 26 ? &PINA : &PINC))))
        #define __avr_pin_to_bit(P) ((P) <= 15 ? ((P)&0x7) : ((P) == 16 ? (7) : ((P) <= 22 ? ((P)-17) : ((P) == 27 ? (6) : ((P)-23)))))
        #endif
        // --- ATtinyX4 + ATtinyX7 ---
        #elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny441__) || defined(__AVR_ATtiny841__) || defined(__AVR_ATtiny87__) || defined(__AVR_ATtiny167__)
        #if defined(ARDUINO_AVR_DIGISPARKPRO) || PIN_PA7 == 5
        // Strange enumeration of pins on Digispark board and core library
        #define __avr_pin_to_port(P) (((P) <= 4) ? &PORTB : &PORTA)
        #define __avr_pin_to_ddr(P) (((P) <= 4) ? &DDRB : &DDRA)
        #define __avr_pin_to_pin(P) (((P) <= 4) ? &PINB : &PINA)
        #define __avr_pin_to_bit(P) (((P) <= 2) ? (P) : (((P) == 3) ? 6 : (((P) == 4) ? 3 : (((P) == 5) ? 7 : (P)-6))))
        #else
        // ATtinyX4: PORTA for 0 to 7, PORTB for 8 to 11
        // ATtinyX7: PORTA for 0 to 7, PORTB for 8 to 15
        #define __avr_pin_to_port(P) (((P) <= 7) ? &PORTA : &PORTB)
        #define __avr_pin_to_ddr(P) (((P) <= 7) ? &DDRA : &DDRB)
        #define __avr_pin_to_pin(P) (((P) <= 7) ? &PINA : &PINB)
        #endif
        #if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny441__) || defined(__AVR_ATtiny841__)
        // https://github.com/SpenceKonde/ATTinyCore/blob/v2.0.0-devThis-is-the-head-submit-PRs-against-this/avr/variants/tinyx41_cw/pins_arduino.h#L334
        // Clockwise layout
        #define __avr_pin_to_bit(P) (((P) <= 7) ? (P) : ((P) == 11 ? (3) : 10 - (P)))
        #else
        #define __avr_pin_to_bit(P) (((P) <= 7) ? (P) : (P)-8)
        #endif
        #endif
        _GIO_INLINE void mode(Pin n, PinMode m){
        #if defined(__avr_pin_to_port)
            if (__builtin_constant_p(n) && __builtin_constant_p(m)){
                switch (m){
                    case INPUT:
                        bitClear(*__avr_pin_to_ddr(n), __avr_pin_to_bit(n));
                        bitClear(*__avr_pin_to_port(n), __avr_pin_to_bit(n));
                        break;
                    case INPUT_PULLUP:
                        bitClear(*__avr_pin_to_ddr(n), __avr_pin_to_bit(n));
                        bitSet(*__avr_pin_to_port(n), __avr_pin_to_bit(n));
                        break;
                    case OUTPUT:
                        bitSet(*__avr_pin_to_ddr(n), __avr_pin_to_bit(n));
                        break;
                }
            } else
        #endif
            {
                switch (m){
                    case INPUT:
                        greg_clr(portModeRegister(digitalPinToPort(n)), digitalPinToBitMask(n));
                        greg_clr(portOutputRegister(digitalPinToPort(n)), digitalPinToBitMask(n));
                        break;
                    case INPUT_PULLUP:
                        greg_clr(portModeRegister(digitalPinToPort(n)), digitalPinToBitMask(n));
                        greg_set(portOutputRegister(digitalPinToPort(n)), digitalPinToBitMask(n));
                        break;
                    case OUTPUT:
                        greg_set(portModeRegister(digitalPinToPort(n)), digitalPinToBitMask(n));
                        break;
                }
            }
        }
        _GIO_INLINE int read(Pin n){ // read
        #if defined(__avr_pin_to_pin)
            if (__builtin_constant_p(n)){ return (bitRead(*__avr_pin_to_pin(n), __avr_pin_to_bit(n))) ? 1 : 0; } else
        #endif
            { return greg_read(portInputRegister(digitalPinToPort(n)), digitalPinToBitMask(n)); }
        }
        _GIO_INLINE void high(Pin n){ // high
        #if defined(__avr_pin_to_port)
            if (__builtin_constant_p(n)){ bitSet(*__avr_pin_to_port(n), __avr_pin_to_bit(n)); } else
        #endif
            { greg_set(portOutputRegister(digitalPinToPort(n)), digitalPinToBitMask(n)); }
        }
        _GIO_INLINE void low(Pin n){ // low
        #if defined(__avr_pin_to_port)
            if (__builtin_constant_p(n)){ bitClear(*__avr_pin_to_port(n), __avr_pin_to_bit(n)); } else
        #endif
            { greg_clr(portOutputRegister(digitalPinToPort(n)), digitalPinToBitMask(n)); }
        }
        _GIO_INLINE void write(Pin n, int V){ V ? high(n) : low(n); } // write
        _GIO_INLINE void toggle(Pin n){ // toggle
        #if defined(__avr_pin_to_pin)
            if (__builtin_constant_p(n)){ bitSet(*__avr_pin_to_pin(n), __avr_pin_to_bit(n));} else
        #endif
            { greg_set(portInputRegister(digitalPinToPort(n)), digitalPinToBitMask(n)); }
        }
        _GIO_INLINE void init(Pin P, PinMode m = {INPUT}){  mode(P, m); } // init
    #elif defined(ESP8266) || defined(ARDUINO_ARCH_ESP8266)
        _GIO_INLINE void mode(Pin n, PinMode m){
            // dsnt work
            // if (m == INPUT) GPE &= ~(1 << pin);
            // else if (m == OUTPUT) GPE |= (1 << pin);
            switch (m){
                case INPUT:
                case INPUT_PULLUP:
                    if (n < 16){
                        GPF(n) = GPFFS(GPFFS_GPIO(n));
                        GPEC = (1 << n);
                        GPC(n) = (GPC(n) & (0xF << GPCI)) | (1 << GPCD);
                        //if (m == INPUT_PULLUP) 
                        GPF(n) |= (1 << GPFPU);
                    } else if (n == 16){
                        GPF16 = GP16FFS(GPFFS_GPIO(n));
                        GPC16 = 0;
                        GP16E &= ~1;
                    }
                    break;
                case OUTPUT:
                    if (n < 16){
                        GPF(n) = GPFFS(GPFFS_GPIO(n));
                        GPC(n) = (GPC(n) & (0xF << GPCI));
                        GPES = (1 << n);
                    } else if (n == 16){
                        GPF16 = GP16FFS(GPFFS_GPIO(n));
                        GPC16 = 0;
                        GP16E |= 1;
                    }
                    break;

                default:
                    pinMode(n, m);
                    break;
            }
        }
        _GIO_INLINE int read(Pin n){ return (n < 16) ? GPIP(n) : (GP16I & 0x01); } // read 
        _GIO_INLINE void low(Pin n){ 
            if (n < 16){GPOC = (1 << n);} else if (n == 16){GP16O &= ~1;} }// low
        _GIO_INLINE void high(Pin n){
            if (n < 16){GPOS = (1 << n);} else if (n == 16){GP16O |= 1;} }// high
        _GIO_INLINE void write(Pin n, uint8_t val){ val ? high(n) : low(n); } // write
        _GIO_INLINE void toggle(Pin n){
            if (n < 16){ if (GPIP(n)){GPOC = (1 << n);} else {GPOS = (1 << n);} } 
            else if (n == 16){ if (GP16I & 0x01){GP16O &= ~1;} else {GP16O |= 1;} } } // toggle
        _GIO_INLINE void init(Pin n, PinMode m = {INPUT}){ mode(n, m); } // init
    #elif defined(ESP32) || defined(ARDUINO_ARCH_ESP32)
        #include <driver/rtc_io.h>
        #include <soc/gpio_struct.h>
        #if ESP_IDF_VERSION_MAJOR < 4
            #define _ESP32_IDF_V3_REG()                                                                               \
            uint32_t rtc_reg(rtc_gpio_desc[pin].reg);                                                             \
            if (rtc_reg) {                                                                                        \
                ESP_REG(rtc_reg) = ESP_REG(rtc_reg) & ~(rtc_gpio_desc[pin].mux);                                  \
                ESP_REG(rtc_reg) = ESP_REG(rtc_reg) & ~(rtc_gpio_desc[pin].pullup | rtc_gpio_desc[pin].pulldown); \
            }
        #else
            #define _ESP32_IDF_V3_REG()
        #endif
        #define _IS_ESP_Cx_ defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
        // mode
        _GIO_INLINE void mode(Pin pin, PinMode mode) {
            switch (mode) {
                case INPUT:
                    pin = digitalPinToGPIONumber(pin);
        #ifdef _IS_ESP_Cx_
                    GPIO.enable_w1tc.val = (1ul << (pin));
        #else
                    if (digitalPinIsValid(pin)) {
                        _ESP32_IDF_V3_REG();
                        if (pin < 32) GPIO.enable_w1tc = (1ul << pin);
                        else GPIO.enable1_w1tc.val = (1ul << (pin - 32));
                    }
        #endif
                    break;

                case OUTPUT:
                    pin = digitalPinToGPIONumber(pin);
        #ifdef _IS_ESP_Cx_
                    GPIO.enable_w1ts.val = (1ul << (pin));
        #else
                    if (digitalPinIsValid(pin)) {
                        _ESP32_IDF_V3_REG();
                        if (pin < 32) GPIO.enable_w1ts = (1ul << pin);
                        else GPIO.enable1_w1ts.val = (1ul << (pin - 32));
                    }
        #endif
                    break;
                default:
                    pinMode(pin, mode);
                    break;
            }
        }

        
        _GIO_INLINE int read(Pin pin){ pin = digitalPinToGPIONumber(pin);
        #ifdef _IS_ESP_Cx_
            return (GPIO.in.val >> pin) & 0x1;
        #else
            if (digitalPinIsValid(pin)) {
                if (pin < 32) return (GPIO.in >> pin) & 0x1;
                else return (GPIO.in1.val >> (pin - 32)) & 0x1;
            }
        #endif
            return 0;
        }// read

        
        _GIO_INLINE void low(Pin pin){ pin = digitalPinToGPIONumber(pin);
        #ifdef _IS_ESP_Cx_
            GPIO.out_w1tc.val = (1ul << pin);
        #else
            if (digitalPinIsValid(pin)) {
                if (pin < 32) GPIO.out_w1tc = (1ul << pin);
                else GPIO.out1_w1tc.val = (1ul << (pin - 32));
            }
        #endif
        }// low

        
        _GIO_INLINE void high(Pin pin){ pin = digitalPinToGPIONumber(pin);
        #ifdef _IS_ESP_Cx_
            GPIO.out_w1ts.val = (1ul << pin);
        #else
            if (digitalPinIsValid(pin)) {
                if (pin < 32) GPIO.out_w1ts = (1ul << pin);
                else GPIO.out1_w1ts.val = (1ul << (pin - 32));
            }
        #endif
        }// high
        _GIO_INLINE void write(Pin pin, uint8_t val) { val ? high(pin) : low(pin); }// write
        _GIO_INLINE void toggle(Pin pin){  write(pin, !read(pin)); }// toggle
        _GIO_INLINE void init(Pin pin, PinMode V = INPUT){ pinMode(pin, V); }// init
    #elif defined(ARDUINO)//use Arduino
        #include <Arduino.h>
        _GIO_INLINE void mode(Pin n, PinMode m){ pinMode(n, m); } // mode
        _GIO_INLINE int read(Pin n){ return digitalRead(n); } // read
        _GIO_INLINE void high(Pin n){ digitalWrite(n, 1);   } // high
        _GIO_INLINE void low(Pin n){ digitalWrite(n, 0);    } // low
        _GIO_INLINE void write(Pin n, int V){ digitalWrite(n, V); }// write
        _GIO_INLINE void toggle(Pin n){ digitalWrite(n, !digitalRead(n)); }// toggle
        _GIO_INLINE void init(Pin n, PinMode m = {INPUT}){ mode(n, m); } // init
    #elif defined(STM32) //if defined(__AVR__) || defined(ARDUINO_ARCH_AVR)        
        _GIO_INLINE void mode(Pin P, PinMode m){
            GPIO_InitTypeDef GPIO_InitStruct = {0};
            GPIO_InitStruct.Pin = P.n;
            GPIO_InitStruct.Pull = m.io;
            GPIO_InitStruct.Mode = m.it;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(P.gpio(), &GPIO_InitStruct);
        } //default HAL functions to works with pins
        _GIO_INLINE bool read(Pin P)   { return P.gpio()->IDR & P.n;} 
        _GIO_INLINE void high(Pin P)   { P.gpio()->BSRR = P.n;      }
        _GIO_INLINE void low(Pin P)    { P.gpio()->BRR = P.n;       }
        _GIO_INLINE void write(Pin P, bool val){ val ? high(P):low(P);}
        _GIO_INLINE void toggle(Pin P) { HAL_GPIO_TogglePin(P.gpio(), P.n);}
        _GIO_INLINE void init(Pin P, PinMode m = {GPIO_PULLUP, GPIO_MODE_IT_RISING_FALLING}){ mode(P, m); }
    #endif //STM32
    #if __cplusplus >= 202002L
        template <Pin pin> 
        class PinT {
            public:
                PinT(PinMode mode = {INPUT_PULLUP}){ gio::init(pin, mode); }
                _GIO_INLINE void mode(PinMode mode){ gio::mode(pin, mode); }
                _GIO_INLINE void write(uint8_t val){ gio::write(pin, val); }
                _GIO_INLINE void high(){             gio::high(pin);       }
                _GIO_INLINE void low(){              gio::low(pin);        }
                _GIO_INLINE void toggle(){           gio::toggle(pin);     }
                _GIO_INLINE int  read(){      return gio::read(pin);       }
        };    
    #else
        template <uint8_t P>
        class PinT {
        public:
            PinT(PinMode mode = {INPUT_PULLUP}) { 
                Pin pin{P}; 
                gio::init(pin, mode); 
            }
            _GIO_INLINE void mode(PinMode mode){ Pin pin{P}; gio::mode(pin, mode); }
            _GIO_INLINE void write(uint8_t val){ Pin pin{P}; gio::write(pin, val); }
            _GIO_INLINE void high(){ Pin pin{P}; gio::high(pin); }
            _GIO_INLINE void low(){ Pin pin{P}; gio::low(pin); }
            _GIO_INLINE void toggle(){ Pin pin{P}; gio::toggle(pin); }
            _GIO_INLINE int read(){ Pin pin{P}; return gio::read(pin); }
        };
    #endif    
        class PinIO {
        public:
            PinIO(){}
            PinIO(Pin pin, PinMode mode = {INPUT}){ init(pin, mode); }
            void init(Pin npin, PinMode mode = {INPUT}){ 
                gio::init(npin, mode);
            #if defined(__AVR__) && !defined(GIO_NO_MASK)
                if (mode == OUTPUT) reg = portOutputRegister(digitalPinToPort(npin));
                else reg = portInputRegister(digitalPinToPort(npin));
                mask = digitalPinToBitMask(npin);
            #else
                pin = npin;
            #endif
            }
            void write(int val){
            #if defined(__AVR__) && !defined(GIO_NO_MASK)
                greg_write(reg, mask, val);
            #else
                gio::write(pin, val);
            #endif
            }
            void high(){
            #if defined(__AVR__) && !defined(GIO_NO_MASK)
                greg_set(reg, mask);
            #else
                gio::high(pin);
            #endif
            }
            void low(){
            #if defined(__AVR__) && !defined(GIO_NO_MASK)
                greg_clr(reg, mask);
            #else
                gio::low(pin);
            #endif
            }
            void toggle(){
            #if defined(__AVR__) && !defined(GIO_NO_MASK)
                *reg ^= mask;
            #else
                gio::toggle(pin);
            #endif
            }
            int read(){
            #if defined(__AVR__) && !defined(GIO_NO_MASK)
                return greg_read(reg, mask);
            #else
                return gio::read(pin);
            #endif
            }
            bool valid(){
            #if defined(__AVR__) && !defined(GIO_NO_MASK)
                return reg;
            #elif defined(STM32)
                return pin.isValid();
            #else
                return pin != 0xff;
            #endif
            }
        private:
            #if defined(__AVR__) && !defined(GIO_NO_MASK)
                volatile uint8_t *reg = nullptr;
                uint8_t mask = 0xff;
            #elif defined(STM32)
                Pin pin{nullptr, 0};
            #else
                Pin pin{0xff};
            #endif
        };
    #ifdef GIO_WITH_SHIFT_HPP
        #ifdef STM32
        static void delayMicroseconds(uint32_t us) { //NOT PRECISE
            for (uint32_t i = 0; i < us * (SystemCoreClock / 1000000 / 5); i++);
        }
        #endif
        namespace shift {
            inline bool read(Pin dat_pin, Pin clk_pin, uint8_t order, uint8_t* data, uint16_t len, uint8_t delay){
                bool dif = 0;
                if (order & 0b10) data += len - 1;
            #if defined(__AVR__) && !defined(GIO_NO_MASK)
                if (!__builtin_constant_p(dat_pin) || !__builtin_constant_p(clk_pin)){
                    volatile uint8_t* c_reg = portOutputRegister(digitalPinToPort(clk_pin));
                    volatile uint8_t* d_reg = portInputRegister(digitalPinToPort(dat_pin));
                    uint8_t c_mask = digitalPinToBitMask(clk_pin);
                    uint8_t d_mask = digitalPinToBitMask(dat_pin);
                    uint8_t val = 0;
                    while (len--){
                        val = 0;
                        for (uint8_t i = 0; i < 8; i++){
                            if (order & 0b01){  // MSBFIRST
                                val <<= 1;
                                if (greg_read(d_reg, d_mask)) val |= 1;
                            } else {
                                val >>= 1;
                                if (greg_read(d_reg, d_mask)) val |= (1 << 7);
                            }
                            if (delay) delayMicroseconds(delay);
                            greg_set(c_reg, c_mask);
                            if (delay) delayMicroseconds(delay);
                            greg_clr(c_reg, c_mask);
                        }
                        if (!dif && *data != val) dif = 1;
                        *data = val;
                        data += (order & 0b10) ? -1 : 1;
                    }
                } else
            #endif
                {
                    uint8_t val = 0;
                    while (len--){
                        val = 0;
                        for (uint8_t i = 0; i < 8; i++){
                            if (order & 0b01){  // MSBFIRST
                                val <<= 1;
                                if (gio::read(dat_pin)) val |= 1;
                            } else {
                                val >>= 1;
                                if (gio::read(dat_pin)) val |= (1 << 7);
                            }
                            if (delay) delayMicroseconds(delay);
                            gio::high(clk_pin);
                            if (delay) delayMicroseconds(delay);
                            gio::low(clk_pin);
                        }
                        if (!dif && *data != val) dif = 1;
                        *data = val;
                        data += (order & 0b10) ? -1 : 1;
                    }
                }
                return dif;
            }
            inline uint8_t read_byte(Pin dat_pin, Pin clk_pin, uint8_t order, uint8_t delay){
                uint8_t value;
                read(dat_pin, clk_pin, order, &value, 1, delay);
                return value;
            }
            inline bool read_cs(Pin dat_pin, Pin clk_pin, Pin cs_pin, uint8_t order, uint8_t* data, uint16_t len, uint8_t delay){
                gio::low(cs_pin);
                bool res = read(dat_pin, clk_pin, order, data, len, delay);
                gio::high(cs_pin);
                return res;
            }
            inline uint8_t read_cs_byte(Pin dat_pin, Pin clk_pin, Pin cs_pin, uint8_t order, uint8_t delay){
                uint8_t value;
                read_cs(dat_pin, clk_pin, cs_pin, order, &value, 1, delay);
                return value;
            }
            inline void send(Pin dat_pin, Pin clk_pin, uint8_t order, uint8_t* data, uint16_t len, uint8_t delay){
            #if defined(__AVR__) && !defined(GIO_NO_MASK)
                if (!__builtin_constant_p(dat_pin) || !__builtin_constant_p(clk_pin)){
                    volatile uint8_t* c_reg = portOutputRegister(digitalPinToPort(clk_pin));
                    volatile uint8_t* d_reg = portOutputRegister(digitalPinToPort(dat_pin));
                    uint8_t c_mask = digitalPinToBitMask(clk_pin);
                    uint8_t d_mask = digitalPinToBitMask(dat_pin);
                    uint8_t val;
                    for (uint16_t b = 0; b < len; b++){
                        val = data[(order & 0b10) ? (len - b - 1) : b];
                        for (uint8_t i = 0; i < 8; i++){
                            if (order & 0b01){  // MSBFIRST
                                greg_write(d_reg, d_mask, val & (1 << 7));
                                val <<= 1;
                            } else {
                                greg_write(d_reg, d_mask, val & 1);
                                val >>= 1;
                            }
                            if (delay) delayMicroseconds(delay);
                            greg_set(c_reg, c_mask);
                            if (delay) delayMicroseconds(delay);
                            greg_clr(c_reg, c_mask);
                        }
                    }
                    greg_clr(d_reg, d_mask);
                } else
            #endif
                {
                    for (uint16_t b = 0; b < len; b++){
                        uint8_t val = data[(order & 0b10) ? (len - b - 1) : b];
                        for (uint8_t i = 0; i < 8; i++){
                            if (order & 0b01){  // MSBFIRST
                                gio::write(dat_pin, val & (1 << 7));
                                val <<= 1;
                            } else {
                                gio::write(dat_pin, val & 1);
                                val >>= 1;
                            }
                            if (delay) delayMicroseconds(delay);
                            gio::high(clk_pin);
                            if (delay) delayMicroseconds(delay);
                            gio::low(clk_pin);
                        }
                        gio::low(dat_pin);
                    }
                }
            }
            inline void send_byte(Pin dat_pin, Pin clk_pin, uint8_t order, uint8_t data, uint8_t delay){
                send(dat_pin, clk_pin, order, &data, 1, delay);
            }
            inline void send_cs(Pin dat_pin, Pin clk_pin, Pin cs_pin, uint8_t order, uint8_t* data, uint16_t len, uint8_t delay){
                gio::low(cs_pin);
                send(dat_pin, clk_pin, order, data, len, delay);
                gio::high(cs_pin);
            }
            inline void send_cs_byte(Pin dat_pin, Pin clk_pin, Pin cs_pin, uint8_t order, uint8_t data, uint8_t delay){
                send_cs(dat_pin, clk_pin, cs_pin, order, &data, 1, delay);
            }
        }  // namespace shift
    #endif // GIO_WITH_SHIFT_HPP
    } // namespace gio
#endif //GIO_LIB_HPP

void* EB_self = nullptr; 
inline bool     EB_read(Pin pin)              { return (bool)gio::read(pin); }
inline void     EB_mode(Pin pin, PinMode mode){ gio::init(pin, mode);  }
inline uint32_t EB_uptime()                   { return millis();       }
namespace encb {
    template <typename T>
    struct Flags { //structure of flags with data types that can be defined
        T flags = 0;
        inline T    mask(const T x)                __attribute__((always_inline)){ return flags & x; }
        inline void set(const T x)                 __attribute__((always_inline)){ flags |= x; }
        inline void clear(const T x)               __attribute__((always_inline)){ flags &= ~x; }
        inline bool read(const T x)                __attribute__((always_inline)){ return flags & x; }
        inline void write(const T x, const bool v) __attribute__((always_inline)){ v ? set(x) : clear(x); }
        inline bool eq(const T x, const T y)       __attribute__((always_inline)){ return (flags & x) == y; }
    };
} 
//template classes ################################ VIRTUAL BUTTON ######################################################
// ===================== FLAGS ======================
constexpr uint16_t EB_PRESS      = (1 << 0); // нажатие на кнопку
constexpr uint16_t EB_HOLD       = (1 << 1); // кнопка удержана
constexpr uint16_t EB_STEP       = (1 << 2); // импульсное удержание
constexpr uint16_t EB_RELEASE    = (1 << 3); // кнопка отпущена
constexpr uint16_t EB_CLICK      = (1 << 4); // одиночный клик
constexpr uint16_t EB_CLICKS     = (1 << 5); // сигнал о нескольких кликах
constexpr uint16_t EB_TURN       = (1 << 6); // поворот энкодера
constexpr uint16_t EB_REL_HOLD   = (1 << 7); // кнопка отпущена после удержания
constexpr uint16_t EB_REL_HOLD_C = (1 << 8); // кнопка отпущена после удержания с предв. кликами
constexpr uint16_t EB_REL_STEP   = (1 << 9); // кнопка отпущена после степа
constexpr uint16_t EB_REL_STEP_C = (1 << 10);// кнопка отпущена после степа с предв. кликами
constexpr uint16_t EB_TIMEOUT    = (1 << 11);// прошёл таймаут после нажатия кнопки или поворота энкодера
enum class EBAction {
    None = 0,
    Press = EB_PRESS,
    Hold = EB_HOLD,
    Step = EB_STEP,
    Release = EB_RELEASE,
    Click = EB_CLICK,
    Clicks = EB_CLICKS,
    Turn = EB_TURN,
    ReleaseHold = EB_REL_HOLD,
    ReleaseHoldClicks = EB_REL_HOLD_C,
    ReleaseStep = EB_REL_STEP,
    ReleaseStepClicks = EB_REL_STEP_C,
    Timeout = EB_TIMEOUT,
};
// =================== TOUT BUILD ===================
#define EB_SHIFT 4
#ifdef EB_DEB_TIME //debounce
#define EB_DEB_T (EB_DEB_TIME)
#else
#endif
#ifdef EB_CLICK_TIME // таймаут между кликами, мс
#define EB_CLICK_T (EB_CLICK_TIME)
#define EB_GET_CLICK_TIME() ((uint16_t)EB_CLICK_T)
#else
#define EB_GET_CLICK_TIME() (uint16_t)(EB_CLICK_T << EB_SHIFT)
#endif
#ifdef EB_HOLD_TIME // таймаут удержания, мс
#define EB_HOLD_T (EB_HOLD_TIME)
#define EB_GET_HOLD_TIME() ((uint16_t)EB_HOLD_T)
#else
#define EB_GET_HOLD_TIME() (uint16_t)(EB_HOLD_T << EB_SHIFT)
#endif
#ifdef EB_STEP_TIME // период степа, мс
#define EB_STEP_T (EB_STEP_TIME)
#define EB_GET_STEP_TIME() ((uint16_t)EB_STEP_T)
#else
#define EB_GET_STEP_TIME() (uint16_t)(EB_STEP_T << EB_SHIFT)
#endif
#ifdef EB_TOUT_TIME // время таймаута, мс
#define EB_TOUT_T (EB_TOUT_TIME)
#define EB_GET_TOUT_TIME() ((uint16_t)EB_TOUT_T)
#else
#define EB_GET_TOUT_TIME() (uint16_t)(EB_TOUT_T << EB_SHIFT)
#endif
// =================== PACK FLAGS ===================
constexpr uint16_t EB_CLKS_R = (1 << 0);
constexpr uint16_t EB_PRS_R  = (1 << 1);
constexpr uint16_t EB_HLD_R  = (1 << 2);
constexpr uint16_t EB_STP_R  = (1 << 3);
constexpr uint16_t EB_REL_R  = (1 << 4);
constexpr uint16_t EB_PRS    = (1 << 5);
constexpr uint16_t EB_HLD    = (1 << 6);
constexpr uint16_t EB_STP    = (1 << 7);
constexpr uint16_t EB_REL    = (1 << 8);
constexpr uint16_t EB_BUSY   = (1 << 9);
constexpr uint16_t EB_DEB    = (1 << 10);
constexpr uint16_t EB_TOUT   = (1 << 11);
constexpr uint16_t EB_INV    = (1 << 12);
constexpr uint16_t EB_BOTH   = (1 << 13);
constexpr uint16_t EB_BISR   = (1 << 14);
constexpr uint16_t EB_EHLD   = (1 << 15);
//################################################### VIRTUAL BUTTON ######################################################
/** @class VirtButton
 *  @brief Base class for a button.
 *  Provides polling, methods and interrupt-based reading of a button. */
class VirtButton { // базовый класс кнопки
#if defined(__AVR__) || defined(EB_NO_FUNCTIONAL)
    typedef void (*ActionHandler)();
#else
    typedef std::function<void()> ActionHandler;
#endif
   public:
    uint8_t clicks;
    //============================================== SET ==============================================
    /** @brief Set the hold timeout for the button.
    *   @param tout Hold timeout in milliseconds (default 600, max 4000). @see EB_HLD_R */ 
    void setHoldTimeout(const uint16_t tout){ 
#ifndef EB_HOLD_TIME
        EB_HOLD_T = tout >> EB_SHIFT;
#endif
    }
    /** @brief Set the step timeout for the button.
    *   @param tout Timeout in milliseconds (default 200, max 4000). @see EB_STP_R */
    void setStepTimeout(const uint16_t tout){
#ifndef EB_STEP_TIME
        EB_STEP_T = tout >> EB_SHIFT;
#endif
    }
    /** @brief Set the click await timeout for the button.
    *   @param tout Timeout in milliseconds (default 500, max 4000). @see EB_CLKS_R */
    void setClickTimeout(const uint16_t tout){
#ifndef EB_CLICK_TIME
        EB_CLICK_T = tout >> EB_SHIFT;
#endif
    }
    /** @brief Set debounce timeout for the button.
    *   @param tout Debounce time in milliseconds (default 50, max 255). @see EB_DEB */
    void setDebTimeout(const uint8_t tout){
#ifndef EB_DEB_TIME
        EB_DEB_T = tout;
#endif
    }
    /** @brief Set timeout for button action. 
    *   @param tout Timeout in milliseconds (default 1000, max 4000). @see EB_TOUT */
    void setTimeout(const uint16_t tout){
#ifndef EB_TOUT_TIME
        EB_TOUT_T = tout >> EB_SHIFT;
#endif
    }
    /** @brief Set button logic level (HIGH or LOW).
    *   @param level HIGH - button shorts VCC, LOW - button shorts GND. @see EB_INV */
    void setBtnLevel(const bool level){
        bf.write(EB_INV, !level);
    }
    /** @brief Mark the button as pressed in ISR.
    *   @warning btnLevel ignored. @see EB_BISR @see EB_DEB */
    void pressISR(){
        if (!bf.read(EB_DEB)) tmr = EB_uptime();
        bf.set(EB_DEB | EB_BISR);
    }
    /** @brief Reset button flags and internal counters.
    *   Clears clicks and system flags (except inversion flag). */
    void reset(){ // сбросить системные флаги (принудительно закончить обработку)
        clicks = 0;
        bf.clear(~EB_INV); 
    }
    /** @brief Force clear button event flags @param resetTout True to reset timeout flag
    *   @see EB_REL_R  @see EB_STP_R  @see EB_PRS_R @see EB_HLD_R @see EB_CLKS_R */
    void clear(bool resetTout = false){
        if (resetTout && bf.read(EB_TOUT)) bf.clear(EB_TOUT);
        if (bf.read(EB_CLKS_R)) clicks = 0;
        if (bf.read(EB_CLKS_R | EB_STP_R | EB_PRS_R | EB_HLD_R | EB_REL_R)){
            bf.clear(EB_CLKS_R | EB_STP_R | EB_PRS_R | EB_HLD_R | EB_REL_R);
        }
    }
    /** @brief Ignore all events until button release. @see EB_EHLD */
    void skipEvents(){ bf.set(EB_EHLD);  }
    /** @brief Attach a callback function for button events (like void f()).
    *   @param handler Function pointer or std::function<void()> */
    void attach(ActionHandler handler){
#ifndef EB_NO_CALLBACK
        cb = handler;
#else
        (void)handler;
#endif
    }
    /** @brief Detach the callback function. */
    void detach(){
#ifndef EB_NO_CALLBACK
        cb = nullptr;
#endif
    }

    //================================================= GET =================================================
    /** @brief Check if button was pressed [event]. @return true if pressed. @see click() */
    bool press(){ return bf.read(EB_PRS_R);                             }// кнопка нажата [событие]
    /** @brief Check if button was pressed after prior clicks [event]. @return true if pressed. @see click(uint8_t) */
    bool press(const uint8_t num){ return(clicks==num) && press();        }// кнопка нажата с предварительными кликами [событие]
    /** @brief Check for a click event (released without hold). @return true if clicked. @see pressing() */
    bool click(){ return bf.eq(EB_REL_R | EB_REL | EB_HLD, EB_REL_R);}// клик по кнопке (отпущена без удержания) [событие]
    /** @brief Check for a click (released without hold) event after prior clicks. @param num Number of clicks. @return true if condition met. @see pressing(uint8_t) */
    bool click(const uint8_t num){ return(clicks==num) && click();        }// клик по кнопке (отпущена без удержания) с предварительными кликами [событие]
    /** @brief Check if button is currently pressed [state]. @return true if pressing. @see hold() */
    bool pressing(){ return bf.read(EB_PRS);                           }// кнопка зажата (между press() и release()) [состояние]
    /** @brief Check if button is pressed (between press() и release()) after prior clicks [state]. @param num Number of clicks. @return true if condition met. @see hold(uint8_t) */
    bool pressing(const uint8_t num){return(clicks==num)&&pressing();    }// кнопка зажата (между press() и release()) с предварительными кликами [состояние]
    /** @brief Check if button was held longer than timeout [event]. @return true if hold event occurred. @see holding(){ */
    bool hold(){ return bf.read(EB_HLD_R);                             }// кнопка была удержана (больше таймаута) [событие]
    /** @brief Check if button was held after prior clicks [event]. @param num Number of clicks. @return true if condition met. @see holding(uint8_t){ */
    bool hold(const uint8_t num){ return(clicks==num) && hold();         }// кнопка была удержана (больше таймаута) с предварительными кликами [событие]
    /** @brief Check if button is being held [state]. @return true if holding. @see release() */
    bool holding(){ return bf.eq(EB_PRS | EB_HLD, EB_PRS | EB_HLD); }// кнопка удерживается (больше таймаута) [состояние]
    /** @brief Check if button is being held after prior clicks [state]. @param num Number of clicks. @return true if condition met. @see release(uint8_t) */
    bool holding(const uint8_t num){ return(clicks==num)&&holding();     }// кнопка удерживается (больше таймаута) с предварительными кликами [состояние]
    /** @brief Check if button was released (anyway) [event]. @return true if released. @see step() */
    bool release(){ return bf.eq(EB_REL_R|EB_REL, EB_REL_R|EB_REL);  }// кнопка отпущена (в любом случае) [событие]
    /** @brief Check if button was released (anyway) after prior clicks [event]. @param num Number of clicks. @return true if condition met. @see step(uint8_t) */
    bool release(const uint8_t num){return(clicks==num) && release();    }// кнопка отпущена (в любом случае) с предварительными кликами [событие]
    /** @brief Check for step (repeated hold pulse) event. @return true if step event occurred. @see hasClicks() */
    bool step(){ return bf.read(EB_STP_R);                             }// импульсное удержание [событие]
    /** @brief Check for step event after prior clicks. @param num Number of clicks. @return true if condition met. @see hasClicks(uint8_t) */
    bool step(const uint8_t num){ return(clicks==num) && step();         }// импульсное удержание с предварительными кликами [событие]
    /** @brief Check if multiple clicks were detected [event]. @return true if clicks detected. @see releaseHold() */
    bool hasClicks(){ return bf.eq(EB_CLKS_R | EB_HLD, EB_CLKS_R);  }// зафиксировано несколько кликов [событие]
    /** @brief Check if specific number of clicks were detected [event]. @param num Number of clicks. @return true if condition met. @see releaseHold(uint8_t) */
    bool hasClicks(const uint8_t num){ return(clicks==num)&&hasClicks(); }// зафиксировано указанное количество кликов [событие]
    /** @brief Check if button was released after hold (event). @return true if condition met. @see bool releaseStep() */
    bool releaseHold(){ return bf.eq(EB_REL_R|EB_REL|EB_HLD|EB_STP,EB_REL_R|EB_HLD); } 
    /** @brief Check if button was released after hold with prior clicks (event). @param num Number of clicks. @return true if condition met. @see releaseStep(uint8_t) */
    bool releaseHold(const uint8_t num){ return clicks==num && bf.eq(EB_CLKS_R|EB_HLD|EB_STP,EB_CLKS_R|EB_HLD); } 
    /** @brief Check if button was released after step (pulse hold) (event). @return true if condition met. @see releaseHoldStep() */
    bool releaseStep(){ return bf.eq(EB_REL_R|EB_REL|EB_STP,EB_REL_R|EB_STP); }
    /** @brief Check if button was released after step with prior clicks (event). @param num Number of clicks. @return true if condition met. @see releaseHoldStep(uint8_t) */
    bool releaseStep(const uint8_t num){ return clicks==num && bf.eq(EB_CLKS_R|EB_STP,EB_CLKS_R|EB_STP); }
    /** @brief Check if button was released after hold or step (event). @return true if condition met. */
    bool releaseHoldStep(){ return releaseHold()||releaseStep(); }
    /** @brief Check if button was released after hold or step with prior clicks (event). @param num Number of clicks. @return true if condition met. @see waiting()*/
    bool releaseHoldStep(const uint8_t num){ return releaseHold(num) || releaseStep(num); }
    /** @brief Check if button is waiting for additional clicks (state). @return true if waiting. @see busy() */
    bool waiting(){ return clicks && bf.eq(EB_PRS|EB_REL,0); }
    /** @brief Check if button processing is in progress (state). @return true if busy. @see getClicks() */
    bool busy(){ return bf.read(EB_BUSY); }
    /** @brief Get number of detected clicks. @return Number of clicks. */
    uint8_t getClicks(){ return clicks;                                  }// получить количество кликов
    /** @brief Get number of step events (calculated over time). @return Number of steps. */
    uint16_t getSteps(){ // получить количество степов
#ifndef EB_NO_FOR
        return ftmr ? ((stepFor() + EB_GET_STEP_TIME() - 1) / EB_GET_STEP_TIME()) : 0;  // (x + y - 1) / y
#endif
        return 0;
    }
    /** @brief Get button action event code (event). @return Event code. */
    uint16_t action(){
        switch (bf.mask(0b111111111)){
            case (EB_PRS | EB_PRS_R):                   return EB_PRESS;
            case (EB_PRS | EB_HLD | EB_HLD_R):          return EB_HOLD;
            case (EB_PRS | EB_HLD | EB_STP | EB_STP_R): return EB_STEP;
            case (EB_REL | EB_REL_R):
            case (EB_REL | EB_REL_R | EB_HLD):
            case (EB_REL | EB_REL_R | EB_HLD | EB_STP): return EB_RELEASE;
            case (EB_REL_R):                            return EB_CLICK;
            case (EB_CLKS_R):                           return EB_CLICKS;
            case (EB_REL_R  | EB_HLD):                  return EB_REL_HOLD;
            case (EB_CLKS_R | EB_HLD):                  return EB_REL_HOLD_C;
            case (EB_REL_R  | EB_HLD | EB_STP):         return EB_REL_STEP;
            case (EB_CLKS_R | EB_HLD | EB_STP):         return EB_REL_STEP_C;
        }
        if (timeoutState()) return EB_TIMEOUT;
        return 0;
    }
    /** @brief Get button action as enum type. @return EBAction enum value. */
    EBAction getAction(){ return (EBAction)action(); }

    //================================================= TIME ================================================
    /** @brief Check if button timeout occurred. @return true if timeout occurred. @see setTimeout(uint16_t) */
    bool timeout(){ // после взаимодействия с кнопкой (или энкодером EncButton) время setTimeout, мс [событие]
        if (timeoutState()){
            bf.clear(EB_TOUT);
            return 1;
        }
        return 0;
    }
    /** @brief Check if button timeout occurred. @return true if timeout occurred. @see setTimeout(uint16_t) */
    bool timeoutState(){ // после взаимодействия с кнопкой (или энкодером EncButton) время setTimeout, мс [состояние]
        return bf.read(EB_TOUT)&&(uint16_t)((uint16_t)EB_uptime()-tmr)>=EB_GET_TOUT_TIME(); 
    }
    /** @brief Get time button has been pressed (since initial press), ms. @return milliseconds. */
    uint16_t pressFor(){// время, которое кнопка удерживается (с начала нажатия), мс
#ifndef EB_NO_FOR
        if (ftmr) return (uint16_t)EB_uptime() - ftmr;
#endif
        return 0;
    }
    /** @brief Check if button has been pressed longer than specified time, ms [state]. @param ms Time in ms. @return true if pressed longer. */
    bool pressFor(const uint16_t ms){ return pressFor() > ms; }// кнопка удерживается дольше чем (с начала нажатия), мс [состояние]
    /** @brief Get time button has been held (since hold start), ms. @return milliseconds. */
    uint16_t holdFor(){// время, которое кнопка удерживается (с начала удержания), мс
#ifndef EB_NO_FOR
        if (bf.read(EB_HLD)) return pressFor() - EB_GET_HOLD_TIME();
#endif
        return 0;
    }
    /** @brief Check if button has been held longer than specified time, ms [state]. @param ms Time in ms. @return true if held longer. */
    bool holdFor(const uint16_t ms){ return holdFor() > ms; }// кнопка удерживается дольше чем (с начала удержания), мс [состояние]
    /** @brief Get time button has been in step state (since step start), ms. @return milliseconds. */
    uint16_t stepFor(){// время, которое кнопка удерживается (с начала степа), мс
#ifndef EB_NO_FOR
        if (bf.read(EB_STP)) return pressFor() - EB_GET_HOLD_TIME() * 2;
#endif
        return 0;
    }
    /** @brief Check if button has been in step state longer than specified time, ms [state]. @param ms Time in ms. @return true if in step state longer. */
    bool stepFor(uint16_t ms){  return stepFor() > ms; } // кнопка удерживается дольше чем (с начала степа), мс [состояние]
    
    //================================================= POLL ================================================
    /** @brief Process simultaneous press of two buttons. @param b0 First button object. @param b1 Second button object. @return true if virtual button active. */
    bool tick(VirtButton& b0, VirtButton& b1){ // обработка виртуальной кнопки как одновременное нажатие двух других кнопок
        if (bf.read(EB_BOTH)){
            if (!b0.pressing() && !b1.pressing()) bf.clear(EB_BOTH);
            if (!b0.pressing()) b0.reset();
            if (!b1.pressing()) b1.reset();
            b0.clear();
            b1.clear();
            return tick(1);
        } else {
            if (b0.pressing() && b1.pressing()) bf.set(EB_BOTH);
            return tick(0);
        }
    }
    /** @brief Process button input and update internal state. @param s Current button state. @return true if button pressed or timeout occurred. */
    bool tick(bool s){// обработка кнопки значением
        clear();
        s = pollBtn(s);
#ifndef EB_NO_CALLBACK
        if (s || timeoutState()) call();
#endif
        return s;
    }
    /** @brief Process button without resetting events or calling callback. @param s Current button state. @return true if button pressed. */
    bool tickRaw(const bool s){ return pollBtn(s); }// обработка кнопки без сброса событий и вызова коллбэка
    /** @brief Call registered handler if action occurred or forced. @param force Force call even if no action. */
    void call(bool force = false){// вызвать обработчик
#ifndef EB_NO_CALLBACK // todo force заменить на флаг
        if (cb && (force || action())){
            if (cb){
                EB_self = this;
                cb();
                EB_self = nullptr;
                timeout();  // todo clear tout
            }
        }
#else
        (void)force;
#endif
    }
    
    // deprecated
        //void setButtonLevel(bool level) __attribute__((deprecated)){ bf.write(EB_INV, !level); }
        // bool timeout(const uint16_t tout) /*__attribute__((deprecated))*/ {
        //     if (timeoutState(tout)){
        //         bf.clear(EB_TOUT);
        //         return 1;
        //     }
        //     return 0;
        // } // после взаимодействия с кнопкой (или энкодером EncButton) прошло указанное время, мс [событие]
        // bool timeoutState(const uint16_t tout) /*__attribute__((deprecated))*/ {
        //     return (bf.read(EB_TOUT) && (uint16_t)((uint16_t)EB_uptime() - tmr) > tout);
        // } // после взаимодействия с кнопкой (или энкодером EncButton) прошло указанное время, мс [состояние]
    //=============================================== PRIVATE ===============================================
   protected:
    uint16_t tmr = 0;
    encb::Flags<uint16_t> bf;
    constexpr bool levelAuto(uint8_t btnLevel, PinMode mode) { //if INPUT_PULLDOWN - inverted logic
#ifdef STM32  //btn doesn't work if mode = INPUT_PULLDOWN && btnLevel = LOW, btn.read() works, but flags not
        return btnLevel==AUTO?(mode.io==INPUT_PULLUP?LOW:(mode.io==INPUT_PULLDOWN?HIGH:LOW)):LOW;
#else
        return btnLevel;
#endif
    }
#ifndef EB_NO_CALLBACK
    ActionHandler cb = nullptr;
#endif
   private:
#ifndef EB_NO_FOR
    uint16_t ftmr = 0;
#endif
#ifndef EB_DEB_TIME
    uint8_t EB_DEB_T = 50;
#endif
#ifndef EB_CLICK_TIME
    uint8_t EB_CLICK_T = (500 >> EB_SHIFT);
#endif
#ifndef EB_HOLD_TIME
    uint8_t EB_HOLD_T = (600 >> EB_SHIFT);
#endif
#ifndef EB_STEP_TIME
    uint8_t EB_STEP_T = (200 >> EB_SHIFT);
#endif
#ifndef EB_TOUT_TIME
    uint8_t EB_TOUT_T = (1000 >> EB_SHIFT);
#endif
    inline bool pollBtn(bool s){
        if (bf.read(EB_BISR)){
            bf.clear(EB_BISR);
            s = 1;
        } else s ^= bf.read(EB_INV);
        if (!bf.read(EB_BUSY)){
            if (s) bf.set(EB_BUSY);
            else return 0;
        }
        uint16_t ms = (uint16_t)EB_uptime();
        uint16_t deb = ms - tmr;
        if (s){                                         // кнопка нажата
            if (!bf.read(EB_PRS)){                    // кнопка не была нажата ранее
                if (!bf.read(EB_DEB) && EB_DEB_T){    // дебаунс ещё не сработал
                    bf.set(EB_DEB);                   // будем ждать дебаунс
                    tmr = ms;                           // сброс таймаута
                } else {                                // первое нажатие
                    if (deb >= EB_DEB_T || !EB_DEB_T){  // ждём EB_DEB_TIME
                        bf.set(EB_PRS | EB_PRS_R);    // флаг на нажатие
#ifndef EB_NO_FOR
                        ftmr = ms;
#endif
                        tmr = ms;  // сброс таймаута
                    }
                }
            } else {  // кнопка уже была нажата
                if (!bf.read(EB_EHLD)){
                    if (!bf.read(EB_HLD)){             // удержание ещё не зафиксировано
                        if (deb >= EB_GET_HOLD_TIME()){  // ждём EB_HOLD_TIME - это удержание
                            bf.set(EB_HLD_R | EB_HLD); // флаг что было удержание
                            tmr = ms;                    // сброс таймаута
                        }
                    } else {  // удержание зафиксировано
                        if (deb >= (uint16_t)(bf.read(EB_STP) ? EB_GET_STEP_TIME() : EB_GET_HOLD_TIME())){
                            bf.set(EB_STP | EB_STP_R);  // флаг степ
                            tmr = ms;                   // сброс таймаута
                        }
                    }
                }
            }
        } else {                                      // кнопка не нажата
            if (bf.read(EB_PRS)){                     // но была нажата
                if (deb >= EB_DEB_T){                 // ждём EB_DEB_TIME
                    if (!bf.read(EB_HLD)) clicks++;   // не удерживали - это клик
                    if (bf.read(EB_EHLD)) clicks=0; //
                    bf.set(EB_REL | EB_REL_R);        // флаг release
                    bf.clear(EB_PRS);                 // кнопка отпущена
                }
            } else if (bf.read(EB_REL)){
                if (!bf.read(EB_EHLD)){
                    bf.set(EB_REL_R);  // флаг releaseHold / releaseStep
                }
                bf.clear(EB_REL | EB_EHLD);
                tmr = ms;                                                                      // сброс таймаута
            } else if (clicks){                                                                // есть клики, ждём EB_CLICK_TIME
                if (bf.read(EB_HLD | EB_STP) || deb >= EB_GET_CLICK_TIME()) bf.set(EB_CLKS_R); // флаг clicks
#ifndef EB_NO_FOR
                else if (ftmr) ftmr = 0;
#endif
            } else if (bf.read(EB_BUSY)){
                bf.clear(EB_HLD | EB_STP | EB_BUSY);
                bf.set(EB_TOUT);
#ifndef EB_NO_FOR
                ftmr = 0;
#endif
                tmr = ms;  // test!!
            }
            if (bf.read(EB_DEB)) bf.clear(EB_DEB);  // сброс ожидания нажатия (дебаунс)
        }
        return bf.read(EB_CLKS_R | EB_PRS_R | EB_HLD_R | EB_STP_R | EB_REL_R);
    }
};
//################################################# VIRTUAL ENCODER #######################################################
#define EB_STEP4_LOW  0
#define EB_STEP4_HIGH 1
#define EB_STEP2      2
#define EB_STEP1      3
// ===================== FLAGS ======================
constexpr uint8_t EB_TYPE (1 << 0);
constexpr uint8_t EB_REV = (1 << 2);
constexpr uint8_t EB_FAST = (1 << 3);
constexpr uint8_t EB_DIR = (1 << 4);
constexpr uint8_t EB_ETRN_R = (1 << 5);
constexpr uint8_t EB_ISR_F = (1 << 6);
constexpr uint8_t EB_EISR = (1 << 7);

constexpr uint16_t faststeps[] = FASTSTEPS;
/** @class VirtEncoder
 *  @brief Base class for a rotary encoder.
 *  Provides polling, methods and interrupt-based reading of a rotary encoder. */
class VirtEncoder { //base encoder class
   public:
    /** @brief Constructor. Initializes encoder pins and position. */
    VirtEncoder(){ p0 = p1 = epos = 0; }
    // ====================== SET ===============================================
    /** @brief Set encoder rotation direction (normal or reversed).
     *  @param rev True to reverse rotation direction, false for normal. */
    void setEncReverse(const bool rev){rev?ef.set(EB_REV):ef.clear(EB_REV);} 
    /** @brief Set the encoder type/step mode. Some encoders gives 2 pulses per step, some 1 pulse.
     *  @param type Step type (EB_STEP4_LOW, EB_STEP4_HIGH, EB_STEP2, EB_STEP1). */
    void setEncType(const uint8_t type){ ef.flags=(ef.flags&0b11111100)|type;}
    /** @brief Enable or disable encoder processing in interrupt service routine (ISR).
     *  @param use True to use ISR, false otherwise. */
    void setEncISR(const bool use){ ef.write(EB_EISR, use);}
    /** @brief Initialize encoder state with current pin values.
     *  @param e0 Current state of pin A (from digitalRead or HAL_GPIO_ReadPin or similar)  
     *  @param e1 Current state of pin B. 
     *  @see tickISR(bool,bool) */
    void initEnc(const bool e0, const bool e1){p0 = e0, p1 = e1;}
    /** @brief Clear encoder event flags.
     *  @see EB_TYPE @see EB_REV @see EB_FAST @see EB_DIR @see EB_ETRN_R @see EB_ISR_F @see EB_EISR @see tick()*/
    void clear(){ if (ef.read(EB_ETRN_R)) {ef.clear(EB_ETRN_R);}}
    // ====================== GET / POLL =======================================
    /** @brief Check if encoder turn event was.
     *  @return True if turned, false if not. */
    bool turn(){ return ef.read(EB_ETRN_R);}
    /** @brief Get the last encoder direction [state].
     *  @return 1 if turned right, -1 if turned left. */
    int8_t dir(){ return ef.read(EB_DIR) ? 1 : -1;}
    // ===================== POLL ISR ===========================================
    /** @brief Poll the encoder in ISR context.
     *  @param e0 Pin A state (from digitalRead or HAL_GPIO_ReadPin or similar).
     *  @param e1 Pin B state (from digitalRead or HAL_GPIO_ReadPin or similar).
     *  @return 1 if turned right, -1 if left, 0 if no movement. */
    int8_t tickISR(const bool e0, const bool e1){ // poll encoder isr; returns 1 or -1 when turns, 0 if stop
        int8_t state = pollEnc(e0, e1);
        if (state){
            ef.set(EB_ISR_F);
            ef.write(EB_DIR, state > 0);
        }
        return state;
    }
    /** @brief Poll encoder and handle events.
     *  @param e0 Pin A state (from digitalRead or HAL_GPIO_ReadPin or similar).
     *  @param e1 Pin B state (from digitalRead or HAL_GPIO_ReadPin or similar).
     *  @return 1 if turned right, -1 if left, 0 if no movement. */
    int8_t tick(const bool e0, const bool e1){
        int8_t state = tickRaw(e0, e1);
        if (!state){ clear(); }
        return state;
    }
    /** @brief Poll encoder (ISR context).
     *  @return 1 if turned right, -1 if left, 0 if no movement. */
    int8_t tick(){
        int8_t state = tickRaw();
        if (!state){ clear(); }
        return state;
    }
    /** @brief Raw polling of encoder without clearing event flags.
     *  @param e0 Pin A state (from digitalRead or HAL_GPIO_ReadPin or similar).
     *  @param e1 Pin B state (from digitalRead or HAL_GPIO_ReadPin or similar).
     *  @return 1 if turned right, -1 if left, 0 if no movement. */
    int8_t tickRaw(const bool e0, const bool e1){
        int8_t state = tickRaw();
        if (state) return state;
        state = pollEnc(e0, e1);
        if (state){
            ef.write(EB_DIR, state > 0);
            ef.set(EB_ETRN_R);
            _faststep=0;
        }
        return state;
    }
    /** @brief Raw polling of encoder without clearing event flags (ISR context).
     *  @return 1 if turned right, -1 if left, 0 if no movement. */
    int8_t tickRaw(){
        if (ef.read(EB_ISR_F)){
            ef.clear(EB_ISR_F);
            ef.set(EB_ETRN_R);
            return dir();
        }
        return 0;
    }
    /** @brief Poll the encoder state using a lookup table.
     *  @param e0 Pin A state (from digitalRead or HAL_GPIO_ReadPin or similar).
     *  @param e1 Pin B state (from digitalRead or HAL_GPIO_ReadPin or similar).
     *  @return 1 if turned right, -1 if left, 0 if no movement. */
    inline int8_t pollEnc(const bool e0, const bool e1){
        int8_t val = (0x24428118 >> (((p0<<3)|(p1<<2)|(e0<<1)|e1)<<1))&0b11;
        int8_t delta = (val==1)-(val==2);
        p0 = e0; p1 = e1;
        if (delta == 0){return 0;}
        epos += delta;
        uint8_t mode = ef.mask(0b11);
        if (mode == EB_STEP4_LOW  && !(e0 & e1)){return 0;}// skip 01, 10, 00
        if (mode == EB_STEP4_HIGH &&  (e0 | e1)){return 0;}// skip 01, 10, 11
        if (mode == EB_STEP2      &&  (e0 ^ e1)){return 0;}// skip 10 01
        int8_t state_dir = ((epos > 0) ^ ef.read(EB_REV)) ? -1 : 1;
        epos = 0;
    #ifndef EB_NO_COUNTER
        counter += state_dir;
    #endif
        return state_dir;
    }
#ifndef EB_NO_COUNTER
    int32_t counter = 0; /**< Encoder tick counter. */
#endif
   protected: // ===================== PRIVATE =====================
    encb::Flags<uint8_t> ef; /**< Encoder flags. */
    uint8_t _faststep = 0;   /**< Fast rotation step counter. */
   private:
    int8_t p0   : 2; /**< Last pin A state. */
    int8_t p1   : 2; /**< Last pin B state. */  // signed 2 bit!
    int8_t epos : 4; /**< Encoder position delta accumulator. */

    //     inline int8_t pollEnc(const bool e0, const bool e1){
        //         if (p0 ^ p1 ^ e0 ^ e1){
        //             (p1 ^ e0) ? ++epos : --epos;
        //             p0 = e0, p1 = e1;
        //             if (!epos) return 0;
        //             switch (ef.mask(0b11)){
        //                 case EB_STEP4_LOW:
        //                     if (!(e0 & e1)) return 0;  // skip 01, 10, 00
        //                     break;
        //                 case EB_STEP4_HIGH:
        //                     if (e0 | e1) return 0;  // skip 01, 10, 11
        //                     break;
        //                 case EB_STEP2:
        //                     if (e0 ^ e1) return 0;  // skip 10 01
        //                     break;
        //             }
        //             int8_t state = ((epos > 0) ^ ef.read(EB_REV)) ? -1 : 1;
        //             epos = 0;
        // #ifndef EB_NO_COUNTER
        //             counter += state;
        // #endif
        //             return state;
        //         }
        //         return 0;
        //     }
        //
        // int8_t enc_table[16] = {
        //     0, -1,  1,  0,  state 0 → 0  → 00
        //     1,  0,  0, -1,  state 1 → -1 → 10
        //     -1,  0,  0,  1, state 2 → +1 → 01
        //     0,  1, -1,  0   state 3 → 0  → 00 → 0x24428118
        // };
};//############################################# VIRTUAL ENCODER END #####################################################
//############################################## VIRTUAL ENCODER BUTTON ###################################################
#ifdef EB_FAST_TIME
#define EB_FAST_T (EB_FAST_TIME)
#endif
#ifdef EB_FAST_STEP_TIME
#define EB_FAST_STEP_T (EB_FAST_STEP_TIME)
#endif
class VirtEncButton : public VirtButton, public VirtEncoder {
   public:
    // ================================================ SET =================================================
    /** @brief Set the fast rotation timeout, ms.
    *   @param tout Timeout in milliseconds. */
    void setFastTimeout(const uint8_t tout){ // установить таймаут быстрого поворота, мс
#ifndef EB_FAST_TIME
        EB_FAST_T = tout;
#endif
    }
    /** @brief Set the fast step rotation timeout, ms.
    *   @param tout Timeout in milliseconds. */
    void setFastStepTimeout(const uint8_t tout){ // установить таймаут шага быстрого поворота, мс
#ifndef EB_FAST_STEP_TIME
        EB_FAST_STEP_T = tout;
#endif
    }
    /** @brief Check if the encoder timeout occurred.
    *   @return true if fast step timeout reached [state]. */
    bool timeoutEnc()  { return (uint16_t)((uint16_t)EB_uptime()-tmr)>=EB_FAST_STEP_T; }
    /** @brief Check if the encoder timeout occurred.
    *   @return true if fast step timeout reached [state]. */
    void clear(bool resetTout = false){ // сбросить флаги энкодера и кнопки
        VirtButton::clear(resetTout);
        VirtEncoder::clear(); 
        if(timeoutEnc()){_faststep = 0;}
    }

    // ================================================= GET ================================================
    /** @brief Check if encoder is turned while pressed. @return true if turned with button pressed [event]. */
    bool turnH()        { return turn() && bf.read(EB_EHLD);                       } // нажатый поворот энкодера [событие]
    /** @brief Check if the encoder is in fast rotation state.  @return true if fast rotation detected [state]. */
    bool fast()         { return ef.read(EB_FAST);                                 } // быстрый поворот энкодера [состояние]
    /** @brief Check if encoder was turned to the right. @return true if turned right [event]. */
    bool right()        { return ef.read(EB_DIR) && turn() && !bf.read(EB_EHLD); } // поворот направо [событие]
    /** @brief Check if encoder was turned to the left. @return true if turned left [event]. */
    bool left()         { return !ef.read(EB_DIR) && turn() && !bf.read(EB_EHLD);} // поворот налево [событие]
    /** @brief Check if encoder was turned to the right while pressed. @return true if turned right with button pressed [event]. */
    bool rightH()       { return ef.read(EB_DIR) && turnH();                        } // нажатый поворот направо [событие]
    /** @brief Check if encoder was turned to the left while pressed.  @return true if turned left with button pressed [event]. */
    bool leftH()        { return !ef.read(EB_DIR) && turnH();                       } // нажатый поворот налево [событие]
    /** @brief Check if encoder button is being held (analog to pressing()). @return true if button is pressed [state]. */
    bool Hold()         { return bf.read(EB_EHLD);                                  } // нажата кнопка энкодера. Аналог pressing() [состояние]
    /** @brief Get the action from the encoder or button. @return Event code for turn or button [event]. */
    uint16_t action()   { return turn() ? EB_TURN : VirtButton::action();             } // было действие с кнопки или энкодера, вернёт код события [событие]
    /** @brief Get the action as EBAction type. @return Event code for turn or button [event]. */
    EBAction getAction(){ return (EBAction)action();                                  } // было действие с кнопки или энкодера, вернёт код события [событие]
    /** @brief Get the current fast step value (linear or lookup). @return Step multiplier based on rotation speed [state]. */
    uint8_t stepRaw() { return _faststep; } // чем быстрее поворот, тем больше значение (квадратично)
    static constexpr size_t faststeps_count = sizeof(faststeps)/sizeof(faststeps[0]);// вычисляем безопасный индекс на этапе компиляции
    /** @brief Get the current fast step value from table. @return Step multiplier based on rotation speed [state]. */
    uint16_t step() {
        constexpr auto max_index = faststeps_count - 1;
        size_t idx = _faststep > max_index ? max_index : _faststep;
        return faststeps[idx];
    }
    /** @brief Get the current fast step value from table or depending on var value. 
        @param var target variable to change value by encoder
        @param trig trig value to switch between dependency on var and lookup table faststeps[](more precise!)
        @return Step multiplier based on rotation speed [state]. */
    uint16_t step(int var, uint8_t trig = 2) {
        constexpr auto max_index = faststeps_count - 1;
        size_t idx = _faststep > max_index ? max_index : _faststep; 
        return (_faststep > trig? var >> (max_index-_faststep) : faststeps[idx]);
    }

    // ============================================== POLL ISR ==============================================
    /** @brief Poll encoder in ISR.
    *   @param e0 Encoder pin 0 state (from digitalRead or HAL_GPIO_ReadPin or similar).
    *   @param e1 Encoder pin 1 state.
    *   @return 0 if idle, 1 or -1 if turned [event]. */
    int8_t tickISR(const bool e0, const bool e1){ // обработка в прерывании (только энкодер). Вернёт 0 в покое, 1 или -1 при повороте
        int8_t state = VirtEncoder::pollEnc(e0, e1);
        if (state){
#ifdef EB_NO_BUFFER
            ef.set(EB_ISR_F);
            ef.write(EB_DIR, state > 0);
            ef.write(EB_FAST, _checkFast());
#else
            for (uint8_t i = 0; i < 15; i += 3){//ebuffer = >>[tdftdftdftdftdft]>>
                if (!(ebuffer & (1 << i))){
                    ebuffer |= (1 << i);                          // turn
                    if (state > 0) ebuffer |= (1 << (i + 1));     // dir
                    if (_checkFast()) ebuffer |= (1 << (i + 2));  // fast
                    break;
                }
            }
#endif
        }
        return state;
    }
    /** @brief Process encoder and button without clearing flags or calling callback.
    *   @param e0 Encoder pin 0 state  from digitalRead or HAL_GPIO_ReadPin or similar).
    *   @param e1 Encoder pin 1 state.
    *   @param btn Button state.
    *   @return true if an event detected [event]. */
    bool tick(const bool e0, const bool e1, const bool btn){ // обработка энкодера и кнопки
        clear();
        return _tick(_tickRaw(btn, pollEnc(e0, e1)));
    }
    /** @brief Process encoder in ISR and button state.
    *   @param btn Button state.
    *   @return true if an event occurred [event]. */
    bool tick(const bool btn){ // обработка энкодера (в прерывании) и кнопки
        clear();
        return _tick(_tickRaw(btn));
    }
    /** @brief Process encoder and button without clearing flags or calling callback.
    *   @param e0 Encoder pin 0 state.
    *   @param e1 Encoder pin 1 state.
    *   @param btn Button state.
    *   @return true if an event detected [event]. */
    bool tickRaw(const bool e0, const bool e1, bool btn){ // обработка без сброса событий и вызова коллбэка
        return _tickRaw(btn, pollEnc(e0, e1));
    }
    /** @brief Process button only without clearing flags or calling callback.
    *   @param btn Button state.
    *   @return true if an event detected [event]. */
    bool tickRaw(const bool btn){ // обработка без сброса событий и вызова коллбэка (кнопка)
        return _tickRaw(btn);
    }
    // ===================== PRIVATE =====================
   protected:
#ifndef EB_FAST_TIME
    uint8_t EB_FAST_T = 100;
#endif
#ifndef EB_FAST_STEP_TIME
    uint8_t EB_FAST_STEP_T = 40;
#endif
#ifndef EB_NO_BUFFER
    uint16_t ebuffer = 0;
#endif
   private:
    inline bool _checkFast() {
        uint16_t ms = EB_uptime();
        uint16_t dt = ms - tmr;
        tmr = ms;
        bool f = dt < (uint16_t)(EB_FAST_T >> _faststep);
        _faststep += f;
        return f;
    }
    inline bool _tick(bool f){
#ifndef EB_NO_CALLBACK
        if (f || timeoutState()) {call(true);}
#endif
        return f;
    }
    bool _tickRaw(bool btn, int8_t estate = 0){
        bool encf = 0;
#ifdef EB_NO_BUFFER
        if (ef.read(EB_ISR_F)){
            ef.clear(EB_ISR_F);
            encf = 1;
        }
#else
        if (ebuffer){
            ef.write(EB_DIR, ebuffer & 0b10);
            ef.write(EB_FAST, ebuffer & 0b100);
            ebuffer >>= 3;
            encf = 1;
        }
#endif
        else if (estate){
            ef.write(EB_DIR, estate > 0);
            ef.write(EB_FAST, _checkFast());
            encf = 1;
        } 

        if (encf){
            if (bf.read(EB_PRS))  {bf.set(EB_EHLD);} // зажать энкодер
            else {clicks = 0;}
            if (!bf.read(EB_TOUT)){bf.set(EB_TOUT);} // таймаут
            ef.set(EB_ETRN_R);                       // флаг поворота
        }
        return VirtButton::tickRaw(btn) | encf;
    }
    // bool _checkFast(){
    //     uint16_t ms = EB_uptime();
    //     bool f = ms - tmr < EB_FAST_T;
    //     tmr = ms;
    //     return f;
    // }
};
/** @class Button
 *  @brief Base class for a button.
 *  Provides polling, methods and interrupt-based reading of a button. */
class Button : public VirtButton {// VAR PIN
   public:
    /** @brief Default constructor. 
    *   @warning: call init() before use. 
    *   @note: added because MX_GPIO_Init is usually in main(), and needed to be called before using button.*/
    Button() = default; //WARNING: use init if empty constructor.
    /** @brief Construct button with pin configuration.
    *   @param pin Button pin.
    *   @param mode Pin mode configuration.
    *   @param btnLevel Button active level (AUTO by default).*/
    Button(Pin pin, PinMode mode, uint8_t btnLevel = AUTO): p(pin){ init(pin, mode, btnLevel); }
    /** @brief Initialize button with pin and mode.
    *   @param pin Button pin.
    *   @param mode Pin mode configuration.
    *   @param btnLevel Button active level (AUTO by default). */
    void init(Pin pin, PinMode mode, uint8_t btnLevel = AUTO){ // указать пин и его режим работы
        p = pin;
        EB_mode(p, mode);
        setBtnLevel(levelAuto(btnLevel,mode));
    }
    /** @brief Read raw button state (no debounce). @return Current logical level [state]. */
    bool read(){ return EB_read(p) ^ bf.read(EB_INV); } // прочитать текущее значение кнопки (без дебаунса)
    /** @brief Process button state. Call in loop(). @return true if event occurred [event]. */
    bool tick(){ return VirtButton::tick(EB_read(p)); } // функция обработки, вызывать в loop
    /** @brief Process button without clearing events or calling callback. @return true if event detected [event]. */
    bool tickRaw(){ return VirtButton::tickRaw(EB_read(p)); } // обработка кнопки без сброса событий и вызова коллбэка
   private:
    Pin p;
};
/** @class MultiButton
 *  @brief Base class for a multiple buttons.
 *  Provides polling, methods and interrupt-based reading of a two button. */
class MultiButton : public VirtButton {
   public:
    /** @brief Process two buttons as a combined input (e.g. simultaneous press).
    *   @tparam T0 Type of first button.
    *   @tparam T1 Type of second button.
    *   @param b0 First button instance.
    *   @param b1 Second button instance.
    *   @return true if combined event occurred [event]. */
    template <typename T0, typename T1>
    bool tick(T0& b0, T1& b1){
        b0.clear();
        b1.clear();
        b0.tickRaw();
        b1.tickRaw();
        if (bf.read(EB_BOTH)){
            if (!b0.pressing() && !b1.pressing()) bf.clear(EB_BOTH);
            if (!b0.pressing()) b0.reset();
            if (!b1.pressing()) b1.reset();
            b0.clear();
            b1.clear();
            return VirtButton::tick(true);
        } else {
            if (b0.pressing() && b1.pressing()) bf.set(EB_BOTH);
            b0.call();
            b1.call();
            return VirtButton::tick(false);
        }
    }
};
/** @class Encoder
 *  @brief Base class for a rotary encoder.
 *  Provides polling, methods and interrupt-based reading of a rotary encoder. */
class Encoder : public VirtEncoder {//VAR PIN
   public:
    /** @brief Default constructor. 
    *   @warning: call init() before use. 
    *   @note: added because MX_GPIO_Init is usually in main(), and needed to be called before using encoder.*/
    Encoder()= default; //WARNING: use init if empty constructor.
    /** @brief Construct encoder with pins and mode.
    *   @param encA Encoder A pin like {GPIOx, GPIO_PIN_X} for STM or {PIN_X} for Arduino/ESP
    *   @param encB Encoder B pin.
    *   @param mode Pin mode configuration. like {GPIO_XXXX, GPIO_MODE_XXXX} or {PIN_MODE} for Arduino*/
    Encoder(Pin encA, Pin encB, PinMode mode): e0(encA), e1(encB){ init(encA, encB, mode); }// указать пины и их режим работы
    /** @brief Initialize encoder pins and mode.
    *   @param encA Encoder A pin like {GPIOx, GPIO_PIN_X} for STM or {PIN_X} for Arduino/ESP
    *   @param encB Encoder B pin.
    *   @param mode Pin mode configuration. like {GPIO_XXXX, GPIO_MODE_XXXX} or {PIN_MODE} for Arduino*/
    void init(Pin encA, Pin encB, PinMode mode){ // указать пины и их режим работы
        e0 = encA;
        e1 = encB;
        EB_mode(e0, mode);
        EB_mode(e1, mode);
        initEnc(EB_read(e0), EB_read(e1));
    }    
    /** @brief Encoder polling in ISR context. @return 0 if idle, 1 or -1 if turned [event]. */
    int8_t tickISR(){ return VirtEncoder::tickISR(EB_read(e0), EB_read(e1)); } // ISR ONLY
    /** @brief Process encoder in loop(). @return Rotation direction or 0 [event]. */
    int8_t tick()   { return ef.read(EB_EISR)?VirtEncoder::tick():VirtEncoder::tick(EB_read(e0),EB_read(e1)); }// LOOP ONLY
    /** @brief Process encoder without event reset.  @return Rotation direction or 0 [event]. */
    int8_t tickRaw(){ return ef.read(EB_EISR)?VirtEncoder::tickRaw():VirtEncoder::tickRaw(EB_read(e0),EB_read(e1)); } // NO EVENT  RESET
   private:
    Pin e0, e1;
};
/** @class EncButton
 *  @brief Base class for a rotary encoder with button.
 *  Provides polling, methods and interrupt-based reading of a rotary encoder. */
class EncButton : public VirtEncButton {
   public:
    /** @brief Default constructor. 
    *   @warning: call init() before use. 
    *   @note: added because MX_GPIO_Init is usually in main(), and needed to be called before using encoder & button.*/
    EncButton() =default; //WARNING: use init if empty constructor.
    EncButton(Pin encA, Pin encB, Pin btn, 
              PinMode modeEnc, 
              PinMode modeBtn, 
              uint8_t btnLevel = AUTO) : e0(encA), e1(encB) {
        init(encA, encB, btn, modeEnc, modeBtn, btnLevel);
    }// настроить пины (энк, энк, кнопка, pinmode энк, pinmode кнопка)
    /** @brief Initialize encoder and button pins.
    *   @param encA Encoder A pin.
    *   @param encB Encoder B pin.
    *   @param btn Button pin.
    *   @param modeEnc Encoder pin mode.
    *   @param modeBtn Button pin mode.
    *   @param btnLevel Button active level (AUTO by default). */
    void init(Pin encA, Pin encB, Pin btn, 
        PinMode modeEnc, 
        PinMode modeBtn, 
        uint8_t btnLevel = AUTO){
        e0 = encA;
        e1 = encB;
        b = btn;
        EB_mode(e0, modeEnc);
        EB_mode(e1, modeEnc);
        EB_mode(b, modeBtn);
        setBtnLevel(levelAuto(btnLevel,modeBtn));
        initEnc(EB_read(e0), EB_read(e1));
    }// настроить пины (энк, энк, кнопка, pinmode энк, pinmode кнопка)
    /** @brief Encoder polling in ISR context. @return 0 if idle, 1 or -1 if turned [event]. */
    int8_t tickISR(){ return VirtEncButton::tickISR(EB_read(e0), EB_read(e1)); } // ISR ONLY
    /** @brief Process encoder and button in loop(). @return true if event occurred [event]. */
    bool tick(){ return ef.read(EB_EISR)?VirtEncButton::tick(EB_read(b)):VirtEncButton::tick(EB_read(e0),EB_read(e1),EB_read(b)); } // LOOP ONLY
    /** @brief Process without event reset.  @return true if event occurred [event]. */
    bool tickRaw(){ return ef.read(EB_EISR)?VirtEncButton::tickRaw(EB_read(b)):VirtEncButton::tickRaw(EB_read(e0),EB_read(e1), EB_read(b)); } //NO EVENT RESET
    /** @brief Read button state. @return Current logical level [state]. */
    bool readBtn(){ return EB_read(b) ^ bf.read(EB_INV); } // прочитать значение кнопки
    // ===================== PRIVATE =====================
    private:
        Pin e0, e1, b;
};



#if __cplusplus >= 202002L
    template <Pin BTN>//TEMPLATE PIN
#else
    template <uint8_t BTN>
#endif
class ButtonT : public VirtButton {
   public:
    ButtonT() {}
    ButtonT(PinMode mode, uint8_t btnLevel = AUTO){ init(mode, btnLevel); }    
    void init(PinMode mode, uint8_t btnLevel = AUTO){ // указать режим работы пина
        EB_mode(BTN, mode);
        setBtnLevel(levelAuto(btnLevel,mode));
    }
    bool read(){ return EB_read(BTN) ^ bf.read(EB_INV); } // прочитать текущее значение кнопки (без дебаунса)
    bool tick(){ return VirtButton::tick(EB_read(BTN)); } // функция обработки, вызывать в loop
    bool tickRaw(){ return VirtButton::tickRaw(EB_read(BTN)); } // обработка кнопки без сброса событий и вызова коллбэка
};  

#if __cplusplus >= 202002L
    template <Pin ENCA, Pin ENCB>
#else
    template <uint8_t ENCA, uint8_t ENCB>
#endif
class EncoderT : public VirtEncoder {
   public:
    EncoderT() {}
    EncoderT(PinMode mode){ // указать режим работы пинов
        init(mode);
    }
    void init(PinMode mode){ // указать режим работы пинов
        EB_mode(ENCA, mode);
        EB_mode(ENCB, mode);
        initEnc(EB_read(ENCA), EB_read(ENCB));
    }
    int8_t tickISR(){ return VirtEncoder::tickISR(EB_read(ENCA),EB_read(ENCB)); } // ISR ONLY
    int8_t tick(){ return ef.read(EB_EISR)?VirtEncoder::tick():VirtEncoder::tick(EB_read(ENCA),EB_read(ENCB)); } // LOOP ONLY
    int8_t tickRaw(){ return ef.read(EB_EISR)?VirtEncoder::tickRaw():VirtEncoder::tickRaw(EB_read(ENCA),EB_read(ENCB)); } // NO EVENT RESET
};

#if __cplusplus >= 202002L
    template <Pin ENCA, Pin ENCB, Pin BTN, PinMode MENC, PinMode MBTN>
#else
    template <uint8_t ENCA, uint8_t ENCB, uint8_t BTN, uint8_t MENC = INPUT, uint8_t MBTN = INPUT_PULLUP>
#endif
class EncButtonT : public VirtEncButton {
   public:
    EncButtonT(uint8_t btnLevel = AUTO){
        init();
    } // настроить пины (энк, энк, кнопка, pinmode энк, pinmode кнопка)
    void init(uint8_t btnLevel = AUTO){// настроить пины (pinmode энк, pinmode кнопка)
        EB_mode(ENCA, MENC);
        EB_mode(ENCB, MENC);
        EB_mode(BTN,  MBTN);
        setBtnLevel(btnLevel);
        initEnc(EB_read(ENCA), EB_read(ENCB));
    }
    // ====================== TICK ======================
    int8_t tickISR(){ return VirtEncButton::tickISR(EB_read(ENCA), EB_read(ENCB)); } // ISR
    bool tick(){ return ef.read(EB_EISR) ? VirtEncButton::tick(EB_read(BTN)) : VirtEncButton::tick(EB_read(ENCA), EB_read(ENCB), EB_read(BTN)); } // LOOP
    bool tickRaw(){ return ef.read(EB_EISR) ? VirtEncButton::tickRaw(EB_read(BTN)) : VirtEncButton::tickRaw(EB_read(ENCA), EB_read(ENCB), EB_read(BTN)); } // NO EVENT RESET
    // ====================== READ ======================
    bool readBtn(){ return EB_read(BTN) ^ bf.read(EB_INV); } // прочитать значение кнопки
};
#endif //LIBBENC_HPP