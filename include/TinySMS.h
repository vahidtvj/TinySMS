#pragma once
#include <Arduino.h>
#include <LinkedList.h>

#define TINY_GSM_MODEM_SIM800 // TODO remove

#ifndef TINY_GSM_MODEM_SIM800
#error "Please define GSM modem model"
#endif
#include <TinyGsmClient.h>

class SMS
{
private:
public:
    String status;
    String sender;
    String message;
    String date;
    char ref;
    uint8_t part = 1;
    uint8_t totalParts = 1;
};

class TinySMS
{
private:
    uint readTimeout = 3000;
    uint handleTimeout = 1000;
    TinyGsm *modem;
    LinkedList<SMS> partialSMS;
    bool parseMultipart(SMS &sms);
    SMS parsePDU(String data);
    String parseDate(String data);
    String parseNumber(String &pdu);
    String parseGSM7(String data);
    void isort(uint8_t *a, int n);
    void swap(String &data);
    String decodeUnicode(String data);

public:
    TinySMS(TinyGsm &modem);
    void begin();
    bool read(uint8_t index, SMS &sms);
    bool readRAW(uint8_t index, String &data);
    void readAll(void (*callBack)(SMS));
    void remove(uint8_t index);
    void removeRead();
    void removeAll();
    void handle();
    void (*newSMSCallback)(SMS);
};
