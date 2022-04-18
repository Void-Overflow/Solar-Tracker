#include <Stepper.h>
#include <Servo.h>
#include <EEPROM.h>

Servo servo;
Stepper myStepper = Stepper(3000, 2, 4, 3, 5); // Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence

int x_pt[] = {A0, A1, A3, A2}; // NW NE SE SW
int lastPos[2] = {0, 0};

int y_pt[] = {A5, A4}; // LOWER UPPER
int lastAngle = 0;

void writeServo(int pos)
{
  if (lastAngle > pos)
  {
    for (int i = lastAngle; i >= pos; i--)
    {
      servo.write(i);
      delay(15);
    }
  }
  else
  {
    for (int i = pos; i <= lastAngle; i++)
    {
      servo.write(i);
      delay(15);
    }
  }

  EEPROM.write(1, pos);
  lastAngle = pos;
}

void setup()
{
  Serial.begin(9600);
  for (int i = 0; i <= 3; i++)
    pinMode(x_pt[i], INPUT);

  myStepper.setSpeed(10);

  // int prevVal = EEPROM.read(0);
  // EEPROM.write(0, 0);
  // myStepper.step(prevVal * 750);

  servo.attach(9);

  lastAngle = EEPROM.read(1);
  writeServo(30);
  lastAngle = 30;
}

void loop()
{
  int G_pt[2] = {0, analogRead(x_pt[0])};
  int Sg_pt[2] = {0, analogRead(x_pt[0])};

  for (int i = 0; i <= 3; i++)
  {
    if (analogRead(x_pt[i]) > G_pt[1])
    {
      G_pt[0] = i;
      G_pt[1] = analogRead(x_pt[i]);
    }
  }

  for (int i = 0; i <= 3; i++)
  {
    if (i == G_pt[0])
    {
      if (i == 0)
      {
        Sg_pt[1] = analogRead(x_pt[i + 1]);
        Sg_pt[0] = 1;
      }
      i++;
    }

    if (analogRead(x_pt[i]) > Sg_pt[1])
    {
      Sg_pt[0] = i;
      Sg_pt[1] = analogRead(x_pt[i]);
    }
  }

  if (G_pt[0] != lastPos[0])
  {
    EEPROM.write(0, G_pt[0]);
    myStepper.step((lastPos[0] < G_pt[0]) ? -750 * (G_pt[0] - lastPos[0]) : 750 * (lastPos[0] - G_pt[0]));

    lastPos[0] = G_pt[0];
    lastPos[1] = Sg_pt[0];
  }
  else
    EEPROM.write(0, G_pt[0]);

  if (analogRead(y_pt[1]) > analogRead(y_pt[0]))
  {
    if (analogRead(y_pt[1]) - analogRead(y_pt[0]) <= 10)
      writeServo(30);
    else
      writeServo(60);
  }
  else if (analogRead(y_pt[0]) > analogRead(y_pt[1]))
  {
    if (analogRead(y_pt[0]) - analogRead(y_pt[1]) <= 10)
      writeServo(30);
    else
      writeServo(0);
  }

  delay(1000);
}
