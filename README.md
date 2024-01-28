# TinySMS

A simple SMS library for GSM modules with unicode support

This library allows you to listen and receive SMS messages from a GSM module (e.g. from SIM800L)

## Features

- [x] Unicode support
- [x] Multipart SMS
- [x] Alphanumeric number
- [ ] Sending messages

### Supported modules

- [x] SIM800L
- [ ] SIMCOM 800 series
- [ ] SIMCOM 900 series

<sup>\* Unchecked boards will probably work. Please notify me if it works on your specific board</sup>

## Installation

#### PlatformIO

- Just add `vahidtvj/TinySMS` to your `lib_deps` list

#### Arduino

- First Install [TinyGSM](https://github.com/vshymanskyy/TinyGSM) and [LinkedList](https://github.com/ivanseidel/LinkedList) libraries
- Download the [Latest release](https://github.com/vahidtvj/TinySMS/releases/latest/) from github
- Unzip and modify the Folder name to "TinySMS"
- Move the folder to your Library folder

## Usage

This library works alongside [TinyGSM](https://github.com/vshymanskyy/TinyGSM)

Please see the examples folder for use cases

Example setup for SIM800L module:

```c
#include <Arduino.h>
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <TinySMS.h>
TinyGsm modem(Serial);
TinySMS modemSMS(modem);

void setup()
{
    // TinyGsm config
    TinyGsmAutoBaud(Serial);
    modem.init();

    // setup modem for sms
    modemSMS.begin();
}
```
