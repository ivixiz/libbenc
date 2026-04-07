 
# LibBEnc – Encoder & Button Control Library
**LibBEnc** is a single-file, header-only library for handling rotary encoders and buttons with advanced features, including interrupt support, debouncing, multiple click detection, step events, and timeout management. It is optimized for **STM32**, **AVR/Arduino**, **ESP8266**, and **ESP32** platforms.
---
STM32 support extended and based on [GitHub](https://github.com/GyverLibs/EncButton)
More detailed instructions see there.

## Added
* Everything placed in single file, gio_shift is optional.
* Objects could be declared using template (EncButtonT) or constructor (EncButton) (if you want to reassign pins dynamically)
* Doxygen function descriptions
* step() function that detects speed pf encoder to make increment value dependent on turn speed.
There are different levels of increment multipliers that you can adjust.
newwal += encoder.step(), faster you move encoder, greater the increment.
* pollEnc() was changed , and XOR's are replaced by bitwise shift.

## Note
* Tested for STM32G474 and ATmega328P

## Features
* **Rotary Encoder Support**
  * Detects rotation direction (normal/reversed)
  * Supports 1-, 2-, and 4-step encoders
  * ISR and polling-based reading
  * Fast step multipliers for accelerated turns
* **Button Support**
  * Debounce handling
  * Single and multiple click detection
  * Hold and step detection
  * Timeout events
  * Virtual buttons combining multiple physical buttons
* **Flexible API**
  * Template-based initialization or runtime constructor
  * Callback support via `std::function` or function pointers
  * Configurable timeouts for press, hold, step, click, and overall events
* **Cross-platform performance**
  * For AVR, STM32, ESP8266, ESP32 (including ESP32C3)
---

## Installation
This library is **header-only**, so simply include the file:
```cpp
#include <libbenc.hpp>
```
Requires **C++20 for STM** or newer for template-based usage.

---
## Usage

### STM32 Example

```cpp
// Using template class (STM32G474 tested)
EncButtonT<
    Pin{GPIOB_BASE, GPIO_PIN_4},
    Pin{GPIOB_BASE, GPIO_PIN_5},
    Pin{GPIOB_BASE, GPIO_PIN_10},
    PinMode{GPIO_PULLUP, GPIO_MODE_IT_RISING_FALLING},
    PinMode{GPIO_PULLDOWN, GPIO_MODE_IT_RISING_FALLING}
> enc;

int main() {
    // Initialize after MX_GPIO_Init();
    enc.init();
    enc.setEncISR(true);

    int i = 0;
    while(1) {
        enc.tick();
        if(enc.left()) i += enc.step(i);
        else if(enc.right()) i -= enc.step(i);
    }
}

// In HAL GPIO EXTI callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == GPIO_PIN_4 || GPIO_Pin == GPIO_PIN_5 || GPIO_Pin == GPIO_PIN_10) {
        enc.tickISR();
    }
}
```
### STM Example
```cpp
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
```

### Arduino AVR Example

```cpp
#define FASTSTEPS {1, 10, 20, 50, 100} //define your own multipliers of fast turn steps
#include <libbenc.hpp>
//EncButton eb(EN_CLK,EN_DAT,EN_SW,INPUT,INPUT,AUTO);// constructor

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

```

---

## API Overview
### Button (`VirtButton`)
* **Events:** `press()`, `click()`, `hold()`, `step()`, `release()`, `releaseHold()`, `releaseStep()`, `releaseHoldStep()`, `timeout()`
* **States:** `pressing()`, `holding()`, `waiting()`, `busy()`
* **Configuration:** `setHoldTimeout()`, `setStepTimeout()`, `setClickTimeout()`, `setDebTimeout()`, `setTimeout()`, `setBtnLevel()`
* **Callback:** `attach(callback)`, `detach()`
* Also multiple button press in MultiButton class.
### EncoderButton (`VirtEncoder`)
* **Events:** `turn()`,`turnH()`,`right()`,`left()`,`rightH()`,`leftH()`
* **States:** `dir()`,`step()`,`fast()`,`Hold()`
* **Configuration:** `setEncReverse()`, `setEncType()`, `setEncISR()`, `initEnc()`, `clear()`




