#include "TinySMS.h"

void TinySMS::isort(uint8_t *a, int n)
{
    for (int i = 1; i < n; ++i)
    {
        int j = a[i];
        int k;
        for (k = i - 1; (k >= 0) && (j < a[k]); k--)
        {
            a[k + 1] = a[k];
        }
        a[k + 1] = j;
    }
}

void TinySMS::swap(String &data)
{
    for (int i = 0; i < data.length(); i += 2)
    {
        char temp = data[i];
        data[i] = data[i + 1];
        data[i + 1] = temp;
    }
}

String TinySMS::decodeUnicode(String data)
{
    String text;
    int n = data.length();
    uint16_t U;
    char S[5];

    uint16_t _highSurrogate;
    uint32_t _codepoint;
    for (int i = 0; i < n; i += 4)
    {
        strcpy(S, data.substring(i, i + 4).c_str());
        U = strtoul(S, NULL, 16);

        //  isHighSurrogate?
        if (U >= 0xD800 && U < 0xDC00)
        {
            _highSurrogate = U & 0x3FF;
            continue;
        }
        //  isLowSurrogate?
        else if (U >= 0xDC00 && U < 0xE000)
            _codepoint =
                uint32_t(0x10000 + ((_highSurrogate << 10) | (U & 0x3FF)));
        else
            _codepoint = U;

        if (_codepoint < 0x80)
            text += char(_codepoint);
        else
        {
            char buf[5];
            char *p = buf;

            *(p++) = 0;
            *(p++) = char((_codepoint | 0x80) & 0xBF);
            uint16_t codepoint16 = uint16_t(_codepoint >> 6);
            if (codepoint16 < 0x20)
            { // 0x800
                *(p++) = char(codepoint16 | 0xC0);
            }
            else
            {
                *(p++) = char((codepoint16 | 0x80) & 0xBF);
                codepoint16 = uint16_t(codepoint16 >> 6);
                if (codepoint16 < 0x10)
                { // 0x10000
                    *(p++) = char(codepoint16 | 0xE0);
                }
                else
                {
                    *(p++) = char((codepoint16 | 0x80) & 0xBF);
                    codepoint16 = uint16_t(codepoint16 >> 6);
                    *(p++) = char(codepoint16 | 0xF0);
                }
            }

            while (*(--p))
                text += *p;
        }
    }
    return text;
}