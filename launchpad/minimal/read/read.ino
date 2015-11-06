/** Minimal working example of a serial reader on the Launchpad side. 
 * Reads bytes from serial port as they arrive.
 * Sets green LED to a brightness corresponding to magnitude of byte.
 */ 
 
int baud = 9600;
int brightness = 0;

void setup()
{
  pinMode(GREEN_LED, OUTPUT);
  Serial.begin(baud);  
}

void loop()
{
  if (Serial.available())
  {
    brightness = Serial.read();
    analogWrite(GREEN_LED, brightness);
  }
  delay(1);  
}
