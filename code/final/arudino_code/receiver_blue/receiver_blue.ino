// autonomous.pde
// Team : DPS LAB Team 1
// Programmer : Akash Mukesh Joshi
// USC ID : 4703642421
// Date : 04/2/2017
#include <VirtualWire.h>
#define CAR_ID  0 //BLUE

#define RSPEED 60
#define LSPEED 60

const int motor_left = 5;
const int motor_right = 6;
const int led_pin = 12;
//const int transmit_pin = 12;
const int receive_pin = 11;
const int transmit_en_pin = 3;
//const int LED_BULLTIN = 12;
//volatile int flag = 2;
//double counter = 0;
unsigned long u_counter = 0;

void setup() {
  //  int flag = 2;
  // put your setup code here, to run once:
  delay(500);
  Serial.begin(9600);
  Serial.println("setup");
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(led_pin, OUTPUT);
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
  delay(10);
}

void motor_f()
{

  analogWrite(motor_left, LSPEED);
  analogWrite(motor_right, RSPEED);
  //  delay(1000);
  //  Serial.print("FORWARD");
}

//void motor_l()
//{
//  analogWrite(motor_left, 0);
//  analogWrite(motor_right, RSPEED*1.40);
//  delay(360);
//  analogWrite(motor_left, 0);
//  analogWrite(motor_right, 0);
//  delay(500);
//  analogWrite(motor_left, LSPEED * 1.20);
//  analogWrite(motor_right, RSPEED * 1.20);
//  delay(20);
//}
//
//void motor_r()
//{
//  analogWrite(motor_left, LSPEED*1.40);
//  analogWrite(motor_right, 0);
//  delay(360);
//  analogWrite(motor_left, 0);
//  analogWrite(motor_right, 0);
//  delay(500);
//  analogWrite(motor_left, LSPEED * 1.20);
//  analogWrite(motor_right, RSPEED * 1.20);
//  delay(20);
//}

void motor_u()
{
  analogWrite(motor_right, RSPEED * 1.40); //BALL:1.2
  analogWrite(motor_left, 0);
  delay(700);
  analogWrite(motor_left, 0);
  analogWrite(motor_right, 0);
  delay(500);
  analogWrite(motor_left, LSPEED * 1.20);
  analogWrite(motor_right, RSPEED * 1.20);
  delay(10);
}
//void motor_ur()
//{
//  analogWrite(motor_left, LSPEED * 1.40); //BALL:1.2
//  analogWrite(motor_right, 0);
//  delay(700);
//  analogWrite(motor_left, LSPEED * 1.20);
//  analogWrite(motor_right, RSPEED * 1.20);
//  delay(10);
//}
void loop() {

  static int flag;

  ////   put your main code here, to run repeatedly:
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  //  if (vw_wait_rx_max(4000))
  //  {
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    Serial.print((int)buf[0]);
    Serial.print(' ');
    Serial.print((int)buf[1]);
    Serial.print(' ');
    Serial.print((int)buf[2]);
    Serial.print(' ');
    Serial.println((int)buf[3]);
    digitalWrite(led_pin, HIGH); // Flash a light to show received good message

    if ((buf[2] == 0 && buf[3] == 1) || (buf[2] == 1 && buf[3] == 0))
    {
      analogWrite(motor_left, 0);
      analogWrite(motor_right, 0);
      delay(650);
      u_counter = millis();
    }
    if (buf[0] == 1 && buf[1] == 1) {
      u_counter = millis();
      motor_u();
    }
    else {
      motor_f();
    }

    digitalWrite(led_pin, LOW);

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
  //}
  //  else
  //  motor_u();

}
