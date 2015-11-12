/** Minimal working example of a serial writer on the Launchpad side.
 * Writes a byte to serial port and increments the counter every interval milliseconds.
 */

int baud = 9600;
int interval = 50;
short counter = 0;

void setup()
{
  Serial.begin(baud);  
}

void loop()
{
  short a; 
  Serial.write( (byte)((counter & 0xFF00) >> 8));
  Serial.write((byte)(counter++ & 0x00FF));
  delay(interval);  
}
