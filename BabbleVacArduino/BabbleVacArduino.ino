#include <SimpleSDAudio.h>
#include "SoundFolder.h"
#include "AccumulationFilter.h"
#include "Wire.h" // This library allows you to communicate with I2C devices.
const int MPU_ADDR = 0x68; // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z; // variables for gyro raw data
int16_t temperature; // variables for temperature data
int maximpact;
int maxupright;
char tmp_str[7]; // temporary variable used in convert function
int av = 0;
int lastFiltered = 0;
int gMin = 0, gMax = 0;

SoundFolder* AfterStart;
SoundFolder* AfterEnd;
SoundFolder* OnCollision;
SoundFolder* WhileCleaning;
SoundFolder* LiftUp;
AccumulationFilter* accFilter;
AccumulationFilter* accFilter2;
AccumulationFilter* accFilter3;

/*
 * 100 als Grenze für aufheben. gyro_y
 * 
 */

char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void InitFolders()
{
  Serial.println("CREATING FOLDERS!!!");
  AfterStart = new SoundFolder('S');
  AfterEnd = new SoundFolder('E');
  OnCollision = new SoundFolder('C');
  WhileCleaning = new SoundFolder('W');
  LiftUp = new SoundFolder('L');
}

void Add(char* file)
{
  switch(file[0])
  {
    case 'S': AfterStart->Add(file);
    break;
    case 'E': AfterEnd->Add(file);
    break;
    case 'C': OnCollision->Add(file);
    break;
    case 'W': WhileCleaning->Add(file);
    break;
    case 'L': LiftUp->Add(file);
    break;
    default: Serial.print("File is in no folder: "); Serial.println(file);
    break;
  }
}

void dir_callback(char *buf) {
  Add(buf);
}


void setup() {
  Serial.begin(115200); 
  Serial.println("Started");
  SdPlay.setSDCSPin(10); // sd card cs pin
  Serial.println("setSDCSPin DONE");
  if (!SdPlay.init(SSDA_MODE_FULLRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER))
  {
    Serial.println("SdPlay.init FAIL");
    while(1);
  }
  Serial.println("SdPlay.init DONE");
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
  Serial.println("gyro Init Start");
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Serial.println("gyro Init Start2");
  Wire.endTransmission(true);
  Serial.println("gyro Init DONE");
  maximpact = 0;
  //InitFolders();
  Serial.println("Folder Init DONE");
 // SdPlay.dir(&dir_callback);
  Serial.println("Folder Fill Called");
  accFilter = new AccumulationFilter(500, true);
  accFilter2 = new AccumulationFilter(10, false);
  accFilter3 = new AccumulationFilter(500, true);
  /*delay(1000);
  Serial.print("GO ");
  delay(1000);
  Serial.print("GO ");
  delay(1000);
  Serial.println("GO ");*/
}

void playFile(char* sfile)
{
  char cstr[4+1+3+1]; // No idea why. For real if you find out why i have to do this... e-mail me!
  cstr[0] = sfile[0];
  cstr[1] = sfile[1];
  cstr[2] = sfile[2];
  cstr[3] = sfile[3];
  cstr[4] = '.';
  cstr[5] = 'W';
  cstr[6] = 'A';
  cstr[7] = 'V';
  cstr[8] = 0;
  if(!SdPlay.setFile(cstr)) // music name file
  {
    Serial.println("SdPlay.setFile FAIL");
    while(1);
  }
}

void loop(void)
{
  /*char* sfile = AfterStart->GetRandomSoundfile();
  Serial.println(sfile);
  return;*/
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7*2, true); // request a total of 7*2=14 registers
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)

  int impact = abs(gyro_x) / 3.0 + abs(gyro_y) / 3.0 + abs(gyro_z) / 3.0;
  if(impact > maximpact)
  {
    //Serial.println(impact);
    maximpact = impact;
  }
  int maybeupright = abs(accelerometer_z);
  //Serial.println(maybeupright);
  if(maybeupright > maxupright)
  {
    maxupright = maybeupright;
    //Serial.println(maxupright);
    
  }
//    Serial.print("aX = "); Serial.print(convert_int16_to_str(accelerometer_x));
//  Serial.print(" | aY = "); Serial.print(convert_int16_to_str(accelerometer_y));
//  Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(accelerometer_z));
//  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
//  Serial.print(" | tmp = "); Serial.print(temperature/340.00+36.53);
//  Serial.print(" | gX = "); Serial.print(convert_int16_to_str(gyro_x));
//  Serial.print(" | gY = "); Serial.print(convert_int16_to_str(gyro_y));
//  Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(gyro_z));
//  Serial.println();


  /*Serial.print(accelerometer_x);
  Serial.print(" ");
  Serial.print(accelerometer_y);
  Serial.print(" ");
  Serial.print(accelerometer_z);
  Serial.print(" ");*/

  /*Serial.print(accelerometer_z);
  Serial.print(" ");
  Serial.print(impact);
  Serial.print(" ");
  Serial.print(20000);
  Serial.print(" ");
  Serial.print(-20000);
  Serial.println(" ");*/


//Serial.print(convert_int16_to_str(gyro_x));
//  Serial.print(" "); Serial.print(convert_int16_to_str(gyro_y));
//  Serial.print(" "); Serial.println(convert_int16_to_str(gyro_z));

  //Serial.print(convert_int16_to_str(accelerometer_x));
  av = 0.01 * gyro_x + (1-0.01) * av;
  
  accFilter->Feed(av);
  if(accFilter->IsReady())
  {
    lastFiltered = accFilter->Empty();
    if(lastFiltered > gMax)
      gMax = lastFiltered;
    if(lastFiltered < gMin)
      gMin = lastFiltered;
    /*accFilter2->Feed(accFilter->Empty());
    if(accFilter2->IsReady())
    {
      accFilter3->Feed(accFilter2->Empty());
      if(accFilter3->IsReady())
      {
        lastFiltered = accFilter3->Empty();
        if(lastFiltered > gMax)
          gMax = lastFiltered;
        if(lastFiltered < gMin)
          gMin = lastFiltered;
      }
    }*/
  }
  Serial.print(1000);
  Serial.print(" ");
  Serial.print(-1000);
  Serial.print(" ");
  Serial.print(gMax);
  Serial.print(" ");
  Serial.print(gMin);
  Serial.print(" ");
  Serial.print(av);
  Serial.print(" ");
  Serial.println(lastFiltered);
  //Serial.print(" ");
  //Serial.print(av);
  //Serial.print(" ");
  //Serial.println(gyro_y);
  //Serial.print(" "); Serial.println(gyro_y);
  //Serial.print(" "); Serial.println(convert_int16_to_str(accelerometer_z));


  /*if(impact > 1000)
  {
    SdPlay.play(); // play music
    while(!SdPlay.isStopped())
    { ;
    }
  }*/
  
}
