#include <string>

namespace std {
    // to_string
    string to_string(int value)
    {
        int len = sprintf(NULL, "%d", value);
        string *newstr = new string(len);
        sprintf((char *)newstr->c_str(), "%d", value);
        return (*newstr);
    }
    string to_string(long value)
    {
        int len = sprintf(NULL, "%ld", value);
        string *newstr = new string(len);
        sprintf((char *)newstr->c_str(), "%ld", value);
        return (*newstr);
    }
    string to_string(long long value)
    {
        int len = sprintf(NULL, "%lld", value);
        string *newstr = new string(len);
        sprintf((char *)newstr->c_str(), "%lld", value);
        return (*newstr);
    }
    string to_string(unsigned value)
    {
        int len = sprintf(NULL, "%u", value);
        string *newstr = new string(len);
        sprintf((char *)newstr->c_str(), "%u", value);
        return (*newstr);
    }
    string to_string(unsigned long value)
    {
        int len = sprintf(NULL, "%lu", value);
        string *newstr = new string(len);
        sprintf((char *)newstr->c_str(), "%lu", value);
        return (*newstr);
    }
    string to_string(unsigned long long value)
    {
        int len = sprintf(NULL, "%llu", value);
        string *newstr = new string(len);
        sprintf((char *)newstr->c_str(), "%llu", value);
        return (*newstr);
    }
    string to_string(float value)
    {
        int len = sprintf(NULL, "%f", value);
        string *newstr = new string(len);
        sprintf((char *)newstr->c_str(), "%f", value);
        return (*newstr);
    }
    string to_string(double value)
    {
        int len = sprintf(NULL, "%f", value);
        string *newstr = new string(len);
        sprintf((char *)newstr->c_str(), "%f", value);
        return (*newstr);
    }

    // to_wstring
    wstring to_wstring(int value)
    {
        int len = swprintf(NULL, 0, L"%d", value);
        wstring *newstr = new wstring(len);
        swprintf((wchar_t *)newstr->c_str(), len + 1, L"%d", value);
        return (*newstr);
    }
    wstring to_wstring(long value)
    {
        int len = swprintf(NULL, 0, L"%ld", value);
        wstring *newstr = new wstring(len);
        swprintf((wchar_t *)newstr->c_str(), len + 1, L"%ld", value);
        return (*newstr);
    }
    wstring to_wstring(long long value)
    {
        int len = swprintf(NULL, 0, L"%lld", value);
        wstring *newstr = new wstring(len);
        swprintf((wchar_t *)newstr->c_str(), len + 1, L"%lld", value);
        return (*newstr);
    }
    wstring to_wstring(unsigned value)
    {
        int len = swprintf(NULL, 0, L"%u", value);
        wstring *newstr = new wstring(len);
        swprintf((wchar_t *)newstr->c_str(), len + 1, L"%u", value);
        return (*newstr);
    }
    wstring to_wstring(unsigned long value)
    {
        int len = swprintf(NULL, 0, L"%lu", value);
        wstring *newstr = new wstring(len);
        swprintf((wchar_t *)newstr->c_str(), len + 1, L"%lu", value);
        return (*newstr);
    }
    wstring to_wstring(unsigned long long value)
    {
        int len = swprintf(NULL, 0, L"%llu", value);
        wstring *newstr = new wstring(len);
        swprintf((wchar_t *)newstr->c_str(), len + 1, L"%llu", value);
        return (*newstr);
    }
    wstring to_wstring(float value)
    {
        int len = swprintf(NULL, 0, L"%f", value);
        wstring *newstr = new wstring(len);
        swprintf((wchar_t *)newstr->c_str(), len + 1, L"%f", value);
        return (*newstr);
    }
    wstring to_wstring(double value)
    {
        int len = swprintf(NULL, 0, L"%f", value);
        wstring *newstr = new wstring(len);
        swprintf((wchar_t *)newstr->c_str(), len + 1, L"%f", value);
        return (*newstr);
    }

    // stox
};
