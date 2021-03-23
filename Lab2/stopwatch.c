#include "address_map_arm.h"

#define INTERVAL 0xF4240

typedef struct _interval_timer {
	int status;
	int control;
	int low_period;
	int high_period;
	int low_counter;
	int high_counter;
} interval_timer;

int tick = 0;
int lapTick = 0;

int readSwitches(volatile int *input);
void displayTime(int time, volatile int *fourDigitDisplay, volatile int *twoDigitDisplay);
void startTimer(volatile interval_timer *timer);
int readButton(volatile int *input);
void setInterval(int interval, volatile interval_timer *timer);
void clear(volatile int *fourDigitDisplay, volatile int *twoDigitDisplay);

int main(void) {
    volatile int *buttons = (int *) KEY_BASE;
    volatile int *switches = (int *) SW_BASE;
    volatile int *fourDigitDisplay = (int *) HEX3_HEX0_BASE;
    volatile int *twoDigitDisplay = (int *) HEX5_HEX4_BASE;
    volatile interval_timer *timer = (interval_timer *) TIMER_BASE;

    int const interval = INTERVAL;
    clear(fourDigitDisplay, twoDigitDisplay);

    setInterval(interval, timer);


    while(1) {
        if (readButton(buttons) == 0b0001) {
            while (readButton(buttons) != 0b0010) {
                startTimer(timer);
                if (timer->status == 0b11) {
                    timer->status = 0b10;
                    tick++;
                    displayTime(tick, fourDigitDisplay, twoDigitDisplay);
                }
                else if (readButton(buttons) == 0b1000) {
                    clear(fourDigitDisplay, twoDigitDisplay);
                    break;
                }
                else if (readButton(buttons) == 0b0100) {
                    lapTick = tick;
                }

                else if (readSwitches(switches) == 0b1) {
                    displayTime(lapTick, fourDigitDisplay, twoDigitDisplay);
                    
                }
            }
            stopTimer(timer);
        }
        else if (readButton(buttons) == 0b1000) {
            clear(fourDigitDisplay, twoDigitDisplay);
        }
        else if (readButton(buttons) == 0b0100) {
            lapTick = tick;
        }
        else if (readSwitches(switches) == 0b1) {
            displayTime(lapTick, fourDigitDisplay, twoDigitDisplay);
            stopTimer(timer);
        }
    } 
}

int readButton(volatile int *input) {
    return *input;
}

void displayTime(int time, volatile int *fourDigitDisplay, volatile int *twoDigitDisplay) {
    const int lookUpTable[10] = {0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x7, 0x7F, 0x67};
    int hundreds = time;
    int seconds = time / 100;
    seconds = seconds % 60;
    int minutes = time / 6000;
    minutes = minutes % 60;
    int h1 = hundreds % 10;
    int h2 = (hundreds / 10) % 10;
    int s1 = seconds % 10;
    int s2 = (seconds / 10) % 10;
    int m1 = minutes % 10;
    int m2 = (minutes / 10) % 10;
    *twoDigitDisplay = (lookUpTable[m2] * 0x100) + lookUpTable[m1];
    *fourDigitDisplay = (lookUpTable[s2] * 0x1000000) + (lookUpTable[s1] * 0x10000) + (lookUpTable[h2] * 0x100) + lookUpTable[h1];

}

void startTimer(volatile interval_timer *timer) {
    timer->control = 0b0100;
}

void stopTimer(volatile interval_timer *timer) {
    timer->control = 0b1000;
}

void setInterval(int interval, volatile interval_timer *timer) {
    
    timer->low_period = interval;
	timer->high_period = interval >> 16;
}

void clear(volatile int *fourDigitDisplay, volatile int *twoDigitDisplay) {
    *fourDigitDisplay = 0x3F + 0x3F * 0x100 + 0x3F * 0x10000 + 0x3F * 0x1000000;
    *twoDigitDisplay = 0x3F + 0x3F * 0x100;
    tick = 0;
}

int readSwitches(volatile int *input) {
    return *input;
}


