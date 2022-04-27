  // Arduino Beat Detector By Damian Peckett 2015
  // License: Public Domain.
  
  
  /*
      9 = 167 bpm
      10 = 150 bmp
      11 = 136 bmp
      12 = 125 bpm
      13 = 115 bpm
      14 = 107 bpm
      15 = 100 bpm
  
  */
  
  // Our Global Sample Rate, 5000hz
  #define SAMPLEPERIODUS 200
  
  // defines for setting and clearing register bits
  #ifndef cbi
  #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
  #endif
  #ifndef sbi
  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
  #endif
  
  int counter;
  const int minBeatLength = 9;
  int lastDistance = 10;
  
  const int Out_beatOut = 5;
  const int Out_backup = 7;
  const int Led_Red = 11;
  const int Led_Yellow = 9;
  const int Inp_pot = A3;
  const int Inp_music = A0;
  
  
  void setup() {
      // Set ADC to 77khz, max for 10bit
      sbi(ADCSRA,ADPS2);
      cbi(ADCSRA,ADPS1);
      cbi(ADCSRA,ADPS0);
      Serial.begin(9600);
  
      pinMode(Out_beatOut, OUTPUT);
      pinMode(Out_backup, OUTPUT);
      pinMode(Led_Red, OUTPUT);
      pinMode(Led_Yellow, OUTPUT);
  
  }
  
  // 20 - 200hz Single Pole Bandpass IIR Filter
  float bassFilter(float sample) {
      static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
      xv[0] = xv[1]; xv[1] = xv[2]; 
      xv[2] = (sample) / 3.f; // change here to values close to 2, to adapt for stronger or weeker sources of line level audio  
      
  
      yv[0] = yv[1]; yv[1] = yv[2]; 
      yv[2] = (xv[2] - xv[0])
          + (-0.7960060012f * yv[0]) + (1.7903124146f * yv[1]);
      return yv[2];
  }
  
  // 10hz Single Pole Lowpass IIR Filter
  float envelopeFilter(float sample) { //10hz low pass
      static float xv[2] = {0,0}, yv[2] = {0,0};
      xv[0] = xv[1]; 
      xv[1] = sample / 50.f;
      yv[0] = yv[1]; 
      yv[1] = (xv[0] + xv[1]) + (0.9875119299f * yv[0]);
      return yv[1];
  }
  
  // 1.7 - 3.0hz Single Pole Bandpass IIR Filter
  float beatFilter(float sample) {
      static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
      xv[0] = xv[1]; xv[1] = xv[2]; 
      xv[2] = sample / 2.7f;
      yv[0] = yv[1]; yv[1] = yv[2]; 
      yv[2] = (xv[2] - xv[0])
          + (-0.7169861741f * yv[0]) + (1.4453653501f * yv[1]);
      return yv[2];
  }
  
  float neg_sum = 0;
  int neg_total = 0;
  float pos_sum = 0;
  int pos_total = 0;
  
  void loop() {
      unsigned long time = micros(); // Used to track rate
      float sample, value, envelope, beat, thresh;
      unsigned char i;
  
      for(i = 0;;++i){
          
          int input = analogRead(Inp_music);
          
          // Read ADC and center so +-512

          float bias = 430.f; // convert the value between 1024 en 0 into -512 en +512
          float amplitude = 1.8; // increase the aplitude of the signal
          sample = ((float)input-bias);// applitude versterken
          
          // make the sum of the samples so we can measure that the input signal is high enough
          if(sample > 0){pos_sum = pos_sum + sample; pos_total++;}
          if(sample < 0){neg_sum = neg_sum + sample; neg_total++;}
  
          // Filter only bass component
          value = bassFilter(sample);
  
          // Take signal amplitude and filter
          if(value < 0)value=-value;
          envelope = envelopeFilter(value);
  
          // Every 200 samples (25hz) filter the envelope 
          if(i == 200) {
                  counter = counter + 1;
                  
                  // Filter out repeating bass sounds 100 - 180bpm
                  beat = beatFilter(envelope);
                  
                  Serial.print(beat);   Serial.print("    ");  
                  
                  // print counter for the frames without beat and distance to the last beat
                  //Serial.print(counter);   Serial.print(" ");
                  //Serial.print(lastDistance);   Serial.print(" "); 
                  
                  // input amplitude
                  // difference between the postive average values and the negative average values
                  // this should be 0 -> if not: change the bias value
                  float pos_avg = pos_sum/(float)pos_total;
                  float neg_avg = neg_sum/(float)neg_total;
                  Serial.print(pos_avg + neg_avg);   Serial.print("    "); 
                  // average aplitude of the positive side
                  Serial.print(pos_avg);   Serial.print("   "); 
                  // flash led if input is high enough on both sides
                  if(neg_avg < -100 && pos_avg > 100){digitalWrite(Led_Red, HIGH);} else {digitalWrite(Led_Red, LOW);}
                  
                  // reset counters
                  neg_sum = 0;  neg_total = 0;  pos_sum = 0;  pos_total = 0;
                  
                  // Threshold it based on potentiometer on AN1
                  thresh =  ((float)analogRead(Inp_pot));
                  Serial.println(thresh);
                  
                  // If we are above threshold, light up LED
                  if(beat > thresh) {
                      if(counter >= minBeatLength) {
                        digitalWrite(Out_beatOut, HIGH);
                        digitalWrite(Out_backup, HIGH);
                        digitalWrite(Led_Yellow, HIGH);
                        lastDistance = counter;
                        counter = 0;
                      }
                      else {
                        digitalWrite(Out_beatOut, LOW);
                        digitalWrite(Out_backup, LOW);
                        digitalWrite(Led_Yellow, LOW);
                        
                      }
                  }
                  else {
                    digitalWrite(Out_beatOut, LOW);
                    digitalWrite(Out_backup, LOW);
                    digitalWrite(Led_Yellow, LOW);
                   
                  }
  
                  //Reset sample counter
                  i = 0;
          }
          
          // Consume excess clock cycles, to keep at 5000 hz
          for(unsigned long up = time+SAMPLEPERIODUS; time > 20 && time < up; time = micros()){
            Serial.println("spare");
          }
          
      }  
  }
  
  
