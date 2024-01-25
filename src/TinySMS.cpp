#include "TinySMS.h"

TinySMS::TinySMS(TinyGsm &modem)
{
    this->modem = &modem;
}

void TinySMS::handle()
{
    String data;
    this->modem->waitResponse(this->handleTimeout, data, "\r\n");

    if (data.indexOf("+CMTI") == -1)
        return;

    data = data.substring(data.indexOf("+CMTI"));
    uint8_t index = strtol(data.substring(data.indexOf(',') + 1).c_str(), NULL, 10);

    SMS sms;
    if (!this->read(index, sms))
        return;
    if (!this->parseMultipart(sms))
        return;
    this->newSMSCallback(sms);
}

void TinySMS::begin()
{
    this->modem->waitResponse("Call Ready");
    this->modem->waitResponse("SMS Ready");
    // *set message format (pdu mode)
    this->modem->sendAT(F("+CMGF=0"));
    this->modem->waitResponse();
    // *set character set
    this->modem->sendAT(F("+CSCS=\"UCS2\""));
    this->modem->waitResponse();
    // *set New SMS Message Indications
    // buffer in TA when online
    // +CMT [<alpha>],<length><CR><LF><pdu>\
    // No CBM indications are routed to the TE
    // No SMS-STATUS-REPORTs are routed to the TE
    // resend buffer after mode change
    // this->modem->sendAT(F("+CNMI=2,2,0,0,0"));
    this->modem->sendAT(F("+CNMI=2,1,0,0,0"));
    this->modem->waitResponse();
    // // set text mode params
    // // ss->println(F("AT+CSMP=17,168,0,8"));
    // ss->println("AT+CSMP=49,167,0,8");
    // modem.sendAT(F("+CSMP=17,167,0,8"));
    // modem.waitResponse();
}

bool TinySMS::readRAW(uint8_t index, String &data)
{
    this->modem->waitResponse();
    this->modem->sendAT("+CMGR=" + String(index));
    this->modem->waitResponse(this->readTimeout, data);
    if (data.indexOf("+CMGR:") == -1)
        return false;
    data = data.substring(data.indexOf("+CMGR:") + 7);
    return true;
}

bool TinySMS::read(uint8_t index, SMS &sms)
{
    String data;
    if (!readRAW(index, data))
        return false;
    sms = parsePDU(data);
    return true;
}

void TinySMS::readAll(void (*callBack)(SMS))
{
    bool success = true;
    SMS sms;
    int i = 1;
    while (read(i, sms))
    {
        if (parseMultipart(sms))
        {
            callBack(sms);
        }
        i++;
    }
}

void TinySMS::removeRead()
{
    this->modem->sendAT(F("+CMGDA=\"DEL READ\""));
    this->modem->waitResponse();
}

void TinySMS::removeAll()
{
    this->modem->sendAT(F("+CMGDA=\"DEL ALL\""));
    this->modem->waitResponse();
}

void TinySMS::remove(uint8_t index)
{
    this->modem->sendAT("+CMGD=" + String(index));
    this->modem->waitResponse();
}