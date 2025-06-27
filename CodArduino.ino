#include <Servo.h>

/// Pini sezor culoare 
const int S0 = 4; 
const int  S1 = 5;
const int  S2 = 10;
const int  S3 = 7;
const int  sensorOut = 8; //PWM senzor culoare

const int ir_red = 2; // Pin IR

Servo servo_red; // Pin SERVO

// Pini motor DC
const int motor_pin1 =11;
const int motor_pin2 = 12;
const int motor_enable = 6;  //PWM motor DC

int SensorState = 0;
int LastSensorState = 0;


bool red_aprins = false;
bool green_aprins = false;
bool blue_aprins = false;

int red = 0;
int green = 0;
int blue = 0;
int objectCount=0;
int color = 0;
bool motorReady = false;  // 🔴 Control pornire banda

void setup() 
{
 pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  pinMode(motor_enable, OUTPUT);
  pinMode(motor_pin1, OUTPUT);
  pinMode(motor_pin2, OUTPUT);

  pinMode(ir_red, INPUT);
  
  servo_red.attach(9);

  Serial.begin(9600);
}

void loop() {
  // 🔄 Ascultă Serial de la ESP
  if (Serial.available()) 
  {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg == "Obiect 1 a fost amplasat!") 
    {
      motorReady = true;
      Serial.println("Arduino: Pornesc motorul!");
    } 
    else 
    {
      motorReady = false;
      stopMotor();
      Serial.println("Arduino: Oprire motor – mesaj necunoscut.");
    }
  }

  if (!motorReady) 
  {
    stopMotor();
    return;
  }

  servo_red.write(0);
  startMotor();

  color = readColor();

  switch (color)
  {
    //culoarea rosu
    case 1: 
    {
      SensorState = digitalRead(ir_red);
      if(SensorState != LastSensorState){
        if(SensorState == LOW)
        {
          stopMotor();
          servo_red.write(180);
          delay(2000);
          servo_red.write(0);
          color = 0;
          objectCount++;
          Serial.print("rosu,");
          Serial.println(objectCount);
        }
        delay(50);
      }
      LastSensorState = SensorState;
    }
    break;
//culoarea verde
    case 2:
      SensorState = digitalRead(ir_red);
      if (SensorState != LastSensorState) 
      {
        do 
        {
           servo_red.write(180);
          startMotor();
        } while (digitalRead(ir_red) == HIGH);

        if (SensorState == LOW) 
        {
          stopMotor();
          servo_red.write(0);
          delay(2000);
          color = 0;
          objectCount++;
          Serial.print("verde,");
          Serial.println(objectCount);
        }
        delay(50);
      }
      LastSensorState = SensorState;
      break;

    //culoarea albastru
 case 3: 
    {
      SensorState = digitalRead(ir_red);
      if(SensorState != LastSensorState)
      {
        if(SensorState == LOW)
        {
          objectCount++;
          Serial.print("albastru,");
          Serial.println(objectCount);
          servo_red.write(0);
          color = 0;
          
        }
        delay(50);
      }
      LastSensorState = SensorState;
    }
    break;

    case 0: break;
  }
 
}

void startMotor() 
{
  digitalWrite(motor_pin1, HIGH);
  digitalWrite(motor_pin2, LOW);
  analogWrite(motor_enable, 150);
}

void stopMotor() 
{
  digitalWrite(motor_pin1, HIGH);
  digitalWrite(motor_pin2, HIGH);
}

int readColor() 
{
//Setări culoarea rosu
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  red = pulseIn(sensorOut, LOW);


//Setări culoarea verde
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  green = pulseIn(sensorOut, LOW);


  
//Setări culoarea albastru
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blue = pulseIn(sensorOut, LOW);
if(red>100 && blue>130 && green > 125)
{
    //Cazul în care nu este identificată nicio culoare din cele 3 culori
    red_aprins = false;
    green_aprins = false;
    blue_aprins = false;
}
  else if(red < blue && red < green)//detectare rosu red > 0 && red < 36 && green > 180 && green < 215 && blue > 36 && blue <55
  { 
    color = 1;
    if(!red_aprins)//controleaza sa nu se opreasca la culoarea constanta, si sa se opreasca doar odata;
    { 
      //oprire motor
      digitalWrite(motor_pin1, HIGH); 
      digitalWrite(motor_pin2, HIGH);
      red_aprins = true;
      delay(2000); // asteapta 2 secunde sub senzor
    }
  }
   else if(green < red && green < blue)//detectare verde red > 117 && red < 139 && green > 74 && green < 95 && blue > 28 && blue <40
   { 
    color = 2;
    if(!green_aprins)
    {
      digitalWrite(motor_pin1, HIGH);
      digitalWrite(motor_pin2, HIGH);
      green_aprins = true;
      delay(2000);
    }
  }
  else if(blue < red && blue < green)//detectare albastru red > 50 && red < 60 && green > 25 && green < 40 && blue > 15 && blue <35
  { 
    color=3;
    if(!blue_aprins)
    {
      digitalWrite(motor_pin1, HIGH);
      digitalWrite(motor_pin2, HIGH);
      blue_aprins = true;
      delay(2000);
    }
  }
return color;
}