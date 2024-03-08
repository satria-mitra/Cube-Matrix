#define myLayer 2
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

#include <SparkFun_SCD30_Arduino_Library.h> // Include the SCD30 library
#define PIN 6
//////////////////////////////////
#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1024




int frequencyHz =60;
void startTimer(int frequencyHz);
void setTimerFrequency(int frequencyHz);
void TC3_Handler();
//////////////////////////////////////
volatile uint16_t mymatrix[16][8];  // color of each LED in the matrix
int brightness = 80;  // set brightness here  0-256 with 10 for easy on eyes

Adafruit_NeoMatrix cube = Adafruit_NeoMatrix(8, 8, 8, 1, PIN,
                          NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                          NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                          NEO_RGB            + NEO_KHZ800);



#define White 43
#define Black 0
#define Red 1
#define Orange 4
#define Yellow 9
#define Green 15
#define Aqua 20
#define Blue 29
#define Purple 34
uint32_t mycolor;
const byte color [44][3] PROGMEM = {  // the color pallette table
  {0, 0, 0}, //Black
  {7, 0, 0}, {7, 1, 0}, {6, 1, 0}, {6, 2, 0}, {5, 2, 0}, {5, 3, 0}, {4, 3, 0}, {4, 4, 0}, {3, 4, 0}, {3, 5, 0}, {2, 5, 0}, {2, 6, 0}, {1, 6, 0}, {1, 7, 0}, // Red to Green
  {0, 7, 0}, {0, 7, 1}, {0, 6, 1}, {0, 6, 2}, {0, 5, 2}, {0, 5, 3}, {0, 4, 3}, {0, 4, 4}, {0, 3, 4}, {0, 3, 5}, {0, 2, 5}, {0, 2, 6}, {0, 1, 6}, {0, 1, 7}, // Green to Blue
  {0, 0, 7}, {1, 0, 7}, {1, 0, 6}, {2, 0, 6}, {2, 0, 5}, {3, 0, 5}, {3, 0, 4}, {4, 0, 4}, {4, 0, 3}, {5, 0, 3}, {5, 0, 2}, {6, 0, 2}, {6, 0, 1}, {7, 0, 1}, // Blue to Red
  {7, 7, 7},  // White
};

byte myloc[3] = {2, 2, 2};
const byte cubetPath [50][3] PROGMEM = {  // cubet path
  {0, 0, 3}, {0, 1, 3}, {0, 2, 3}, {0, 3, 3}, {1, 3, 3}, {2, 3, 3}, {3, 3, 3}, {3, 2, 3}, {3, 1, 3}, {3, 0, 3}, {2, 0, 3}, {1, 0, 3}, {0, 0, 3},
  {0, 2, 0}, {0, 2, 1}, {0, 2, 2}, {0, 2, 3}, {1, 2, 3}, {2, 2, 3}, {3, 2, 3}, {3, 2, 2}, {3, 2, 1}, {3, 2, 0}, {2, 2, 0}, {1, 2, 0}, {0, 2, 0},
  {0, 0, 2}, {1, 0, 2}, {2, 0, 2}, {3, 0, 2}, {3, 1, 2}, {3, 2, 2}, {3, 3, 2}, {2, 3, 2}, {1, 3, 2}, {0, 3, 2}, {0, 2, 2}, {0, 1, 2}, {0, 0, 2},
  {2, 0, 1}, {1, 0, 1}, {0, 1, 1}, {0, 2, 1}, {1, 3, 1}, {2, 3, 1}, {3, 1, 1}, {3, 2, 1}, {2, 0, 1}
};
unsigned long start;

//////////////
SCD30 airSensor;
bool co2good;
int numspeed = 40;
int numstay= 400;





void setup() {
  Wire.begin();

  Serial.begin(9600);
  airSensor.begin(Wire); // Initialize the SCD30 sensor
    if (airSensor.begin() == false) {
    Serial.println("SCD30 not detected, please check the wiring!");
    while (1); // If the sensor is not detected, halt the program
  }
  //////////////////////////
  cube.begin();
  cube.setTextWrap(false);
  cube.setBrightness(brightness);
  /////////////////////////



  startTimer(10);

  

  
  delay(2000);
}



void loop() {
  // int co2= airSensor.getCO2();
  // datadisplay(co2);
  // float tump= airSensor.getTemperature();
  // float hum= airSensor.getHumidity();
  // showHum(hum);
  // rainVersionTwo();
  // Serial.println(tump);
  // Serial.print(hum);
  //color_wheelTWO();
  //sinwaveTwo(tump,hum);
  //bouncyvTwo(co2);
  // num0();
  // num1();
  // num2();
  // num3();
  // num4();
  // num5();
  // num6();
  // num7();
  // num8();
  // num9();
  //T();
  //U();
  //M();
  //P();
  //H();
  //C();
  //colon();
  //percentage();
  //exclamation();
  //degree();
  rain();
  //waterdrop();
  //rainVersionTwo();
  clearCube();
  delay(1000);
  //sinwaveTwo(tump,hum);
//bouncyvTwo(co2);

  //clearCube();
  //delay(1000);
    //clearCube();
}

////////////////////////////////////////////////////
void setTimerFrequency(int frequencyHz) {
  int compareValue = (CPU_HZ / (TIMER_PRESCALER_DIV * frequencyHz)) - 1;
  TcCount16* TC = (TcCount16*) TC3;
  // Make sure the count is in a proportional position to where it was
  // to prevent any jitter or disconnect when changing the compare value.
  TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
  TC->CC[0].reg = compareValue;
  Serial.println(TC->COUNT.reg);
  Serial.println(TC->CC[0].reg);
  while (TC->STATUS.bit.SYNCBUSY == 1);
}

void startTimer(int frequencyHz) {
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // wait for sync

  TcCount16* TC = (TcCount16*) TC3;

  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  // Use the 16-bit timer
  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  // Use match mode so that the timer counter resets when the count matches the compare register
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  // Set prescaler to 1024
  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync

  setTimerFrequency(frequencyHz);

  // Enable the compare interrupt
  TC->INTENSET.reg = 0;
  TC->INTENSET.bit.MC0 = 1;

  NVIC_EnableIRQ(TC3_IRQn);

  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
}

void TC3_Handler() {
  TcCount16* TC = (TcCount16*) TC3;
  // If this interrupt is due to the compare register matching the timer count
  // we toggle the LED.
  if (TC->INTFLAG.bit.MC0 == 1) {
    TC->INTFLAG.bit.MC0 = 1;
    cube.show();
  }
}
////////////////////////////////////////////////////




void LED(int x, int y, int z, unsigned int mycolor){
  if (z%2) cube.drawPixel((z+1)*8-x-1, y, GetColor(mycolor));
  else cube.drawPixel(x+(z*8),y, GetColor(mycolor));
}
void LED1(int z, int y, int x, unsigned int mycolor){
  if (z%2) cube.drawPixel((z+1)*8-x-1, y, GetColor(mycolor));
  else cube.drawPixel(x+(z*8),y, GetColor(mycolor));
}
void LED2(int z, int y, int x, byte red, byte green, byte blue){
  if (z%2) cube.drawPixel((z+1)*8-x-1, y, GetColor2(red,green,blue));
  else cube.drawPixel(x+(z*8),y, GetColor2(red,green,blue));
  //cube.show();
}


uint32_t GetColor(int thecolor) {
  byte myred = 36*pgm_read_byte_near(&color[thecolor][0]);
  byte mygreen = 36* pgm_read_byte_near(&color[thecolor][1]);
  byte myblue = 36*pgm_read_byte_near(&color[thecolor][2]);
  return cube.Color(myred,mygreen, myblue);
}

uint32_t GetColor2(byte red, byte green, byte blue) {
  byte myred = 36*pgm_read_byte_near(red);
  byte mygreen = 36* pgm_read_byte_near(green);
  byte myblue = 36*pgm_read_byte_near(blue);
  return cube.Color(myred,mygreen, myblue);
}


void clearCube(){
cube.fill(Black);
}

void pause() {  // pause between animations
  clearCube();
  delay(500);
}

void showCube(int mytime) {
  delay(20 * mytime);
  clearCube();
}

//////////////////////////////
void sinwaveTwo(float tump,float hum) {
  int wavespeed;
  if(tump<30){
    mycolor =29;
  }else{
    mycolor =1;
  }

  if(hum<30){
    wavespeed=80;
  }else{
    wavespeed=30;
  }
  int sinewavearray[8], addr, sinemult[8], colselect, rr = 0, gg = 0, bb = 15, addrt;
  int sinewavearrayOLD[8], select, subZ = -7, subT = 7, multi = 0;
  sinewavearray[0] = 0;
  sinemult[0] = 1;
  sinewavearray[1] = 1;
  sinemult[1] = 1;
  sinewavearray[2] = 2;
  sinemult[2] = 1;
  sinewavearray[3] = 3;
  sinemult[3] = 1;
  sinewavearray[4] = 4;
  sinemult[4] = 1;
  sinewavearray[5] = 5;
  sinemult[5] = 1;
  sinewavearray[6] = 6;
  sinemult[6] = 1;
  sinewavearray[7] = 7;
  sinemult[7] = 1;

  start = millis();

  while (millis() - start < 10000) {
    for (addr = 0; addr < 8; addr++) {
      if (sinewavearray[addr] == 7) {
        sinemult[addr] = -1;
      }
      if (sinewavearray[addr] == 0) {
        sinemult[addr] = 1;
      }
      sinewavearray[addr] = sinewavearray[addr] + sinemult[addr];
    }
    if (sinewavearray[0] == 7) {
      select = random(3);
      if (select == 0) {
        rr = random(1, 16);
        gg = random(1, 16);
        bb = 0;
      }
      if (select == 1) {
        rr = random(1, 16);
        gg = 0;
        bb = random(1, 16);
      }
      if (select == 2) {
        rr = 0;
        gg = random(1, 16);
        bb = random(1, 16);
      }


    }

/////////////////////////////////////////////////
    for (addr = 0; addr < 8; addr++) {
      LED1(sinewavearrayOLD[addr], addr, 0, Black);
      LED1(sinewavearrayOLD[addr], 0, addr,Black);
      LED1(sinewavearrayOLD[addr], subT - addr, 7, Black);
      LED1(sinewavearrayOLD[addr], 7, subT - addr, Black);
      LED1(sinewavearray[addr], addr, 0, mycolor);
      LED1(sinewavearray[addr], 0, addr, mycolor);
      LED1(sinewavearray[addr], subT - addr, 7, mycolor);
      LED1(sinewavearray[addr], 7, subT - addr, mycolor);
    }

    for (addr = 1; addr < 7; addr++) {
      LED1(sinewavearrayOLD[addr + multi * 1], addr, 1, Black);
      LED1(sinewavearrayOLD[addr + multi * 1], 1, addr, Black);
      LED1(sinewavearrayOLD[addr + multi * 1], subT - addr, 6, Black);
      LED1(sinewavearrayOLD[addr + multi * 1], 6, subT - addr, Black);
      LED1(sinewavearray[addr + multi * 1], addr, 1, mycolor);
      LED1(sinewavearray[addr + multi * 1], 1, addr, mycolor);
      LED1(sinewavearray[addr + multi * 1], subT - addr, 6, mycolor);
      LED1(sinewavearray[addr + multi * 1], 6, subT - addr, mycolor);
    }

    for (addr = 2; addr < 6; addr++) {
      LED1(sinewavearrayOLD[addr + multi * 2], addr, 2, Black);
      LED1(sinewavearrayOLD[addr + multi * 2], 2, addr, Black);
      LED1(sinewavearrayOLD[addr + multi * 2], subT - addr, 5,Black);
      LED1(sinewavearrayOLD[addr + multi * 2], 5, subT - addr, Black);
      LED1(sinewavearray[addr + multi * 2], addr, 2, mycolor);
      LED1(sinewavearray[addr + multi * 2], 2, addr, mycolor);
      LED1(sinewavearray[addr + multi * 2], subT - addr, 5, mycolor);
      LED1(sinewavearray[addr + multi * 2], 5, subT - addr, mycolor);
    }
    for (addr = 3; addr < 5; addr++) {
      LED1(sinewavearrayOLD[addr + multi * 3], addr, 3, Black);
      LED1(sinewavearrayOLD[addr + multi * 3], 3, addr, Black);
      LED1(sinewavearrayOLD[addr + multi * 3], subT - addr, 4, Black);
      LED1(sinewavearrayOLD[addr + multi * 3], 4, subT - addr, Black);
      LED1(sinewavearray[addr + multi * 3], addr, 3, mycolor);
      LED1(sinewavearray[addr + multi * 3], 3, addr, mycolor);
      LED1(sinewavearray[addr + multi * 3], subT - addr, 4, mycolor);
      LED1(sinewavearray[addr + multi * 3], 4, subT - addr, mycolor);
    }
    /////////////////////////////////////////////////////
    for (addr = 0; addr < 8; addr++)
      sinewavearrayOLD[addr] = sinewavearray[addr];
    delay(wavespeed);



  }


}

//////////////////////////////
void color_wheelTWO() {
  int xx, yy, zz, ww, rr = 1, gg = 1, bb = 1, ranx, rany , ranz, select, swiper;

  start = millis();

  while (millis() - start < 10000) {
    swiper = random(6);
    select = random(3);
    if (select == 0) {
      ranx = 0;
      rany = random(16);
      ranz = random(16);
    }
    if (select == 1) {
      ranx = random(16);
      rany = 0;
      ranz = random(16);
    }
    if (select == 2) {
      ranx = random(16);
      rany = random(16);
      ranz = 0;
    }


    if (swiper == 0) {
      for (yy = 0; yy < 8; yy++) {
        for (xx = 0; xx < 8; xx++) {
          for (zz = 0; zz < 8; zz++) {
            LED2(xx, yy, zz,  ranx, ranz, rany);
          }
        }
        delay(30);
      }
    }
    if (swiper == 1) {
      for (xx = 0; xx < 8; xx++) {
        for (yy = 0; yy < 8; yy++) {
          for (zz = 0; zz < 8; zz++) {
            LED2(xx, yy, zz,  ranx, ranz, rany);
          }
        }
        delay(30);
      }
    }
    if (swiper == 2) {
      for (zz = 0; zz < 8; zz++) {
        for (xx = 0; xx < 8; xx++) {
          for (yy = 0; yy < 8; yy++) {
            LED2(xx, yy, zz,  ranx, ranz, rany);
          }
        }
        delay(30);
      }
    }
    if (swiper == 3) {
      for (yy = 7; yy >= 0; yy--) {
        for (xx = 0; xx < 8; xx++) {
          for (zz = 0; zz < 8; zz++) {
            LED2(xx, yy, zz,  ranx, ranz, rany);
          }
        }
        delay(30);
      }
    }
    if (swiper == 4) {
      for (xx = 7; xx >= 0; xx--) {
        for (yy = 0; yy < 8; yy++) {
          for (zz = 0; zz < 8; zz++) {
            LED2(xx, yy, zz,  ranx, ranz, rany);
          }
        }
        delay(30);
      }
    }
    if (swiper == 5) {
      for (zz = 7; zz >= 0; zz--) {
        for (xx = 0; xx < 8; xx++) {
          for (yy = 0; yy < 8; yy++) {
            LED2(xx, yy, zz,  ranx, ranz, rany);
          }
        }
        delay(30);
      }
    }




  }

}
/////////////////////////////
void bouncyvTwo(int co22222) {
  int delaynum;
  if(co22222<1500){
    delaynum=100;
  }else{
    delaynum=15;
  }


  int wipex, wipey, wipez, ranr, rang, ranb, select, oldx[50], oldy[50], oldz[50];
  int x[50], y[50], z[50], addr, ledcount = 20, direct, direcTwo;
  int xx[50], yy[50], zz[50];
  int xbit = 1, ybit = 1, zbit = 1;
  for (addr = 0; addr < ledcount + 1; addr++) {
    oldx[addr] = 0;
    oldy[addr] = 0;
    oldz[addr] = 0;
    x[addr] = 0;
    y[addr] = 0;
    z[addr] = 0;
    xx[addr] = 0;
    yy[addr] = 0;
    zz[addr] = 0;

  }

  start = millis();

  while (millis() - start < 20000) {
    direct = random(3);
    if(co22222<1500){
      for (addr = 1; addr < ledcount + 1; addr++) {
      LED1(oldx[addr], oldy[addr], oldz[addr], Black);
      LED2(x[addr], y[addr], z[addr], xx[addr], yy[addr], zz[addr]);
      }
    }else{
      for (addr = 1; addr < ledcount + 1; addr++) {
      LED1(oldx[addr], oldy[addr], oldz[addr], Black);
      //LED2(x[addr], y[addr], z[addr], xx[addr], yy[addr], zz[addr]);
      LED1(x[addr], y[addr], z[addr], Red);
      }
      
    }


    for (addr = 1; addr < ledcount + 1; addr++) {
      oldx[addr] = x[addr];
      oldy[addr] = y[addr];
      oldz[addr] = z[addr];
    }
    delay(delaynum);







    if (direct == 0)
      x[0] = x[0] + xbit;
    if (direct == 1)
      y[0] = y[0] + ybit;
    if (direct == 2)
      z[0] = z[0] + zbit;

    if (direct == 3)
      x[0] = x[0] - xbit;
    if (direct == 4)
      y[0] = y[0] - ybit;
    if (direct == 5)
      z[0] = z[0] - zbit;





    if (x[0] > 7) {
      xbit = -1;
      x[0] = 7;
      xx[0] = random(16);
      yy[0] = random(16);
      zz[0] = 0;

    }
    if (x[0] < 0) {
      xbit = 1;
      x[0] = 0;
      xx[0] = random(16);
      yy[0] = 0;
      zz[0] = random(16);

    }
    if (y[0] > 7) {
      ybit = -1;
      y[0] = 7;
      xx[0] = 0;
      yy[0] = random(16);
      zz[0] = random(16);

    }
    if (y[0] < 0) {
      ybit = 1;
      y[0] = 0;
      xx[0] = 0;
      yy[0] = random(16);
      zz[0] = random(16);

    }
    if (z[0] > 7) {
      zbit = -1;
      z[0] = 7;
      xx[0] = random(16);
      yy[0] = 0;
      zz[0] = random(16);

    }
    if (z[0] < 0) {
      zbit = 1;
      z[0] = 0;
      xx[0] = random(16);
      yy[0] = random(16);
      zz[0] = 0;

    }

    for (addr = ledcount; addr > 0; addr--) {
      x[addr] = x[addr - 1];
      y[addr] = y[addr - 1];
      z[addr] = z[addr - 1];
      xx[addr] = xx[addr - 1];
      yy[addr] = yy[addr - 1];
      zz[addr] = zz[addr - 1];
    }


  }
}

void num0(){
  int z,y,x;
  for(x=7;x>-1;x--){
    for(y=2;y<6;y++){
      LED1(0, y, x, Red);
      LED1(7, y, x, Red);
    }
    for(z=1;z<7;z++){
      LED1(z, 1, x, Red);
      LED1(z, 6, x, Red);
    }
    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
//////////////////////////////
void num1(){
  for(int x=7;x>-1;x--){
    for(int y1=1;y1<7;y1++){
      LED1(0, y1, x, Red);
    }
    for(int z1=1;z1<8;z1++){
      LED1(z1, 3, x, Red);
      LED1(z1, 4, x, Red);
    }
    LED1(5, 1, x, Red);
    LED1(6, 2, x, Red);
    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
///////////////////////////

void num2(){
  int z2[4]={1,2,3,4};
  int y2[4]={2,3,4,5};
  for(int x=7;x>-1;x--){
    for(int y1=2;y1<7;y1++){
      LED1(0, y1, x, Red);
    }
    for(int i=0;i<4;i++){
      LED1(z2[i],y2[i],x,Red);
    }
    for(int z3=5;z3<7;z3++){
      LED1(z3,2,x,Red);
      LED1(z3,6,x,Red);
    }
    for(int y2=3;y2<6;y2++){
      LED1(7,y2,x,Red);
    }
    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}

////////////////////////////

void num3(){
  for(int x=7;x>-1;x--){
    for(int y1=3;y1<6;y1++){
      LED1(0,y1,x,Red);
      LED1(7,y1,x,Red);
    }
    for(int z1=1;z1<3;z1++){
      LED1(z1,6,x,Red);
    }
    for(int z2=5;z2<7;z2++){
      LED1(z2,6,x,Red);
    }
    for(int z3=3;z3<5;z3++){
      LED1(z3,5,x,Red);
    }
    LED1(1,2,x,Red);
    LED1(6,2,x,Red);
    LED1(3,4,x,Red);

    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}

//////////////////////////////////////////////

void num4(){
  for(int x=7;x>-1;x--){
    for(int z1=0;z1<8;z1++){
      LED1(z1,4,x,Red);
    }
    for(int z2=3;z2<8;z2++){
      LED1(z2,1,x,Red);
    }
    for(int y1=1;y1<7;y1++){
      LED1(3,y1,x,Red);
    }


    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}

///////////////////////////////////

void num5(){
  for(int x=7;x>-1;x--){
    for(int y1=2;y1<7;y1++){
      LED1(7,y1,x,Red);
    }
    for(int y2=2;y2<6;y2++){
      LED1(4,y2,x,Red);
      LED1(0,y2,x,Red);
    }
    for(int z1=5;z1<7;z1++){
      LED1(z1,2,x,Red);
    }
    for(int z2=1;z2<4;z2++){
      LED1(z2,6,x,Red);
    }
    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);

}
////////////////////

void num6(){
  for(int x=7;x>-1;x--){
    for(int y1=2;y1<7;y1++){
      LED1(0,y1,x,Red);
      LED1(4,y1,x,Red);
      LED1(7,y1,x,Red);
    }
    for(int z1=1;z1<4;z1++){
      LED1(z1,2,x,Red);
    }
    for(int z2=1;z2<7;z2++){
      LED1(z2,2,x,Red);
    }
    for(int z2=0;z2<5;z2++){
      LED1(z2,6,x,Red);
    }

    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}

//////////////
void num7(){
  int z1[7]={0,0,0,0,2,4,6};
  int z2[7]={0,0,0,2,4,6,8};
  for(int x=7;x>-1;x--){
    for(int y1=2;y1<7;y1++){
      LED1(7,y1,x,Red);
    }
    for(int y2=3;y2<7;y2++){
      for(int z=z1[y2];z<z2[y2];z++){
        LED1(z,y2,x,Red);
      }
    }

    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
/////////////////
void num8(){
  int z[6]={6,6,5,5,3,3};
  int y[6]={6,1,5,2,5,2,};
  for(int x=7;x>-1;x--){
    for(int y1=2;y1<6;y1++){
      LED1(0,y1,x,Red);
      LED1(7,y1,x,Red);
    }
    for(int z1=1;z1<3;z1++){
      LED1(z1,1,x,Red);
      LED1(z1,6,x,Red);
    }
    for(int y2=3;y2<5;y2++){
      LED1(4,y2,x,Red);
    }
    for(int i=0;i<6;i++){
      LED1(z[i],y[i],x,Red);
    }
    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
/////////////
void num9(){
  for(int x=7;x>-1;x--){
    for(int y1=3;y1<6;y1++){
      LED1(0,y1,x,Red);
      LED1(3,y1,x,Red);
      LED1(7,y1,x,Red);
    }
    for(int z1=1;z1<7;z1++){
      LED1(z1,6,x,Red);
    }
    for(int z2=4;z2<7;z2++){
      LED1(z2,2,x,Red);
    }
    LED1(1,2,x,Red);

    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
/////////////
void T(){
  for(int x=7;x>-1;x--){
    for(int y1=1;y1<8;y1++){
      LED1(7,y1,x,Red);
    }
    for(int z1=0;z1<7;z1++){
      LED1(z1,4,x,Red);
    }
    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
///////////////////
void U(){
  for(int x=7;x>-1;x--){
    for(int y1=2;y1<6;y1++){
      LED1(0,y1,x,Red);
    }
    for(int z1=1;z1<8;z1++){
      LED1(z1,1,x,Red);
      LED1(z1,6,x,Red);
    }
    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
///////////////////////
void M(){
  for(int x=7;x>-1;x--){
    for(int z1=0;z1<8;z1++){
      LED1(z1,1,x,Red);
      LED1(z1,7,x,Red);
    }
    for(int z2=3;z2<5;z2++){
      LED1(z2,4,x,Red);
    }
    LED1(6,2,x,Red);
    LED1(6,6,x,Red);
    LED1(5,3,x,Red);
    LED1(5,5,x,Red);


    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
///////////////////////////////
void P(){
  for(int x=7;x>-1;x--){
    for(int z1=0;z1<8;z1++){
      LED1(z1,1,x,Red);
    }
    for(int y1=1;y1<5;y1++){
      LED1(2,y1,x,Red);
      LED1(7,y1,x,Red);
    }
    LED1(6,5,x,Red);
    LED1(3,5,x,Red);
    LED1(4,6,x,Red);
    LED1(5,6,x,Red);
    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
///////////////////////////
void H(){
  for(int x=7;x>-1;x--){
    for(int z1=0;z1<8;z1++){
      LED1(z1,2,x,Red);
      LED1(z1,6,x,Red);
    }
    for(int y1=2;y1<7;y1++){
      LED1(4,y1,x,Red);
    }
    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
/////////////////////////
void C(){
  for(int x=7;x>-1;x--){
    for(int y1=3;y1<6;y1++){
      LED1(0,y1,x,Red);
      LED1(7,y1,x,Red);
    }
    for(int x1=2;x1<6;x1++){
      LED1(x1,1,x,Red);
    }
    LED1(1,2,x,Red);
    LED1(1,6,x,Red);
    LED1(6,2,x,Red);
    LED1(6,6,x,Red);
    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
///////////////////////
void colon(){
  for(int x=7;x>-1;x--){
    for(int z1=5;z1<7;z1++){
      for(int y1=3;y1<5;y1++){
        LED1(z1,y1,x,Red);
      }
    }
    for(int z1=1;z1<3;z1++){
      for(int y1=3;y1<5;y1++){
        LED1(z1,y1,x,Red);
      }
    }

    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
//////////////////////////////
void percentage(){
  for(int x=7;x>-1;x--){
    for(int z1=5;z1<7;z1++){
      for(int y1=1;y1<3;y1++){
        LED1(z1,y1,x,Red);
      }
    }
    for(int z1=1;z1<3;z1++){
      for(int y1=5;y1<7;y1++){
        LED1(z1,y1,x,Red);
      }
    }
    for(int z1=2;z1<4;z1++){
      LED1(z1,3,x,Red);
    }
    for(int z1=4;z1<6;z1++){
      LED1(z1,4,x,Red);
    }
    LED1(0,1,x,Red);
    LED1(1,2,x,Red);
    LED1(6,5,x,Red);
    LED1(7,6,x,Red);


    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
////////////////////
void exclamation(){
  for(int x=7;x>-1;x--){
    for(int z1=3;z1<8;z1++){
      LED1(z1,3,x,Red);
      LED1(z1,4,x,Red);
    }
    for(int z1=0;z1<2;z1++){
      for(int y1=3;y1<5;y1++){
        LED1(z1,y1,x,Red);
      }
    }


    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
/////////////////
void degree(){
  for(int x=7;x>-1;x--){
    for(int z1=6;z1<8;z1++){
      for(int y1=0;y1<2;y1++){
        LED1(z1,y1,x,Red);
      }
    }
    for(int z1=1;z1<6;z1++){
      LED1(z1,2,x,Red);
    }
    for(int y1=3;y1<6;y1++){
      LED1(0,y1,x,Red);
      LED1(6,y1,x,Red);
    }
    LED1(1,6,x,Red);
    LED1(5,6,x,Red);


    delay(numspeed);
    if(x!=0){
      clearCube();
    }
  }
  delay(numstay);
}
///////////////////
void datadisplay(int co2){
  String co2data=String(co2);
  int numlength= co2data.length();
  Serial.println(co2data);
  Serial.println(numlength);
  numspeed=20;
  C();
  num0();
  num2();
  colon();
  numspeed=30;

  for(int i=0;i<co2data.length();i++){
    char Digit= co2data.charAt(i);
    Serial.println(Digit);
    switch(Digit){
      case '0':
      num0();
      break;
      case '1':
      num1();
      break;
      case '2':
      num2();
      break;
      case '3':
      num3();
      break;
      case '4':
      num4();
      break;
      case '5':
      num5();
      break;
      case '6':
      num6();
      break;
      case '7':
      num7();
      break;
      case '8':
      num8();
      break;
      case '9':
      num9();
      break;
      default:
      break;
    }
    delay(450);
  }
  delay(2000);
}



void showHum(float hum){
  String humdata=String(hum,0);

  int numlength= humdata.length();
  Serial.println(humdata);
  Serial.println(numlength);
  for(int i=0;i<humdata.length();i++){
    char Digit= humdata.charAt(i);
    Serial.println(Digit);
    switch(Digit){
      case '0':
      num0();
      break;
      case '1':
      num1();
      break;
      case '2':
      num2();
      break;
      case '3':
      num3();
      break;
      case '4':
      num4();
      break;
      case '5':
      num5();
      break;
      case '6':
      num6();
      break;
      case '7':
      num7();
      break;
      case '8':
      num8();
      break;
      case '9':
      num9();
      break;
      default:
      break;
    }
    delay(450);
  }
  delay(500);
  clearCube();
}

void rainVersionTwo() {
  int num=10;
  int x[num], y[num], z[num], addr, leds = num, bright = 1;
  int xold[num], yold[num], zold[num];

  for (addr = 0; addr < num; addr++) {
    x[addr] = random(8);
    y[addr] = random(8);
    z[addr] = random(8);
  }
  start = millis();
  while (millis() - start < 20000) {
    for (addr = 0; addr < leds; addr++) {
      if(zold[addr]>0){
        LED1(zold[addr], xold[addr], yold[addr], Black);
      }
      LED1(z[addr], x[addr], y[addr], Blue);
    }
    for (addr = 0; addr < leds; addr++) {
      xold[addr] = x[addr];
      yold[addr] = y[addr];
      zold[addr] = z[addr];
    }
    delay(60);
    for (addr = 0; addr < leds; addr++) {
      z[addr] = z[addr] - 1;
      if (z[addr] < random(-100, 0)) {
        x[addr] = random(8);
        y[addr] = random(8);
        z[addr] = 7;

      }
    }
  }

}

void rain(){
  int num=64;
  int num2=0;

  int x[num], y[num], z[num], zlevel[num], leds = 10,lednum;
  int xold[num], yold[num], zold[num];
  int ranled[leds];

    for(int x1=0;x1<8;x1++){
      for(int y1=0;y1<8;y1++){
        x[num2] = x1;
        y[num2] = y1;
        z[num2] = random(7,26);
        zlevel[num2] = 0;
        num2++;
      }
    }
  for(int i = 0;i<leds;i++){
    ranled[i]=random(0,63);
  }
    int numnnnn=0;
  start = millis();
  while (millis() - start < 200000){
    for(lednum = 0;lednum<leds;lednum++){
      // if(z[ranled[lednum]]>0){
        
      // }
      LED1(z[ranled[lednum]], x[ranled[lednum]], y[ranled[lednum]], Blue);
    }
    for (lednum = 0; lednum < leds; lednum++){
      xold[ranled[lednum]] = x[ranled[lednum]];
      yold[ranled[lednum]] = y[ranled[lednum]];
      zold[ranled[lednum]] = z[ranled[lednum]];
    }
    
    delay(150);
    for (lednum = 0; lednum < leds; lednum++) {
      if(z[ranled[lednum]]!=zlevel[ranled[lednum]]){
        z[ranled[lednum]] = z[ranled[lednum]] -1;
        LED1(zold[ranled[lednum]], xold[ranled[lednum]], yold[ranled[lednum]], Black);
        //Serial.println(z[ranled[lednum]]);
      }
      if (z[ranled[lednum]] == zlevel[ranled[lednum]]) {
        z[ranled[lednum]] = random(7,26);
        LED1(zlevel[ranled[lednum]], x[ranled[lednum]], y[ranled[lednum]], Blue);
        zlevel[ranled[lednum]]++;
        Serial.println(zlevel[ranled[lednum]]);
        ranled[lednum]=random(0,63);
      }
      
      
    }
  }
}




























