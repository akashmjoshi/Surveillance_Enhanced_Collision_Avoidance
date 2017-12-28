#include <VirtualWire.h>

const int led_pin = 13;
const int transmit_pin = 12;
const int receive_pin = 2;
const int transmit_en_pin = 7;
const int interruptPin = 3;     // ISR - Pin87
const int pin_4 = 4;     // LRU
const int pin_5 = 5;
const int pin_6 = 6;     // C.ar Choice
const int pin_7 = 7;
int flag = 0;

// variables will change:
int stat_1 = 0;         // variable for reading the pushbutton status
int stat_2 = 0;         // variable for reading the pushbutton status
int stat_3 = 0;
int i = 0;
unsigned char msg[4] = {'0', '0', '0', '0'};

void checkpins()
{

  if (digitalRead(pin_7) == 0)
  {
    flag = 0;
    if (digitalRead(pin_6) == 0)
    {
      msg[0] = digitalRead(pin_4);
      msg[1] = digitalRead(pin_5);
    }
    else if (digitalRead(pin_6) == 1)
    {
      msg[2] = digitalRead(pin_4);
      msg[3] = digitalRead(pin_5);
    }
  }
  else if (digitalRead(pin_7) == 1)
  {
    flag = 1;
    Serial.print((int)msg[0]);
    Serial.print(' ');
    Serial.print((int)msg[1]);
    Serial.print(' ');
    Serial.print((int)msg[2]);
    Serial.print(' ');
    Serial.println((int)msg[3]);

  }
  flag = 1;
}

void setup()
{
  //Serial Begins
  Serial.begin(9600);  // Debugging only
  Serial.println("setup");
  // Initialise the IO and ISR
  vw_set_tx_pin(transmit_pin);
  vw_set_rx_pin(receive_pin);
  vw_set_ptt_pin(transmit_en_pin);
  //  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);   // Bits per sec
  pinMode(led_pin, OUTPUT);
  pinMode(pin_4, INPUT_PULLUP);
  pinMode(pin_5, INPUT_PULLUP);
  pinMode(pin_6, INPUT_PULLUP);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), checkpins, FALLING);
}

void loop()
{
 
  if (flag == 1)
  {
    flag = 0;
    digitalWrite(led_pin, HIGH); // Flash a light to show transmitting
    vw_send((uint8_t *)msg, 4);
    vw_wait_tx(); // Wait until the whole message is gone
    digitalWrite(led_pin, LOW);
//    delay(100);
    msg[0] = 0;
    msg[1] = 0;
    msg[2] = 0;
    msg[3] = 0;
  }

}
