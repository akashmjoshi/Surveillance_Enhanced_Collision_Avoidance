// autonomous.pde
// Team : DPS LAB Team 1
// Programmer : Akash Mukesh Joshi
// USC ID : 4703642421
// Date : 04/2/2017
#include <VirtualWire.h>
#define CAR_ID  1 //O

#define RSPEED 76
#define LSPEED 72

const int motor_left = 5;
const int motor_right = 6;
const int led_pin = 13;
//const int transmit_pin = 12;
const int receive_pin = 11;
const int transmit_en_pin = 3;
const int LED_BULLTIN = 12;
volatile int flag = 2;
//double counter = 0;
unsigned long u_counter = 0;

void setup() {
  // put your setup code here, to run once:
  delay(500);
  Serial.begin(9600);
  Serial.println("setup");
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  pinMode(motor_left, OUTPUT);   // sets the pin as output
  pinMode(motor_right, OUTPUT);   // sets the pin as output
  analogWrite(motor_left, LSPEED );
  analogWrite(motor_right, RSPEED );
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

  analogWrite(motor_left, LSPEED * 1.40);
  analogWrite(motor_right, RSPEED * 1.40);
  delay(50);
}

void motor_f()
{

  analogWrite(motor_left, LSPEED);
  analogWrite(motor_right, RSPEED);
  //  delay(1000);
  //  Serial.print("FORWARD");
}

void motor_l()
{
  analogWrite(motor_left, 0);
  analogWrite(motor_right, RSPEED * 1.50);
  delay(380);
  //  analogWrite(motor_left, 0);
  //  analogWrite(motor_right, 0);
  //  delay(500);
  analogWrite(motor_left, LSPEED * 1.40);
  analogWrite(motor_right, RSPEED * 1.40);
  delay(250);
}

void motor_r()
{
  analogWrite(motor_left, LSPEED * 1.50);
  analogWrite(motor_right, 0);
  delay(370);
  //  analogWrite(motor_left, 0);
  //  analogWrite(motor_right, 0);
  //  delay(500);
  analogWrite(motor_left, LSPEED * 1.40);
  analogWrite(motor_right, RSPEED * 1.40);
  delay(250);
}

void motor_u()
{ analogWrite(motor_right, RSPEED * 1.40); //BALL:1.2
  analogWrite(motor_left, 0);
  delay(800);
  analogWrite(motor_left, 0);
  analogWrite(motor_right, 0);
  delay(500);
  analogWrite(motor_left, LSPEED * 1.40);
  analogWrite(motor_right, RSPEED * 1.40);
  delay(20);
}
//void motor_ur()
//{
//  analogWrite(motor_left, LSPEED * 1.40); //BALL:1.2
//  analogWrite(motor_right, 0);
//  delay(770);
//  analogWrite(motor_left, LSPEED * 1.20);
//  analogWrite(motor_right, RSPEED * 1.20);
//  delay(10);
//}
void loop() {


  //        motor_f();
  //        delay(1000);
  //        motor_l();
  //        motor_f();
  //          delay(1000);
  //        motor_r();
  //
  //       motor_f();
  //       delay(1000);
  //       motor_u();



  ////   put your main code here, to run repeatedly:
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {

    Serial.print((int)buf[0]);
    Serial.print(' ');
    Serial.print((int)buf[1]);
    Serial.print(' ');
    Serial.print((int)buf[2]);
    Serial.print(' ');
    Serial.println((int)buf[3]);
    digitalWrite( LED_BULLTIN, HIGH); // Flash a light to show received good message
    // Message with a good checksum received, print it.

    if (buf[2] == 1 && buf[3] == 0) {
      motor_r();
      u_counter = millis();
    }
    else if (buf[2] == 0 && buf[3] == 1) {
      motor_l();
      u_counter = millis();
    }
    else if (buf[2] == 1 && buf[3] == 1) {
      motor_u();
      u_counter = millis();
    }
    else {
      motor_f();
    }
    digitalWrite( LED_BULLTIN, LOW);
  }
  else {
    if (millis() - u_counter > 4000)
    {
      motor_u();
      analogWrite(motor_left, LSPEED);
      analogWrite(motor_right, RSPEED);
      delay(1000);
      u_counter = millis();
    }
    motor_f();
  }
}
