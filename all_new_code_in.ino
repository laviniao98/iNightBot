#include <Servo.h>
#include <LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN 7     
#define DHTTYPE DHT22   // DHT 22 (AM2302)

Servo s;
DHT dht(DHTPIN, DHTTYPE); // Initializam senzorul DHT cu valoarea normala 16mhz Arduino
LiquidCrystal lcd(2, A1, A2, A3, A4, A5);

//"Home" sensors pins and values
int vib_pin=8;
int chk;
float hum;  // valoarea umiditatii
float temp; // valoarea temperaturii
int smokeA0 = A0;
int buzzer = 10;

int sensorThres = 400;

//Servo motor turn values
int servoStraight = 85; 
int servoRight = 0;
int servoLeft = 180;
//Servo motor data pin
int servoPin = 9;

//Motor data pins
int lmotorFwdPin = 6;
int lmotorRevPin = 5;
int rmotorFwdPin = 4;
int rmotorRevPin = 3;

//Motor enable pins for the motor driver
int lmotorEnablePin = 13;
int rmotorEnablePin = 1;

//Motor speed - PWM values that drive both motors
int lmotorFwdSpeed = 255;
int rmotorFwdSpeed = 205;
int lmotorRevSpeed = 255;
int rmotorRevSpeed = 205;

//Ultrasound sensor pins
const int trigPin = 12;
const int echoPin = 11;
//Ultrasound sensor - variables 
long duration;
float distance;

float ldist;
float rdist;

void setup() {
  s.attach(servoPin);
  servo(servoStraight);
  Serial.end();
  //Serial.begin(9600);
  //Serial.println("Obstacle avoider robot by NotesPoint");
  pinMode(lmotorFwdPin, OUTPUT);
  pinMode(lmotorRevPin, OUTPUT);
  pinMode(rmotorFwdPin, OUTPUT);
  pinMode(rmotorRevPin, OUTPUT);
  pinMode(lmotorEnablePin, OUTPUT);
  pinMode(rmotorEnablePin, OUTPUT);
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  pinMode(vib_pin,INPUT);
  
  dht.begin();
  lcd.begin(16,2);
}

void servo(int pos)
{
  s.write(pos);
  delay(1000);
}

void motor(int rmotorFwd, int rmotorRev, int lmotorFwd, int lmotorRev)
{
  if (lmotorFwd == 1)
  {
    digitalWrite(lmotorFwdPin, HIGH);
    analogWrite(lmotorEnablePin, lmotorFwdSpeed);
  }
  else 
  {
    digitalWrite(lmotorFwdPin, LOW);
  }

  if (lmotorRev == 1)
  {
    digitalWrite(lmotorRevPin, HIGH);
    analogWrite(lmotorEnablePin, lmotorRevSpeed);
  }
  else 
  {
    digitalWrite(lmotorRevPin, LOW);
  }

  if (rmotorFwd == 1)
  {
    digitalWrite(rmotorFwdPin, HIGH);
    analogWrite(rmotorEnablePin, rmotorFwdSpeed);
  }
  else 
  {
    digitalWrite(rmotorFwdPin, LOW);
  }

  if (rmotorRev == 1)
  {
    digitalWrite(rmotorRevPin, HIGH);
    analogWrite(rmotorEnablePin, rmotorRevSpeed);
  }
  else 
  {
    digitalWrite(rmotorRevPin, LOW);
  }
}
//
//Stop all motors
//
void still()
{
  motor(0, 0, 0, 0);
}

//
//Calculates current distance from the obstacle
//
int calcDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.0343 / 2;
  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(distance);

  return distance;
}

void loop() {
  if (calcDistance() > 15.0)
  {
    motor(1, 0, 1, 0); // Both Motors Forward
  }
  else //if (calcDistance() <= 15.0)
  {
    still(); // Stop both motors
    motor(0, 1, 0, 1); // Now Reverse both motors for 500ms
    delay(500);
    still(); // Stop again, after reversing
    servo(servoLeft); // Turn Ultrasonic sensor Left
    ldist = calcDistance(); // Calculate obstacle distance to the left
    servo(servoRight);      // Turn Ultrasonic sensor right
    rdist = calcDistance(); // Calculate obstackle distance to the right
    servo(servoStraight);   // Turn Ultrasonic sensor straight
    //
    // Handle "in a pocket" condition
    //
    while (ldist < 15.0 && rdist < 15.0) // While the obstackle distances to the right and left are lesser than 15(In a pocket), go back a bit and check again
    {
      motor(0, 1, 0, 1);                 // Move back for 1000ms(1 second)
      delay(1000);            
      still();                           // Stop both motors
      servo(servoLeft);                  // Turn Ultrasonic sensor left
      ldist = calcDistance();            // Calculate obstacle distance to the left
      servo(servoRight);                 // Turn Ultrasonic sensor right
      rdist = calcDistance();            // Calculate obstacle distance to the right
      servo(servoStraight);              // Turn Ultrasonic sensor straight
    }

    if (rdist > ldist)                   // if the robot can go more to the right,
    {
      //motor(1, 0, 0, 1);                 // Turn right for 500ms
      motor(0, 1, 1, 0);
      delay(500); 
    }
    else if (ldist > rdist)              // if the robot can go more to the left,
    {
      //motor(0, 1, 1, 0);                 // Turn left 500ms
      motor(1, 0, 0, 1);
      delay(500);
    }
    else if (ldist == rdist)             // If both sides are same, 
    {
      //motor(1, 0, 0, 1);                 // Turn right for 500ms
      motor(0, 1, 1, 0);
      delay(500);
    }
  }

  //"Home" sensors indications  
  delay(100);
  int analogSensor = analogRead(smokeA0);
  int val=digitalRead(vib_pin);
  // Citeste datele si le stocam in variabilele hum si temp
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  // Afisare temp si hum in terminalul serial
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  Serial.print("Gas sensor: ");
  Serial.println(analogSensor);
  lcd.clear();
  lcd.print("Humidity: ");
  lcd.print(hum);
  lcd.print("%");
  lcd.setCursor(2,1); 
  lcd.print("Temp: ");
  lcd.print(temp);

  if(temp > 50)
  {
    tone(buzzer, 1000, 4000);
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Temp over 50 C degrees");
  }
  
  if(hum > 90)
  {
    tone(buzzer, 1000, 100);
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Humidity > 90%");
  }
  
  // Verificare depasire valoare senzor gaz
  if(analogSensor > sensorThres)
  {
    tone(buzzer, 1000, 4000);
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("MQ2: DANGER");
  }
  else if(analogSensor < sensorThres)
  {
    noTone(buzzer);
  }
  if(val==1)
  {
    tone(buzzer, 1000, 3000);
  }
  delay(100);
  }
