#include <SPI.h>
//====================================================================
#define LED_COUNT            64     // <<< CHANGE THE LED-COUNT 2..256
//====================================================================
#define RESET_PIN            9      // PIN9  = SLAVE RESET
#define CS_PIN               10     // PIN10 = SLAVE SELECT
//====================================================================
// LED-BOOSTER COMMAND-TABLE
//====================================================================
#define BOOSTER_SETRGB       0xA1
#define BOOSTER_SETRGBW      0xA2
#define BOOSTER_SETHSV       0xA3
#define BOOSTER_SETLED       0xA4
#define BOOSTER_SETALL       0xA5
#define BOOSTER_SETRANGE     0xA6
#define BOOSTER_SETRAINBOW   0xA7
#define BOOSTER_GRADIENT     0xA8

#define BOOSTER_INIT         0xB1
#define BOOSTER_SHOW         0xB2
#define BOOSTER_SHIFTUP      0xB3
#define BOOSTER_SHIFTDOWN    0xB4
#define BOOSTER_COPYLED      0xB5
#define BOOSTER_REPEAT       0xB6

#define BOOSTER_RGBORDER     0xC1
//====================================================================
// LOCAL BUFFER
//====================================================================
uint8_t buf[256];
uint8_t pos = 0;
//====================================================================
static void AddByte(u8 n) { buf[pos++] = n; }
static void AddEnd(u8 n)  { 
  buf[pos++] = n;
  if(pos >= (256 - 16)) { booster_send(); delay(1); }
}
//====================================================================
void booster_send(void)
{
  if(pos == 0) { return; }
  digitalWrite(CS_PIN, LOW);
    for(int i = 0; i < pos; i++) {
    SPI.transfer(buf[i]);
  }  
  digitalWrite(CS_PIN, HIGH);
  pos = 0;
}
//====================================================================
// booster_init(count, bits)
// count = 1..256, bits = 24 | 32
//====================================================================
void booster_init(u16 cnt, u8 bits)
{
  AddByte(BOOSTER_INIT);
  if(cnt >= 256) { cnt = 0; }
  AddByte(cnt);
  if((bits != 24) && (bits !=32)) { bits = 24; }
  AddEnd(bits); 
}
//====================================================================
// booster_rgborder(r, g, b)
// WS2812: r = 2, g = 3, b = 1 
// OTHER:  r = 3, g = 2, b = 1 
//====================================================================
void booster_rgborder(u8 r, u8 g, u8 b)
{
  AddByte(BOOSTER_RGBORDER);
  AddByte(r);
  AddByte(g);
  AddEnd(b);
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
void booster_setrange(u8 beg, u8 ende)
{
  AddByte(BOOSTER_SETRANGE);
  AddByte(beg);
  AddEnd(ende);
}
//====================================================================
// booster_setled(n)
// n = 0..255
//====================================================================
void booster_setled(u8 n)
{
  AddByte(BOOSTER_SETLED);
  AddEnd(n);
}
//====================================================================
// booster_setrgb(r, g, b)
// r, g, b = 0..255
//====================================================================
void booster_setrgb(u8 r, u8 g, u8 b)
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
void booster_setrgbw(u8 r, u8 g, u8 b, u8 w)
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
void booster_sethsv(u16 h, u8 s, u8 v)
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
void booster_setrainbow(u16 h, u8 s, u8 v, u8 beg, u8 ende, u8 inc)
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
void booster_shiftup(u8 beg, u8 ende, u8 cnt)
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
void booster_shiftdown(u8 beg, u8 ende, u8 cnt)
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
void booster_copyled(u8 src, u8 dest)
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
void booster_repeat(u8 beg, u8 ende, u8 cnt)
{
  AddByte(BOOSTER_REPEAT);
  AddByte(beg);
  AddByte(ende);
  AddEnd(cnt);
}
//====================================================================
void setup() {
  Serial.begin(9600);        // start serial for output
  Serial.println("Startup Test");   
  pinMode (CS_PIN, OUTPUT);  
  pinMode (RESET_PIN, OUTPUT);  
  digitalWrite(CS_PIN, HIGH);
  SPI.begin();     
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  digitalWrite(RESET_PIN, LOW);
  delay(100);
  digitalWrite(RESET_PIN, HIGH);
  delay(100);
  
  booster_init(LED_COUNT, 24);
  booster_rgborder(2, 3, 1);
  booster_send();
  delay(50);
}
//====================================================================
void Demo1(void)
{
  delay(500);
  booster_setrgb(128, 0, 0);
  booster_setall();
  booster_show();

  delay(500);   
  booster_sethsv(60, 255, 128);
  booster_setall();
  booster_show();

  delay(500);   
  booster_sethsv(240, 255, 128);
  booster_setrange(3, 8);
  booster_sethsv(120, 255, 128);
  booster_setrange(16, 24);
  booster_show();
}
//====================================================================
void Demo2(void)
{
  static u16 hue = 0;
  static int start = 0; 
  
  delay(40);   
  booster_setrainbow(hue, 255, 128, 0, LED_COUNT - 1, 10);
  hue = (hue + 5) % 360;
  booster_show();   
}

//====================================================================
void Demo3(void)
{
  static int start = 0; 

  if(start == 0) {
    booster_sethsv(330,255,128);
    booster_setall();
    booster_setrgb(100,100,0);
    booster_setled(0);
    booster_show();
    start++;
  } else {
    booster_shiftup(0, LED_COUNT - 1, 1);
    booster_setrgb(0, 0, 128);
    booster_setled(0);
    booster_show();
    if(++start >= LED_COUNT) { start = 0; }
  }
  delay(100); 
}  
//====================================================================
void Demo4(void)
{
  static int start = 0; 

  if(start == 0) {
    booster_sethsv(330,255,128);
    booster_setall();
    booster_setrgb(0, 100, 0);
    booster_setled(LED_COUNT - 1);
    booster_show();
    start++;
  } else {
    booster_shiftdown(0, LED_COUNT - 1, 2);
    booster_setrgb(0, 100, 0);
    booster_setled(LED_COUNT - 1);
    booster_setrgb(100, 100, 0);
    booster_setled(LED_COUNT - 2);
    booster_show();
    if(++start > (LED_COUNT / 2)) { start = 0; }
  }
  delay(100); 
}  
//====================================================================
void Demo5(void)
{
  static u16 hue = 0;
  static int start = 0; 

  if(start & 0x04) {
    booster_setrgb(100, 100, 100);
    booster_setrange(0, (LED_COUNT / 2) - 10);
    booster_setrgb(00, 00, 100);
    booster_setrange(LED_COUNT / 2 + 9, LED_COUNT - 1);
  } else {
    booster_setrgb( 0,  0, 100);
    booster_setrange(0, (LED_COUNT / 2) - 10);
    booster_setrgb(100, 100, 100);
    booster_setrange(LED_COUNT / 2 + 9, LED_COUNT - 1);
  }
  booster_setrainbow(hue, 255, 100, LED_COUNT / 2 - 9, LED_COUNT / 2 + 8, 20);
  hue = (hue + 10) % 360;
  booster_show();
  delay(40);
  start++;  
}  
//====================================================================
void Demo6(void)
{
  u16 idex = random(0, LED_COUNT);
  u16 ihue = random(0, 359);  
  booster_sethsv(ihue, 255, 128);
  booster_setled(idex);
  booster_show();
  delay(40);
}
//====================================================================
void Demo7(void)
{
  static u8 start = 0;

  booster_setrgb(0, 0, 0);
  booster_setrange(0, 7);
//  if((start > 0) && (start < 8)) {
  if(start < 8) {
    booster_sethsv(90, 255, 100);
    booster_setrange(0, start);
  }  
  if(start > 7) {
    booster_sethsv(90, 255, 100);
//    booster_setrange(15 - start, 7);
    booster_setrange(start - 7, 7);
  }  
  booster_repeat(0, 7, 7);
  
  booster_show();
  start++; if(start > 15) { start = 0; }
  delay(100);
}
//====================================================================
// MAIN LOOP
//====================================================================
void loop() {
//  Demo1();      // Simple Demo for SETRGB, SETHSV, SETALL, SETRANGE
//  Demo2();      // Demo for SETRAINBOW
//  Demo3();      // Demo for SHIFTUP
//  Demo4();      // Demo for SHIFTDOWN
//  Demo5();      // Demo for Blink + Rainbow Mix
//  Demo6();      // Demo for Random Color
  Demo7();      // Demo for REPEAT
}
//====================================================================

