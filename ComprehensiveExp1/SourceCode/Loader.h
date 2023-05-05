#pragma once

class Loader
{
private:
    static char* FilePath;

public:
    static char* ReadText(const char* FilePath);
    static void WriteText(const char* FilePath, const char*Text);
    static char* GetFilePath();
};

