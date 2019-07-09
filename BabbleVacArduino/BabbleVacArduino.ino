#include <SimpleSDAudio.h>
#include "SoundFolder.h"
#include "MeanFinder.h"
#include "MaxFilter.h"
#include "Wire.h" 
const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
bool wasDriving;
#define IS_CLEANING_EDGE 160

SoundFolder* AfterStart;
SoundFolder* AfterEnd;
SoundFolder* OnCollision;
SoundFolder* WhileCleaning;
SoundFolder* LiftUp;
MeanFinder* meanForceX;
MeanFinder* meanForceY;
MeanFinder* meanForceZ;
MeanFinder* meanForces;
MaxFilter* maxForceX;
MaxFilter* maxForceY;
MaxFilter* maxForceZ;
MaxFilter* maxForceAcY;
MeanFinder* meanForceImpact;
int lastDipValue = 0;
unsigned long lastImpact = 0;

int DiscoverFolder(char prefix)
{
  int i = 0;
  char file[] = "s001.wav"; // example 
  file[0] = prefix;
  for(; i < 256; i++)
  {
    file[1] = (char)(48 + i / 100);
    file[2] = (char)(48 + (i % 100) / 10);
    file[3] = (char)(48 + i % 10);
    if(!SdPlay.setFile(file)) // music name file
    {
      break;
    }
  }
  return i;
}

void InitFolders()
{
  AfterStart = new SoundFolder('S', DiscoverFolder('S'));
  AfterEnd = new SoundFolder('E', DiscoverFolder('E'));
  OnCollision = new SoundFolder('C', DiscoverFolder('C'));
  WhileCleaning = new SoundFolder('W', DiscoverFolder('W'));
  LiftUp = new SoundFolder('L', DiscoverFolder('L'));
}

void setup() {
  Serial.begin(115200); 
  Serial.println("Setup started");
  SdPlay.setSDCSPin(10); // sd card cs pin
  Serial.println("setSDCSPin DONE");
  if (!SdPlay.init(SSDA_MODE_FULLRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER))
  {
    Serial.println("SdPlay.init FAIL");
    while(1);
  }
   if(!SdPlay.setFile("s000.wav")) // music name file
  {
    while(1);
  }
  Serial.println("SdPlay.init DONE");
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Serial.println("gyro Init END");
  Wire.endTransmission(true);

  meanForceImpact = new MeanFinder();
  meanForceX = new MeanFinder();
  meanForceY = new MeanFinder();
    
  meanForces = new MeanFinder();
  meanForceZ = new MeanFinder();
  maxForceX = new MaxFilter();
  maxForceY = new MaxFilter();
  maxForceZ = new MaxFilter();
  maxForceAcY = new MaxFilter();
  wasDriving = false;
  InitFolders();
  delay(1000);
}

void playFile(int sfile, char prefix)
{
  char cstr[] = "s000.wav"; 
  cstr[0] = prefix;
  cstr[1] = (char)(48 + sfile / 100);
  cstr[2] = (char)(48 + (sfile % 100) / 10);
  cstr[3] = (char)(48 + sfile % 10);
  cstr[4] = '.';
  cstr[5] = 'W';
  cstr[6] = 'A';
  cstr[7] = 'V';
  if(!SdPlay.setFile(cstr)) // music name file
  {
    Serial.println("sF");
    while(1);
  }
  SdPlay.play(); // play music
}

void loop(void)
{
  if(!SdPlay.isStopped())
    return;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  {
    int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
    int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
    accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
    Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
    gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
    gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
    gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
    
    meanForceX->Feed(gyro_x);
    meanForceY->Feed(gyro_y);
    meanForceZ->Feed(gyro_z);
    {
      int envForce = abs(meanForceX->Get()) + abs(meanForceY->Get());
      meanForces->Feed(envForce);
      if(envForce > 900)
        meanForces->Feed(envForce);
      if(envForce > 1000)
        meanForces->Feed(envForce);
      /*if(envForce > 600)
        meanForces->Feed(envForce);
      if(envForce > 700)
        meanForces->Feed(envForce);
      if(envForce > 700)
        meanForces->Feed(envForce);*/
      if(meanForces->mean > IS_CLEANING_EDGE + 50)
        meanForces->mean = IS_CLEANING_EDGE + 50;
    }
    maxForceX->Feed(abs(meanForceX->Get()));
    maxForceY->Feed(abs(meanForceY->Get()));
    maxForceZ->Feed(abs(meanForceZ->Get()));
    maxForceAcY->Feed(abs(accelerometer_y));

    Serial.print(meanForces->mean);
    Serial.print(" ");
    Serial.print(IS_CLEANING_EDGE);
    Serial.print(" ");
    Serial.print(abs(meanForceX->Get()) + abs(meanForceY->Get()));
    Serial.print(" ");
    Serial.print(gyro_x);
    Serial.print(" ");
    Serial.print(gyro_y);
    Serial.print(" ");
    Serial.print(meanForceX->mean);
    Serial.print(" ");
    Serial.print(meanForceY->mean);
    Serial.print(" ");
    Serial.print(wasDriving * 700);
    Serial.println(" ");
  }
  if(millis() % 1 == 0) // Feed all Finders and Filters for 1 milisec (Not really waiting its always true)
  {
    int forceX = maxForceX->Get();
    int forceY = maxForceY->Get();
    int forceZ = maxForceZ->Get();
    int accY = maxForceAcY->Get();
    meanForceImpact->Feed(abs(forceX) / 2.0 + abs(accY) / 2.0);
    int forceImpact = meanForceImpact->Get();
    int impact = max(0, forceImpact - abs(forceZ));

    if((millis() - lastImpact) < 1500) // Only play songs every 1.5 seconds
      return;
    
    if(wasDriving && meanForces->mean > IS_CLEANING_EDGE && impact > 3500) // Collision
    {
      Serial.print("c: "); Serial.println(impact);
      lastImpact = millis();
      byte sfile = OnCollision->GetRandomSoundfile();
      playFile(sfile, OnCollision->prefix);
      return;
    }

    if(wasDriving && meanForces->mean < IS_CLEANING_EDGE) // Finished cleaning
    {
      Serial.println("fin");
      lastImpact = millis();
      wasDriving = false;
      byte sfile = AfterEnd->GetRandomSoundfile();
      playFile(sfile, AfterEnd->prefix);
      return;
    }

    if(!wasDriving && meanForces->mean > IS_CLEANING_EDGE) // Started cleaning
    {
      Serial.println("sta");
      lastImpact = millis();
      wasDriving = true;
      byte sfile = AfterStart->GetRandomSoundfile();
      playFile(sfile, AfterStart->prefix);
      return;
    }
  }
}
