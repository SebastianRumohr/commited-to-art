#include <SPI.h>

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

#define LED_COUNT            64
#define NUM_LEDS             LED_COUNT
//====================================================================
// set pin 10 as the slave select
const int csPin = 10;
// set pin 9 as the slave reset
const int ResetPin = 9;

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
  digitalWrite(csPin, LOW);
    for(int i = 0; i < pos; i++) {
    SPI.transfer(buf[i]);
  }  
  digitalWrite(csPin, HIGH);
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
// FUNKTIONEN FÜR DEMOS
//====================================================================
void setAll(u8 r, u8 g, u8 b)
{
  booster_setrgb(r, g, b);
  booster_setall();  
  if(pos >= (256 - 16)) { booster_send(); delay(1); }
}
//====================================================================
void setPixel(u8 n, u8 r, u8 g, u8 b)
{
  booster_setrgb(r, g, b);
  booster_setled(n);  
  if(pos >= (256 - 16)) { booster_send(); delay(1); }
}
//====================================================================
void showStrip(void)
{
  booster_show();
}
//====================================================================
void setup() {
  Serial.begin(9600);        // start serial for output
  Serial.println("Startup Test");   
  pinMode (csPin, OUTPUT);  
  pinMode (ResetPin, OUTPUT);  
  digitalWrite(csPin, HIGH);
  SPI.begin();     
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  digitalWrite(ResetPin, LOW);
  delay(100);
  digitalWrite(ResetPin, HIGH);
  delay(100);
  
  booster_init(LED_COUNT, 24);
  booster_rgborder(2, 3, 1);
  booster_send();
  delay(50);
}
//====================================================================
void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {

  for(int i =((NUM_LEDS-EyeSize)/2); i>=0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue); 
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}
//====================================================================
void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i<=((NUM_LEDS-EyeSize)/2); i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue); 
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}
//====================================================================
void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}
//====================================================================
void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}
//====================================================================
void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){
  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);

  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue); 
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}
//====================================================================
void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
  for (int i=0; i<Count; i++) {
     setPixel(random(NUM_LEDS),red,green,blue);
     showStrip();
     delay(SpeedDelay);
     if(OnlyOne) { 
       setAll(0,0,0); 
     }
   }
  delay(SpeedDelay);
}
//====================================================================
void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
  for (int i=0; i<Count; i++) {
    setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));
    showStrip();
    delay(SpeedDelay);
    if(OnlyOne) { 
      setAll(0,0,0); 
    }
  }
  delay(SpeedDelay);
}
//====================================================================
void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position=0;
  for(int i=0; i < NUM_LEDS * 2; i++)
  {
    Position++; // = 0; //Position + Rate;
    for(int i = 0; i < NUM_LEDS; i++) {
      // sine wave, 3 offset waves make a rainbow!
      //float level = sin(i+Position) * 127 + 128;
      //setPixel(i,level,0,0);
      //float level = sin(i+Position) * 127 + 128;
      
      setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                 ((sin(i+Position) * 127 + 128)/255)*green,
                 ((sin(i+Position) * 127 + 128)/255)*blue);
    }
    showStrip();
    delay(WaveDelay);
  }
}
//====================================================================
void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}
//====================================================================
void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    setPixel(i, red, green, blue);
    showStrip();
    delay(SpeedDelay);
  }
}
//====================================================================
void theaterChase(byte red, byte green, byte blue, int SpeedDelay) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < NUM_LEDS; i=i+3) {
        setPixel(i+q, red, green, blue);    //turn every third pixel on
      }
      showStrip();
      delay(SpeedDelay);
      for (int i=0; i < NUM_LEDS; i=i+3) {
        setPixel(i+q, 0,0,0);        //turn every third pixel off
      }
    }
  }
}
//====================================================================
void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}
//====================================================================
void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }
  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }
  showStrip();
  delay(SpeedDelay);
}
//====================================================================
void BouncingBalls(byte red, byte green, byte blue, int BallCount) {
  float Gravity = -9.81;
  int StartHeight = 1;
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];

  for (int i = 0 ; i < BallCount ; i++) {   
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0; 
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2); 
  }
  while (true) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }
    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i],red,green,blue);
    }
    showStrip();
    delay(20);
    setAll(0,0,0);
  }
}
//====================================================================
void BouncingColoredBalls(int BallCount, byte colors[][3]) {
  float Gravity = -9.81;
  int StartHeight = 1;
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];

  for (int i = 0 ; i < BallCount ; i++) {   
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0; 
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2); 
  }

  while (true) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }
    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i],colors[i][0],colors[i][1],colors[i][2]);
    }
    showStrip();
    delay(20);
    setAll(0,0,0);
  }
}
//====================================================================
void loop() {
  CylonBounce(0xff, 0, 0, 4, 10, 50);
//  Twinkle(0xff, 0, 0, 10, 100, false);
//  TwinkleRandom(20, 100, false);
//  NewKITT(0xFF, 0, 0, 8, 20, 50);
//  RunningLights(0xff,0xff,0x00, 50);
//  colorWipe(0x00,0xff,0x00, 50);
//  colorWipe(0x00,0x00,0x00, 50);
//  theaterChase(0xff,0,0,50);
//  Fire(55,120,15);
//  BouncingBalls(0xff,0,0, 3);

/*
  byte colors[4][3] = { {0xff, 0,0}, 
                        {0xff, 0xff, 0x00}, 
                        {0x0, 0xff, 0x00}, 
                        {0   , 0   , 0xff} };
  BouncingColoredBalls(4, colors);
*/
}
//====================================================================

