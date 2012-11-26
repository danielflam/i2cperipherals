#ifndef _HMC5883_H
#define _HMC5883_H

#define HMC5883_ADDRESS 0x1E 

class HMC5883 : public I2CPeripheral
{
  public:
    HMC5883(int interval = 66)
    {
       setInterval(interval);
    }

	// Yes C++ ugly - but efficient for arduino
    int X, Y, Z;
    int avgX, avgY, avgZ;
   
    void setup()
    {
        Serial.println("Starting up HMC5883...");
        delay(50);
        I2c.write(HMC5883_ADDRESS, 0x02, 0x00);// continuous mode 
        delay(50);
        I2c.write(HMC5883_ADDRESS, 0x03);
        Serial.println("HMC5883 Initialized");
        avgX=0;
        avgY=0;
        avgZ=0;
    }

    void onUpdate()
    {
		byte buf[6];

         // Read data from the sensor
         
         I2c.write(HMC5883_ADDRESS, 0x03);
         I2c.read(HMC5883_ADDRESS, 6, buf);

         X = (buf[0] <<= 8) | buf[1];
         Y = (buf[2] <<= 8) | buf[3];
         Z = (buf[4] <<= 8) | buf[5];

         avgX = (avgX + X) >> 1; // divide by 2
         avgY = (avgY + Y) >> 1; // divide by 2
         avgZ = (avgZ + Z) >> 1; // divide by 2
         
         newDataAvailable();
    }  

	void log()
	{
         Serial.print("HMC5883: ");
         //for (int i=0;i<6;i++)
         //{
         //  Serial.print(buf[i], HEX);
         //  Serial.print(" ");           
         //}
         Serial.print(X);           
         Serial.print(" ");           
         Serial.print(Y);           
         Serial.print(" ");           
         Serial.print(Z);           
         Serial.print(" ");           
         Serial.print(avgX);           
         Serial.print(" ");           
         Serial.print(avgY);           
         Serial.print(" ");           
         Serial.print(avgZ);           
         Serial.println();
	}
  
  private:
};


#endif

