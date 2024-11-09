#include "TinySMS.h"

TinySMS::TinySMS(TinyGsm &modem)
{
    this->modem = &modem;
}

void TinySMS::clearPartial()
{
    this->partialSMS.clear();
}

void TinySMS::handle()
{
    String data;
    this->modem->waitResponse(this->handleTimeout, data, GF("\r\n"));
    if (data.indexOf(GF("+CMTI")) == -1)
        return;

    data = data.substring(data.indexOf(GF("+CMTI")));
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
    this->modem->waitResponse(GF("Call Ready"));
    this->modem->waitResponse(GF("SMS Ready"));
    // *set message format (pdu mode)
    this->modem->sendAT(GF("+CMGF=0"));
    this->modem->waitResponse();
    // *set character set
    this->modem->sendAT(GF("+CSCS=\"UCS2\""));
    this->modem->waitResponse();
    // *set New SMS Message Indications
    // buffer in TA when online
    // +CMT [<alpha>],<length><CR><LF><pdu>\
    // No CBM indications are routed to the TE
    // No SMS-STATUS-REPORTs are routed to the TE
    // resend buffer after mode change
    // this->modem->sendAT(F("+CNMI=2,2,0,0,0"));
    this->modem->sendAT(GF("+CNMI=2,1,0,0,0"));
    this->modem->waitResponse();
}

bool TinySMS::readRAW(uint8_t index, String &data)
{
    this->modem->waitResponse();
    this->modem->sendAT("+CMGR=" + String(index));
    this->modem->waitResponse(this->readTimeout, data);
    if (data.indexOf(GF("+CMGR:")) == -1)
        return false;
    data = data.substring(data.indexOf(GF("+CMGR:")) + 7);
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

void TinySMS::readAll(void (*callBack)(SMS), bool remove)
{
    SMS sms;
    LinkedList<uint8_t> indexList;
    int i = 1;
    while (read(i, sms))
    {
        if (parseMultipart(sms))
        {
            callBack(sms);
        }
        indexList.add(i);
        i++;
    }
    while (indexList.size() > 0)
    {
        this->remove(indexList.pop());
    }
}

void TinySMS::removeRead()
{
#ifdef TINY_GSM_MODEM_SIM800
    this->modem->sendAT(GF("+CMGDA=\"DEL READ\""));
#else
    this->modem->sendAT(GF("+CMGD=,3"));
#endif
    this->modem->waitResponse();
}

void TinySMS::removeAll()
{
#ifdef TINY_GSM_MODEM_SIM800
    this->modem->sendAT(GF("+CMGDA=\"DEL ALL\""));
#else
    this->modem->sendAT(GF("+CMGD=,4"));
#endif
    this->modem->waitResponse();
}

void TinySMS::remove(uint8_t index)
{
    this->modem->sendAT("+CMGD=" + String(index));
    this->modem->waitResponse();
}