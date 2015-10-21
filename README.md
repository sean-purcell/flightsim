se101-lab
======

#### To do
1. Flight simulator. (Jasper, Sean)
2. Launchpad integration. (Neerajen)
	- Serial port listener program (C++?)
	- Serial port sender program (ino)
	- Encoding to bytes / decoding from bytes
	- Data transmission rate?
	- How to deal with packet loss
3. Angles maths.
	- Intepreting accelerometer input (16-bit twos complement?)
	- Determining orientation in space
	- How to map orientation to input (linear? nonlinear?)
4. Javarobot
	- Serial port stuff might overlap with Launchpad integration
	- Maths: angles -> VK fire rates
	- Would allow control of other flight sims where we can't invoke functions directly
5. Documentation
	- Annotate stuff
6. Flowchart
	- Flowcharts are cool
	- Also helps to make sense of who's doing what goes where in a large project
7. Testing and debugging
	
#### Finished
1. Write proposal.
	- Full name, ID numbers, and quest IDs
	- Timeline showing breakdown of milestones leading up to project completion.
	- Detailed description of hardware components
	- Expected challenges

#### Schedule
- Oct 16, 2015 23:59: Proposal due.
- Oct 26, 2015: Launchpad output demo "due"
- Oct 26, 2015: Javarobot demo "due"
- Oct 29, 2015: Booster pack angle demo "due"
- Oct 29, 2015: Proposal feedback.
- Nov 02, 2015: C++ serial port reader demo "due"
- Nov 26, 2015: Project demo due.

#### Dependencies
- Energia 16 (Launchpad Tiva C TM4C123): http://energia.nu/download/
- Orbit Booster Pack (Accelerometer and other stuff): https://www.digilentinc.com/Products/Detail.cfm?NavPath=2,396,1181&Prod=ORBIT-BOOSTER or https://learn.uwaterloo.ca/d2l/le/content/220517/viewContent/1314193/View?ou=220517
- SFML 2.3.2 (C++ graphics): http://www.sfml-dev.org/download/sfml/2.3.2/

#### References
- Lab 2 Specifications: https://learn.uwaterloo.ca/d2l/le/content/220517/viewContent/1313255/View?ou=220517
- TM4C123 Guide: http://energia.nu/pin-maps/guide_tm4c123launchpad/
- ADXL345 Data Sheet: http://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf
- ADXL345 Angle Maths: http://www.analog.com/media/en/technical-documentation/application-notes/AN-1057.pdf
