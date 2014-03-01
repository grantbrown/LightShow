#include "Tlc5940.h"

// Multiplexer input output variables
const int mltplxInput1 = A2;
const int mltplxInput2 = A3;

const int s0 = 8;
const int s1 = 7;
const int s2 = 6;

int paramPotVal1 = 0;
int paramPotVal2 = 0;
int paramPotVal3 = 0;
int paramPotVal4 = 0;

unsigned long toggleTime;
unsigned long randToggleTime = 0;
int toggleState = 0;

int mode = 0;

int modeButtonVal1 = 0;
int modeButtonVal2 = 0;
int modeButtonVal3 = 0;
int modeButtonVal4 = 0;
int modeButtonVal5 = 0;

int rgbPotVal1 = 0;
int rgbPotVal2 = 0;
int rgbPotVal3 = 0;

// Input variables from MSGEQ
int msgeqInputPin = A0; // read from multiplexer using analog input 0
int msgeqStrobePin = 2; // strobe is attached to digital pin 2
int msgeqResetPin = 4; // reset is attached to digital pin 4
int msgeqReadIterations = 2;
int spectrumValue[7]; // to hold a2d values


// Output Variables for TLC
// RGBA
int LED1[4];
int LED2[4];
int LED3[4];
int LED4[4];
int LED5[4];
int LED6[4];
int LED7[4];

int CurrentR;
int CurrentG;
int CurrentB;
int NextR;
int NextG;
int NextB;

int newR;
int newG;
int newB;

int smooth1(int x)
{
  int offset = paramPotVal3;
  if (x > 255 + offset - 100)
  {
    return(x/4);
  }
  else if (x > 200 + offset - 100)
  {
    return(x/3);
  }
  else if (x > 100+ offset - 100)
  {
    return(0);
    return(x/2);
  }
  return(0);
}

int smooth2(int x)
{
  if (x > 400)
  {
    return(x/4);
  }
  return(0);
}

int smooth3(int x)
{
  return(smooth1(x)*16);
}

int smooth4(int x)
{
  int threshold = paramPotVal3;
  if (x > threshold)
  {
    return(255);
  }
  return(0);
}

void SpectrumPrint()
{
 Serial.print("(");  
 for (int i = 0; i < 7; i++)
 {
     Serial.print(spectrumValue[i]); Serial.print(",");
 }
 Serial.print(") "); 
 Serial.print("\n");
}

void SerialPrintMultiplexerInputs()
{
  Serial.print("(");  
  Serial.print(paramPotVal1 ); Serial.print(",");
  Serial.print(paramPotVal2 ); Serial.print(",");
  Serial.print(paramPotVal3 ); Serial.print(",");
  Serial.print(paramPotVal4 ); Serial.print(",");
  Serial.print(modeButtonVal1 ); Serial.print(",");
  Serial.print(modeButtonVal2 ); Serial.print(",");
  Serial.print(modeButtonVal3 ); Serial.print(",");
  Serial.print(modeButtonVal4 ); Serial.print(",");
  Serial.print(modeButtonVal5 ); Serial.print(",");

  Serial.print(rgbPotVal1 ); Serial.print(",");
  Serial.print(rgbPotVal2 ); Serial.print(",");
  Serial.print(rgbPotVal3 ); Serial.print(",");

  Serial.print(") ");  
  Serial.print("\n");
  delay(2);  
}


void getMultiplexerInputs(int del)
{
  digitalWrite(s0, LOW); 
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  delay(del);
  analogRead(mltplxInput1);
  analogRead(mltplxInput2);
  paramPotVal1 = analogRead(mltplxInput1);
  modeButtonVal5 = analogRead(mltplxInput2);
  
  
  digitalWrite(s0, HIGH); 
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  delay(del);
  analogRead(mltplxInput1);
  analogRead(mltplxInput2);
  paramPotVal2 = analogRead(mltplxInput1);
  rgbPotVal1 = analogRead(mltplxInput2);
  
  
  digitalWrite(s0, LOW); 
  digitalWrite(s1, HIGH);
  digitalWrite(s2, LOW);
  delay(del);
  analogRead(mltplxInput1);
  analogRead(mltplxInput2);
  paramPotVal3 = analogRead(mltplxInput1);
  rgbPotVal2 = analogRead(mltplxInput2);
  
  digitalWrite(s0, HIGH); 
  digitalWrite(s1, HIGH);
  digitalWrite(s2, LOW);
  delay(del);
  analogRead(mltplxInput1);
  analogRead(mltplxInput2);
  paramPotVal4 = analogRead(mltplxInput1);
  rgbPotVal3 = analogRead(mltplxInput2);

  digitalWrite(s0, LOW); 
  digitalWrite(s1, LOW);
  digitalWrite(s2, HIGH);
  delay(del);
  analogRead(mltplxInput1);
  analogRead(mltplxInput2);
  modeButtonVal1 = analogRead(mltplxInput1);
  
  digitalWrite(s0, HIGH); 
  digitalWrite(s1, LOW);
  digitalWrite(s2, HIGH);
  delay(del);
  analogRead(mltplxInput1);
  analogRead(mltplxInput2);
  modeButtonVal2 = analogRead(mltplxInput1);

  digitalWrite(s0, LOW); 
  digitalWrite(s1, HIGH);
  digitalWrite(s2, HIGH);
  delay(del);
  analogRead(mltplxInput1);
  analogRead(mltplxInput2);
  modeButtonVal3 = analogRead(mltplxInput1);
  
  digitalWrite(s0, HIGH); 
  digitalWrite(s1, HIGH);
  digitalWrite(s2, HIGH);
  delay(del);
  analogRead(mltplxInput1);
  analogRead(mltplxInput2);
  modeButtonVal4 = analogRead(mltplxInput1);
}

void readMSGEQ()
{
 digitalWrite(msgeqResetPin, HIGH);
 delayMicroseconds(5);
 digitalWrite(msgeqResetPin, LOW);

 for (int i = 0; i < 7; i++)
 {
    digitalWrite(msgeqStrobePin, LOW);
    delayMicroseconds(50); // to allow the output to settle
    spectrumValue[i] = analogRead(msgeqInputPin);
    digitalWrite(msgeqStrobePin, HIGH);
 }
}

void clearToggle()
{
  toggleTime = millis();
  toggleState = 0;
}

void setMode()
{
  if (modeButtonVal1 > 800)
  {
    if (mode == 1){return;}
    clearToggle();
    mode = 1;return;
  }
  else if (modeButtonVal2 > 800)
  {
    if (mode == 2){return;}
    clearToggle();
    mode = 2;return;
  }
  else if (modeButtonVal3 > 800)
  {
    if (mode == 3){return;}
    clearToggle();
    mode = 3;return;
  }
  else if (modeButtonVal4 > 800)
  {
    if (mode == 4){return;}
    clearToggle();
    mode = 4;return;
  }
  else if (modeButtonVal5 > 800)
  {
    if (mode == 5){return;}
    clearToggle();
    mode = 5; return;
  }
}

void setSingleLightAudioAlpha()
{
  int maximum = 0;
  int maxidx = 0;
  for (int i = 0; i < 7; i++)
  {
    if ((spectrumValue[i]) > maximum)
    {
        maximum = spectrumValue[i];
        maxidx = i;
    }
  }

  LED1[3] = (maxidx == 0 ? smooth3(spectrumValue[0]) : 0);
  LED2[3] = (maxidx == 1 ? smooth3(spectrumValue[1]) : 0);
  LED3[3] = (maxidx == 2 ? smooth3(spectrumValue[2]) : 0);
  LED4[3] = (maxidx == 3 ? smooth3(spectrumValue[3]) : 0);
  LED5[3] = (maxidx == 4 ? smooth3(spectrumValue[4]) : 0);
  LED6[3] = (maxidx == 5 ? smooth3(spectrumValue[5]) : 0);
  LED7[3] = (maxidx == 6 ? smooth3(spectrumValue[6]) : 0);
  delay(paramPotVal2);
}

void setSingleLightAudioAlpha2()
{
  int maximum = 0;
  int maxidx = 0;
  for (int i = 0; i < 7; i++)
  {
    if ((spectrumValue[i]) > maximum)
    {
        maximum = spectrumValue[i];
        maxidx = i;
    }
  }

  LED1[3] = (maxidx == 0 ? 4095 : 0);
  LED2[3] = (maxidx == 1 ? 4095 : 0);
  LED3[3] = (maxidx == 2 ? 4095 : 0);
  LED4[3] = (maxidx == 3 ? 4095 : 0);
  LED5[3] = (maxidx == 4 ? 4095 : 0);
  LED6[3] = (maxidx == 5 ? 4095 : 0);
  LED7[3] = (maxidx == 6 ? 4095 : 0);
  delay(paramPotVal2);
}

void setAudioAlpha()
{
  LED1[3] = smooth3(spectrumValue[0]);
  LED2[3] = smooth3(spectrumValue[1]);
  LED3[3] = smooth3(spectrumValue[2]);
  LED4[3] = smooth3(spectrumValue[3]);
  LED5[3] = smooth3(spectrumValue[4]);
  LED6[3] = smooth3(spectrumValue[5]);
  LED7[3] = smooth3(spectrumValue[6]);
}

void setRandomStrobeAlpha()
{
  int A;
  if ((millis() - toggleTime) > randToggleTime)
  {
    randToggleTime = random(paramPotVal1);
    toggleState = !toggleState;
    toggleTime = millis() - 10;
    delay(10);
  }
  A = toggleState*4095;
  LED1[3] = A;
  LED2[3] = A;
  LED3[3] = A;
  LED4[3] = A;
  LED5[3] = A;
  LED6[3] = A;
  LED7[3] = A;
}

void setStrobeAlphaRandRGB()
{
  int A;
  
  int offs =0;
  if ((millis() - toggleTime) > paramPotVal1)
  {
    newR = random(rgbPotVal1*4);
    newG = random(rgbPotVal2*4);
    newB = random(rgbPotVal3*4);
    
    newR += offs; newG += offs; newB += offs;
    toggleState = !toggleState;
    toggleTime = millis() - 10;
    setAllColors(newR, newG, newB);
    delay(10);
    
  }
  A = toggleState*4095;
  LED1[3] = A;
  LED2[3] = A;
  LED3[3] = A;
  LED4[3] = A;
  LED5[3] = A;
  LED6[3] = A;
  LED7[3] = A;
  
}


void setStrobeAlpha()
{
  int A;
  if ((millis() - toggleTime) > paramPotVal1)
  {
    toggleState = !toggleState;
    toggleTime = millis() - 10;
    delay(10);
  }
  A = toggleState*4095;
  LED1[3] = A;
  LED2[3] = A;
  LED3[3] = A;
  LED4[3] = A;
  LED5[3] = A;
  LED6[3] = A;
  LED7[3] = A;
}
void setRandomStrobeAlpha2()
{
  int A;
  if (random(paramPotVal1) < 5)
  {
    toggleState = !toggleState;
    delayMicroseconds(10);
    //toggleTime = millis();
  }
  A = toggleState*4095;
  LED1[3] = A;
  LED2[3] = A;
  LED3[3] = A;
  LED4[3] = A;
  LED5[3] = A;
  LED6[3] = A;
  LED7[3] = A;
}

void setConstantAlpha(int A)
{
  LED1[3] = A;
  LED2[3] = A;
  LED3[3] = A;
  LED4[3] = A;
  LED5[3] = A;
  LED6[3] = A;
  LED7[3] = A;
}
 
int* colorSmooth1(int x)
{
  int cutoff1 = paramPotVal3;
  int cutoff2 = paramPotVal4;
  int out[3];
  out[0] = 0; out[1] = 0; out[2] = 0;
  if (x < cutoff1)
  {
    out[0] = x; out[1] = 0; out[2] = 0;
  }
  else if (x < cutoff2)
  {
    out[0] = x; out[1] = x/3; out[3] = 0;
  }
  else
  {
    out[0] = x; out[1] = x/2; out[3] = x/3;
  }
  return(out);
}


void setAudioColors()
{
  
  // Blue, Green, Red
  int* LED_1 = colorSmooth1(spectrumValue[0]);
  int* LED_2 = colorSmooth1(spectrumValue[1]);
  int* LED_3 = colorSmooth1(spectrumValue[2]);
  int* LED_4 = colorSmooth1(spectrumValue[3]);
  int* LED_5 = colorSmooth1(spectrumValue[4]);
  int* LED_6 = colorSmooth1(spectrumValue[5]);
  int* LED_7 = colorSmooth1(spectrumValue[6]);
  
  LED1[0] = LED_1[0];
  LED2[0] = LED_2[0];
  LED3[0] = LED_3[0];
  LED4[0] = LED_4[0];
  LED5[0] = LED_5[0];
  LED6[0] = LED_6[0];
  LED7[0] = LED_7[0];
  
  LED1[1] = LED_1[1];
  LED2[1] = LED_2[1];
  LED3[1] = LED_3[1];
  LED4[1] = LED_4[1];
  LED5[1] = LED_5[1];
  LED6[1] = LED_6[1];
  LED7[1] = LED_7[1];
  
  LED1[2] = LED_1[2];
  LED2[2] = LED_2[2];
  LED3[2] = LED_3[2];
  LED4[2] = LED_4[2];
  LED5[2] = LED_5[2];
  LED6[2] = LED_6[2];
  LED7[2] = LED_7[2];
  
  setConstantAlpha(4095);
  
} 
  
void setAllColors(int R, int G, int B)
{
  LED1[0] = R;
  LED2[0] = R;
  LED3[0] = R;
  LED4[0] = R;
  LED5[0] = R;
  LED6[0] = R;
  LED7[0] = R;
  
  LED1[1] = G;
  LED2[1] = G;
  LED3[1] = G;
  LED4[1] = G;
  LED5[1] = G;
  LED6[1] = G;
  LED7[1] = G;
  
  LED1[2] = B;
  LED2[2] = B;
  LED3[2] = B;
  LED4[2] = B;
  LED5[2] = B;
  LED6[2] = B;
  LED7[2] = B;
}

void sendTLCData()
{
  Tlc.clear();
  
  Tlc.set(0, LED1[0]); //R
  Tlc.set(1, LED1[1]); //G
  Tlc.set(2, LED1[2]); //B
  Tlc.set(3, LED1[3]); //A
  
  Tlc.set(4, LED2[0]); //R
  Tlc.set(5, LED2[1]); //G
  Tlc.set(6, LED2[2]); //B
  Tlc.set(7, LED2[3]); //A
  
  Tlc.set(8, LED3[0]); //R
  Tlc.set(9, LED3[1]); //G
  Tlc.set(10, LED3[2]); //B
  Tlc.set(11, LED3[3]); //A
  
  Tlc.set(12, LED4[0]); //R
  Tlc.set(13, LED4[1]); //G
  Tlc.set(14, LED4[2]); //B
  Tlc.set(16, LED4[3]); //A
  
  Tlc.set(17, LED5[0]); //R
  Tlc.set(18, LED5[1]); //G
  Tlc.set(19, LED5[2]); //B
  Tlc.set(20, LED5[3]); //A
  
  Tlc.set(21, LED6[0]); //R
  Tlc.set(22, LED6[1]); //G
  Tlc.set(23, LED6[2]); //B
  Tlc.set(24, LED6[3]); //A
  
  Tlc.set(25, LED7[0]); //R
  Tlc.set(26, LED7[1]); //G
  Tlc.set(27, LED7[2]); //B
  Tlc.set(28, LED7[3]); //A
  Tlc.update();
}

/* SETUP and LOOP */

void setup()
{
 //Serial.begin(9600);
 Tlc.init();

 pinMode(msgeqInputPin, INPUT);
 pinMode(mltplxInput1, INPUT);
 pinMode(mltplxInput2, INPUT);
 pinMode(msgeqStrobePin, OUTPUT);
 pinMode(msgeqResetPin, OUTPUT);
 
 pinMode(s0, OUTPUT); 
 pinMode(s1, OUTPUT); 
 pinMode(s2, OUTPUT); 
 
 analogReference(DEFAULT);

 digitalWrite(msgeqResetPin, LOW);
 digitalWrite(msgeqStrobePin, HIGH);
 setAllColors(0,0,4095); 
}


void loop()
{
  readMSGEQ();
  getMultiplexerInputs(2);
  setMode();
  //SerialPrintMultiplexerInputs();
  
  //setAllColors(rgbPotVal1,rgbPotVal2,rgbPotVal3);
  // Don't bother setting colors if mode is going to do this
  if (mode != 1){
    setAllColors(rgbPotVal1,rgbPotVal2,rgbPotVal3);
  }
  
  if (mode == 1){setStrobeAlphaRandRGB();}
  else if (mode == 2){setAudioAlpha();}
  else if (mode == 3){setSingleLightAudioAlpha2();}
  else if (mode == 4){setStrobeAlpha();}
  else if (mode == 5){setRandomStrobeAlpha();} 
  
  /*OUTPUT TO LED-DRIVER*/
  sendTLCData();
}
