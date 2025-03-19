#include "TinySMS.h"

String parseEscape(char data)
{
    if (data == 0x0A)
        return "";
    else if (data == 0x14)
        return "^";
    else if (data == 0x28)
        return "{";
    else if (data == 0x29)
        return "}";
    else if (data == 0x2F)
        return "\\";
    else if (data == 0x3C)
        return "[";
    else if (data == 0x3D)
        return "~";
    else if (data == 0x3E)
        return "]";
    else if (data == 0x40)
        return "|";
    else if (data == 0x65)
        return "€";
    return "";
}
String TinySMS::parseGSM7(String data, uint8_t paddingBits, uint8_t septetCount)
{
    bool escape = false;
    String message = "";
    char temp = 0;
    int j = (7 - paddingBits) % 7;
    uint8_t count = 0;

    for (int i = 0; i < data.length(); i += 2)
    {
        uint8_t o = strtol(data.substring(i, i + 2).c_str(), nullptr, 16);
        char s = ((o << j) & 127) | temp;
        temp = o >> (7 - j);
        if (paddingBits == 0 || i != 0)
        {
            if (escape == true)
            {
                message += parseEscape(s);
                escape = false;
            }
            else
            {
                if (s == 0x1B)
                    escape = true;
                else
                    message += GSM7Lookup[s];
            }
            if (++count == septetCount)
                break;
        }
        if (++j == 7)
        {
            if (escape == true)
            {
                message += parseEscape(temp);
                escape = false;
            }
            else
            {
                if (temp == 0x1B)
                    escape = true;
                else
                    message += GSM7Lookup[temp];
            }
            temp = 0;
            j = 0;
            if (++count == septetCount)
                break;
        }
    }
    return message;
}

String TinySMS::parseNumber(String &pdu)
{
    String number;

    uint8_t size = strtol(pdu.substring(0, 2).c_str(), NULL, 16);
    bool isOdd = size % 2;
    size = isOdd ? size + 1 : size;

    String data = pdu.substring(2, 4 + size);
    pdu = pdu.substring(size + 4);

    char addressType = strtol(data.substring(0, 2).c_str(), NULL, 16) >> 4;
    data.remove(0, 2);
    if (addressType == 0b1101)
    {
        // alphanumeric
        number = parseGSM7(data);
    }
    else
    {
        // numeric
        swap(data);
        // Odd length numbers are padded with an F in the end.
        if (isOdd && data[data.length() - 1] == 'F')
        {
            data = data.substring(0, data.length() - 1);
        }
        number = "+" + data;
    }

    return number;
}

String TinySMS::parseDate(String data)
{
    String date = "20";
    swap(data);
    date += data;
#define INSERT(x, i, c) x = x.substring(0, i) + c + date.substring(i);
    INSERT(date, 4, "/");
    INSERT(date, 7, "/");
    INSERT(date, 10, " ");
    INSERT(date, 13, ":");
    INSERT(date, 16, ":");
    const int len = date.length();
    date.remove(len - 2, len - 1);
    // REMOVE(date, len - 2, len - 1);

    return date;
}

SMS TinySMS::parsePDU(String data)
{
    SMS sms;
    String sender;
    String message;
    String date;
    int i = data.indexOf('\n');
    String pdu = data.substring(i + 1, data.indexOf('\n', i + 1) - 1);

    uint8_t size = strtol(pdu.substring(0, 2).c_str(), NULL, 16);
    pdu = pdu.substring(2 + size * 2);
    char TPDU = (char)strtol(pdu.substring(0, 2).c_str(), NULL, 16);
    bool hasMoreMessage = !bitRead(TPDU, 2);
    bool hasHeader = bitRead(TPDU, 6);
    pdu = pdu.substring(2);

    // parse number
    sender = parseNumber(pdu);

    // check if unicode or gsm7
    bool isUnicode = strtol(pdu.substring(2, 4).c_str(), NULL, 16);
    pdu = pdu.substring(4);

    // parse date
    date = parseDate(pdu.substring(0, 14));
    pdu = pdu.substring(14);

    // parse message size
    uint8_t messageSeptets = strtol(pdu.substring(0, 2).c_str(), NULL, 16);
    pdu = pdu.substring(2);

    uint8_t padding = 0;
    // parse message
    if (hasHeader)
    {
        uint8_t headerOctets = strtol(pdu.substring(0, 2).c_str(), NULL, 16);
        if (headerOctets == 5)
        {
            sms.ref = pdu.substring(6, 8);
            sms.totalParts = strtol(pdu.substring(8, 10).c_str(), NULL, 16);
            sms.part = strtol(pdu.substring(10, 12).c_str(), NULL, 16);
            pdu = pdu.substring(12);
        }
        else
        {
            sms.ref = pdu.substring(6, 10);
            sms.totalParts = strtol(pdu.substring(10, 12).c_str(), NULL, 16);
            sms.part = strtol(pdu.substring(12, 14).c_str(), NULL, 16);
            pdu = pdu.substring(14);
        }
        if (!isUnicode)
        {
            // UDH padding when using gsm7
            // +1 because header length octet is also included
            int udhBits = (headerOctets + 1) * 8;
            padding = (7 - (udhBits % 7)) % 7;

            messageSeptets -= (udhBits / 7) + (padding > 0);
        }
    }

    if (!isUnicode)
        message = parseGSM7(pdu, padding, messageSeptets);
    else
        message = decodeUnicode(pdu);

    sms.sender = sender;
    sms.date = date;
    sms.message = message;
    return sms;
}

bool TinySMS::parseMultipart(SMS &sms)
{
    if (sms.totalParts <= 1)
        return true;

    uint8_t count = 1;
    uint8_t index[sms.totalParts];
    for (int i = 0; i < partialSMS.size(); i++)
    {
        SMS t = partialSMS.get(i);
        if (t.ref != sms.ref)
            continue;
        index[t.part - 1] = i;
        count++;
    }
    if (count != sms.totalParts)
    {
        partialSMS.add(sms);
        return false;
    }

    SMS combined;
    combined.sender = sms.sender;
    combined.date = sms.date;
    combined.message = "";
    for (int i = 0; i < count; i++)
    {
        if (sms.part == i + 1)
        {
            combined.message += sms.message;
            index[i] = 255;
        }
        else
        {
            combined.message += partialSMS.get(index[i]).message;
        }
    }
    isort(index, count);
    for (int i = count - 1; i >= 0; i--)
        partialSMS.remove(index[i]);
    sms = combined;
    return true;
}
