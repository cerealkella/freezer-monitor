/*
  Mash-up of email client and DeepFreeze

  Deep freeze logic adapted from here:
  https://www.youtube.com/watch?v=z59XS2wHOB8
  and here:
  https://www.patreon.com/posts/6602219

  Email client sketch for IDE v1.0.5 and w5100/w5200
  Posted 7 May 2015 by SurferTim
  https://playground.arduino.cc/Code/Email/

  Requires send2go to be set up. Tied to
  my domain in send2go
  A good resource:
  http://www.jucetechnology.com/wordpress/simple-burglar-ethernet-shield-connection/

*/

#include <SPI.h>
#include <Ethernet.h>
#include "email_settings.h"


// this must be unique
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x52, 0x64 };

char server[] = email_server;
int port = email_port;

int AlarmTemp = 23;
int sensors_to_read = 2;

EthernetClient client;

void setup()
{
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  Ethernet.begin(mac);
  delay(2000);
  // Serial.println(F("Ready. Press 'e' to send."));
}

void loop()
{
  int analog_input = 0;
  String email_msg;

  while (analog_input < sensors_to_read)
  {
    int rawvoltage = analogRead(analog_input);
    float volts = rawvoltage / 205.0;
    float celsiustemp = 100.0 * volts - 54;
    float fahrenheittemp = celsiustemp * 9.0 / 5.0 + 32.0;
    Serial.print(" Freezer  #");
    Serial.println(analog_input);
    Serial.print(fahrenheittemp);
    Serial.println(" Fahrenheit ");
    Serial.print(celsiustemp);
    Serial.println(" Celsius ");

    if (fahrenheittemp > AlarmTemp) {
      Serial.println("ALARM");

      email_msg = F("Temperature threshold Exceeded on Freezer ");
      email_msg += String(analog_input);
      email_msg += F("\n");
      email_msg += F("Temperature Reading: ");
      email_msg += String(fahrenheittemp, 2);
      email_msg += F(" Degrees Fahrenheit");
      {
        if (sendEmail(email_msg))
        {
          Serial.println(F("Email sent"));
          Serial.println(" ");
          Serial.println("Waiting for five minutes to check temperature again...");
          delay(300000); //wait for five minutes after sending email
        }
        else Serial.println(F("Email failed"));
      }

    }

    else if (fahrenheittemp <= AlarmTemp) {
      Serial.println("Threshold within range.");
    }
    analog_input++;
  }
  Serial.println(" ");
  delay(5000);
}

byte sendEmail(String msg)
{
  byte thisByte = 0;
  byte respCode;

  if (client.connect(server, port) == 1) {
    Serial.println(F("connected"));
  } else {
    Serial.println(F("connection failed"));
    return 0;
  }

  if (!eRcv()) return 0;

  Serial.println(F("Sending hello"));
  // replace 1.2.3.4 with your Arduino's ip
  // client.println("EHLO 1.2.3.4");
  // commenting above line
  // evidently the ip address is not necessary
  client.println("EHLO");
  if (!eRcv()) return 0;

  Serial.println(F("Sending auth login"));
  client.println("auth login");
  if (!eRcv()) return 0;

  Serial.println(F("Sending User"));
  // Change to your base64 encoded user
  client.println(email_user);

  if (!eRcv()) return 0;

  Serial.println(F("Sending Password"));
  client.println(email_password);

  if (!eRcv()) return 0;

  Serial.println(F("Sending From"));
  client.print("MAIL From: ");
  client.println(email_from);
  if (!eRcv()) return 0;

  Serial.println(F("Sending To"));
  client.print("RCPT To: ");
  client.println(email_to);
  if (!eRcv()) return 0;

  Serial.println(F("Sending DATA"));
  client.println("DATA");
  if (!eRcv()) return 0;

  Serial.println(F("Sending email"));

  client.print("To: Alert Recipient ");
  client.println(email_to);

  client.print("From: Embedded Device Alert ");
  client.println(email_from);

  client.println("Subject: Arduino Temperature Alert\r\n");

  client.println(msg);

  client.println(".");

  if (!eRcv()) return 0;

  Serial.println(F("Sending QUIT"));
  client.println("QUIT");
  if (!eRcv()) return 0;

  client.stop();

  Serial.println(F("disconnected"));

  return 1;
}

byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;

  while (!client.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  respCode = client.peek();

  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  if (respCode >= '4')
  {
    efail();
    return 0;
  }

  return 1;
}


void efail()
{
  byte thisByte = 0;
  int loopCount = 0;

  client.println(F("QUIT"));

  while (!client.available()) {
    delay(1);
    loopCount++;

    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }

  while (client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  client.stop();
  Serial.println(F("disconnected"));
}
