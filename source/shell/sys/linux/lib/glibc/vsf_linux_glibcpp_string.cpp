#include <string>

namespace std {
    // to_string
    string to_string(int value)
    {
        char buf[32];
        sprintf(buf, "%d", value);
        return (*new string(buf));
    }
    string to_string(long value)
    {
        char buf[32];
        sprintf(buf, "%ld", value);
        return (*new string(buf));
    }
    string to_string(long long value)
    {
        char buf[32];
        sprintf(buf, "%lld", value);
        return (*new string(buf));
    }
    string to_string(unsigned value)
    {
        char buf[32];
        sprintf(buf, "%u", value);
        return (*new string(buf));
    }
    string to_string(unsigned long value)
    {
        char buf[32];
        sprintf(buf, "%lu", value);
        return (*new string(buf));
    }
    string to_string(unsigned long long value)
    {
        char buf[32];
        sprintf(buf, "%llu", value);
        return (*new string(buf));
    }
    string to_string(float value)
    {
        char buf[32];
        sprintf(buf, "%f", value);
        return (*new string(buf));
    }
    string to_string(double value)
    {
        char buf[32];
        sprintf(buf, "%f", value);
        return (*new string(buf));
    }

    // to_wstring
    wstring to_wstring(int value)
    {
        wchar_t wbuf[32];
        swprintf(wbuf, sizeof(wbuf), L"%d", value);
        return (*new wstring(wbuf));
    }
    wstring to_wstring(long value)
    {
        wchar_t wbuf[32];
        swprintf(wbuf, sizeof(wbuf), L"%ld", value);
        return (*new wstring(wbuf));
    }
    wstring to_wstring(long long value)
    {
        wchar_t wbuf[32];
        swprintf(wbuf, sizeof(wbuf), L"%lld", value);
        return (*new wstring(wbuf));
    }
    wstring to_wstring(unsigned value)
    {
        wchar_t wbuf[32];
        swprintf(wbuf, sizeof(wbuf), L"%u", value);
        return (*new wstring(wbuf));
    }
    wstring to_wstring(unsigned long value)
    {
        wchar_t wbuf[32];
        swprintf(wbuf, sizeof(wbuf), L"%lu", value);
        return (*new wstring(wbuf));
    }
    wstring to_wstring(unsigned long long value)
    {
        wchar_t wbuf[32];
        swprintf(wbuf, sizeof(wbuf), L"%llu", value);
        return (*new wstring(wbuf));
    }
    wstring to_wstring(float value)
    {
        wchar_t wbuf[32];
        swprintf(wbuf, sizeof(wbuf), L"%f", value);
        return (*new wstring(wbuf));
    }
    wstring to_wstring(double value)
    {
        wchar_t wbuf[32];
        swprintf(wbuf, sizeof(wbuf), L"%f", value);
        return (*new wstring(wbuf));
    }

    // stox
};
