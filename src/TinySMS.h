#pragma once
#include <Arduino.h>
#include <LinkedList.h>
#include <TinyGsmClient.h>

String GSM7Lookup[] = {"@", "£", "$", "¥", "è", "é", "ù", "ì", "ò", "Ç", "\n", "Ø", "ø", "\r", "Å", "å", "Δ", "_", "Φ", "Γ", "Λ", "Ω", "Π", "Ψ", "Σ", "Θ", "Ξ", "", "Æ", "æ", "ß", "É", " ", "!", "\"", "#", "¤", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?", "¡", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Ä", "Ö", "Ñ", "Ü", "§", "¿", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ä", "ö", "ñ", "ü", "à"};

class SMS
{
private:
public:
    String status;
    String sender;
    String message;
    String date;
    String ref;
    uint8_t part = 1;
    uint8_t totalParts = 1;
};

class TinySMS
{
private:
    uint32_t readTimeout = 3000;
    uint32_t handleTimeout = 1000;
    TinyGsm *modem;
    LinkedList<SMS> partialSMS;
    bool parseMultipart(SMS &sms);
    SMS parsePDU(String data);
    String parseDate(String data);
    String parseNumber(String &pdu);
    String parseGSM7(String data, uint8_t paddingBits = 0);
    void isort(uint8_t *a, int n);
    void swap(String &data);
    String decodeUnicode(String data);

public:
    TinySMS(TinyGsm &modem);
    void begin();
    bool read(uint8_t index, SMS &sms);
    bool readRAW(uint8_t index, String &data);
    void readAll(void (*callBack)(SMS), bool remove = false);
    void remove(uint8_t index);
    void removeRead();
    void removeAll();
    void handle();
    void clearPartial();
    void (*newSMSCallback)(SMS);
};

#include "helper.tpp"
#include "functions.tpp"
#include "TinySMS.tpp"