#include "booster.h"
#include <Arduino.h>
#include <SPI.h>

//====================================================================
// LED-BOOSTER COMMAND-TABLE
//====================================================================
#define BOOSTER_SETRGB 0xA1
#define BOOSTER_SETRGBW 0xA2
#define BOOSTER_SETHSV 0xA3
#define BOOSTER_SETLED 0xA4
#define BOOSTER_SETALL 0xA5
#define BOOSTER_SETRANGE 0xA6
#define BOOSTER_SETRAINBOW 0xA7
#define BOOSTER_GRADIENT 0xA8

#define BOOSTER_INIT 0xB1
#define BOOSTER_SHOW 0xB2
#define BOOSTER_SHIFTUP 0xB3
#define BOOSTER_SHIFTDOWN 0xB4
#define BOOSTER_COPYLED 0xB5
#define BOOSTER_REPEAT 0xB6

#define BOOSTER_RGBORDER 0xC1
//====================================================================
// LOCAL BUFFER
//====================================================================
uint8_t buf[256];
uint8_t pos = 0;
//====================================================================

//====================================================================
static uint8_t _ledCount = 64; // <<< CHANGE THE LED-COUNT 2..256
//====================================================================
static uint8_t _resetPin = 9; // SLAVE RESET
static uint8_t _csPin = 10;   // SLAVE SELECT

static void AddByte(uint8_t n)
{
    buf[pos++] = n;
}
static void AddEnd(uint8_t n)
{
    buf[pos++] = n;
    if (pos >= (256 - 16))
    {
        booster_send();
        delay(1);
    }
}
//====================================================================
void booster_send(void)
{
    if (pos == 0)
    {
        return;
    }

    digitalWrite(_csPin, LOW);
    for (int i = 0; i < pos; i++)
    {
        SPI.transfer(buf[i]);
    }
    digitalWrite(_csPin, HIGH);
    pos = 0;
}
//====================================================================
// booster_init(count, bits)
// count = 1..256, bits = 24 | 32
//====================================================================
void booster_init(uint16_t cnt, uint8_t bits)
{
    AddByte(BOOSTER_INIT);
    if (cnt >= 256)
    {
        cnt = 0;
    }
    AddByte(cnt);
    if ((bits != 24) && (bits != 32))
    {
        bits = 24;
    }
    AddEnd(bits);
}

//====================================================================
// booster_rgborder(r, g, b)
// WS2812: r = 2, g = 3, b = 1
// OTHER:  r = 3, g = 2, b = 1
//====================================================================
void booster_rgborder(uint8_t r, uint8_t g, uint8_t b)
{
    AddByte(BOOSTER_RGBORDER);
    AddByte(r);
    AddByte(g);
    AddEnd(b);
}

void booster_setup(uint16_t ledCount, uint8_t resetPin, uint8_t csPin)
{
    _ledCount = ledCount;
    _resetPin = resetPin;
    _csPin = csPin;

    pinMode(_csPin, OUTPUT);
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4);
    digitalWrite(_resetPin, LOW);
    delay(100);
    digitalWrite(_resetPin, HIGH);
    delay(100);

    booster_init(_ledCount, 24);
    booster_rgborder(2, 3, 1);
    booster_send();
    delay(50);
}

//====================================================================
// booster_show() + Send Data
//====================================================================
void booster_show(void)
{
    AddEnd(BOOSTER_SHOW);
    booster_send();
}
//====================================================================
// booster_setall()
//====================================================================
void booster_setall(void)
{
    AddEnd(BOOSTER_SETALL);
}
//====================================================================
// booster_setrange(beg, end)
// beg, end = 0..255 (end >= beg)
//====================================================================
void booster_setrange(uint8_t beg, uint8_t ende)
{
    AddByte(BOOSTER_SETRANGE);
    AddByte(beg);
    AddEnd(ende);
}
//====================================================================
// booster_setled(n)
// n = 0..255
//====================================================================
void booster_setled(uint8_t n)
{
    AddByte(BOOSTER_SETLED);
    AddEnd(n);
}
//====================================================================
// booster_setrgb(r, g, b)
// r, g, b = 0..255
//====================================================================
void booster_setrgb(uint8_t r, uint8_t g, uint8_t b)
{
    AddByte(BOOSTER_SETRGB);
    AddByte(r);
    AddByte(g);
    AddEnd(b);
}
//====================================================================
// booster_setrgbw(r, g, b, w)
// r, g, b, w = 0..255
//====================================================================
void booster_setrgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
    AddByte(BOOSTER_SETRGBW);
    AddByte(r);
    AddByte(g);
    AddByte(b);
    AddEnd(w);
}
//====================================================================
// booster_sethsv(h, s, v)
// h = 0..359, s = 0..255, v = 0..255
//====================================================================
void booster_sethsv(uint16_t h, uint8_t s, uint8_t v)
{
    AddByte(BOOSTER_SETHSV);
    AddByte(h & 0xFF);
    AddByte(h >> 8);
    AddByte(s);
    AddEnd(v);
}
//====================================================================
// booster_setrainbow(h, s, v, beg, end, inc)
// h = 0..359, s = 0..255, v = 0..255,
// beg = 0..255, end = 0..255 (end >= beg)
// inc = 1..255
//====================================================================
void booster_setrainbow(uint16_t h, uint8_t s, uint8_t v, uint8_t beg, uint8_t ende, uint8_t inc)
{
    AddByte(BOOSTER_SETRAINBOW);
    AddByte(h & 0xFF);
    AddByte(h >> 8);
    AddByte(s);
    AddByte(v);
    AddByte(beg);
    AddByte(ende);
    AddEnd(inc);
}
//====================================================================
// booster_shiftup(beg, end, cnt)
// beg = 0..255, end = 0..255 (end >= beg)
// cnt = 1..255
//====================================================================
void booster_shiftup(uint8_t beg, uint8_t ende, uint8_t cnt)
{
    AddByte(BOOSTER_SHIFTUP);
    AddByte(beg);
    AddByte(ende);
    AddEnd(cnt);
}
//====================================================================
// booster_shiftdown(beg, end, cnt)
// beg = 0..255, end = 0..255 (end >= beg)
// cnt = 1..255
//====================================================================
void booster_shiftdown(uint8_t beg, uint8_t ende, uint8_t cnt)
{
    AddByte(BOOSTER_SHIFTDOWN);
    AddByte(beg);
    AddByte(ende);
    AddEnd(cnt);
}
//====================================================================
// booster_copyled(src, dest)
// src = 0..255, dest = 0..255
//====================================================================
void booster_copyled(uint8_t src, uint8_t dest)
{
    AddByte(BOOSTER_COPYLED);
    AddByte(src);
    AddEnd(dest);
}
//====================================================================
// booster_repeat(beg, end, count)
// beg = 0..255, end = 0..255 (end >= beg)
// cnt = 1..255
//====================================================================
void booster_repeat(uint8_t beg, uint8_t ende, uint8_t cnt)
{
    AddByte(BOOSTER_REPEAT);
    AddByte(beg);
    AddByte(ende);
    AddEnd(cnt);
}
