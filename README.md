# ArduinoDMX

This code snippit wil output a DMX signal on the TX pin of the arduino (pin 1)
DMX.write(A, B) will give any DMX address A the value B (between 0 and 255) via 

Once the arduino is working, you need to add a drive chip to convert the ardiuno 0 to 5 volt signal into a -2,5v to 2,5v signal.

You use a driver chip for this: SN75176BP, MAX485, 75176 are some product names that can be used. 

Connect the pins of this 8-pin chip in the following way: 

pin 1: connect to the ground via a resistor of 100 ohm

pin 2: connect to ground

pin 3: connect to 5v

pin 4: this is the data input, so connect tot het TX pin of the arduino (pin 1)

pin 5: connect to 5v

pin 6: DMX pin 2 (this is one of the 2 functional DMX outputs: connect to pin 2 of an XLR connector)

pin 7: DMX pin 3 (this is one of the 2 functional DMX outputs: connect to pin 3 of an XLR connector)

pin 8: DMX pin 1, and connect to the ground


