/*
    Library for IMU 10DOF Adafruit Breakout:
    Motion Processing Unit (MPU 9250) and Digital Pressure Sensor (BMP280) 
    
    Based on:
        MPU 9250 Product Specification Revision 1.1
        Document number: PS-MPU-9250A-01 Release date: 06/20/2016
 
        MPU 9250 register map Revision 1.6
        Document number: RM-MPU-9250A-00 Release date: 01/07/2015 
        
        BMP280 BOCH datasheet Revision 1.14
        Document number: BST-BMP280-DS001-11 Release date: 05/05/2015 

*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define PI 3.14159265
#include "imudof.h"
#include "i2c_master.h"
#include "math.h"

I2C_master i2c;
imudof::imudof(){
    i2c.init();
}
imudof::~imudof(){}

bool imudof::testConnection(){
    i2c.readByte(IMUADDR, WHO_AM_I, data);	
    if (data[0]==IMUID){
        return true;
    }
    else{
        return false;
    }
}

void imudof::init(){

       // i2c.writeByte(IMUADDR, USER_CTRL, 0x43);  // Enable FIFO, Enable I2C, Reset FIFO I2C and GYROS 
       // i2c.writeByte(IMUADDR, CONFIG, 0x00);     // FIFO_MODE rewrite fifo register alowed 
                                              // GYRO BDW 8800 Hz, TEMP BDW 4000 Hz 
        i2c.writeByte(IMUADDR, PWR_MGMT_1, 0x81); // Reset internall registers, auto select clock
        i2c.writeByte(IMUADDR, FIFO_EN, 0xFF);    // Write all register data to de FIFO bus at the sample rate 
        i2c.writeByte(IMUADDR, GYRO_CONFIG,0x00); // Gyro scale = 1000 dps
        i2c.writeByte(IMUADDR, ACCEL_CONFIG,0x00);
        //i2c.writeByte(IMUADD, 0x25, 0x8C); //Configuring SLV0 to the Magnetometer 
        //i2c.writeByte(IMUADD, 0x26, 0x03);
        //i2c.writeByte(IMUADD, 0x27, 0xA6);  
        i2c.writeByte(IMUADDR, 0x36,0x80);
        i2c.writeByte(IMUADDR, 0x37,0x02); //Pass through mode ON
        for (int k =0;k<3;k++)
        {
             kAccelC[k]=(char*)malloc(8);
             kGyroC[k] =(char*)malloc(8);
             kMagC[k] = (char*)malloc(8);

             kAccelC[k][7]='\0';
             kGyroC[k][7]='\0';
             kMagC[k][7]='\0';
        }    
         angleC = (char*)malloc(8);
         angleC[7] = '\0';
}

void imudof::readData()
{
    i2c.readBytes(IMUADDR, 0x3B, 14, data);  
    kAccel[0] = (((int16_t)data[0]) << 8) | data[1]; //x
    kAccel[1] = (((int16_t)data[2]) << 8) | data[3]; //y 
    kAccel[2] = (((int16_t)data[4]) << 8) | data[5]; //z

    kGyro[0] = (((int16_t)data[8])  << 8) | data[9]; //x
    kGyro[1] = (((int16_t)data[10]) << 8) | data[11];//y
    kGyro[2] = (((int16_t)data[12]) << 8) | data[13];//z
    
    i2c.readByte(0x0C, 0x00, data);
    if(data[0]==0x48){
        i2c.readBytes(0x0C, 0x03, 6, data);
        kMag[0] = (((int16_t)data[1]) << 8) | data[0]; //x
        kMag[1] = (((int16_t)data[3]) << 8) | data[2]; //y
        kMag[2] = (((int16_t)data[5]) << 8) | data[6]; //z
    }

    for (int k=0;k<3;k++)
    {
    // kAccelF[k] =acos((float)kAccel[k] / (1<<15 -1))*180/PI    ;
    if (-1<((float)kAccel[k])/(1<<15 -1) &&((float)kAccel[k])/(1<<15 -1) <1 )
        kAccelF[k] = acos(((float)kAccel[k])/(1<<15 -1))*180/PI;
    else 
       if(((float)kAccel[2])/(1<<15 -1) <0) kAccelF[k]=180;
        else kAccelF[k]=0;
       // kGyroF[k] = ((float) kGyro[k]/ 16384 )*90;
     // kGyroF[k] = (float) kGyro[k] *250/32768;
      dtostrf(kAccelF[k],7, 3, (char*)kAccelC[k]);
      dtostrf(kGyroF[k],7, 3, (char*)kGyroC[k]);
      dtostrf(kMag[k],7, 3,   (char*)kMagC[k]);
    }
    if (kAccelF[1] < 90.0 && kAccelF[2] < 90.0){
      angleF = kAccelF[1]-90.0; 
      dtostrf(angleF,7, 3, (char*)angleC);
   }else if (kAccelF[1] < 90.0 && kAccelF[2] > 90.0){
      angleF = kAccelF[1]-90.0; 
      dtostrf(angleF,7, 3, (char*)angleC);
   }else if (kAccelF[1] > 90.0 && kAccelF[2] < 90.0){
      angleF = kAccelF[2];
      dtostrf(angleF,7, 3, (char*)angleC);
   }else{
      angleF = kAccelF[2];
      dtostrf(angleF,7, 3, (char*)angleC);
   } 
}
