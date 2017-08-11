//Arduino program for the "A.R.M.I.N.A.T.O.R"
//motors used: 9 gram servo motor; SG-90
//each motor can handle a weight of appx 140 grams
//red wire: 5v power supply
//brown wire: Ground
//yellow wire: pwm pin

#include<Servo.h>     //Adds the library containing fuctions for operating servo motor
#include<math.h>      //Adds the library for mathematical functions

//We define objects and variables to be used before the 'setup()' function.
//We use names instead of pin number in order to make it easier to remember.

int shoulderPin = 9;    //Assigning pin 9 for the first pivot motor.
int elbowPin = 10;      //Assigning pin 10 for the second motor.
int wristPin = 7;       //Assigning pin 7 for the final motor.
int ack=99;             //Acknowledgment signal to MATLAB.

//We will be using the horizontal and vertical distance of the current location from the
//pivot point to obtain required angles

int horizontal, vertical, absHorizontal ;     //Kind of given ,,,+1
int diagonal;                 //To be calculated
int servoDown, servoUp;       //For Wrist servo.(constants)

double upperArmLength, lowerArmLength; //Absolutely constant
double shoulderAngle, elbowAngle, diagonalAngle;      //Most important ones for our project.
int verticalAngle;                                   //(deg)Another constant for calliberation
int angleShoulder, angleElbow, angleDiagonal;         //The ones we will be using will be in int.

Servo servoShoulder, servoElbow, servoWrist;       //These are objects using which we will control servos.

int imageData;                //Data recieved from matlab will be stored in this.

 //The setup() function contains commands that need to run only once.
 
 void setup()
 {
  Serial.begin(9600);     //Sets the speed of data being sent/recieved.
  servoShoulder.attach(shoulderPin);  //Basically assigns pin 9 to this servo object
  servoElbow.attach(elbowPin);        //Almost same as above
  servoWrist.attach(wristPin);
 }

 //The loop() function contains commands that need to run repeatedly.
 void loop()
 {
  while(Serial.available()>0)  //A loop that will only run when data is available on port
  {
   
    recieve:
    imageData = Serial.read(); //Read the data on the port and store it.
    if(imageData = 10)
    {
      continue;             //if garbage value is recieved from matlab, skip the loop and continue.
    }

    // 'spot' contains set of commands to calculate the position
    spot:
    horizontal = absHorizontal-1;   //reduce the horizontal distance from pivot
    if(horizontal==0)
    {
      vertical--;                  //bring the pen lower as the line ends
      horizontal = absHorizontal;  //and reinitialise the horizontal distance.
    }
   
    check:
    if(imageData==1) //if the image is white at that point
    {
      goto ack;      //skip the rest of the loop and proceed with next data.
    }

    //At this point we have recieved the data, calculated the position of the pen
    //and confirmed that we have to darken that location
    //Now we will calculate the diagonal and angles.

    diagonal = sq(horizontal) + sq(vertical);
    diagonal = sqrt(diagonal);

    //Now the sides of the triangle are stored in upperArmLength, lowerArmLength
    //and diagonal. Angles can be calculated as
    //refer to: http://www.calculator.net/triangle-calculator.html for the formula used below.
    shoulderAngle = acos((sq(upperArmLength)+sq(diagonal)-sq(lowerArmLength))/(2*upperArmLength*diagonal));
    elbowAngle = acos((sq(lowerArmLength)+sq(upperArmLength)-sq(diagonal))/(2*lowerArmLength*upperArmLength));

    //The result obtained is in 'double' and in radians,
    //converting it to 'int' and degrees.
    shoulderAngle = shoulderAngle*57296/1000;
    elbowAngle = elbowAngle*57296/1000;

    angleShoulder = (int)shoulderAngle;
    angleElbow = (int)elbowAngle;

    //This information needs to coordinated with a couple others in order to get the
    //shoulder servo to the required position. We have enough info for Elbow servo.
    //First we get the angle of diagonal from the vertical, by using the same triangle calculator
    //in the triangle made by vertical, horizontal and diagonal.

     diagonalAngle = acos((sq(vertical)+sq(diagonal)-sq(horizontal))/(2*vertical*diagonal));
     diagonalAngle = diagonalAngle*57296/1000;
     angleDiagonal = (int)diagonalAngle;

     //Now substracting this value from angleShoulder
     //and adding to the vertical angle we will get the final required value.

     angleShoulder = angleDiagonal-angleShoulder+verticalAngle;

    //Now we have all the parameters required to turn the servos!
    servoShoulder.write(angleShoulder);
    servoElbow.write(angleElbow);

    //Since the working of wrist motor is undecided, that part remains.
    //All we need to do is for it to come down and go back up at this point.
    delay(500);
    servoWrist.write(servoDown);
    delay(500);
    servoWrist.write(servoUp);

    //Acknowledgement to MATLAB, so that it sends next data.
    ack:
    Serial.println(ack);
   
    //Tada!
 }
 }
