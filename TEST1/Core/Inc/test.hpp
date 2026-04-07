#pragma once
#include <stdint.h>
template<typename... Args>
class Signal {
public:
    using Slot = void(*)(void*, Args...);
    struct Handler {
        Slot fn = nullptr;
        void* ctx = nullptr;
    };
    void connect(Slot fn, void* ctx = nullptr) {
        if(count < MAX_SLOTS){
            slots[count++] = {fn, ctx};
        }
    }
    void emit(Args... args) {
        for(uint8_t i = 0; i < count; i++){
            if(slots[i].fn){
                slots[i].fn(slots[i].ctx, args...);
            }
        }
    }
private:
    static constexpr uint8_t MAX_SLOTS = 4; // можно менять
    Handler slots[MAX_SLOTS];
    uint8_t count = 0;
};

class VirtButtonSignal {
public:
    Signal<> pressed;
    Signal<> released;
    Signal<> clicked;
    Signal<> held;
    Signal<uint8_t> clicksSignal;

    void poll(bool state) {
        if(state && !pressedFlag){
            pressedFlag = true;
            pressed.emit();
        } 
        if(!state && pressedFlag){
            pressedFlag = false;
            released.emit();
            clicked.emit();
        }
    }

private:
    bool pressedFlag = false;
};