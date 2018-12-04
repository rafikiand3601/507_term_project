// nrf24_server

#include <SPI.h>
#include <RF24.h>

uint8_t trig_pin = 3;
uint8_t led_pin = 13;

uint8_t cs_pin = 8;
uint8_t ce_pin = 7;
RF24 radio(ce_pin, cs_pin);


void setup() 
{

  // Init pins
  pinMode(trig_pin, OUTPUT);  // Trigger pin
  pinMode(led_pin, OUTPUT);  // LED pin
  digitalWrite(trig_pin, LOW);
  digitalWrite(led_pin, LOW);

  // Start radio and serial
  Serial.begin(115200);
  radio.begin();
  uint8_t addresses[][6] = {0x0010, 0x0001};
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  Serial.println("Sender Running");
}

void loop()
{
  // Send the character 'a'
  digitalWrite(led_pin, HIGH);
  uint8_t data[] = "a";
  radio.write(data, sizeof(data));
  Serial.println("Sent a reply");
  delay(100);
  digitalWrite(led_pin, LOW);
}
