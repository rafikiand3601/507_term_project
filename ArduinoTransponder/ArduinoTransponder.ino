// nrf24_server

#include <SPI.h>
#include <RF24.h>

uint8_t trig_pin = 3;

uint8_t cs_pin = 8;
uint8_t ce_pin = 7;
RF24 radio(ce_pin, cs_pin);


void setup() 
{
  // Start radio and serial
  Serial.begin(9600);
  radio.setChannel(0x01);
  radio.setPayloadSize(0x02);
  radio.printDetails();
  radio.begin();
  uint8_t addresses[][6] = {"node1", "node2"};
  radio.openReadingPipe(1, addresses[0]);
  Serial.println("Server Running");
  radio.startListening();
}

void loop()
{
  // Wait until a message is recieved
  if (radio.available())
  { 
    char buf[20];
    uint8_t len = sizeof(buf);
    // Read message into buffer
    radio.read(buf, &len);
    if (buf[0] == 'a' || buf[1] == 'a')
    {
      Serial.println(buf);
      //digitalWrite(trig_pin, HIGH);
      //delayMicroseconds(10);
      //digitalWrite(trig_pin, LOW);
    }
      
    // Send a reply
    /*
    uint8_t data[] = "received";
    radio.stopListening();
    radio.write(data, sizeof(data));
    Serial.println("Sent a reply");
    radio.startListening();
    */
  }
}
