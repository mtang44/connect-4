#pragma once
#include <string>
#include <vector>
using namespace std;


struct LogStuff{
    string timeStamp;
    string loggingMessage;
    enum LogLevel { INFO, WARNING, ERROR, GAME_EVENT } level;
   
    };
class Logger{
public:
    vector<LogStuff> log;
    static Logger&  GetInstance();
    string getTimeStamp();
    void LogInfo(string message);
    void LogGameEvent(string event);
    void LogError(string error);
    void LogWarning(string warning);
    static Logger* instance;
    Logger();  // Private constructor for singleton pattern
   
    
};