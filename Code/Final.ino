#include <Wire.h>

//threshold constants
double ratioThreshold = 0.0002;
int maxless = 0;
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
int motor = 9;

void setup() {
  Wire.begin();
  Wire.beginTransmission(mpu);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(motor, OUTPUT);
}

void loop() {
  Serial.println(initialCountdown);
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
    //Serial.println(1);
  } 
  gyroVals[2] = GyY;
  
  for (int i = 0; i < 19; i++)
  {
    accVals[i] = accVals[i+1];
    //Serial.println(2);
  } 
  accVals[19] = AcX;
  
  if(initialCountdown == 0)
  {
    localMax = isMax();
  
    if (localMax > 5000)
    {
      maxless = 0;
      localFrequency = findFrequency();
      ratio = localFrequency/localMax;
      //Serial.println(localFrequency);
      //Serial.println(ratio);
      
      if(ratio > ratioThreshold)
      {
        freezing = true;
      }
      else
      {
        freezing = false;
      }
    }
    else
    {
      maxless++;
      //Serial.println(maxless);
      if(maxless >= 20 )
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
   digitalWrite(led, HIGH);
   digitalWrite(motor, HIGH);
 }
 else
 {
   digitalWrite(led,LOW);
   digitalWrite(motor, LOW);
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
    //Serial.println(3);
    if((accVals[i] >= lim && accVals[i-1] < lim) || (accVals[i-1] >= lim && accVals[i] < lim))
    {
      count = count + 1;     
    }
  }

  return count;
}
