se101-lab
======

#### To do
1. Flight simulator. (Jasper, Sean)
	- Time clocking
	- HUD
2. Potentiometer as throttle.
3. Testing and debugging
	- Angle maths
	- Joystick response
	- etc.
4. Documentation
	- Annotate stuff	
	
#### Dependencies
- Energia 16 (Launchpad Tiva C TM4C123): http://energia.nu/download/
- Orbit Booster Pack (Accelerometer and other stuff): https://www.digilentinc.com/Products/Detail.cfm?NavPath=2,396,1181&Prod=ORBIT-BOOSTER or https://learn.uwaterloo.ca/d2l/le/content/220517/viewContent/1314193/View?ou=220517
- SFML 2.3.2 (C++ graphics): http://www.sfml-dev.org/download/sfml/2.3.2/
- Boost (serial port stuffs): http://www.boost.org/
	- Note that only a small subset of boost is required by asio. See: http://www.boost.org/doc/libs/1_59_0/doc/html/boost_asio/using.html
	- Building: `b2 --with-system --with-thread --with-date_time --with-regex --with-serialization [toolset=(insert compiler stuffs here; see "5.2.2 Identify Your Toolset" from getting started guide in index.html)] stage`
	
#### References
- Lab 2 specifications: https://learn.uwaterloo.ca/d2l/le/content/220517/viewContent/1313255/View?ou=220517
- TM4C123 guide: http://energia.nu/pin-maps/guide_tm4c123launchpad/
- ADXL345 data sheet: http://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf
- ADXL345 angle maths: http://www.analog.com/media/en/technical-documentation/application-notes/AN-1057.pdf
- Boost and serial port: http://www.webalice.it/fede.tft/serial_port/serial_port.html
- Timeout serial reader: http://www.ridgesolutions.ie/index.php/2012/12/13/boost-c-read-from-serial-port-with-timeout-example
- Quaternions: https://en.wikipedia.org/wiki/Quaternion
- Quaternion rotation intuition: http://math.stackexchange.com/a/1434368/201000
- Tait-Bryan angles: https://en.wikipedia.org/wiki/Euler_angles#Tait.E2.80.93Bryan_angles
- Euler angles: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
- Orientation, Rotation, Velocity and Acceleration, and the SRM: http://www.sedris.org/wg8home/Documents/WG80485.pdf

#### Finished
1. Write proposal.
	- Full name, ID numbers, and quest IDs
	- Timeline showing breakdown of milestones leading up to project completion.
	- Detailed description of hardware components
	- Expected challenges
2. Launchpad integration.
	- Serial port reading program
	- Serial port sending program (ino)
	- Encoding to bytes / decoding from bytes
	- Data transmission rate?
	- How to deal with packet loss
3. Angles maths.
	- Intepreting accelerometer input (16-bit twos complement)
	- Determining orientation in space
	- Mapping acceleration to pitch and roll angle
	
#### Schedule
- Oct 16, 2015 23:59: Proposal due.
- Oct 26, 2015: Launchpad output demo "due"
- Oct 26, 2015: Javarobot demo "due"
- Oct 29, 2015: Booster pack angle demo "due"
- Oct 29, 2015: Proposal feedback.
- Nov 02, 2015: C++ serial port reader demo "due"
- Nov 18, 2015: We didn't really follow this at all, did we. Oh well.
- Nov 26, 2015: Project demo due.
