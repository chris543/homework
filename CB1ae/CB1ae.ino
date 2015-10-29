#include "firmata.h"
#include <Servo.h>
#define esp8266 Serial1
#define DEBUG true
// int led = 13;
String str;
char inChar;
char compareString[4] = {'I', 'P', 'D', ','};
int index = 0;
int dataLength, ones, tens;
byte firmataData[17];
int temp = 10;

char compare[20];
const char close[] = "CLOSED";
const char IPD[] = "IPD,";
int a = 0;

typedef struct
{
  uint8_t mode;
  uint8_t value;
} pin_status;

pin_status BROAD_IO[4] ;

int SD0 = 11;
int SD1 = 6;
int SD2 = 13;
int SD3 = 5;

int SDc0 = 4;
int SDc1 = 7;
int SDc2 = 8;
int SDc3 = 12;

Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;

int wifiWakeup = A4;
void setup() {
  delay(2000);
  // printTest("hello");

  for (int i = 0; i < 4; ++i)
  {
    BROAD_IO[i].mode = PWM;
  }

  pinMode(SD0, OUTPUT);
  pinMode(SD1, OUTPUT);
  pinMode(SD2, OUTPUT);
  pinMode(SD3, OUTPUT);
  pinMode(SDc0, OUTPUT);
  pinMode(SDc1, OUTPUT);
  pinMode(SDc2, OUTPUT);
  pinMode(SDc3, OUTPUT);



  pinMode(SD0, OUTPUT);
  pinMode(SD1, OUTPUT);
  pinMode(SD2, OUTPUT);
  pinMode(SD3, OUTPUT);
  pinMode(SDc0, OUTPUT);
  pinMode(SDc1, OUTPUT);
  pinMode(SDc2, OUTPUT);
  pinMode(SDc3, OUTPUT);



  pinMode(wifiWakeup, OUTPUT);
  digitalWrite(wifiWakeup, HIGH);
  // esp8266.println("test");
  // initialize the digital pin as an output.
  // pinMode(led, OUTPUT);
  Serial.begin(115200);
  esp8266.begin(115200);

  sendCommand("AT+RST\r\n", 2000, DEBUG);
  sendCommand("AT+CWSAP=\"CB1ae_dev\",\"\",3,0\r\n", 2000, DEBUG);

  sendCommand("AT+CWMODE=2\r\n", 1000, DEBUG);
  sendCommand("AT+CIPAP=\"192.168.33.33\"\r\n", 2000, DEBUG);
  sendCommand("AT+CIPMUX=1\r\n", 1000, DEBUG);
  sendCommand("AT+CIPSERVER=1,8888\r\n", 1000, DEBUG);

  const char haystack[20] = "TutorialsPoint";
  const char needle[10] = "tor";

  // char tt[10];
  char *ret;

  // ret = strstr(haystack, needle);
  ret = strstr(haystack, needle);

  if (ret == NULL)
  {
    Serial.println("no found");
  }
  else {
    Serial.println("OK");
  }
  // memcpy(tt,ret,6);
  // strncpy (ret,"sample",6);

  // Serial.println(tt);
}
char *findStr;
boolean match = false;
char matchdata[7] ;

int b = 0;
// the loop routine runs over and over again forever:
void loop() {
  //  if(esp8266.available()>0) // check if the esp is sending a message
  // {
  //   Serial.println(esp8266.read());
  // }
  //receive form ESP8266 esp8266
  // delay(2000);
  // Serial.println(compareString);


  if (esp8266.available() > 0) {
    inChar = esp8266.read();

    Serial.print(inChar);
    // Serial.print("-");

    // Serial.print(inChar, HEX);
    // Serial.print("-");

    if (a < sizeof(compare))
    {
      compare[a] = inChar;

      // Serial.print(inChar, HEX);


      a++;

      if (match)
      {

        if (b < 7)
        {
          matchdata[b] = inChar;

        }

        b++;

        if (b == 7)
        {
          Serial.println("match exe");

          // Serial.println(matchdata);

          dataLength = matchdata[2] - 48;

          // for (int i = 0; i < 7; i++)
          // {
          //   /* code */
          //   Serial.println((unsigned byte)matchdata[i], HEX);

          // }

          for (int i = 0 ; i < dataLength; i++)
          {

            firmataData[i] = (unsigned byte)matchdata[4 + i];
          }

          // for (int i = 0; i < dataLength; i++)
          // {
          //   Serial.println(firmataData[i], HEX);

          // }

          executive_firmata( dataLength);

          match = false;
          b = 0;
          a = 0;
        }

      }

      if (strstr(compare, "CONNECT") != NULL)
      {
        a = 0;

        Serial.println("CONNECT esp");
        memset(compare, 0, sizeof(compare) - 1);

      }

      else if (strstr(compare, "CLOSED") != NULL)
      {
        a = 0;

        Serial.println("CLOSED esp");
        memset(compare, 0, sizeof(compare) - 1);
        resetIO();
      }

      if (strstr(compare, "+IPD,") != NULL)
      {
        // a = 0;

        // Serial.println("-exe..-");
        Serial.println("match..");

        // Serial.println(compare);
        match  = true;
        memset(compare, 0, sizeof(compare) - 1);

      }

    }


    if (inChar == '\r' || inChar == '\n')
    {
      /* code */
      // Serial.println("-rn-");
      //       if (strstr(compare, "OK") != NULL)
      // {
      //   a = 0;

      //   Serial.println("-exe..-");
      // }
      a = 0;
    }

    // if (inChar == '\r')
    // {
    //   /* code */
    //   Serial.println("rr");

    // }
    // if (inChar == '\n')
    // {
    //   Serial.println("nn");

    //   // Serial.println(inChar,HEX);
    // }

    // if (a == 0 && inChar == '\n')
    // {
    //   /* code */
    //   Serial.print("0n");

    // } else {

    //   compare[a] = inChar;

    //   if (inChar == '\n' )
    //   {

    //     Serial.print("--");
    //     Serial.print(compare);
    //     Serial.println("--");
    //     memset(compare, 0, sizeof(compare) - 1);

    //     a = 0;
    //   }
    //   else {
    //     a++;
    //   }

    // }

    // if (a >= 9)
    // {
    //   // memcpy(compare, compare, 9);
    //   /* code */
    // }
    // else {
    //   a++;
    // }

    // if (inChar == '\n' )
    // {
    //   Serial.print("--");
    //   Serial.print(compare);
    //   Serial.println("--");
    //   memset(compare, 0, sizeof(compare) - 1);

    //   a = 0;
    // }
    // else {
    //   a++;
    // }

    // if (inChar == '\n')
    // {
    //   // Serial.println("end");
    //   /* code */

    //   Serial.print("--");
    //   Serial.println(compare);

    //   // findStr = strstr(compare, "CONNECT");
    //   if (strstr(compare, "CONNECT") != NULL)
    //   {
    //     a = 0;

    //     Serial.println("Connect esp");
    //   }
    //   // findStr = strstr(compare, "CLOSED");

    //   if (strstr(compare, "CLOSED") != NULL)
    //   {
    //     Serial.println("disconnect esp");
    //     resetIO();
    //     a = 0;

    //   }

    //   if (strstr(compare, "+IPD,") != NULL)
    //   {
    //     Serial.println("data esp");
    //     // Serial.println(compare[7] - 48, HEX);
    //     dataLength = compare[7] - 48;
    //     // Serial.println(compare);
    //     // Serial.println((unsigned byte) compare[9], HEX);
    //     // Serial.println(compare[10], HEX);
    //     // Serial.println(compare[11], HEX);

    //     for (int i = 0 ; i < dataLength; i++)
    //     {

    //       firmataData[i] = (unsigned byte)compare[9 + i];
    //     }

    //     // for (int i = 0; i < dataLength; i++)
    //     // {
    //     //   Serial.println(firmataData[i], HEX);

    //     // }

    //     // executive_firmata( dataLength);

    //   }

    //   Serial.print("-)");

    //   // Serial.println(compare);
    //   memset(compare, 0, sizeof(compare) - 1);
    //   a = 0;
    // }
    // else {
    //   a++;
    // }

    // if (compareString[index] == inChar)
    // {

    //   if (index == sizeof(compareString) - 1)
    //   {
    //     delay(1);


    //     index = 0;
    //     return;
    //   }

    //   index++;
    // }
    // else {
    //   index = 0;

    // }


    // getData();
  }



  //receive form USB port


  if (Serial.available() > 0) {
    // read the incoming byte:
    // char chr = Serial.read();
    esp8266.print((char)Serial.read());
  }

  // digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  // Serial.println("hello");
  // delay(1000);               // wait for a second
  // digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  // delay(1000);               // wait for a second
}
void resetIO(void) {

  // analogWrite(SD0, 0);
  // analogWrite(SD1, 0);
  // analogWrite(SD2, 0);
  // analogWrite(SD3, 0);

  for (int i = 0; i < 3; ++i)
  {
    /* code */
    if (BROAD_IO[i].mode == PWM)
    {
      switch (i) {
      case 0:
        analogWrite(SD0, 0);
        break;
      case 1:
        analogWrite(SD1, 0);
        break;
      case 2:
        analogWrite(SD2, 0);
        break;
      case 3:
        analogWrite(SD3, 0);
        break;
      }
    } else if (BROAD_IO[i].mode == SERVO) {
      switch (i) {
      case 0 :
        servo0.writeMicroseconds(1500);
        break;
      case 1 :
        servo1.writeMicroseconds(1500);
        break;
      case 2 :
        servo2.writeMicroseconds(1500);
        break;
      case 3 :
        servo3.writeMicroseconds(1500);
        break;
      }
    }


  }

}

void getData(void) {

  if (compareString[index] == inChar)
  {

    if (index == sizeof(compareString) - 1)
    {
      delay(1);

      int connectionId = esp8266.read() - 48;

      int i = 0;
      char temp;

      esp8266.find(",");

      tens = esp8266.read();
      delay(1);
      ones = esp8266.read();

      if (ones == ':')
      {

        ones = tens;
        dataLength = ones - 48;
        // Serial.println(dataLength);

      }

      else {
        dataLength = (((tens - 48) * 10) + (ones - 48));

        char temp = esp8266.read();

      }
      // delay(1);
      memset(firmataData, 0, 17);

      for (i = 0 ; i < dataLength; i++)
      {

        firmataData[i] = esp8266.read();
      }

      //------------print receive data from esp8266----------

      //       for (i = 0; i < dataLength; i++)
      // {
      //   Serial.println(firmataData[i], HEX);

      // }

      executive_firmata( dataLength);


      index = 0;
      return;
    }

    index++;
    /* code */
  }
  else {
    index = 0;

  }
}
char Comp(char* comp)
{

  return (0);
}

/*
* Name: sendCommand
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendCommand(String command, const int timeout, boolean debug)
{
  String response = "";

  esp8266.print(command); // send the read character to the esp8266

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {

      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }

  if (debug)
  {
    Serial.print(response);

  }

  return response;
}
