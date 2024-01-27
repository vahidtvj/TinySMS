/**
 * * This example reads one SMS message at the given index
 *
 * For All examples you need to install TinyGSM & LinkedList libraries:
 *   https://github.com/ivanseidel/LinkedList
 *   https://github.com/vshymanskyy/TinyGSM
 */

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM800
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() \
//     {                    \
//         delay(2);        \
//     }
#define SerialMon Serial
#define SerialAT Serial2
// #define TINY_GSM_DEBUG SerialMon
// #define TINY_GSM_RX_BUFFER 650
#include <StreamDebugger.h>
#include <TinyGsmClient.h>
#include <TinySMS.h>
// StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(SerialAT);
TinySMS modemSMS(modem);

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
    SerialMon.println("Reading message");

    SerialMon.println("Raw data");
    String raw;
    modemSMS.readRAW(smsIndex, raw);
    SerialMon.println(raw);

    SerialMon.println("Parsed message");
    SMS sms;
    modemSMS.read(smsIndex, sms);
    SerialMon.println(sms.sender);
    SerialMon.println(sms.message);
    SerialMon.println(sms.date);
}

void loop()
{
}