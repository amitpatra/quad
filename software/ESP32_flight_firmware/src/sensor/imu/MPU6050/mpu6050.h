#include "../imu.h"

#pragma once

// Use 9-axis motion apps if 3D-magnetometer is available

#ifdef HAS_MAGNETOMETER
    #include "MPU6050_9Axis_MotionApps41.h"
#else
    #include "MPU6050_6Axis_MotionApps612.h"
#endif

class IMU_mpu6050 {

    public:

    bool initialize(){

        Wire.begin(); delay(10);
        Serial.println("Wire Initialized");

        mpu.initialize();
        Serial.println("MPU Initialized");
        devStatus = mpu.dmpInitialize();
        if (devStatus == 0){
            Serial.println("devStatus : GOOD");
            mpu.CalibrateAccel(6);
            mpu.CalibrateGyro(6);
            mpu.setDMPEnabled(true);
            mpu.setDLPFMode(MPU6050_DLPF_BW_188);
            return true;
        } else {
            Serial.println("devStatus : FAIL");
            return false;
        }
    }
    
    bool isUpdated(){

        uint8_t fifoBuffer[64];
        if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)){

            VectorInt16 acc;
            VectorInt16 gyro;
            VectorFloat grav;
            timestamp = millis();

            mpu.dmpGetQuaternion(&quaternion, fifoBuffer);
            mpu.dmpGetAccel(&acc, fifoBuffer);
            mpu.dmpGetGravity(&grav, &quaternion);
            mpu.dmpGetGyro(&gyro, fifoBuffer);
            mpu.dmpGetYawPitchRoll(ypr,&quaternion,&grav);

            accelerations = acc.getFraction(1668.43);
            angular_rates = gyro.getFraction(939.65);

            return true;
        } else { return false; }
    }

    unsigned long getTimestamp(){
        return timestamp;
    }

    VectorFloat getAccel () {
        return accelerations;
    }

    VectorFloat getGyro () {
        return angular_rates;
    }

    VectorFloat getPry () { return VectorFloat{ypr[1],ypr[2],ypr[0]}; }

    Quaternion getQuaternion () {
        return quaternion;
    }

    bool statusGood () { return (devStatus == 0) ? true : false; }

    private:
    
    uint8_t devStatus;
    unsigned long timestamp;
    float ypr [3];
    Quaternion quaternion;
    VectorFloat angular_rates;
    VectorFloat accelerations;
    MPU6050 mpu;

};