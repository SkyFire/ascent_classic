#pragma once

/************************************************************************/
/* Log Class                                                            */
/************************************************************************/
class CLog
{
public:
    int loglevel;

#define TRED FOREGROUND_RED | FOREGROUND_INTENSITY
#define TGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TYELLOW FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define TNORMAL FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
#define TWHITE TNORMAL | FOREGROUND_INTENSITY
#define TBLUE FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY

#define SetColor(color) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color)

    void PrintTime()
    {
        time_t tt = time(NULL);
        tm * t = localtime(&tt);
        printf("%02u:%02u:%02u", t->tm_hour, t->tm_min, t->tm_sec);
    }

    void Notice(const char* Type, const char* Format, ...)
    {
        va_list l;
        va_start(l, Format);
        SetColor(TNORMAL);
        PrintTime();
        printf(" N ");
        SetColor(TWHITE);
        printf(Type);
        printf(": ");
        SetColor(TNORMAL);
        vprintf(Format, l);
        putchar('\n');
        va_end(l);                
    }

    void Warning(const char* Type, const char* Format, ...)
    {
        if(loglevel < 2)
            return;

        va_list l;
        va_start(l, Format);
        SetColor(TNORMAL);
        PrintTime();
        SetColor(TYELLOW);
        printf(" W ");
        SetColor(TWHITE);
        printf(Type);
        printf(": ");
        SetColor(TYELLOW);
        vprintf(Format, l);
        putchar('\n');
        va_end(l);
    }

    void Error(const char* Type, const char* Format, ...)
    {
        if(loglevel < 1)
            return;

        va_list l;
        va_start(l, Format);
        SetColor(TNORMAL);
        PrintTime();
        SetColor(TRED);
        printf(" E ");
        SetColor(TWHITE);
        printf(Type);
        printf(": ");
        SetColor(TRED);
        vprintf(Format, l);
        putchar('\n');
        va_end(l);
    }

    void Success(const char* Type, const char* Format, ...)
    {
        va_list l;
        va_start(l, Format);
        SetColor(TNORMAL);
        PrintTime();
        SetColor(TGREEN);
        printf(" S ");
        SetColor(TWHITE);
        printf(Type);
        printf(": ");
        SetColor(TGREEN);
        vprintf(Format, l);
        putchar('\n');
        va_end(l);
    }

    void Debug(const char* Type, const char* Format, ...)
    {
        if(loglevel < 3)
            return;

        va_list l;
        va_start(l, Format);
        SetColor(TNORMAL);
        PrintTime();
        SetColor(TBLUE);
        printf(" D ");
        SetColor(TWHITE);
        printf(Type);
        printf(": ");
        SetColor(TBLUE);
        vprintf(Format, l);
        putchar('\n');
        va_end(l);                
    }
};

extern CLog Log;