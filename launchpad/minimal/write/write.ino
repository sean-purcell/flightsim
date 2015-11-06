/** Minimal working example of a serial writer on the Launchpad side.
 * Writes a byte to serial port and increments the counter every interval milliseconds.
 */

int baud = 9600;
int interval = 50;
unsigned char counter = 0;

void setup()
{
  Serial.begin(baud);  
}

void loop()
{
  Serial.write(counter++);
  delay(interval);  
}
