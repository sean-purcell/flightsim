#### Ideas
- use booster pack as controller/joystick for Google Earth flight sim
  - can control pitch and roll
- output text to 128x32 screen on booster pack?
  - status messages / input mode indicator?
- flight simulator with arbitrary planet atmosphere settings (see: https://what-if.xkcd.com/30/)
- orbital simulator
- sliding window over 3 axes and infer vertical component from checking that variance of all axes is close to 0 and magnitude approaches 9.81 ms^-1 (or ~256 for full resolution ADXL345 reading)
- Note: cannot return boost::asio::serial_port by value because it contains private basic_io_object variables and cannot be copied. 
 