#include <Wire.h>
//threshold constants
double ratioThreshold = 0.0002;
int initialCountdown = 20;
int freezing = false;
double localMax;
double localFrequency;
double ratio;
double gyroVals[3];
double accVals[20];
int mpu = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int led = 13;
void setup() {
  Wire.begin();
  Wire.beginTransmission(mpu);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  pinMode(led, OUTPUT);  
}
void loop() {
  Wire.beginTransmission(mpu);
  Wire.write(0x3B);
  Wire.endTransmission(false); //Nothing actually happens until now
  Wire.requestFrom(mpu, 14, true);
  AcX = Wire.read()<<8|Wire.read();
  AcY = Wire.read()<<8|Wire.read();
  AcZ = Wire.read()<<8|Wire.read();
  Tmp = Wire.read()<<8|Wire.read();
  GyX = Wire.read()<<8|Wire.read();
  GyY = Wire.read()<<8|Wire.read();
  GyZ = Wire.read()<<8|Wire.read();
  //update memory arrays
  for (int i = 0; i < 2; i++)
  {
    gyroVals[i] = gyroVals[i+1];
  } 
  gyroVals[2] = GyY;
  
  for (int i = 0; i < 19; i++)
  {
    accVals[i] = accVals[i+1];
  } 
  accVals[19] = AcX;
  
  if(initialCountdown == 0)
  {
    localMax = isMax();
  
    if (localMax > 5000)
    {
      localFrequency = findFrequency();
      ratio = localFrequency/localMax;
      
      
      if(ratio > ratioThreshold)
      {
        freezing = true;
      }
      else
      {
        freezing = false;
      }
    }
  }
  else
  {
    initialCountdown--;
  }
 
 if (freezing == true)
 {
   digitalWrite(led,HIGH);
 }
 else
 {
   digitalWrite(led,LOW);
 }
  delay(33);
}
double isMax()
{
  int leftVal = gyroVals[1] - gyroVals[0];
  int rightVal = gyroVals[1] - gyroVals[2];
  if(leftVal >= 0 && rightVal >= 0)
  {
    if(gyroVals[1] >= 5000)
    {
      return gyroVals[1];
    }
  }
  return -1;
}
double findFrequency()
{
  int lim = -10000;
  int range = 20;
  int count = 0;
  for(int i = 0; i < 20; i++)
  {
    if((accVals[i] >= lim && accVals[i-1] < lim) || (accVals[i-1] >= lim && accVals[i] < lim))
    {
      Serial.println(count);
      count = count + 1;     
    }
  }
  return count;
}
