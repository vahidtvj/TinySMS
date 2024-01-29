/**
 * * This example reads All messages stored in the GSM module
 *
 * For All examples you need to install TinyGSM & LinkedList libraries:
 *   https://github.com/ivanseidel/LinkedList
 *   https://github.com/vshymanskyy/TinyGSM
 */

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM800
#define SerialMon Serial
#define SerialAT Serial2
#include <TinyGsmClient.h>
#include <TinySMS.h>
TinyGsm modem(SerialAT);
TinySMS modemSMS(modem);

void readAllCallback(SMS sms)
{
    SerialMon.println("---------------------------------------------");
    SerialMon.println(sms.sender);
    SerialMon.println(sms.message);
    SerialMon.println(sms.date);
}

void setup()
{
    SerialMon.begin(115200);
    SerialMon.println("Initializing modem...");

    // TinyGsm config
    TinyGsmAutoBaud(SerialAT);
    // modem.restart();
    modem.init();

    // setup modem for sms
    modemSMS.begin();

    int smsIndex = 35;
    SerialMon.println("Reading all messages");
    modemSMS.readAll(readAllCallback);
}

void loop()
{
}