#ifndef _WIICAM_H
#define _WIICAM_H

// Wii Remote IR sensor  test sample code  by kako
// modified output for Wii-BlobTrack program by RobotFreak

#define WIICAM_SLAVE_ADDRESS 0x58
class WiiCam : public I2CPeripheral
{
  public:
    WiiCam()
    {
      setInterval(10); // wii detect interval
      lastDetected.setInterval(300); // how long before we lose sight of ball
      
      for (i=0; i<3; i++)
      {
        Ix[i] = 0;
        Iy[i] = 0;
        avgIx[i] = 0;
        avgIy[i] = 0;
      }
      avgX = 0;
      avgY = 0;
    }
    
    unsigned int Ix[4];
    unsigned int Iy[4];

    unsigned int avgIx[4];
    unsigned int avgIy[4];
    
    unsigned int avgX;
    unsigned int avgY;
    
    uint8_t printres(char * str, uint8_t res)
    {
      Serial.print(str);
      Serial.println(res, HEX);
    }  
    void setup()
    {
        const byte sequence1[]= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09 };
        const byte sequence2[]= { 0x00, 0x41 };
        const byte sequence3[]= { 0x40, 0x00 };

        Serial.println("Initializing PixArt Camera...");

        // MUST BE BLOCKING CALLS
        // IR sensor initialize
        
		I2c.write(WIICAM_SLAVE_ADDRESS, 0x30,0x01); delay(10);
        I2c.write(WIICAM_SLAVE_ADDRESS, 0x00,sequence1, sizeof(sequence1));
        delay(10);
        I2c.write(WIICAM_SLAVE_ADDRESS, 0x07,sequence2, sizeof(sequence2));
        delay(10);
        I2c.write(WIICAM_SLAVE_ADDRESS, 0x1A,sequence3, sizeof(sequence3));
        delay(10);
        I2c.write(WIICAM_SLAVE_ADDRESS, 0x33,0x33); 
        delay(10);
        I2c.write(WIICAM_SLAVE_ADDRESS, 0x30, 0x08);
        Serial.println("PixArt Camera Initialized");
        delay(100);
    }

	
    void onUpdate()
    {
        I2c.write(WIICAM_SLAVE_ADDRESS,0x36);
        
        I2c.read(WIICAM_SLAVE_ADDRESS, 16, data_buf);
        byte j;
        int found = 0;
        unsigned int tmpX = 0;
        unsigned int tmpY = 0;
        
        for (i = 0; i<4; i++)
        {
          j = 3*i+1;
          Ix[i] = data_buf[j];
          Iy[i] = data_buf[j+1];
          s   = data_buf[j+2];
          Ix[i] += (s & 0x30) <<4;
          Iy[i] += (s & 0xC0) <<2;
          
          if ( Ix[i] >0 && Ix[i] < 1023 && Iy[i] >0 && Iy[i] < 1023)
          {
            found++;
            avgIx[i] = (7*avgIx[i] + Ix[i])/8;
            avgIy[i] = (7*avgIx[i] + Ix[i])/8;
            tmpX += Ix[i];
            tmpY += Iy[i];
          }
        }
        
        if (found > 0)
        {
          lastDetected.reset();
          found += 8;
          avgX = (8 * avgX + tmpX)/found;
          avgY = (8 * avgY + tmpY)/found;
		  
		  newDataAvailable();
        } else if (lastDetected.timeout())
        {
          avgX = 0;
          avgY = 0;
          for (i = 0; i<4; i++)
          {
            avgIx[i] = 0;
            avgIy[i] = 0;
          }
		  newDataAvailable();
        }
      }
    }  

    void printNum(int num)
    {
          if (num < 1000)
            Serial.print(" ");
          if (num < 100)  
            Serial.print(" ");
          if (num < 10)  
            Serial.print(" ");
          Serial.print(num);
    }
    
    void log()
    {  
        for(i=0; i<4; i++)
        {
          printNum(Ix[i]);
          Serial.print(",");
          printNum(Iy[i]);
          Serial.print(",");
        }
        Serial.print("||,");
        for(i=0; i<4; i++)
        {
          printNum(avgIx[i]);
          Serial.print(",");
          printNum(avgIy[i]);
          Serial.print(",");
        }
        Serial.print("||,");
        printNum(avgX);
        Serial.print(",");
        printNum(avgY);
        
        Serial.println("");
   }
    
  private:
  
    Stopwatch lastDetected;
    byte data_buf[16];
    int i;
    
    int s;
};

#endif
