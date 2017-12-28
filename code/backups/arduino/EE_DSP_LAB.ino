// autonomous.pde
// Team : DPS LAB Team 1
// Programmer : Akash Mukesh Joshi
// USC ID : 4703642421
// Date : 04/2/2017
#include <VirtualWire.h>

#define SPEED 60

const int motor_left = 5;
const int motor_right = 6;
const int led_pin = 13;
const int transmit_pin = 12;
const int receive_pin = 11;
const int transmit_en_pin = 3;
volatile int flag = 48;
double counter = 0;

void setup() {
  // put your setup code here, to run once:
  delay(500);
  Serial.begin(9600);
  //  Serial.println("setup");

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  pinMode(motor_left, OUTPUT);   // sets the pin as output
  pinMode(motor_right, OUTPUT);   // sets the pin as output
  analogWrite(motor_left, SPEED * 1.5);
  analogWrite(motor_right, SPEED * 1.5);
  delay(100);
  //  analogWrite(motor_left, 0);
  //  analogWrite(motor_right, 0);


  // Initialise the IO and ISR
  //  vw_set_tx_pin(transmit_pin);
  vw_set_rx_pin(receive_pin);
  vw_set_ptt_pin(transmit_en_pin);
  //    vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);  // Bits per sec

  vw_rx_start();       // Start the receiver PLL running
}

void motor_f()
{

  analogWrite(motor_left, SPEED);
  analogWrite(motor_right, SPEED);
  //  delay(1000);
  //  Serial.print("FORWARD");
}

void motor_l()
{
  analogWrite(motor_left, 0);
  analogWrite(motor_right, SPEED);
  delay(460);
  //  Serial.print("LEFT");
}

void motor_r()
{
  analogWrite(motor_left, SPEED);
  analogWrite(motor_right, 0);
  delay(460);
  //  Serial.print("RIGHT");
}

void motor_u()
{
  analogWrite(motor_right, SPEED);
  analogWrite(motor_left, 0);
  delay(1300);
  analogWrite(motor_left, SPEED * 1.5);
  analogWrite(motor_right, SPEED * 1.5);
  delay(50);
}
void loop() {

//  while(1)
//  {
//    motor_f();
//delay(2000);
//motor_u();
//  }
//   put your main code here, to run repeatedly:
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    int i;
    counter ++;

    digitalWrite(led_pin, HIGH); // Flash a light to show received good message
    // Message with a good checksum received, print it.
    Serial.print("Got: ");

    for (i = 0; i < buflen; i++)
    {
      Serial.print(buf[i]);
      Serial.print(' ');
    }
    Serial.print(flag);
    Serial.print(' ');
    Serial.println();
    digitalWrite(led_pin, LOW);
  }


  if (counter > 10)
  {
    if (buf[3] != flag)
    {
      motor_u();
      flag = (int)buf[3];
    }
    else
      motor_f();
  }



}
