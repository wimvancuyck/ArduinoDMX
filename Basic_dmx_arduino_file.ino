  #include <ArduinoDMX.h>
  #include <ArduinoRS485.h> // the ArduinoDMX library depends on ArduinoRS485


    /*
        Main setup class
    */
    void setup() {
      
      // initialize the DMX library with the universe size
      if (!DMX.begin(512)) { // DMX universe set to 512, can also be set smaller
        Serial.println("Failed to initialize DMX!");
        while (1); // wait for ever
      }
      
    }
    
    
    
    /*
        Main loop class
    */
    void loop() {
      
      DMX.beginTransmission();
      
      // DMX write (address, value between 0 and 255)
      DMX.write(0, 0); 
      DMX.write(1, 255); 
      DMX.write(2, 0);
        
      DMX.endTransmission();
      
      delay(1);
    }
    
    
    
 
