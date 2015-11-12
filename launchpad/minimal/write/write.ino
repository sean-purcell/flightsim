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
  writeShort(counter++);
  delay(interval);  
}

void writeShort(short a)
{
  Serial.write(0xFF);  // Marker for alignment to prevent read.cpp from desyncing with byte stream
  Serial.write((byte)(a >> 8));
  Serial.write((byte)(a));
}
