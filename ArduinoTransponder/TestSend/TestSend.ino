// nrf24_server

#include <SPI.h>
#include <RF24.h>

uint8_t trig_pin = 3;

uint8_t cs_pin = 8;
uint8_t ce_pin = 7;
RF24 radio(ce_pin, cs_pin);


void setup() 
{

  // Init pins
  pinMode(trig_pin, OUTPUT);  // Trigger pin
  digitalWrite(trig_pin, LOW);

  // Start radio and serial
  Serial.begin(9600);
  radio.begin();
  uint8_t addresses[][6] = {"node1", "node2"};
  radio.openWritingPipe(addresses[0]);
  Serial.println("Sender Running");
}

void loop()
{
  // Send the character 'a'
  char data[] = " a";
  radio.write(data, sizeof(data));
  Serial.print("sent: ");
  Serial.println(data);
  delay(1000);
}
