#pragma once
//4400kB
#ifdef ARDUINO
    #if defined(USE_MICRO_WIRE)
        #define MICROWIRE_IMPLEMENTATION
        #include <microwire.hpp> 
    #else
        #include <Wire.h>
    #endif 
    #include <Wire.h>      
    #include <SPI.h>
    #include <Arduino.h>
#else

    #if defined(STM32WB)
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
    #else
        #error "ERROR in SSD1306.hpp: unsupported platform."
    #endif
    
    #define PRINT_H_INTERNAL_IMPLEMENT

   
    //#define constrain(val,min,max) val>max?max:val<min?min:val
    // #define min(a,b) ((a)<(b)?(a):(b))
    // #define max(a,b) ((a)>(b)?(a):(b))
    // template<typename T> T max (T x, T y){return x >  y ? x : y;}
    // template<typename T> T min (T x, T y){return x <= y ? x : y;}
    // #define abs(x)   ((x)>0?(x):-(x))

    #include <cstring>
    #include <type_traits>

    #define _GIO_INLINE static inline __attribute__((always_inline))

    template<typename T> _GIO_INLINE constexpr T        abs(T x)                  { return (x < 0) ? -x : x;   }
    template<typename T> _GIO_INLINE constexpr bool bitRead(T& value, uint8_t bit){ return (value >> bit) & 1; }
    template<typename T> _GIO_INLINE void            bitSet(T& value, uint8_t bit){ value |= (T(1) << bit);    }
    template<typename T> _GIO_INLINE void          bitClear(T& value, uint8_t bit){ value &= ~(T(1) << bit);   } 
    template<typename T> _GIO_INLINE void         bitToggle(T& value, uint8_t bit){ value ^= (T(1) << bit);    }
    template<typename T> _GIO_INLINE void bitWrite(T& value, uint8_t bit, bool bitvalue){ 
                                                bitvalue?bitSet(value, bit):bitClear(value, bit);}
    template<typename T, typename U, typename V> 
                       _GIO_INLINE constexpr auto constrain(T val, U min, V max)  { return(val>max)?max:(val<min)?min:val; }



#endif


#ifndef SSD_OLED_HPP
#define SSD_OLED_HPP

#ifdef USE_ICONS8X8
const static uint8_t icons[][8] = {
        {0xff, 0x85, 0x89, 0x91, 0x91, 0x89, 0x85, 0xff}, //0 email
        {0x3c, 0x5e, 0xa3, 0x03, 0x03, 0xe3, 0x7e, 0x3c}, //1 github
        {0x80, 0xc0, 0xe6, 0xef, 0xef, 0xe6, 0xc0, 0x80}, //2 user
        {0xf0, 0xfe, 0xf1, 0x91, 0x91, 0xf1, 0xfe, 0xf0}, //3 lock, password
        {0x18, 0x7e, 0x7e, 0xe7, 0xe7, 0x7e, 0x7e, 0x18}, //4 cog, settings
        {0x3c, 0x42, 0x81, 0x9d, 0x91, 0x91, 0x42, 0x3c}, //5 clock
        {0x1e, 0x33, 0x21, 0x21, 0x33, 0x7e, 0xe0, 0xc0}, //6 search
        {0x7e, 0x3f, 0x15, 0x15, 0x15, 0x1d, 0x1f, 0x0e}, //7 message
        {0x00, 0x42, 0x24, 0x18, 0xff, 0x99, 0x66, 0x00}, //8 bluetooth
        {0x00, 0x3c, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e},//9 battery 100%
        {0x00, 0x3c, 0x7e, 0x42, 0x7e, 0x7e, 0x7e, 0x7e},//10 battery 75%
        {0x00, 0x3c, 0x7e, 0x42, 0x42, 0x7e, 0x7e, 0x7e},//11 battery 50%
        {0x00, 0x3c, 0x7e, 0x42, 0x42, 0x42, 0x7e, 0x7e},//12 battery 25%
        {0x00, 0x3c, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x7e},//13 battery 0%
        {0xe7, 0x81, 0x81, 0x00, 0x00, 0x81, 0x81, 0xe7}, //14 full screen
        {0x24, 0x24, 0xe7, 0x00, 0x00, 0xe7, 0x24, 0x24}, //15 small screen
        {0x02, 0xfa, 0x8b, 0xfb, 0xfb, 0x8b, 0xfa, 0x02}, //16 trash, remove, delete
        {0x7c, 0x82, 0x83, 0x82, 0xb2, 0xb3, 0x82, 0x7c}, //17 calendar
        {0x7e, 0x81, 0x81, 0x9d, 0x98, 0x94, 0x82, 0x71}, //18 minimize
        {0x7e, 0x81, 0x81, 0x91, 0x88, 0x85, 0x83, 0x77}, //19 maximize
        {0x7c, 0x44, 0xe7, 0xa5, 0xa5, 0xe7, 0x44, 0x7c}, //20 printer
        {0x00, 0x18, 0x18, 0x24, 0x24, 0xc3, 0xc3, 0x00}, //21 share
        {0x1c, 0x3e, 0x73, 0xe1, 0xe1, 0x73, 0x3e, 0x1c}, //22 map
        {0xd8, 0x88, 0xde, 0xda, 0xda, 0xde, 0x88, 0xd8}, //23 case
        {0xe0, 0xfc, 0xfe, 0xa3, 0xfe, 0xfc, 0xe0, 0x00}, //24 warning
        {0x3c, 0x7e, 0xe7, 0xe7, 0xe7, 0xe7, 0x7e, 0x3c}, //25 error
        {0x3c, 0x42, 0x81, 0x99, 0x99, 0x81, 0x42, 0x3c}, //26 record
        {0xff, 0xff, 0xff, 0x7e, 0x7e, 0x3c, 0x3c, 0x18}, //27 play
        {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, //28 stop
        {0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00}, //29 pause
        {0xff, 0x7e, 0x3c, 0x18, 0xff, 0x7e, 0x3c, 0x18}, //30 forward
        {0x18, 0x3c, 0x7e, 0xff, 0x18, 0x3c, 0x7e, 0xff}, //31 backward
        {0x07, 0x0f, 0x1f, 0xff, 0xff, 0x1f, 0x0f, 0x07}, //32 filter
        {0x07, 0x09, 0x11, 0xe1, 0xe1, 0x11, 0x09, 0x07}, //33 filter outline
        {0x3c, 0x7e, 0xff, 0x00, 0x18, 0x42, 0x3c, 0x00}, //34 sound 100%
        {0x3c, 0x7e, 0xff, 0x00, 0x18, 0x00, 0x00, 0x00}, //35 sound 50%
        {0x3c, 0x7e, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00}, //36 sound 0%
        {0x01, 0x02, 0x3c, 0x7e, 0xff, 0x20, 0x40, 0x80}, //37 sound off
        {0xe0, 0x88, 0x90, 0xbf, 0xbf, 0x90, 0x88, 0xe0}, //38 download
        {0xe0, 0x84, 0x82, 0xbf, 0xbf, 0x82, 0x84, 0xe0}, //39 upload
        {0x7e, 0x81, 0xa1, 0xa1, 0xaf, 0xa9, 0x8a, 0x7c}, //40 text file
        {0xf0, 0x88, 0xa0, 0x92, 0x49, 0x05, 0x11, 0x0f}, //41 link
        {0xe0, 0x90, 0x88, 0x44, 0x26, 0x19, 0x0a, 0x04}, //42 pencil
        {0xfe, 0x7f, 0x3f, 0x1f, 0x1f, 0x3f, 0x7f, 0xfe}, //43 bookmark
        {0x00, 0x98, 0xdc, 0xfe, 0x7f, 0x3b, 0x19, 0x18}, //44 flash, lighting
        {0x7e, 0x81, 0x99, 0xa5, 0xa5, 0xa5, 0xa1, 0x9e}, //45 attach
        {0x1e, 0x3f, 0x7f, 0xfe, 0xfe, 0x7f, 0x3f, 0x1e}, //46 heart
        {0x1e, 0x21, 0x41, 0x82, 0x82, 0x41, 0x21, 0x1e}, //47 heart outline
        {0x72, 0x77, 0x77, 0xff, 0xff, 0x77, 0x77, 0x27}, //48 direction
        {0x18, 0x24, 0x42, 0x5a, 0x5a, 0x42, 0x24, 0x18}, //49 eye, visible
        {0x06, 0x09, 0x11, 0xff, 0xff, 0x11, 0x09, 0x06}, //50 antenna
        {0x00, 0xe0, 0x00, 0xf0, 0x00, 0xfc, 0x00, 0xff}, //51 mobile network 100%
        {0x00, 0xe0, 0x00, 0xf0, 0x00, 0xfc, 0x00, 0x80}, //52 mobile network 75%
        {0x00, 0xe0, 0x00, 0xf0, 0x00, 0x80, 0x00, 0x80}, //53 mobile network 50%
        {0x00, 0xe0, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80}, //54 mobile network 25%
        {0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80}, //55 mobile network 0%
        {0x20, 0x40, 0xff, 0x00, 0x00, 0xff, 0x02, 0x04}, //56 sync
        {0x7e, 0x7e, 0x7e, 0x7e, 0x3c, 0x18, 0x3c, 0x7e}, //57 video
        {0x02, 0x09, 0x25, 0x95, 0x95, 0x25, 0x09, 0x02}, //58 wifi
        {0x70, 0x88, 0x84, 0x84, 0xa4, 0x98, 0x90, 0x60}, //59 cloud
        {0x01, 0x0a, 0xf8, 0xfb, 0xf8, 0x0a, 0x01, 0x00}, //60 flashlight on
        {0x00, 0x08, 0xf8, 0x88, 0xf8, 0x08, 0x00, 0x00}, //61 flashlight off
        {0x70, 0x4e, 0x41, 0xc1, 0xc1, 0x41, 0x4e, 0x70}, //62 bell
        {0xf0, 0xfe, 0xf1, 0x91, 0x91, 0xf1, 0xf2, 0xf0}, //63 unlock
        {0x3c, 0x00, 0xff, 0x81, 0x81, 0xff, 0x00, 0x3c}, //64 vibrate
        {0x7e, 0x81, 0x91, 0xa1, 0x91, 0x89, 0x84, 0x72}, //65 checked
        {0x7e, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7e}, //66 unchecked
        {0x54, 0x00, 0xff, 0x81, 0x81, 0xff, 0x00, 0x54}, //67 chip
        {0x48, 0x48, 0xfd, 0x82, 0x82, 0xfd, 0x48, 0x48}, //68 bug
        {0xff, 0x89, 0xe9, 0xa9, 0xa9, 0xe9, 0x8a, 0xfc}, //69 save
        {0xfe, 0xa2, 0x92, 0x8a, 0x8a, 0xcc, 0x28, 0x18}, //70 open
        {0xc0, 0xe0, 0x70, 0x3e, 0x19, 0x11, 0x10, 0x0c}, //71 tool
        {0xff, 0xdd, 0xeb, 0xb7, 0xbf, 0xbf, 0xbf, 0xff}, //72 console
        {0xdb, 0xdb, 0x00, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb}, //73 todo, list
        {0xe7, 0xe7, 0xe7, 0x00, 0x00, 0xe2, 0xe7, 0xe2}, //74 apps
        {0xf1, 0x8a, 0xa4, 0x84, 0x84, 0xa4, 0x8a, 0xf1}, //75 android
        {0x00, 0x0e, 0x71, 0xa1, 0xa1, 0x71, 0x0e, 0x00}, //76 bulb
        {0x7e, 0x81, 0x99, 0x99, 0x18, 0x5a, 0x3c, 0x18}, //77 logout
        {0x18, 0x18, 0x5a, 0x3c, 0x99, 0x81, 0x81, 0x7e}, //78 login
        {0x3e, 0x41, 0x49, 0x5d, 0x49, 0x41, 0xfe, 0xc0}, //79 zoom in
        {0x3e, 0x41, 0x49, 0x49, 0x49, 0x41, 0xfe, 0xc0}, //80 zoom out
        {0xff, 0xff, 0xff, 0x00, 0xf7, 0xf7, 0xf7, 0xf7}, //81 dashboard
        {0xc3, 0x99, 0x3c, 0x7e, 0x7e, 0x3c, 0x99, 0xc3}, //82 all out
        {0xc0, 0xe0, 0xf1, 0xff, 0xff, 0xf1, 0xe0, 0xc0}, //83 science
        {0x3c, 0x42, 0xa1, 0x91, 0x89, 0x85, 0x42, 0x3c}, //84 block
        {0x7f, 0x82, 0xb4, 0x84, 0x84, 0xb4, 0x82, 0x7f}, //85 cat, fox
        {0x06, 0x09, 0x09, 0x06, 0x78, 0x84, 0x84, 0x48}, //86 celsius
        {0x06, 0x09, 0x09, 0x06, 0x08, 0xfc, 0x88, 0xc0}, //87 temperature
        {0x06, 0x09, 0x09, 0x06, 0xf8, 0x20, 0x50, 0x88}, //88 kelvin
        {0x24, 0x24, 0xff, 0x24, 0x24, 0xff, 0x24, 0x24}, //89 tag
        {0x00, 0x00, 0x81, 0xc3, 0x66, 0x3c, 0x18, 0x00}, //90 chevron right
        {0x00, 0x18, 0x3c, 0x66, 0xc3, 0x81, 0x00, 0x00}, //91 chevron left
        {0x30, 0x18, 0x0c, 0x06, 0x06, 0x0c, 0x18, 0x30}, //92 chevron up
        {0x0c, 0x18, 0x30, 0x60, 0x60, 0x30, 0x18, 0x0c}, //93 chevron down
        {0x00, 0x60, 0x90, 0x90, 0x7f, 0x00, 0x00, 0x00}, //94 note 1
        {0x00, 0x60, 0xf0, 0xf0, 0x7f, 0x00, 0x00, 0x00}, //95 note 2
        {0x00, 0x60, 0x90, 0x90, 0x7f, 0x01, 0x01, 0x00}, //96 note 3
        {0x00, 0x60, 0xf0, 0xf0, 0x7f, 0x01, 0x01, 0x00}, //97 note 4
        {0x00, 0x60, 0x90, 0x90, 0x7f, 0x05, 0x05, 0x00}, //98 note 5
        {0x00, 0x60, 0xf0, 0xf0, 0x7f, 0x05, 0x05, 0x00}, //99 note 6
        {0x7f, 0x81, 0x81, 0x81, 0x81, 0x7f, 0x22, 0x1e}, //100 cup empty
        {0x7f, 0xe1, 0xe1, 0xe1, 0xe1, 0x7f, 0x22, 0x1e}, //101 cup 20%
        {0x7f, 0xf1, 0xf1, 0xf1, 0xf1, 0x7f, 0x22, 0x1e}, //102 cup 40%
        {0x7f, 0xf9, 0xf9, 0xf9, 0xf9, 0x7f, 0x22, 0x1e}, //103 cup 60%
        {0x7f, 0xfd, 0xfd, 0xfd, 0xfd, 0x7f, 0x22, 0x1e}, //104 cup 80%
        {0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x22, 0x1e}, //105 cup 100%
        {0x07, 0x09, 0x91, 0xe1, 0xe1, 0x91, 0x09, 0x07}, //106 wineglass
        {0x07, 0x19, 0xe1, 0x81, 0x81, 0xe1, 0x19, 0x07}, //107 glass
        {0x04, 0x06, 0xff, 0xff, 0xff, 0x06, 0x0f, 0x0f}, //108 hammer
        {0xff, 0x81, 0x81, 0x81, 0x82, 0x82, 0x82, 0xfc}, //109 folder outline
        {0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfc}, //110 folder
        {0xff, 0x81, 0x81, 0x91, 0xba, 0x92, 0x82, 0xfc}, //111 add folder
        {0x1c, 0x30, 0x20, 0xef, 0xef, 0x20, 0x30, 0x1c}, //112 microphone
        {0xe0, 0xe0, 0x00, 0xff, 0xff, 0x00, 0xfc, 0xfc}, //113 equalizer
        {0xff, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00, 0xff}, //114 next
        {0xff, 0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0xff}, //115 prev
        {0x3f, 0xa1, 0xa1, 0xe1, 0xe1, 0xa1, 0xa1, 0x3f}, //116 monitor, display
        {0x7c, 0xe2, 0xe1, 0x01, 0x01, 0xe1, 0xe2, 0x7c}, //117 headset, earphones
        {0x18, 0x3c, 0x7e, 0x18, 0x18, 0x7e, 0x3c, 0x18}, //118 workout, fitness
        {0xff, 0x0a, 0x15, 0x0a, 0x15, 0x0a, 0x15, 0x0a}, //119 sport flag
        {0x18, 0x18, 0x24, 0xc3, 0xc3, 0x24, 0x18, 0x18}, //120 location
        {0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0x01, 0xbd}, //121 cellular 1
        {0xc0, 0xa0, 0x90, 0x88, 0x84, 0x82, 0x01, 0xbd}, //122 cellular 2
        {0xc0, 0xa0, 0x90, 0x88, 0x84, 0x82, 0x81, 0xff}, //123 cellular 3
        {0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff, 0xff}, //124 cellular 4
        {0x43, 0x66, 0x7c, 0x78, 0x74, 0x6e, 0xdf, 0xbf}, //125 cellular 5
        {0xc0, 0xe0, 0xf0, 0xf8, 0x0c, 0xae, 0x4f, 0xaf}, //126 cellular 6
        {0x92, 0x49, 0x49, 0x92, 0x92, 0x49, 0x49, 0x92}, //127 waves
        {0x18, 0xf4, 0x92, 0x91, 0x91, 0x92, 0xf4, 0x18}, //128 home
        {0x7e, 0x81, 0xa5, 0x81, 0x81, 0xa5, 0x81, 0x7e}, //129 dice 1
        {0x7e, 0x81, 0x85, 0x81, 0x81, 0xa1, 0x81, 0x7e}, //130 dice 2
        {0x3c, 0x44, 0x47, 0xc4, 0xc4, 0x47, 0x44, 0x3c}, //131 plug
        {0xf8, 0x84, 0x82, 0x81, 0xb1, 0xb2, 0x84, 0xf8}, //132 home 2
        {0xf0, 0x90, 0xf0, 0xf1, 0x92, 0x94, 0x98, 0xf0}, //133 radio
        {0x24, 0x7e, 0xc3, 0x5a, 0x5a, 0xc3, 0x7e, 0x24}, //134 memory
        {0x44, 0x92, 0xba, 0x92, 0x82, 0xaa, 0x82, 0x44}, //135 gamepad
        {0x70, 0x88, 0xa8, 0x8a, 0xa9, 0x8d, 0x89, 0x72}, //136 router
        {0x7e, 0x81, 0x95, 0xa1, 0xa1, 0x95, 0x81, 0x7e}, //137 smile 1
        {0x7e, 0x81, 0xa5, 0x91, 0x91, 0xa5, 0x81, 0x7e}, //138 smile 2
        {0x7e, 0x81, 0xa5, 0xa1, 0xa1, 0xa5, 0x81, 0x7e}, //139 smile 3
        {0x7e, 0x81, 0x85, 0xb1, 0xb1, 0x85, 0x81, 0x7e}, //140 smile 4
        {0x7e, 0x81, 0x8d, 0xe1, 0xe1, 0x8d, 0x81, 0x7e}, //141 smile 5
        {0x01, 0x03, 0xff, 0xfb, 0xbb, 0xab, 0xab, 0xff}, //142 sms
        {0x3c, 0x7e, 0x7e, 0x7e, 0x7e, 0x66, 0x66, 0x3c}, //143 toggle on
        {0x3c, 0x5a, 0x5a, 0x42, 0x42, 0x42, 0x42, 0x3c}, //144 toggle off
        {0x00, 0x00, 0xff, 0x7e, 0x3c, 0x18, 0x00, 0x00}, //145 arrow type 1 right
        {0x00, 0x00, 0x18, 0x3c, 0x7e, 0xff, 0x00, 0x00}, //146 arrow type 1 left
        {0x04, 0x0c, 0x1c, 0x3c, 0x3c, 0x1c, 0x0c, 0x04}, //147 arrow type 1 down
        {0x20, 0x30, 0x38, 0x3c, 0x3c, 0x38, 0x30, 0x20}, //148 arrow type 1 up
        {0x18, 0x18, 0x18, 0x18, 0xff, 0x7e, 0x3c, 0x18}, //149 arrow type 2 right
        {0x18, 0x3c, 0x7e, 0xff, 0x18, 0x18, 0x18, 0x18}, //150 arrow type 2 left
        {0x10, 0x30, 0x70, 0xff, 0xff, 0x70, 0x30, 0x10}, //151 arrow type 2 down
        {0x08, 0x0c, 0x0e, 0xff, 0xff, 0x0e, 0x0c, 0x08}, //152 arrow type 2 up
        {0x02, 0x79, 0x85, 0xb4, 0xa4, 0x85, 0x79, 0x02}, //153 alarm clock
        {0x3c, 0x42, 0x42, 0x3c, 0x08, 0x18, 0x08, 0x38}, //154 key
    };
#endif
#ifdef USE_ICONS7X7
const static uint8_t icons[][7] = {
        {0x7f, 0x45, 0x49, 0x51, 0x49, 0x45, 0x7f}, //0 email
        {0x3e, 0x6f, 0x51, 0x01, 0x71, 0x7f, 0x3e}, //1 github
        {0x40, 0x60, 0x77, 0x77, 0x77, 0x60, 0x40}, //2 user
        {0x70, 0x7e, 0x71, 0x51, 0x71, 0x7e, 0x70}, //3 lock, password
        {0x08, 0x3e, 0x36, 0x63, 0x36, 0x3e, 0x08}, //4 cog, settings
        {0x3e, 0x63, 0x41, 0x4d, 0x49, 0x63, 0x3e}, //5 clock
        {0x0e, 0x11, 0x11, 0x11, 0x3e, 0x60, 0x40}, //6 search
        {0x7e, 0x3f, 0x15, 0x15, 0x1d, 0x1f, 0x0e}, //7 message
        {0x00, 0x22, 0x14, 0x7f, 0x49, 0x36, 0x00}, //8 bluetooth
        {0x1c, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e}, //9 battery 100%
        {0x1c, 0x3e, 0x22, 0x3e, 0x3e, 0x3e, 0x3e}, //10 battery 75%
        {0x1c, 0x3e, 0x22, 0x22, 0x3e, 0x3e, 0x3e}, //11 battery 50%
        {0x1c, 0x3e, 0x22, 0x22, 0x22, 0x3e, 0x3e}, //12 battery 25%
        {0x1c, 0x3e, 0x22, 0x22, 0x22, 0x22, 0x3e}, //13 battery 0%
        {0x77, 0x41, 0x41, 0x00, 0x41, 0x41, 0x77}, //14 full screen
        {0x22, 0x63, 0x00, 0x00, 0x00, 0x63, 0x22}, //15 small screen
        {0x02, 0x7a, 0x4b, 0x7b, 0x4b, 0x7a, 0x02}, //16 trash, remove, delete
        {0x3c, 0x42, 0x43, 0x52, 0x53, 0x42, 0x3c}, //17 calendar
        {0x3e, 0x41, 0x5d, 0x58, 0x54, 0x42, 0x31}, //18 minimize
        {0x3e, 0x41, 0x41, 0x48, 0x45, 0x43, 0x37}, //19 maximize
        {0x3c, 0x24, 0x77, 0x55, 0x77, 0x24, 0x3c}, //20 printer
        {0x1c, 0x1c, 0x1c, 0x22, 0x77, 0x77, 0x77}, //21 share
        {0x0c, 0x1e, 0x3b, 0x71, 0x3b, 0x1e, 0x0c}, //22 map
        {0x6c, 0x47, 0x6d, 0x6d, 0x6d, 0x47, 0x6c}, //23 case
        {0x70, 0x7c, 0x7e, 0x53, 0x7e, 0x7c, 0x70}, //24 warning
        {0x1c, 0x3e, 0x77, 0x77, 0x77, 0x3e, 0x1c}, //25 error
        {0x3e, 0x41, 0x5d, 0x5d, 0x5d, 0x41, 0x3e}, //26 record
        {0x7f, 0x7f, 0x3e, 0x3e, 0x1c, 0x1c, 0x08}, //27 play
        {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f}, //28 stop
        {0x00, 0x7f, 0x7f, 0x00, 0x7f, 0x7f, 0x00}, //29 pause
        {0x7f, 0x3e, 0x1c, 0x00, 0x7f, 0x3e, 0x1c}, //30 forward
        {0x1c, 0x3e, 0x7f, 0x00, 0x1c, 0x3e, 0x7f}, //31 backward
        {0x03, 0x07, 0x0f, 0x7f, 0x0f, 0x07, 0x03}, //32 filter
        {0x03, 0x05, 0x09, 0x71, 0x09, 0x05, 0x03}, //33 filter out
        {0x1c, 0x3e, 0x7f, 0x00, 0x08, 0x22, 0x1c}, //34 sound 100%
        {0x1c, 0x3e, 0x7f, 0x00, 0x1c, 0x00, 0x00}, //35 sound 50%
        {0x1c, 0x3e, 0x7f, 0x00, 0x00, 0x00, 0x00}, //36 sound 0%
        {0x01, 0x02, 0x1c, 0x3e, 0x7f, 0x20, 0x40}, //37 sound off
        {0x70, 0x44, 0x48, 0x5f, 0x48, 0x44, 0x70}, //38 download
        {0x70, 0x44, 0x42, 0x5f, 0x42, 0x44, 0x70}, //39 upload
        {0x3e, 0x41, 0x41, 0x4f, 0x49, 0x4a, 0x3c}, //40 text upload
        {0x70, 0x48, 0x50, 0x2a, 0x05, 0x09, 0x07}, //41 link
        {0x70, 0x48, 0x44, 0x26, 0x19, 0x0a, 0x04}, //42 pencil
        {0x7e, 0x3f, 0x1f, 0x0f, 0x1f, 0x3f, 0x7e}, //43 bookmark
        {0x08, 0x4c, 0x6e, 0x7f, 0x3b, 0x19, 0x08}, //44 flash, lighting
        {0x3e, 0x41, 0x49, 0x55, 0x55, 0x51, 0x4e}, //45 attach
        {0x0e, 0x1f, 0x3e, 0x7c, 0x3e, 0x1f, 0x0e}, //46 heart
        {0x0e, 0x11, 0x22, 0x44, 0x22, 0x11, 0x0e}, //47 heart out
        {0x32, 0x33, 0x33, 0x7f, 0x33, 0x33, 0x23}, //48 direction
        {0x08, 0x14, 0x22, 0x2a, 0x22, 0x14, 0x08}, //49 eye, visible
        {0x02, 0x05, 0x09, 0x7f, 0x09, 0x05, 0x02}, //50 antenna
        {0x60, 0x00, 0x70, 0x00, 0x7c, 0x00, 0x7f}, //51 mobile network 100%
        {0x60, 0x00, 0x70, 0x00, 0x7c, 0x00, 0x40}, //52 mobile network 75%
        {0x60, 0x00, 0x70, 0x00, 0x40, 0x00, 0x40}, //53 mobile network 50%
        {0x60, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40}, //54 mobile network 25%
        {0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40}, //55 mobile network 0%
        {0x10, 0x20, 0x7f, 0x00, 0x7f, 0x02, 0x04}, //56 sync
        {0x3e, 0x3e, 0x3e, 0x1c, 0x08, 0x1c, 0x3e}, //57 video
        {0x02, 0x09, 0x25, 0x15, 0x25, 0x09, 0x02}, //58 wifi
        {0x38, 0x44, 0x42, 0x52, 0x4c, 0x48, 0x30}, //59 cloud
        {0x01, 0x0a, 0x78, 0x7b, 0x78, 0x0a, 0x01}, //60 flashlight on
        {0x00, 0x08, 0x78, 0x48, 0x78, 0x08, 0x00}, //61 flashlight off
        {0x38, 0x26, 0x61, 0x61, 0x61, 0x26, 0x38}, //62 bell
        {0x70, 0x7e, 0x71, 0x51, 0x71, 0x72, 0x70}, //63 unlock
        {0x1c, 0x00, 0x7f, 0x41, 0x7f, 0x00, 0x1c}, //64 vibrate
        {0x3e, 0x41, 0x49, 0x51, 0x49, 0x44, 0x32}, //65 checked
        {0x3e, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3e}, //66 unchecked
        {0x2a, 0x00, 0x7f, 0x41, 0x7f, 0x00, 0x2a}, //67 chip
        {0x28, 0x28, 0x7d, 0x42, 0x7d, 0x28, 0x28}, //68 bug
        {0x7f, 0x49, 0x69, 0x69, 0x69, 0x4a, 0x7c}, //69 save
        {0x7e, 0x52, 0x4a, 0x4a, 0x6c, 0x18, 0x08}, //70 open
        {0x60, 0x70, 0x3e, 0x19, 0x11, 0x10, 0x0c}, //71 tool
        {0x7f, 0x75, 0x5b, 0x5f, 0x5f, 0x5f, 0x7f}, //72 console
        {0x49, 0x49, 0x00, 0x49, 0x49, 0x49, 0x49}, //73 todo, list
        {0x77, 0x77, 0x77, 0x00, 0x72, 0x77, 0x72}, //74 apps
        {0x71, 0x4a, 0x64, 0x44, 0x64, 0x4a, 0x71}, //75 android
        {0x00, 0x06, 0x39, 0x51, 0x39, 0x06, 0x00}, //76 bulb
        {0x3e, 0x41, 0x49, 0x08, 0x2a, 0x1c, 0x08}, //77 logout
        {0x08, 0x2a, 0x1c, 0x49, 0x41, 0x41, 0x3e}, //78 login
        {0x3e, 0x41, 0x49, 0x5d, 0x49, 0x61, 0x7e}, //79 zoom in
        {0x3e, 0x41, 0x49, 0x49, 0x69, 0x71, 0x7e}, //80 zoom out
        {0x7f, 0x7f, 0x7f, 0x00, 0x77, 0x77, 0x77}, //81 dashboard
        {0x63, 0x49, 0x1c, 0x3e, 0x1c, 0x49, 0x63}, //82 all out
        {0x60, 0x70, 0x79, 0x7f, 0x79, 0x70, 0x60}, //83 science
        {0x1c, 0x22, 0x51, 0x49, 0x45, 0x22, 0x1c}, //84 block
        {0x3f, 0x42, 0x54, 0x44, 0x54, 0x42, 0x3f}, //85 cat, fox
        {0x02, 0x05, 0x02, 0x38, 0x44, 0x44, 0x28}, //86 celsius
        {0x02, 0x05, 0x02, 0x08, 0x7c, 0x48, 0x60}, //87 temperature
        {0x02, 0x05, 0x02, 0x7c, 0x10, 0x28, 0x44}, //88 kelvin
        {0x14, 0x14, 0x7f, 0x14, 0x7f, 0x14, 0x14}, //89 tag
        {0x00, 0x41, 0x63, 0x36, 0x1c, 0x08, 0x00}, //90 chevron right
        {0x00, 0x08, 0x1c, 0x36, 0x63, 0x41, 0x00}, //91 chevron left
        {0x30, 0x18, 0x0c, 0x06, 0x0c, 0x18, 0x30}, //92 chevron up
        {0x06, 0x0c, 0x18, 0x30, 0x18, 0x0c, 0x06}, //93 chevron down
        {0x00, 0x30, 0x48, 0x48, 0x3f, 0x00, 0x00}, //94 note 1
        {0x00, 0x30, 0x78, 0x78, 0x3f, 0x00, 0x00}, //95 note 2
        {0x00, 0x30, 0x48, 0x48, 0x3f, 0x01, 0x00}, //96 note 3
        {0x00, 0x30, 0x78, 0x78, 0x3f, 0x01, 0x00}, //97 note 4
        {0x00, 0x30, 0x48, 0x48, 0x3f, 0x05, 0x00}, //98 note 5
        {0x00, 0x30, 0x78, 0x78, 0x3f, 0x05, 0x00}, //99 note 6
        {0x3f, 0x41, 0x41, 0x41, 0x3f, 0x12, 0x0e}, //100 cup empty
        {0x3f, 0x61, 0x61, 0x61, 0x3f, 0x12, 0x0e}, //101 cup 20%
        {0x3f, 0x71, 0x71, 0x71, 0x3f, 0x12, 0x0e}, //102 cup 40%
        {0x3f, 0x79, 0x79, 0x79, 0x3f, 0x12, 0x0e}, //103 cup 60%
        {0x3f, 0x7d, 0x7d, 0x7d, 0x3f, 0x12, 0x0e}, //104 cup 80%
        {0x3f, 0x7f, 0x7f, 0x7f, 0x3f, 0x12, 0x0e}, //105 cup 100%
        {0x07, 0x09, 0x51, 0x71, 0x51, 0x09, 0x07}, //106 wine class
        {0x07, 0x39, 0x41, 0x41, 0x41, 0x39, 0x07}, //107 glass
        {0x04, 0x06, 0x7f, 0x7f, 0x7f, 0x02, 0x07}, //108 hammer
        {0x7f, 0x41, 0x41, 0x42, 0x42, 0x42, 0x7c}, //109 folder outline
        {0x7f, 0x7f, 0x7f, 0x7e, 0x7e, 0x7e, 0x7c}, //110 folder
        {0x7f, 0x41, 0x41, 0x52, 0x7a, 0x52, 0x7c}, //111 add folder
        {0x0c, 0x18, 0x10, 0x77, 0x10, 0x18, 0x0c}, //112 microphone
        {0x70, 0x00, 0x7f, 0x00, 0x7c, 0x00, 0x7e}, //113 equalizer
        {0x7f, 0x7f, 0x3e, 0x1c, 0x08, 0x00, 0x7f}, //114 next
        {0x7f, 0x00, 0x08, 0x1c, 0x3e, 0x7f, 0x7f}, //115 prev
        {0x1f, 0x51, 0x51, 0x71, 0x51, 0x51, 0x1f}, //116 monitor, display
        {0x3c, 0x72, 0x71, 0x01, 0x71, 0x72, 0x3c}, //117 headset, earphones
        {0x1c, 0x3e, 0x08, 0x08, 0x08, 0x3e, 0x1c}, //118 workout, fitness
        {0x7f, 0x0a, 0x05, 0x0a, 0x05, 0x0a, 0x05}, //119 sport flag
        {0x08, 0x08, 0x14, 0x63, 0x14, 0x08, 0x08}, //120 location
        {0x60, 0x70, 0x78, 0x7c, 0x7e, 0x01, 0x5d}, //121 cellular 1
        {0x60, 0x50, 0x48, 0x44, 0x42, 0x01, 0x5d}, //122 cellular 2
        {0x60, 0x50, 0x48, 0x44, 0x42, 0x41, 0x7f}, //123 cellular 3
        {0x60, 0x70, 0x78, 0x7c, 0x7e, 0x7f, 0x7f}, //124 cellular 4
        {0x23, 0x36, 0x3c, 0x38, 0x36, 0x6f, 0x5f}, //125 cellular 5
        {0x60, 0x70, 0x78, 0x04, 0x56, 0x27, 0x57}, //126 cellular 6
        {0x44, 0x22, 0x22, 0x44, 0x22, 0x22, 0x44}, //127 waves
        {0x18, 0x74, 0x52, 0x51, 0x52, 0x74, 0x18}, //128 home
        {0x3e, 0x41, 0x55, 0x41, 0x55, 0x41, 0x3e}, //129 dice 1
        {0x3e, 0x41, 0x45, 0x41, 0x51, 0x41, 0x3e}, //130 dice 2
        {0x1c, 0x27, 0x24, 0x64, 0x24, 0x27, 0x1c}, //131 plug
        {0x78, 0x44, 0x42, 0x41, 0x52, 0x44, 0x78}, //132 home 2
        {0x70, 0x50, 0x71, 0x52, 0x54, 0x58, 0x70}, //133 radio
        {0x14, 0x3e, 0x63, 0x2a, 0x63, 0x3e, 0x14}, //134 memory
        {0x49, 0x5d, 0x49, 0x41, 0x51, 0x45, 0x41}, //135 gamepad
        {0x78, 0x48, 0x6a, 0x49, 0x6d, 0x49, 0x7a}, //136 router
        {0x3e, 0x41, 0x55, 0x61, 0x55, 0x41, 0x3e}, //137 smile 1
        {0x3e, 0x41, 0x65, 0x51, 0x65, 0x41, 0x3e}, //138 smile 2
        {0x3e, 0x41, 0x55, 0x51, 0x55, 0x41, 0x3e}, //139 smile 3
        {0x3e, 0x41, 0x45, 0x51, 0x45, 0x41, 0x3e}, //140 smile 4
        {0x3e, 0x41, 0x4d, 0x61, 0x4d, 0x41, 0x3e}, //141 smile 5
        {0x01, 0x03, 0x7f, 0x5d, 0x55, 0x55, 0x7f}, //142 sms
        {0x1e, 0x3f, 0x3f, 0x3f, 0x33, 0x33, 0x1e}, //143 toggle on
        {0x1e, 0x2d, 0x2d, 0x21, 0x21, 0x21, 0x1e}, //144 toggle off
        {0x00, 0x00, 0x7f, 0x3e, 0x1c, 0x08, 0x00}, //145 arrow type 1 right
        {0x00, 0x08, 0x1c, 0x3e, 0x7f, 0x00, 0x00}, //146 arrow type 1 left
        {0x04, 0x0c, 0x1c, 0x3c, 0x1c, 0x0c, 0x04}, //147 arrow type 1 down
        {0x10, 0x18, 0x1c, 0x1e, 0x1c, 0x18, 0x10}, //148 arrow type 1 up
        {0x08, 0x08, 0x08, 0x7f, 0x3e, 0x1c, 0x08}, //149 arrow type 2 right
        {0x08, 0x1c, 0x3e, 0x7f, 0x08, 0x08, 0x08}, //150 arrow type 2 left
        {0x08, 0x18, 0x38, 0x7f, 0x38, 0x18, 0x08}, //151 arrow type 2 down
        {0x08, 0x0c, 0x0e, 0x7f, 0x0e, 0x0c, 0x08}, //152 arrow type 2 up
        {0x02, 0x39, 0x44, 0x54, 0x44, 0x39, 0x02}, //153 alarm clock
        {0x1c, 0x22, 0x1c, 0x08, 0x18, 0x08, 0x38}, //154 key
    };
#endif
const uint8_t _charMap[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, //   0x20 32
    {0x00, 0x00, 0x6f, 0x00, 0x00}, // ! 0x21 33
    {0x00, 0x07, 0x00, 0x07, 0x00}, // " 0x22 34
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // # 0x23 35
    {0x8C, 0x92, 0xFF, 0x92, 0x62}, // $ 0x24 36
    {0x23, 0x13, 0x08, 0x64, 0x62}, // % 0x25 37
    {0x36, 0x49, 0x56, 0x20, 0x50}, // & 0x26 38
    {0x00, 0x00, 0x07, 0x00, 0x00}, // ' 0x27 39
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // ( 0x28 40
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // ) 0x29 41
    {0x14, 0x08, 0x3e, 0x08, 0x14}, // * 0x2a 42
    {0x08, 0x08, 0x3e, 0x08, 0x08}, // + 0x2b 43
    {0x00, 0x50, 0x30, 0x00, 0x00}, // , 0x2c 44
    {0x08, 0x08, 0x08, 0x08, 0x08}, // - 0x2d 45
    {0x00, 0x60, 0x60, 0x00, 0x00}, // . 0x2e 46
    {0x20, 0x10, 0x08, 0x04, 0x02}, // / 0x2f 47
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0 0x30 48
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1 0x31 49
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2 0x32 50
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3 0x33 51
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4 0x34 52
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5 0x35 53
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6 0x36 54
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7 0x37 55
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8 0x38 56
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9 0x39 57
    {0x00, 0x36, 0x36, 0x00, 0x00}, // : 0x3a 58
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ; 0x3b 59
    {0x08, 0x14, 0x22, 0x41, 0x00}, // < 0x3c 60
    {0x14, 0x14, 0x14, 0x14, 0x14}, // = 0x3d 61
    {0x00, 0x41, 0x22, 0x14, 0x08}, // > 0x3e 62
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ? 0x3f 63
    {0x3e, 0x41, 0x5d, 0x49, 0x4e}, // @ 0x40 64
    {0x7e, 0x09, 0x09, 0x09, 0x7e}, // A 0x41 65
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // B 0x42 66
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // C 0x43 67
    {0x7f, 0x41, 0x41, 0x41, 0x3e}, // D 0x44 68
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // E 0x45 69
    {0x7f, 0x09, 0x09, 0x09, 0x01}, // F 0x46 70
    {0x3e, 0x41, 0x49, 0x49, 0x7a}, // G 0x47 71
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H 0x48 72
    {0x00, 0x41, 0x7f, 0x41, 0x00}, // I 0x49 73
    {0x20, 0x40, 0x41, 0x3f, 0x01}, // J 0x4a 74
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // K 0x4b 75
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // L 0x4c 76
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // M 0x4d 77
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // N 0x4e 78
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // O 0x4f 79
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // P 0x50 80
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // Q 0x51 81
    {0x7f, 0x09, 0x19, 0x29, 0x46}, // R 0x52 82
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S 0x53 83
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // T 0x54 84
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // U 0x55 85
    {0x0f, 0x30, 0x40, 0x30, 0x0f}, // V 0x56 86
    {0x3f, 0x40, 0x30, 0x40, 0x3f}, // W 0x57 87
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X 0x58 88
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y 0x59 89
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z 0x5a 90
    {0x00, 0x00, 0x7f, 0x41, 0x00}, // [ 0x5b 91
    {0x02, 0x04, 0x08, 0x10, 0x20}, // \ 0x5c 92
    {0x00, 0x41, 0x7f, 0x00, 0x00}, // ] 0x5d 93
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^ 0x5e 94
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _ 0x5f 95
    {0x00, 0x00, 0x03, 0x04, 0x00}, // ` 0x60 96
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a 0x61 97
    {0x7f, 0x48, 0x44, 0x44, 0x38}, // b 0x62 98
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c 0x63 99
    {0x38, 0x44, 0x44, 0x48, 0x7f}, // d 0x64 100
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e 0x65 101
    {0x08, 0x7e, 0x09, 0x01, 0x02}, // f 0x66 102
    {0x0c, 0x52, 0x52, 0x52, 0x3e}, // g 0x67 103
    {0x7f, 0x08, 0x04, 0x04, 0x78}, // h 0x68 104
    {0x00, 0x44, 0x7d, 0x40, 0x00}, // i 0x69 105
    {0x20, 0x40, 0x44, 0x3d, 0x00}, // j 0x6a 106
    {0x00, 0x7f, 0x10, 0x28, 0x44}, // k 0x6b 107
    {0x00, 0x41, 0x7f, 0x40, 0x00}, // l 0x6c 108
    {0x7c, 0x04, 0x18, 0x04, 0x78}, // m 0x6d 109
    {0x7c, 0x08, 0x04, 0x04, 0x78}, // n 0x6e 110
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o 0x6f 111
    {0x7c, 0x14, 0x14, 0x14, 0x08}, // p 0x70 112
    {0x08, 0x14, 0x14, 0x18, 0x7c}, // q 0x71 113
    {0x7c, 0x08, 0x04, 0x04, 0x08}, // r 0x72 114
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s 0x73 115
    {0x04, 0x3f, 0x44, 0x40, 0x20}, // t 0x74 116
    {0x3c, 0x40, 0x40, 0x20, 0x7c}, // u 0x75 117
    {0x1c, 0x20, 0x40, 0x20, 0x1c}, // v 0x76 118
    {0x3c, 0x40, 0x30, 0x40, 0x3c}, // w 0x77 119
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x 0x78 120
    {0x0c, 0x50, 0x50, 0x50, 0x3c}, // y 0x79 121
    {0x44, 0x64, 0x54, 0x4c, 0x44}, // z 0x7a 122
    {0x00, 0x08, 0x36, 0x41, 0x41}, // { 0x7b 123
    {0x00, 0x00, 0x7f, 0x00, 0x00}, // | 0x7c 124
    {0x41, 0x41, 0x36, 0x08, 0x00}, // } 0x7d 125
    {0x04, 0x02, 0x04, 0x08, 0x04}, // ~ 0x7e 126
    
    {0x7E, 0x11, 0x11, 0x11, 0x7E},    //__А (0xC0).
    {0x7F, 0x49, 0x49, 0x49, 0x33},    //__Б (0xC1).
    {0x7F, 0x49, 0x49, 0x49, 0x36},    //__В (0xC2).
    {0x7F, 0x01, 0x01, 0x01, 0x03},    //__Г (0xC3).
    {0xE0, 0x51, 0x4F, 0x41, 0xFF},    //__Д (0xC4).
    {0x7F, 0x49, 0x49, 0x49, 0x41},    //__Е (0xC5).
    {0x77, 0x08, 0x7F, 0x08, 0x77},    //__Ж (0xC6).
    {0x41, 0x49, 0x49, 0x49, 0x36},    //__З (0xC7).
    {0x7F, 0x10, 0x08, 0x04, 0x7F},    //__И (0xC8).
    {0x7C, 0x21, 0x12, 0x09, 0x7C},    //__Й (0xC9).
    {0x7F, 0x08, 0x14, 0x22, 0x41},    //__К (0xCA).
    {0x20, 0x41, 0x3F, 0x01, 0x7F},    //__Л (0xCB).
    {0x7F, 0x02, 0x0C, 0x02, 0x7F},    //__М (0xCC).
    {0x7F, 0x08, 0x08, 0x08, 0x7F},    //__Н (0xCD).
    {0x3E, 0x41, 0x41, 0x41, 0x3E},    //__О (0xCE).
    {0x7F, 0x01, 0x01, 0x01, 0x7F},    //__П (0xCF).
    {0x7F, 0x09, 0x09, 0x09, 0x06},    //__Р (0xD0).
    {0x3E, 0x41, 0x41, 0x41, 0x22},    //__С (0xD1).
    {0x01, 0x01, 0x7F, 0x01, 0x01},    //__Т (0xD2).
    {0x47, 0x28, 0x10, 0x08, 0x07},    //__У (0xD3).
    {0x1C, 0x22, 0x7F, 0x22, 0x1C},    //__Ф (0xD4).
    {0x63, 0x14, 0x08, 0x14, 0x63},    //__Х (0xD5).
    {0x7F, 0x40, 0x40, 0x40, 0xFF},    //__Ц (0xD6).
    {0x07, 0x08, 0x08, 0x08, 0x7F},    //__Ч (0xD7).
    {0x7F, 0x40, 0x7F, 0x40, 0x7F},    //__Ш (0xD8).
    {0x7F, 0x40, 0x7F, 0x40, 0xFF},    //__Щ (0xD9).
    {0x01, 0x7F, 0x48, 0x48, 0x30},    //__Ъ (0xDA).
    {0x7F, 0x48, 0x30, 0x00, 0x7F},    //__Ы (0xDB).
    {0x00, 0x7F, 0x48, 0x48, 0x30},    //__Ь (0xDC).
    {0x22, 0x41, 0x49, 0x49, 0x3E},    //__Э (0xDD).
    {0x7F, 0x08, 0x3E, 0x41, 0x3E},    //__Ю (0xDE).
    {0x46, 0x29, 0x19, 0x09, 0x7F},    //__Я (0xDF).

    {0x20, 0x54, 0x54, 0x54, 0x78},    //__а (0xE0).
    {0x3C, 0x4A, 0x4A, 0x49, 0x31},    //__б (0xE1).
    {0x7C, 0x54, 0x54, 0x28, 0x00},    //__в (0xE2).
    {0x7C, 0x04, 0x04, 0x0C, 0x00},    //__г (0xE3).
    {0xE0, 0x54, 0x4C, 0x44, 0xFC},    //__д (0xE4).
    {0x38, 0x54, 0x54, 0x54, 0x18},    //__е (0xE5).
    {0x6C, 0x10, 0x7C, 0x10, 0x6C},    //__ж (0xE6).
    {0x44, 0x54, 0x54, 0x28, 0x00},    //__з (0xE7).
    {0x7C, 0x20, 0x10, 0x08, 0x7C},    //__и (0xE8).
    {0x78, 0x42, 0x24, 0x12, 0x78},    //__й (0xE9).
    {0x7C, 0x10, 0x28, 0x44, 0x00},    //__к (0xEA).
    {0x20, 0x44, 0x3C, 0x04, 0x7C},    //__л (0xEB).
    {0x7C, 0x08, 0x10, 0x08, 0x7C},    //__м (0xEC).
    {0x7C, 0x10, 0x10, 0x10, 0x7C},    //__н (0xED).
    {0x38, 0x44, 0x44, 0x44, 0x38},    //__о (0xEE).
    {0x7C, 0x04, 0x04, 0x04, 0x7C},    //__п (0xEF).
    {0x7C, 0x14, 0x14, 0x14, 0x08},    //__р (0xF0).
    {0x38, 0x44, 0x44, 0x44, 0x00},    //__с (0xF1).
    {0x04, 0x04, 0x7C, 0x04, 0x04},    //__т (0xF2).
    {0x0C, 0x50, 0x50, 0x50, 0x3C},    //__у (0xF3).
    {0x30, 0x48, 0xFE, 0x48, 0x30},    //__ф (0xF4).
    {0x44, 0x28, 0x10, 0x28, 0x44},    //__х (0xF5).
    {0x7C, 0x40, 0x40, 0x7C, 0xC0},    //__ц (0xF6).
    {0x0C, 0x10, 0x10, 0x10, 0x7C},    //__ч (0xF7).
    {0x7C, 0x40, 0x7C, 0x40, 0x7C},    //__ш (0xF8).
    {0x7C, 0x40, 0x7C, 0x40, 0xFC},    //__щ (0xF9).
    {0x04, 0x7C, 0x50, 0x50, 0x20},    //__ъ (0xFA).
    {0x7C, 0x50, 0x50, 0x20, 0x7C},    //__ы (0xFB).
    {0x7C, 0x50, 0x50, 0x20, 0x00},    //__ь (0xFC).
    {0x28, 0x44, 0x54, 0x54, 0x38},    //__э (0xFD).
    {0x7C, 0x10, 0x38, 0x44, 0x38},    //__ю (0xFE).
    {0x08, 0x54, 0x34, 0x14, 0x7C},    //__я (0xFF). 
    {0x38, 0x55, 0x54, 0x55, 0x18},    //__ё (0xFF). 
    /*
    {0x7e, 0x09, 0x09, 0x09, 0x7e}, // А 192
    {0x7F, 0x49, 0x49, 0x49, 0x71}, // Б
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // В
    {0x7F, 0x01, 0x01, 0x01, 0x01}, // Г
    {0x60, 0x3E, 0x21, 0x3F, 0x60}, // Д
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // Е
    {0x76, 0x08, 0x7F, 0x08, 0x76}, // Ж
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // З
    {0x7F, 0x20, 0x10, 0x08, 0x7F}, // И
    {0x7E, 0x20, 0x11, 0x08, 0x7E}, // Й
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // К
    {0x70, 0x0E, 0x01, 0x01, 0x7F}, // Л
    {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // М
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // Н
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // О
    {0x7F, 0x01, 0x01, 0x01, 0x7F}, // П
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // Р
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // С
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // Т
    {0x07, 0x48, 0x48, 0x48, 0x7F}, // У
    {0x1C, 0x22, 0x7F, 0x22, 0x1C}, // Ф
    {0x63, 0x14, 0x08, 0x14, 0x63}, // Х
    {0x7F, 0x40, 0x40, 0x7F, 0xC0}, // Ц
    {0x07, 0x08, 0x08, 0x08, 0x7F}, // Ч
    {0x7F, 0x40, 0x7F, 0x40, 0x7F}, // Ш
    {0x7F, 0x40, 0x7F, 0x40, 0xFF}, // Щ
    {0x01, 0x7F, 0x48, 0x48, 0x70}, // Ъ
    {0x7F, 0x48, 0x70, 0x00, 0x7F}, // Ы
    {0x00, 0x7F, 0x48, 0x48, 0x70}, // Ь
    {0x22, 0x41, 0x49, 0x49, 0x3E}, // Э
    {0x7F, 0x08, 0x3E, 0x41, 0x3E}, // Ю
    {0x46, 0x29, 0x19, 0x09, 0x7F}, // Я 223

    {0x20, 0x54, 0x54, 0x54, 0x78}, //a 224
    {0x3c, 0x4a, 0x4a, 0x49, 0x31}, //б
    {0x7c, 0x54, 0x54, 0x28, 0x00}, //в
    {0x7c, 0x04, 0x04, 0x04, 0x0c}, //г
    {0xe0, 0x54, 0x4c, 0x44, 0xfc}, //д
    {0x38, 0x54, 0x54, 0x54, 0x18}, //e
    {0x6c, 0x10, 0x7c, 0x10, 0x6c}, //ж
    {0x44, 0x44, 0x54, 0x54, 0x28}, //з
    {0x7c, 0x20, 0x10, 0x08, 0x7c}, //и
    {0x7c, 0x41, 0x22, 0x11, 0x7c}, //й
    {0x7c, 0x10, 0x28, 0x44, 0x00}, //к
    {0x20, 0x44, 0x3c, 0x04, 0x7c}, //л
    {0x7c, 0x08, 0x10, 0x08, 0x7c}, //м
    {0x7c, 0x10, 0x10, 0x10, 0x7c}, //н
    {0x38, 0x44, 0x44, 0x44, 0x38}, //o
    {0x7c, 0x04, 0x04, 0x04, 0x7c}, //п
    {0x7C, 0x14, 0x14, 0x14, 0x08}, //p
    {0x38, 0x44, 0x44, 0x44, 0x20}, //c
    {0x04, 0x04, 0x7c, 0x04, 0x04}, //т
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, //у
    {0x30, 0x48, 0xfc, 0x48, 0x30}, //ф
    {0x44, 0x28, 0x10, 0x28, 0x44}, //x
    {0x7c, 0x40, 0x40, 0x40, 0xfc}, //ц
    {0x0c, 0x10, 0x10, 0x10, 0x7c}, //ч
    {0x7c, 0x40, 0x7c, 0x40, 0x7c}, //ш
    {0x7c, 0x40, 0x7c, 0x40, 0xfc}, //щ
    {0x04, 0x7c, 0x50, 0x50, 0x20}, //ъ
    {0x7c, 0x50, 0x50, 0x20, 0x7c}, //ы
    {0x7c, 0x50, 0x50, 0x20, 0x00}, //ь
    {0x28, 0x44, 0x54, 0x54, 0x38}, //э
    {0x7c, 0x10, 0x38, 0x44, 0x38}, //ю
    {0x08, 0x54, 0x34, 0x14, 0x7c}, //я 255
*/
};

#ifdef PRINT_H_INTERNAL_IMPLEMENT
#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <cstdio>
class Print {
public:
    virtual ~Print() = default;
    virtual size_t write(uint8_t data) = 0;
    virtual size_t write(const uint8_t *buffer, size_t size) {
        size_t n = 0;
        while (size--) { n += write(*buffer++); }
        return n;
    }
    size_t write(const char *str) {
        if (!str) return 0;
        return write(reinterpret_cast<const uint8_t *>(str), std::strlen(str));
    }
    size_t print(const char *s) {
        return write(s);
    }
    size_t print(char c) {
        return write(static_cast<uint8_t>(c));
    }
    size_t print(int v) {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%d", v);
        return write(buf);
    }
    size_t print(unsigned v) {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%u", v);
        return write(buf);
    }
    size_t print(long v) {
        char buf[24];
        std::snprintf(buf, sizeof(buf), "%ld", v);
        return write(buf);
    }
    size_t print(unsigned long v) {
        char buf[24];
        std::snprintf(buf, sizeof(buf), "%lu", v);
        return write(buf);
    }
    size_t print(double v, int digits = 2) {
        char fmt[8];
        char buf[32];
        std::snprintf(fmt, sizeof(fmt), "%%.%df", digits);
        std::snprintf(buf, sizeof(buf), fmt, v);
        return write(buf);
    }
    size_t println() {
        return write('\r') + write('\n');
    }
    template<typename T>
    size_t println(const T &v) {
        return print(v) + println();
    }
    int16_t printf(const char *format, ...) { //+5948B additional memory usage
        char buf[128];
        va_list ap;
        va_start(ap, format);
        int n = std::vsnprintf(buf, sizeof(buf), format, ap);
        va_end(ap);
        if (n < 0) return n;
        write(reinterpret_cast<const uint8_t *>(buf), static_cast<size_t>(n));
        return static_cast<int16_t>(n);
    }
};
#endif

#define SSD1306_128x32 0
#define SSD1306_128x64 1
#define SSH1106_128x64 2

#define OLED_I2C       0
#define OLED_SPI       1

#define OLED_NO_BUFFER 0
#define OLED_BUFFER    1

#define OLED_CLEAR     0
#define OLED_FILL      1
#define OLED_STROKE    2

#define BUF_ADD        0
#define BUF_SUBTRACT   1
#define BUF_REPLACE    2

#define BITMAP_NORMAL  0
#define BITMAP_INVERT  1

#define OLED_WIDTH              128
#define OLED_HEIGHT_32          0x02
#define OLED_HEIGHT_64          0x12
#define OLED_64                 0x3F
#define OLED_32                 0x1F

#define OLED_DISPLAY_OFF        0xAE
#define OLED_DISPLAY_ON         0xAF

#define OLED_COMMAND_MODE       0x00
#define OLED_ONE_COMMAND_MODE   0x80
#define OLED_DATA_MODE          0x40
#define OLED_ONE_DATA_MODE      0xC0

#define OLED_ADDRESSING_MODE    0x20
#define OLED_HORIZONTAL         0x00
#define OLED_VERTICAL           0x01

#define OLED_NORMAL_V           0xC8
#define OLED_FLIP_V             0xC0
#define OLED_NORMAL_H           0xA1
#define OLED_FLIP_H             0xA0

#define OLED_CONTRAST           0x81
#define OLED_SETCOMPINS         0xDA
#define OLED_SETVCOMDETECT      0xDB
#define OLED_CLOCKDIV           0xD5
#define OLED_SETMULTIPLEX       0xA8
#define OLED_COLUMNADDR         0x21
#define OLED_PAGEADDR           0x22
#define OLED_CHARGEPUMP         0x8D

#define OLED_NORMALDISPLAY      0xA6
#define OLED_INVERTDISPLAY      0xA7

constexpr uint16_t BUFSIZE_128x64 (128*64/8);
constexpr uint16_t BUFSIZE_128x32 (128*32/8);

#ifndef OLED_SPI_SPEED
#define OLED_SPI_SPEED 1000000ul
#endif

static const uint8_t _oled_init[] = {
    OLED_DISPLAY_OFF,
    OLED_CLOCKDIV,
    0x80,    // value
    OLED_CHARGEPUMP,
    0x14,    // value
    OLED_ADDRESSING_MODE,
    OLED_VERTICAL,
    OLED_NORMAL_H,
    OLED_NORMAL_V,
    OLED_CONTRAST,
    0x7F,    // value
    OLED_SETVCOMDETECT,
    0x40,     // value
    OLED_NORMALDISPLAY,
    OLED_DISPLAY_ON,
};

template <
    typename Interface,
    int _TYPE = SSD1306_128x32,
    int _BUFF = OLED_BUFFER,
    GPIO_TypeDef* CS_PORT = nullptr,
    GPIO_TypeDef* DC_PORT = nullptr,
    GPIO_TypeDef* RST_PORT = nullptr,
    int16_t _CS = -1,
    int16_t _DC = -1,
    int16_t _RST = -1
>
#ifndef OLED_NO_PRINT
class SSD_OLED : public Print { 
#else
class SSD_OLED {
#endif
public:
    bool _CONN;
    Interface* _interface;
    const uint8_t _address = 0x3C;
    static constexpr int _bufSize = (_BUFF == 1) ? (_TYPE ? BUFSIZE_128x64 : BUFSIZE_128x32) : 0;
    uint8_t _oled_buffer[_bufSize];
    
    SSD_OLED(Interface* interface, uint8_t address = 0x3C): _interface(interface), _address(address){}
    void init() {
        if constexpr (std::is_same_v<Interface, SPI_HandleTypeDef>){
            _CONN = 1;
            HAL_GPIO_WritePin(CS_PORT, _CS, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(DC_PORT, _DC, GPIO_PIN_RESET);
            if(_RST >= 0){
                HAL_GPIO_WritePin(RST_PORT, _RST, GPIO_PIN_SET); 
                HAL_Delay(1);
                HAL_GPIO_WritePin(RST_PORT, _RST, GPIO_PIN_RESET); 
                HAL_Delay(10);
                HAL_GPIO_WritePin(RST_PORT, _RST, GPIO_PIN_SET); 
            }
        }
        else if constexpr (std::is_same_v<Interface, I2C_HandleTypeDef>){
            _CONN = 0;
        }

        sendData(_oled_init, sizeof(_oled_init), OLED_COMMAND_MODE);

        uint8_t cmd[] = { OLED_SETCOMPINS,
                        _TYPE ? OLED_HEIGHT_64 : OLED_HEIGHT_32,
                        OLED_SETMULTIPLEX,
                        _TYPE ? OLED_64 : OLED_32 };
        sendData(cmd, sizeof(cmd), OLED_COMMAND_MODE);

        if (_CONN){ HAL_GPIO_WritePin(CS_PORT, _CS, GPIO_PIN_SET); }

        setCursor(0, 0);
        if (_BUFF) setWindow(0, 0, _maxX, _maxRow);   
        clear();
    }
    void sendData(const uint8_t* data, size_t nbytes = 0, uint8_t prefix = OLED_COMMAND_MODE) {
        if constexpr (std::is_same_v<Interface, SPI_HandleTypeDef>) {
            HAL_GPIO_WritePin(CS_PORT, _CS, GPIO_PIN_RESET); // select OLED
            prefix == OLED_COMMAND_MODE ? HAL_GPIO_WritePin(DC_PORT, _DC, GPIO_PIN_RESET)  // command
                                        : HAL_GPIO_WritePin(DC_PORT, _DC, GPIO_PIN_SET);   // data
            #ifdef USE_DMA
            HAL_SPI_Transmit_DMA(_interface, const_cast<uint8_t*>(data), nbytes);
            while (HAL_SPI_GetState(_interface) != HAL_SPI_STATE_READY);
            #else
            HAL_SPI_Transmit(_interface, const_cast<uint8_t*>(data), nbytes ? nbytes : 1, HAL_MAX_DELAY);
            #endif
            HAL_GPIO_WritePin(CS_PORT, _CS, GPIO_PIN_SET);
        } else { // I2C
            auto flush = [&]() {
                if (idx > 1) {
                    buf[0] = OLED_DATA_MODE;
                    #ifdef USE_DMA
                    HAL_I2C_Master_Transmit_DMA(_interface, _address << 1, buf, idx);
                    while (HAL_I2C_GetState(_interface) != HAL_I2C_STATE_READY);
                    #else
                    HAL_I2C_Master_Transmit(_interface, _address << 1, buf, idx, HAL_MAX_DELAY);
                    #endif
                    idx = 1;
                }
            };
            if (nbytes == 0) { nbytes = 1;// send as it is
               uint32_t timeout = 100000;
               #ifdef USE_DMA
                HAL_I2C_Master_Transmit_DMA(_interface, _address << 1, const_cast<uint8_t*>(data), 0, HAL_MAX_DELAY);
               #else
                I2C_TypeDef *i2c = _interface->Instance;
                while ((i2c->ISR & I2C_ISR_BUSY) && timeout--) {}
                i2c->CR2 =
                    ((uint32_t)_address << 1) |         // 7-bit address
                    (1U << I2C_CR2_NBYTES_Pos) |        // 1 uint8_t
                    I2C_CR2_START |                     // START
                    I2C_CR2_AUTOEND;                    // STOP после байта
                while ((i2c->ISR & I2C_ISR_BUSY) && timeout--) {}
                i2c->TXDR = *data;
                while ((i2c->ISR & I2C_ISR_BUSY) && timeout--) {}
                i2c->ICR = I2C_ICR_STOPCF;
                #endif
                return;
            }else if (prefix == OLED_DATA_MODE) {
                for (size_t i = 0; i < nbytes; i++) {
                    buf[idx++] = data[i];
                    if (idx >= sizeof(buf)) { // буфер полный
                        flush();
                    }
                }
                return;
            }
            flush();          // ===== если COMMAND =====
            uint8_t tmp[128]; // сначала отправляем накопленные DATA
            tmp[0] = prefix;  // команды не буферизуем!
            if (nbytes > 127) nbytes = 127;
            memcpy(tmp + 1, data, nbytes);
            #ifdef USE_DMA
            HAL_I2C_Master_Transmit_DMA(_interface, _address << 1, tmp, nbytes + 1);
            while (HAL_I2C_GetState(_interface) != HAL_I2C_STATE_READY);
            #else
            HAL_I2C_Master_Transmit(_interface, _address << 1, tmp, nbytes + 1, HAL_MAX_DELAY);
            #endif
        }
    }
    void writeData(uint8_t data, uint8_t offsetY = 0, uint8_t offsetX = 0, int mode = 0) {
        if (_BUFF) {
            switch (mode) {
            case 0: _oled_buffer[_bufIndex(_x+offsetX, _y) + offsetY] |= data; break; // добавить     
            case 1: _oled_buffer[_bufIndex(_x+offsetX, _y) + offsetY] &= ~data; break;// вычесть
            case 2: _oled_buffer[_bufIndex(_x+offsetX, _y) + offsetY] = data; break;  // заменить                   
            }
        } else {
            if (_buf_flag) {
                int x = _x - _bufX;
                int y = _y - _bufY;
                if (x < 0 || x > _bufsizeX || y < 0 || y > (_bufsizeY << 3)) return;
                switch (mode) {
                case 0: _buf_ptr[ (y>>3) + x * _bufsizeY ] |=  data; break;  // добавить
                case 1: _buf_ptr[ (y>>3) + x * _bufsizeY ] &= ~data; break; // вычесть
                case 2: _buf_ptr[ (y>>3) + x * _bufsizeY ] =   data; break;   // заменить
                }                
            } else {
                const uint8_t b = data;
                sendData(&b, 1, OLED_DATA_MODE);
            }
        }
    }
    void setWindow(int x0, int y0, int x1, int y1){
        uint8_t cmd[] = { OLED_COLUMNADDR,
                        constrain8(x0, 0, _maxX),
                        constrain8(x1, 0, _maxX),
                        OLED_PAGEADDR,
                        constrain8(y0, 0, _maxRow),
                        constrain8(y1, 0, _maxRow) };
        sendData(cmd, sizeof(cmd),OLED_COMMAND_MODE);
    }
    void setWindowShift(int x0, int y0, int sizeX, int sizeY) {
        _shift = y0&0b111;
        if (!_BUFF) setWindow(x0,(y0>>3),x0+sizeX,(y0+sizeY-1)>>3);
    }
    void setCursor(int x, int y){               
        setWindowShift((_x=x), (_y=y), _maxX, _scaleY);
    }
    void setScale(uint8_t scale){
        scale = constrain(scale, (uint8_t)1, (uint8_t)4);
        _scaleX = scale;
        _scaleY = scale * 8;
        setCursor(_x, _y);
    }
    void fill(uint8_t data) {
        if (_BUFF) {
            memset(_oled_buffer, data, _bufSize);
        } else {
            const uint16_t bufsize =_TYPE ? 1024 : 512;
            setWindow(0, 0, _maxX, _maxRow);
            uint8_t buf[bufsize];
            memset(buf, data, sizeof(buf));
            sendData(buf, sizeof(buf), OLED_DATA_MODE);
        }
        setCursor(_x, _y);
    }
    void clear() { 
        fill(0);
    }
    void update() {
        if (!_BUFF) return;   // если нет буфера — нечего обновлять
        const int bufSize = _TYPE ? 1024 : 512;  // размер буфера для SSD1306
        setWindow(0, 0, _maxX, _maxRow);
        sendData(_oled_buffer, bufSize, OLED_DATA_MODE);
    }
    void update(int x0, int y0, int x1, int y1) {
        if (!_BUFF) return;
        y0 >>= 3;
        y1 >>= 3;
        x0 = constrain(x0, 0, _maxX);
        x1 = constrain(x1, 0, _maxX);
        y0 = constrain(y0, 0, _maxRow);
        y1 = constrain(y1, 0, _maxRow);
        setWindow(x0, y0, x1, y1);
        for (int x = x0; x <= x1; x++) {
            uint8_t line[8]; // максимум страниц
            int len = 0;
            for (int y = y0; y <= y1; y++) {
                line[len++] = _oled_buffer[y + x * (_TYPE ? 8 : 4)];
            }
            sendData(line, len, OLED_DATA_MODE);
        }
    }
    void dot(int x, int y, uint8_t fill = 1) {
        if (x < 0 || x > _maxX || y < 0 || y > _maxY) return;

        if (_BUFF) {
            bitWrite(_oled_buffer[_bufIndex(x, y)], y & 7, fill);
            return;
        }

        if (_buf_flag) {
            x -= _bufX;
            y -= _bufY;
            if (x < 0 || x > _bufsizeX || y < 0 || y > (_bufsizeY << 3)) return;
            bitWrite(_buf_ptr[(y >> 3) + x * _bufsizeY], y & 7, fill);
            return;
        }
        static int last_x = -1;
        static int last_page = -1;
        int page = y >> 3;
        uint8_t data = (fill ? 1 : 0) << (y & 7);
        if (x != last_x || page != last_page) {
            uint8_t cmd[] = {
                OLED_COLUMNADDR, (uint8_t)x, (uint8_t)x,
                OLED_PAGEADDR,   (uint8_t)page, (uint8_t)page
            };
            sendData(cmd, sizeof(cmd), OLED_COMMAND_MODE);
            last_x = x;
            last_page = page;
        }

        sendData(&data, 1, OLED_DATA_MODE);
    }   
    void line(int x0, int y0, int x1, int y1, uint8_t fill = 1) {
        _x = 0;
        _y = 0;
        if (x0 == x1) lineV(x0, y0, y1, fill);
        else if (y0 == y1) lineH(y0, x0, x1, fill);
        else {
            int sx, sy, e2, err;
            int dx = abs(x1 - x0);
            int dy = abs(y1 - y0);
            sx = (x0 < x1) ? 1 : -1;
            sy = (y0 < y1) ? 1 : -1;
            err = dx - dy;
            for (;;) {
                dot(x0, y0, fill);
                if (x0 == x1 && y0 == y1) return;
                e2 = err<<1;
                if (e2 > -dy) { 
                    err -= dy; 
                    x0 += sx; 
                }
                if (e2 < dx) { 
                    err += dx; 
                    y0 += sy; 
                }
            }
        }
    }
    void lineH(int y, int x0, int x1, uint8_t fill = 1) {
        _x = 0;
        _y = 0;
        if (x0 > x1) _swap(x0, x1);
        if (y < 0 || y > _maxY) return;
        if (x0 == x1) {
            dot(x0, y, fill);
            return;        
        }
        x1++;
        x0 = constrain(x0, 0, _maxX);
        x1 = constrain(x1, 0, _maxX);
        if (_BUFF) {
            for (int x = x0; x < x1; x++) dot(x, y, fill);
        } else {
            if (_buf_flag) {
                for (int x = x0; x < x1; x++) dot(x, y, fill);
                return;
            }
            uint8_t data = 0b1 << (y & 0b111);
            y >>= 3;            
            setWindow(x0, y, x1, y);
            for (int x = x0; x < x1; x++) writeData(data, y, x);
        }            
    }
    void lineV(int x, int y0, int y1, uint8_t fill = 1) {
        _x = 0;
        _y = 0;
        if (y0 > y1) _swap(y0, y1);
        if (x < 0 || x > _maxX) return;
        if (y0 == y1) {
            dot(x, y0, fill);
            return;            
        }
        y1++;
        if (_BUFF) {
            for (int y = y0; y < y1; y++) dot(x, y, fill);
        } else {
            if (_buf_flag) {
                for (int y = y0; y < y1; y++) dot(x, y, fill);
                return;
            }
            if (fill) fill = 255;
            uint8_t shift = y0 & 0b111;
            uint8_t shift2 = 8 - (y1 & 0b111);
            if (shift2 == 8) shift2 = 0;
            int height = y1 - y0;
            y0 >>= 3;
            y1 = (y1 - 1) >> 3;
            uint8_t numuint8_ts = y1 - y0;
            setWindow(x, y0, x, y1);

            if (numuint8_ts == 0) {            
                if (_inRange(y0, 0, _maxRow)) writeData( (fill >> (8-height)) << shift, y0, x );
            } else {
                if (_inRange(y0, 0, _maxRow)) writeData(fill << shift, y0, x);                                        // начальный кусок
                y0++;
                for (uint8_t i = 0; i < numuint8_ts - 1; i++, y0++) if (_inRange(y0, 0, _maxRow)) writeData(fill, y0, x);    // столбик
                if (_inRange(y0, 0, _maxRow)) writeData(fill >> shift2, y0, x);                                        // нижний кусок            
            }
        }
    }
    void circle(int x, int y, int radius, uint8_t fill = OLED_FILL) {
        int f = 1 - radius;
        int ddF_x = 1;
        int ddF_y = -2 * radius;
        int x1 = 0;
        int y1 = radius;
        uint8_t fillLine = (fill == OLED_CLEAR) ? 0 : 1;
        dot(x, y + radius, fillLine);
        dot(x, y - radius, fillLine);
        dot(x + radius, y, fillLine);
        dot(x - radius, y, fillLine);        
        if (fill != OLED_STROKE) lineV(x, y - radius, y + radius-1, fillLine);
        while (x1 < y1) {
            if(f >= 0) {
                y1--;
                ddF_y += 2;
                f += ddF_y;
            }
            x1++;
            ddF_x += 2;
            f += ddF_x;
            if (fill == OLED_STROKE) {            
                dot(x + x1, y + y1);
                dot(x - x1, y + y1);
                dot(x + x1, y - y1);
                dot(x - x1, y - y1);            
                dot(x + y1, y + x1);
                dot(x - y1, y + x1);
                dot(x + y1, y - x1);
                dot(x - y1, y - x1);
            } else {    // FILL / CLEAR
                lineV(x + x1, y - y1, y + y1, fillLine);
                lineV(x - x1, y - y1, y + y1, fillLine);
                lineV(x + y1, y - x1, y + x1, fillLine);
                lineV(x - y1, y - x1, y + x1, fillLine);           
            }
        }
    } 
    void clear(int x0, int y0, int x1, int y1) {
        if (_TYPE < 2) {    // для SSD1306        
            if (_BUFF) rect(x0, y0, x1, y1, OLED_CLEAR);
            else {
                x1++;
                y1++;
                y0 >>= 3;
                y1 = (y1 - 1) >> 3;
                y0 = constrain(y0, 0, _maxRow);
                y1 = constrain(y1, 0, _maxRow);
                x0 = constrain(x0, 0, _maxX);
                x1 = constrain(x1, 0, _maxX);                
                setWindow(x0, y0, x1, y1);
                for (int x = x0; x < x1; x++)
                for (int y = y0; y < y1+1; y++)
                writeData(0, y, x, 2);
            }
        } else {
            // для SSH1108
        }
        setCursor(_x, _y);
    }
    void rect(int x0, int y0, int x1, int y1, uint8_t fill = 1) {
        _x = 0;
        _y = 0;        
        if (x0 > x1) _swap(x0, x1);
        if (y0 > y1) _swap(y0, y1);
        if (fill == OLED_STROKE) {
            lineH(y0, x0+1, x1-1);
            lineH(y1, x0+1, x1-1);
            lineV(x0, y0, y1);
            lineV(x1, y0, y1);
        } else {
            if (x0 == x1 && y0 == y1) { dot(x0, y0, fill); return; }
            if (x0 == x1) { lineV(x0, y0, y1); return; }
            if (y0 == y1) { lineH(y0, x0, x1); return; }
            if (!_BUFF && fill == OLED_CLEAR) { clear(x0, y0, x1, y1); return; }            
            if (_buf_flag) { // если рисуем в мини-буфер
                x0 = constrain(x0, 0, _maxX);
                x1 = constrain(x1, 0, _maxX);
                for (uint8_t x = x0; x <= x1; x++) lineV(x, y0, y1, fill == OLED_FILL ? 1 : 0);
                return;
            }
            uint8_t thisFill = (fill == OLED_FILL ? 0 : 1);
            x1++; // рисуем в олед и в большой буфер
            y1++;
            uint8_t shift = y0 & 0b111;
            uint8_t shift2 = 8 - (y1 & 0b111);
            if (shift2 == 8) shift2 = 0;
            int height = y1 - y0;
            y0 >>= 3;
            y1 = (y1 - 1) >> 3;
            uint8_t numuint8_ts = y1 - y0;            
            x0 = constrain(x0, 0, _maxX);
            x1 = constrain(x1, 0, _maxX);
            if (!_BUFF) setWindow(x0, y0, x1, y1);
            for (uint8_t x = x0; x < x1; x++) {
                int y = y0;
                if (numuint8_ts == 0) {            
                    if (_inRange(y, 0, _maxRow)) writeData( (255 >> (8-height)) << shift, y, x, thisFill);
                } else {
                    if (_inRange(y, 0, _maxRow)) writeData(255 << shift, y, x, thisFill);                                        // начальный кусок
                    y++;
                    for (uint8_t i = 0; i < numuint8_ts - 1; i++, y++) if (_inRange(y, 0, _maxRow)) writeData(255, y, x, thisFill);    // столбик
                    if (_inRange(y, 0, _maxRow)) writeData(255 >> shift2, y, x, thisFill);                                        // нижний кусок            
                }
            }                 
        }        
    } 
    void roundRect(int x0, int y0, int x1, int y1, uint8_t fill = OLED_FILL) {
        if (fill == OLED_STROKE) {
            lineV(x0, y0+2, y1-2);
            lineV(x1, y0+2, y1-2);
            lineH(y0, x0+2, x1-2);
            lineH(y1, x0+2, x1-2);
            dot(x0+1, y0+1);
            dot(x1-1, y0+1);
            dot(x1-1, y1-1);
            dot(x0+1, y1-1);             
        } else {            
            lineV(x0, y0+2, y1-2, fill);
            lineV(x0+1, y0+1, y1-1, fill);
            lineV(x1-1, y0+1, y1-1, fill);
            lineV(x1, y0+2, y1-2, fill);
            rect(x0+2, y0, x1-2, y1, fill);
        }
    } 
    void bezier(int* arr, uint8_t size, uint8_t dense, uint8_t fill = 1) {
        constexpr uint8_t MAX_POINTS = 8;
        if (size < 2 || size > MAX_POINTS) return;
        if (dense > 10) dense = 10;
        int a[MAX_POINTS * 2];
        const uint16_t steps = 1 << dense;
        for (uint16_t i = 0; i < steps; i++) {
            for (uint8_t j = 0; j < size * 2; j++)
                a[j] = arr[j];
            for (int j = (size - 1) * 2 - 1; j > 0; j -= 2) {
                for (int k = 0; k <= j - 2; k++) {
                    a[k] = a[k] + (((int32_t)(a[k + 2] - a[k]) * i) >> dense);
                }
            }
            dot(a[0], a[1], fill);
        }
    } 
    void drawBitmap(int x, int y, const uint8_t *frame, int width, int height, uint8_t invert = 0, uint8_t mode = 0) {
        _x = 0;
        _y = 0;
        if (invert) invert = 255;
        uint8_t left = height & 0b111;
        if (left != 0) height += (8 - left);    // округляем до ближайшего кратного степени 2
        int shiftY = (y>>3)+(height>>3);        // строка (row) крайнего байта битмапа
        setWindowShift(x, y, width, height);    // выделяем окно
        bool bottom = (_shift != 0 && shiftY >= 0 && shiftY <= _maxRow);    // рисовать ли нижний сдвинутый байт
    
        for (int X = x, countX = 0; X < x+width; X++, countX++) {                        // в пикселях
            uint8_t prevData = 0;
            if (_inRange(X, 0, _maxX)) {                                                // мы внутри дисплея по X
                for (int Y = y>>3, countY = 0; Y < shiftY; Y++, countY++) {                // в строках (пикс/8)                     
                    uint8_t data = frame[countY * width + countX] ^ invert;    // достаём байт
                    if (_shift == 0) {                                                    // без сдвига по Y
                        if (_inRange(Y, 0, _maxRow)) writeData(data, Y, X, mode);        // мы внутри дисплея по Y
                    } else {                                                            // со сдвигом по Y
                        if (_inRange(Y, 0, _maxRow)) writeData((prevData >> (8 - _shift)) | (data << _shift), Y, X, mode);    // задвигаем
                        prevData = data;                        
                    }                    
                }
                if (bottom) writeData(prevData >> (8 - _shift), shiftY, X, mode);        // нижний байт
            }
        }
    }
    void drawByte(uint8_t data) {
        if (++_x > _maxX) return;
        if (_TYPE < 2 || 1) {                            // для SSD1306
            if (_shift == 0) {                            // если вывод без сдвига на строку
                writeData(data);                        // выводим
            } else {                                    // со сдвигом
                writeData(data << _shift);                // верхняя часть
                writeData(data >> (8 - _shift), 1);        // нижняя часть со сдвигом на 1
            }      
        }       
    }
    void drawBytes(uint8_t* data, uint8_t size) {
        for (uint8_t i = 0; i < size; i++) {
            if (++_x > _maxX) return;            
            if (_shift == 0) {                          // если вывод без сдвига на строку
                writeData(data[i]);                     // выводим
            } else {                                    // со сдвигом
                writeData(data[i] << _shift);           // верхняя часть
                writeData(data[i] >> (8 - _shift), 1);  // нижняя часть со сдвигом на 1
            }            
        }
    }
    void setContrast(uint8_t value) {
        uint8_t cmd[] = { OLED_CONTRAST, value };
         sendData(cmd, sizeof(cmd), OLED_COMMAND_MODE);
    }
    void setPower(bool mode) {
        uint8_t cmd = mode ? OLED_DISPLAY_ON : OLED_DISPLAY_OFF;
        sendData(&cmd, 1, OLED_COMMAND_MODE);
    }
    void flipH(bool mode) {
        uint8_t cmd = mode ? OLED_FLIP_H : OLED_NORMAL_H;
        sendData(&cmd, 1, OLED_COMMAND_MODE);
    }
    void flipV(bool mode) {
        uint8_t cmd = mode ? OLED_FLIP_V : OLED_NORMAL_V;
        sendData(&cmd, 1, OLED_COMMAND_MODE);
    }
    void invert(bool mode) {
        uint8_t cmd = mode ? OLED_INVERTDISPLAY : OLED_NORMALDISPLAY;
        sendData(&cmd, 1, OLED_COMMAND_MODE);
    }
    uint8_t getFont(uint8_t font, uint8_t row) {
#ifndef OLED_NO_PRINT
        if (row > 4) return 0;
        font = font - '0' + 16;
        if (font <= 95) {
            return _charMap[font][row];
        } else if (font >= 96 && font <= 111) {
            return _charMap[font + 47][row];
        } else if (font <= 159) {
            return _charMap[font - 17][row];
        } else {
            return _charMap[font - 1][row];
        }
#endif
    }  
    virtual size_t write(uint8_t data) {
#ifndef OLED_NO_PRINT
        // переносы и пределы
        bool newPos = false;        
        if (data == '\r') { _x = 0; newPos = true; data = 0; }                        // получен возврат каретки
        if (data == '\n') { _y += _scaleY; newPos = true; data = 0; _getn = 1; }    // получен перевод строки
        if (_println && (_x + 6*_scaleX) >= _maxX) { _x = 0; _y += _scaleY; newPos = true; }    // строка переполненена, перевод и возврат
        if (newPos) setCursor(_x, _y);                                            // переставляем курсор
        if (_y + _scaleY > _maxY + 1) data = 0;                                        // дисплей переполнен
        if (_getn && _println && data == ' ' && _x == 0) { _getn = 0; data = 0; }   // убираем первый пробел в строке
        
        // фикс русских букв и некоторых символов
        if (data > 127) {
            uint8_t thisData = data;
            // data = 0 - флаг на пропуск
            if (data > 191) data = 0;
            else if (_lastChar == 209 && data == 145) data = 192;   // ё кастомная
            else if (_lastChar == 208 && data == 129) data = 149;    // Е вместо Ё
            else if (_lastChar == 226 && data == 128) data = 0;        // тире вместо длинного тире (начало)
            else if (_lastChar == 128 && data == 148) data = 45;    // тире вместо длинного тире    
            _lastChar = thisData;
        }
        if (data == 0) return 1;    
        // если тут не вылетели - печатаем символ        
        
        if (_TYPE < 2 || 1) {                        // для SSD1306
            int newX = _x + _scaleX * 6;
            if (newX < 0 || _x > _maxX) _x = newX;    // пропускаем вывод "за экраном"
            else {                
                for (uint8_t col = 0; col < 6; col++) {                    // 6 стобиков буквы
                    uint8_t bits = getFont(data, col);                    // получаем байт                    
                    if (_invState) bits = ~bits;                        // инверсия                    
                    if (_scaleX == 1) {                                    // если масштаб 1                        
                        if (_x >= 0 && _x <= _maxX) {                    // внутри дисплея
                            if (_shift == 0) {                            // если вывод без сдвига на строку
                                writeData(bits, 0, 0, _mode);            // выводим
                            } else {                                    // со сдвигом
                                writeData(bits << _shift, 0, 0, _mode);    // верхняя часть
                                writeData(bits >> (8 - _shift), 1, 0, _mode);    // нижняя часть
                            }    
                        }                                    
                    } else {                                            // масштаб 2, 3 или 4 - растягиваем шрифт                        
                        uint32_t newData = 0;                                // буфер
                        for (uint8_t i = 0, count = 0; i < 8; i++)
                        for (uint8_t j = 0; j < _scaleX; j++, count++)                            
                        bitWrite(newData, count, bitRead(bits, i));        // пакуем растянутый шрифт
                        
                        for (uint8_t i = 0; i < _scaleX; i++) {            // выводим. По Х
                            uint8_t prevData = 0;
                            if (_x + i >= 0 && _x + i <= _maxX)         // внутри дисплея
                            for (uint8_t j = 0; j < _scaleX; j++) {        // выводим. По Y
                                uint8_t data = newData >> (j * 8);            // получаем кусок буфера
                                if (_shift == 0) {                        // если вывод без сдвига на строку
                                    writeData(data, j, i, _mode);            // выводим                            
                                } else {                                // со сдвигом
                                    writeData((prevData >> (8 - _shift)) | (data << _shift), j, i, _mode);    // склеиваем и выводим
                                    prevData = data;                    // запоминаем предыдущий
                                }
                            }
                            if (_shift != 0) writeData(prevData >> (8 - _shift), _scaleX, i, _mode);            // выводим нижний кусочек, если со сдвигом
                        }
                    }
                    _x += _scaleX;                                        // двигаемся на ширину пикселя (1-4)
                }
            }
        } else {                        
            // для SSH1106
        }
#endif
        return 1;
    } 
    
    void home() { setCursor(0, 0); }
    void autoPrintln(bool mode) { _println = mode; }
    void invertText(bool inv){ _invState = inv; }
    void textMode(uint8_t mode) { _mode = mode; }
    bool isEnd()       { return (_y > _maxRow); }
    int _bufIndex(int x, int y) {
        return ((y) >> 3) + ((x) << (_TYPE ? 3 : 2));   // _y / 8 + _x * (4 или 8)
    }
    void _swap(int& x, int& y) {
        int z = x;
        x = y;
        y = z;
    }
    bool _inRange(int x, int mi, int ma) {
        return x >= mi && x <= ma;
    }
    const uint8_t _maxRow = (_TYPE ? 8 : 4) - 1;
    const uint8_t _maxY = (_TYPE ? 64 : 32) - 1;
    const uint8_t _maxX = OLED_WIDTH - 1;    // на случай добавления мелких дисплеев   
    constexpr uint8_t constrain8(int val, int min, int max) {
        return static_cast<uint8_t>(val < min ? min : (val > max ? max : val));
    }
private:
    uint8_t buf[128];   // [0]=0x40, [1..128]=data
    uint8_t idx = 1;    // начинаем с 1 (0 — prefix)
    bool _invState = 0;
    bool _println = false;
    bool _getn = false;
    uint8_t _scaleX = 1, _scaleY = 8;
    int _x = 0, _y = 0;
    uint8_t _shift = 0;
    uint8_t _lastChar;
    uint8_t _writes = 0;
    uint8_t _mode = 2;
    int _bufsizeX, _bufsizeY;
    int _bufX, _bufY;
    uint8_t *_buf_ptr;
    bool _buf_flag = false;    
};
#endif








// //FILE oled.h
// #pragma once
// #include <stdbool.h>
// #include <stdint.h>
// #include <string.h>
// #include "stm32g4xx_hal.h"

// #ifndef INC_OLED_H_
// #define INC_OLED_H_

// #define SSD1306_128x32  0
// #define SSD1306_128x64  1
// #define SSH1106_128x64  2
// #define OLED_I2C        0
// #define OLED_SPI        1
// #define OLED_NO_BUFFER  0
// #define OLED_BUFFER     1
// #define OLED_CLEAR      0
// #define OLED_FILL       1
// #define OLED_STROKE     2
// #define BUF_ADD         0
// #define BUF_SUBTRACT    1
// #define BUF_REPLACE     2
// #define BITMAP_NORMAL   0
// #define BITMAP_INVERT   1

// #define OLED_WIDTH      	  128
// #define OLED_HEIGHT_32  	  0x02
// #define OLED_HEIGHT_64 	      0x12
// #define OLED_64               0x3F
// #define OLED_32               0x1F

// #define OLED_DISPLAY_OFF      0xAE
// #define OLED_DISPLAY_ON       0xAF

// #define OLED_COMMAND_MODE     0x00
// #define OLED_ONE_COMMAND_MODE 0x80
// #define OLED_DATA_MODE        0x40
// #define OLED_ONE_DATA_MODE    0xC0

// #define OLED_ADDRESSING_MODE  0x20
// #define OLED_HORIZONTAL       0x00
// #define OLED_VERTICAL         0x01

// #define OLED_SETOFFSET        0xD3
// #define OLED_NORMAL_V         0xC8
// #define OLED_FLIP_V           0xC0
// #define OLED_NORMAL_H         0xA1
// #define OLED_FLIP_H           0xA0

// #define OLED_CONTRAST         0x81
// #define OLED_SETCOMPINS       0xDA
// #define OLED_SETVCOMDETECT    0xDB
// #define OLED_CLOCKDIV         0xD5
// #define OLED_SETMULTIPLEX     0xA8
// #define OLED_COLUMNADDR       0x21
// #define OLED_PAGEADDR         0x22
// #define OLED_CHARGEPUMP       0x8D
// #define OLED_NORMALDISPLAY    0xA6
// #define OLED_INVERTDISPLAY    0xA7
// #define BUFSIZE_128x64        1024
// #define BUFSIZE_128x32 		  512
// #ifndef OLED_SPI_SPEED
// 	#define OLED_SPI_SPEED    1000000ul
// #endif
// #define I2C_MAX_CHUNK 16

// #define constrain(val,min,max) ((val)<(min)?(min):((val)>(max)?(max):(val)))
// #define bitWrite(value, bit, bitvalue)  ( (bitvalue) ? bitSet(value, bit) : bitClear(value, bit) )
// #define bitSet(value, bit)              ( (value) |= (1UL << (bit)) )
// #define bitClear(value, bit)            ( (value) &= ~(1UL << (bit)) )

// static bool _invState = 0;
// static bool _println = false;
// static bool _getn = false;
// static uint8_t _scaleX = 1, _scaleY = 8;
// static int _x = 0, _y = 0;
// static uint8_t _shift = 0;
// static uint8_t _lastChar;
// static uint8_t _writes = 0;
// static uint8_t _mode = 2;

// static int _bufsizeX, _bufsizeY;
// static int _bufX, _bufY;
// static uint8_t *_buf_ptr;
// static bool _buf_flag = false;

// typedef struct SSD_OLED SSD_OLED;
// typedef struct {
//     void (*init)            (struct SSD_OLED *self);
//     void (*update)          (struct SSD_OLED *self);
//     void (*setCursor)       (struct SSD_OLED *self, int x, int y);
//     void (*setCursorXY)     (struct SSD_OLED *self, int x, int y);
//     void (*setWindowShift)  (struct SSD_OLED *self, int x0, int y0, int sizeX, int sizeY);
//     void (*setWindow)       (struct SSD_OLED *self, int x0, int y0, int x1, int y1);
//     void (*dot)             (struct SSD_OLED *self, int x, int y, uint8_t fill);
//     void (*lineV)           (struct SSD_OLED *self, int x, int y0, int y1, uint8_t fill);
//     void (*lineH)           (struct SSD_OLED *self, int x, int y0, int y1, uint8_t fill);
//     void (*rect)			(struct SSD_OLED *self, int x0, int y0, int x1, int y1, uint8_t fill);
// 	void (*clear) 			(struct SSD_OLED *self);
// 	void (*clearXY)         (struct SSD_OLED *self, int x0, int y0, int x1, int y1);
// 	void (*fill)            (struct SSD_OLED *self, uint8_t data);
//     void (*writeData)       (struct SSD_OLED *self, uint8_t data, uint8_t offsetY, uint8_t offsetX, int mode);
//     void (*sendByteRaw)     (struct SSD_OLED *self, uint8_t data);
//     void (*sendCommand)     (struct SSD_OLED *self, uint8_t cmd1);
//     void (*send2Command)    (struct SSD_OLED *self, uint8_t cmd1, uint8_t cmd2);
//     void (*begin1Command)   (struct SSD_OLED *self);
//     void (*beginCommand)    (struct SSD_OLED *self);
//     void (*beginData)       (struct SSD_OLED *self);
//     void (*endTransm)       (struct SSD_OLED *self);
// //    size_t (*write)         (struct SSD_OLED *self, uint8_t data);


// } OLED_Interface;

// void OLED_impl_init	   	 	 (struct SSD_OLED *self);
// void OLED_impl_update		 (struct SSD_OLED *self);
// void OLED_impl_setCursor     (struct SSD_OLED *self, int x, int y);
// void OLED_impl_setCursorXY   (struct SSD_OLED *self, int x, int y);
// void OLED_impl_setWindowShift(struct SSD_OLED *self, int x0, int y0, int sizeX, int sizeY);
// void OLED_impl_setWindow	 (struct SSD_OLED *self, int x0, int y0, int x1, int y1);
// void OLED_impl_dot           (struct SSD_OLED *self, int x, int y, uint8_t fill);
// void OLED_impl_lineV         (struct SSD_OLED *self, int x, int y0, int y1, uint8_t fill);
// void OLED_impl_lineH         (struct SSD_OLED *self, int y, int x0, int x1, uint8_t fill);
// void OLED_impl_rect          (struct SSD_OLED *self, int x0, int y0, int x1, int y1, uint8_t fill);
// void OLED_impl_fill          (struct SSD_OLED *self, uint8_t data);
// void OLED_impl_clear         (struct SSD_OLED *self);
// void OLED_impl_clearXY       (struct SSD_OLED *self, int x0, int y0, int x1, int y1);

// void OLED_impl_writeData     (struct SSD_OLED *self, uint8_t data, uint8_t offsetY, uint8_t offsetX, int mode);
// void OLED_impl_sendByteRaw	 (struct SSD_OLED *self, uint8_t data);
// void OLED_impl_sendCommand	 (struct SSD_OLED *self, uint8_t cmd1);
// void OLED_impl_send2Command	 (struct SSD_OLED *self, uint8_t cmd1, uint8_t cmd2);
// void OLED_impl_begin1Command (struct SSD_OLED *self);
// void OLED_impl_beginCommand	 (struct SSD_OLED *self);
// void OLED_impl_beginData     (struct SSD_OLED *self);
// void OLED_impl_endTransm	 (struct SSD_OLED *self);
// //size_t OLED_impl_write       (struct SSD_OLED *self, uint8_t data);



// bool _inRange                (int x, int mi, int ma);
// void _swap					 (int *x, int *y);
// int _bufIndex                (struct SSD_OLED *self, int x, int y);

// struct SSD_OLED {
//     OLED_Interface *ctrl;
//     I2C_HandleTypeDef* i2c;
//     uint8_t address;
//     int 	_TYPE;
//     int 	_BUFF;
//     int 	_CONN;
//     int8_t  _CS;
//     int8_t  _DC;
//     int8_t  _RST;

//     int     _maxX;
//     int     _maxY;
//     int     _maxRow;
//     int     _bufSize;
//     uint8_t _oled_buffer[BUFSIZE_128x64];
//     int     _x;
//     int     _y;
//     uint8_t txBuf[1 + I2C_MAX_CHUNK];
//     uint8_t txLen;
// };

// static const uint8_t oled_init[] = {
// 	    OLED_DISPLAY_OFF,
// 	    OLED_CLOCKDIV,
// 	    0x80,    // value
// 	    OLED_CHARGEPUMP,
// 	    0x14,    // value
// 		OLED_ADDRESSING_MODE, OLED_HORIZONTAL,
// 	    OLED_NORMAL_H,
// 	    OLED_NORMAL_V,
// 	    OLED_CONTRAST,
// 	    0x7F,    // value
// 	    OLED_SETVCOMDETECT,
// 	    0x40,     // value
// 		OLED_INVERTDISPLAY,
// 	    OLED_DISPLAY_ON,
// };

// extern OLED_Interface OLED_Driver;


// OLED_Interface OLED_Driver = {
//     .init           = OLED_impl_init,
//     .update         = OLED_impl_update,
//     .setCursor      = OLED_impl_setCursor,
//     .setCursorXY    = OLED_impl_setCursorXY,
//     .setWindowShift = OLED_impl_setWindowShift,
//     .setWindow      = OLED_impl_setWindow,
// 	.dot            = OLED_impl_dot,
// //	.write		    = OLED_impl_write,
// 	.lineV		    = OLED_impl_lineV,
// 	.lineH		    = OLED_impl_lineH,
// 	.rect		    = OLED_impl_rect,
// 	.fill		    = OLED_impl_fill,
// 	.clear		    = OLED_impl_clear,
// 	.clearXY		= OLED_impl_clearXY,

// 	.writeData		= OLED_impl_writeData,
//     .sendByteRaw    = OLED_impl_sendByteRaw,
//     .sendCommand    = OLED_impl_sendCommand,
//     .send2Command   = OLED_impl_send2Command,
//     .begin1Command  = OLED_impl_begin1Command,
//     .beginCommand   = OLED_impl_beginCommand,
//     .beginData      = OLED_impl_beginData,
//     .endTransm      = OLED_impl_endTransm,
// };

// void OLED_impl_init(SSD_OLED *self) {
//     self->_maxRow = (self->_TYPE ? 8 : 4) - 1;
//     self->_maxY = (self->_TYPE ? 64 : 32) - 1;
//     self->_maxX = OLED_WIDTH - 1;
//     if (self->_BUFF) {
//         self->_bufSize = self->_TYPE ? BUFSIZE_128x64 : BUFSIZE_128x32;
//         memset(self->_oled_buffer, 0, self->_bufSize);
//     } else {
//         self->_bufSize = 0;
//     }
//     OLED_impl_beginCommand(self);
//     for (uint8_t i = 0; i < sizeof(oled_init); i++) {
//         OLED_impl_sendByteRaw(self, oled_init[i]);
//     }
//     OLED_impl_endTransm(self);
//     OLED_impl_setCursor(self, 0,0);
//     if (self->_BUFF) OLED_impl_setWindow(self, 0, 0, self->_maxX, self->_maxRow);
// }
// void OLED_impl_update(struct SSD_OLED *self) {
//     if (self->_BUFF) {
//         if (self->_TYPE < 2) {    // для 1306
//         	OLED_impl_setWindow(self, 0, 0, self->_maxX, self->_maxRow);
//         	OLED_impl_beginData(self);
//             //if (_CONN) SPI.transfer(_oled_buffer, _TYPE ? 1024 : 512);
//             if (self->_CONN) for (int i = 0; i < (self->_TYPE ? 1024 : 512); i++){}// SPI.transfer(_oled_buffer[i]);
//             else for (int i = 0; i < (self->_TYPE ? 1024 : 512); i++) OLED_impl_sendByteRaw(self, self->_oled_buffer[i]);
//             OLED_impl_endTransm(self);
//         } else {            // для 1106
//         	OLED_impl_sendCommand(self,0x00);
//         	OLED_impl_sendCommand(self,0x10);
//         	OLED_impl_sendCommand(self,0x40);
//             uint16_t ptr = 0;
//             for (uint8_t i = 0; i < (64 >> 3); i++) {
//             	OLED_impl_sendCommand(self, 0xB0 + i + 0);    //set page address
//             	OLED_impl_sendCommand(self, 2 & 0xf);        //set lower column address
//             	OLED_impl_sendCommand(self, 0x10);            //set higher column address
//                 for (uint8_t a = 0; a < 8; a++) {
//                 	OLED_impl_beginData(self);
//                     for (uint8_t b = 0; b < (OLED_WIDTH >> 3); b++) {
//                     	OLED_impl_sendByteRaw(self, self->_oled_buffer[((ptr&0x7F)<<3)+(ptr>>7)]);
//                         ptr++;
//                     }
//                     OLED_impl_endTransm(self);
//                 }
//             }
//         }
//     }
// }
// void OLED_impl_setCursor(struct SSD_OLED *self, int x, int y) {
//     OLED_impl_setCursorXY(self, x, y << 3);
// }
// void OLED_impl_setCursorXY(struct SSD_OLED *self, int x, int y) {
//     self->_x = x; self->_y = y;
//     OLED_impl_setWindowShift(self, x, y, self->_maxX, _scaleY);
// }
// void OLED_impl_setWindowShift(struct SSD_OLED *self, int x0, int y0, int sizeX, int sizeY) {
//     if (!self->_BUFF) {
//         OLED_impl_setWindow(self,x0,(y0 >> 3),x0 + sizeX,((y0 + sizeY - 1) >> 3));
//     }
// }
// void OLED_impl_setWindow(struct SSD_OLED *self, int x0, int y0, int x1, int y1) {
//     OLED_impl_beginCommand(self);
//       OLED_impl_sendByteRaw(self, OLED_COLUMNADDR);
//       OLED_impl_sendByteRaw(self, constrain(x0,0,self->_maxX));
//       OLED_impl_sendByteRaw(self, constrain(x1,0,self->_maxX));
//       OLED_impl_sendByteRaw(self, OLED_PAGEADDR);
//       OLED_impl_sendByteRaw(self, constrain(y0,0,self->_maxRow));
//       OLED_impl_sendByteRaw(self, constrain(y1,0,self->_maxRow));
//     OLED_impl_endTransm(self);
// }
// void OLED_impl_dot(struct SSD_OLED *self, int x, int y, uint8_t fill) {
//     if (x < 0 || x > self->_maxX || y < 0 || y > self->_maxY) return;
//     self->_x = 0;
//     self->_y = 0;
//     if (self->_BUFF) {
//         bitWrite(self->_oled_buffer[_bufIndex(self, x, y)], y & 0b111, fill);
//     } else {
//         // без буфера — отправляем сразу на дисплей
//         if (!_buf_flag) {
//             OLED_impl_setWindow(self, x, y>>3,self->_maxX,self->_maxRow);
//             OLED_impl_beginData(self);
//             OLED_impl_sendByteRaw(self, 1 << (y & 0b111));
//             OLED_impl_endTransm(self);
//         } else { // mini-буфер: аналогично записываем в _buf_ptr
//             x -= _bufX;
//             y -= _bufY;
//             if (x < 0 || x > _bufsizeX || y < 0 || y > (_bufsizeY << 3)) return;
//             bitWrite(_buf_ptr[ (y>>3) + x * _bufsizeY ], y & 0b111, fill);
//         }
//     }
// }
// //size_t OLED_impl_write(SSD_OLED *self, uint8_t data) {}

// void OLED_impl_lineV(struct SSD_OLED *self, int x, int y0, int y1, uint8_t fill) {
//     // сброс «курсорной позиции»
//     self->_x = self->_y = 0;

//     if (y0 > y1) _swap(&y0, &y1);
//     if (x < 0 || x > self->_maxX) return;

//     y1++;
//     if (self->_BUFF) {
//         // в буфере просто точечно
//         for (int y = y0; y < y1; y++)
//             OLED_impl_dot(self, x, y, fill);
//         return;
//     }

//     // без буфера сразу на экран
//     if (!_buf_flag) {
//         // конвертим fill=1→0xFF, fill=0→0x00
//         uint8_t byteFill = fill ? 0xFF : 0x00;
//         uint8_t shift  =  y0 & 0x07;
//         uint8_t shift2 = (8 - (y1 & 0x07)) & 0x07;
//         int height     = y1 - y0;
//         int page0      = y0 >> 3;
//         int page1      = (y1 - 1) >> 3;
//         uint8_t pages  = page1 - page0;

//         // окно по вертикали
//         self->ctrl->beginCommand(self);
//         OLED_impl_sendByteRaw(self, OLED_COLUMNADDR);
//         OLED_impl_sendByteRaw(self, x);
//         OLED_impl_sendByteRaw(self, x);
//         OLED_impl_sendByteRaw(self, OLED_PAGEADDR);
//         OLED_impl_sendByteRaw(self, page0);
//         OLED_impl_sendByteRaw(self, page1);
//         self->ctrl->endTransm(self);

//         // начинаем передавать данные
//         self->ctrl->beginData(self);

//         if (pages == 0) {
//             // всё в одном байте
//             uint8_t mask = (byteFill >> (8 - height)) << shift;
//             OLED_impl_sendByteRaw(self, mask);
//         } else {
//             // первый кусок
//             OLED_impl_sendByteRaw(self, byteFill << shift);
//             // сплошные страницы
//             for (int p = 0; p < pages - 1; p++)
//                 OLED_impl_sendByteRaw(self, byteFill);
//             // последний кусок
//             OLED_impl_sendByteRaw(self, byteFill >> shift2);
//         }
//         self->ctrl->endTransm(self);
//     }
// }
// void OLED_impl_lineH(struct SSD_OLED *self, int y, int x0, int x1, uint8_t fill) {
//     self->_x = self->_y = 0;

//     if (x0 > x1) _swap(&x0, &x1);
//     if (y < 0 || y > self->_maxY) return;

//     x1++;
//     if (self->_BUFF) {
//         for (int x = x0; x < x1; x++)
//             OLED_impl_dot(self, x, y, fill);
//         return;
//     }

//     if (!_buf_flag) {
//         uint8_t mask = (1 << (y & 0x07));
//         uint8_t page = y >> 3;

//         // окно: одна страница, от x0 до x1-1
//         self->ctrl->beginCommand(self);
//         OLED_impl_sendByteRaw(self, OLED_COLUMNADDR);
//         OLED_impl_sendByteRaw(self, x0);
//         OLED_impl_sendByteRaw(self, x1 - 1);
//         OLED_impl_sendByteRaw(self, OLED_PAGEADDR);
//         OLED_impl_sendByteRaw(self, page);
//         OLED_impl_sendByteRaw(self, page);
//         self->ctrl->endTransm(self);

//         // и данные
//         self->ctrl->beginData(self);
//         for (int x = x0; x < x1; x++)
//             OLED_impl_sendByteRaw(self, mask);
//         self->ctrl->endTransm(self);
//     }
// }
// void OLED_impl_rect(struct SSD_OLED *self, int x0, int y0, int x1, int y1, uint8_t fill) {
//     // обнуляем «курсор»
//     self->_x = self->_y = 0;

//     if (x0 > x1) _swap(&x0, &x1);
//     if (y0 > y1) _swap(&y0, &y1);

//     if (fill == OLED_STROKE) {
//         OLED_impl_lineH(self, y0, x0 + 1, x1 - 1, 1);
//         OLED_impl_lineH(self, y1, x0 + 1, x1 - 1, 1);
//         OLED_impl_lineV(self, x0, y0, y1, 1);
//         OLED_impl_lineV(self, x1, y0, y1, 1);
//     } else {
//         // сплошной или очистка
//         if (!self->_BUFF && fill == OLED_CLEAR) {
//             OLED_impl_clearXY(self, x0, y0, x1, y1);
//             return;
//         }
//         for (int x = x0; x <= x1; x++)
//             OLED_impl_lineV(self, x, y0, y1, fill);
//     }
// }
// void OLED_impl_fill(struct SSD_OLED *self, uint8_t data) {
//     if (self->_BUFF) {
//         memset(self->_oled_buffer, data, self->_bufSize);
//     } else { //for SSD1306
//     	int px = self->_TYPE ? 1024 : 512;
//     	if (self->_TYPE < 1) {
//     		OLED_impl_setWindow(self, 0, 0, self->_maxX, self->_maxRow);
//     		OLED_impl_beginData(self);
//     		for (int i = 0; i < px; i++) {
//     			OLED_impl_sendByteRaw(self, data);
//     		}
//     		OLED_impl_endTransm(self);
//     	}   //for SSH1108
//     }
//     OLED_impl_setCursorXY(self, self->_x, self->_y);
// }

// void OLED_impl_clear(struct SSD_OLED *self) {
//     OLED_impl_fill(self, 0);
// }
// void OLED_impl_clearXY(struct SSD_OLED *self, int x0, int y0, int x1, int y1)
// {
//     if (!self->_BUFF) {
//         // если без буфера, делаем прямоугольник с fill=0
//         OLED_impl_rect(self, x0, y0, x1, y1, OLED_CLEAR);
//     } else {
//         // в буфере — просто сбросим нужные биты
//         for (int x = x0; x <= x1; x++)
//             for (int y = y0; y <= y1; y++) {
//                 int idx = _bufIndex(self, x, y);
//                 self->_oled_buffer[idx] &= ~(1 << (y & 7));
//             }
//     }
// }

// void OLED_impl_writeData(SSD_OLED *self,
//                          uint8_t data,
//                          uint8_t offsetY,
//                          uint8_t offsetX,
//                          int mode)
// {
//     if (self->_BUFF) {
//         // 0-add, 1-subtract, 2-replace
//         int idx = _bufIndex(self, self->_x + offsetX, self->_y) + offsetY;
//         switch (mode) {
//             case BUF_ADD:      self->_oled_buffer[idx] |=  data; break;
//             case BUF_SUBTRACT: self->_oled_buffer[idx] &= ~data; break;
//             case BUF_REPLACE:  self->_oled_buffer[idx]  =  data; break;
//         }
//     } else {
//         // сразу в txBuf / I2C
//         OLED_impl_sendByteRaw(self, data);
//     }
// }
// void OLED_impl_sendByteRaw(struct SSD_OLED *self, uint8_t data) {
//     self->txBuf[self->txLen++] = data;
//     if (self->txLen ==  I2C_MAX_CHUNK + 1) { //send each 16 bytes
//         HAL_I2C_Master_Transmit(self->i2c,self->address,self->txBuf,self->txLen,HAL_MAX_DELAY);
//         self->txBuf[0] = OLED_DATA_MODE;
//         self->txLen    = 1;
//     }
// }
// void OLED_impl_sendCommand(struct SSD_OLED *self, uint8_t cmd1) {
//     OLED_impl_begin1Command(self);
//     OLED_impl_sendByteRaw(self, cmd1);
//     OLED_impl_endTransm(self);
// }
// void OLED_impl_send2Command(struct SSD_OLED *self, uint8_t cmd1, uint8_t cmd2) {
//     OLED_impl_beginCommand(self);
//     OLED_impl_sendByteRaw(self, cmd1);
//     OLED_impl_sendByteRaw(self, cmd2);
//     OLED_impl_endTransm(self);
// }
// void OLED_impl_begin1Command(struct SSD_OLED *self) {
//     self->txBuf[0] = OLED_ONE_COMMAND_MODE;
//     self->txLen    = 1;
// }
// void OLED_impl_beginCommand(struct SSD_OLED *self) {
//     self->txBuf[0] = OLED_COMMAND_MODE;
//     self->txLen    = 1;
// }
// void OLED_impl_beginData(struct SSD_OLED *self) {
//     self->txBuf[0] = OLED_DATA_MODE;
//     self->txLen    = 1;
// }
// void OLED_impl_endTransm(struct SSD_OLED *self) {
//     if (self->txLen > 0) {
//         HAL_I2C_Master_Transmit(self->i2c,self->address,self->txBuf,self->txLen,HAL_MAX_DELAY);
//     }
//     self->txLen = 0;
// }




// int _bufIndex(struct SSD_OLED *self, int x, int y) {
//     return ((y) >> 3) + ((x) << (self->_TYPE ? 3 : 2));
// }
// void _swap(int *x, int *y) {
//     int tmp = *x;
//     *x = *y;
//     *y = tmp;
// }
// bool _inRange(int x, int mi, int ma) {
//     return x >= mi && x <= ma;
// }

// #endif /* INC_OLED_H_ */
