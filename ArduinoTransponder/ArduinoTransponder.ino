// nrf24_server

#include <SPI.h>
#include <RF24.h>

uint8_t trig_pin = 2;
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
  uint8_t addresses[][6] = {"1Node", "2Node"};
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  Serial.println("Server Running");
  radio.startListening();
}

void loop()
{
  if (radio.available())
  {
    // Should be a message for us now   
    uint8_t buf[20];
    uint8_t len = sizeof(buf);
    radio.read(buf, &len);
    if (buf[0] == 'a')
    {
      digitalWrite(led_pin, HIGH);
      digitalWrite(trig_pin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig_pin, LOW);
    }
    Serial.print("got request: ");
    Serial.println((char*)buf);
      
    // Send a reply
    uint8_t data[] = "And hello back to you";
    radio.stopListening();
    radio.write(data, sizeof(data));
    Serial.println("Sent a reply");
    radio.startListening();
    delay(100);
    digitalWrite(led_pin, LOW);
  }
}
