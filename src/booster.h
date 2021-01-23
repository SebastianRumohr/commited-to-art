#ifndef BOOSTER_H
#define BOOSTER_H

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>

extern void booster_setup(uint16_t ledCount, uint8_t resetPin, uint8_t csPin);
extern void booster_show();
extern void booster_setled(uint8_t n);
extern void booster_setall(void);
extern void booster_setrgb(uint8_t r, uint8_t g, uint8_t b);
extern void booster_sethsv(uint16_t h, uint8_t s, uint8_t v);
extern void booster_shiftup(uint8_t beg, uint8_t ende, uint8_t cnt);
extern void booster_shiftdown(uint8_t beg, uint8_t ende, uint8_t cnt);
extern void booster_setrainbow(uint16_t h, uint8_t s, uint8_t v, uint8_t beg, uint8_t ende, uint8_t inc);
extern void booster_send(void);

#endif