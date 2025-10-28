#include "imgui/imgui.h"
#include "Logger.h"
#include <vector>
#include <string>
#include <ctime>
#include <iostream>


using namespace std;

Logger* Logger::instance = nullptr;

// Define the constructor (was declared in the header but not defined) -
// this is the symbol the linker was missing.
Logger::Logger()
{
    // Initialize log vector
    
    log.clear();
}

Logger& Logger::GetInstance(){
    
    if(!instance)
    {
      
        instance =  new Logger();
        string time = instance->getTimeStamp();
        

    }
    return *instance;
}
       string Logger :: getTimeStamp()
        {
            time_t now = time(nullptr);
            tm* tm_info = localtime(&now);
            char buffer[9];
            strftime(buffer, sizeof(buffer), "%H:%M:%S", tm_info);
            return buffer;
        }
       void Logger:: LogInfo(string message){
            // Create logstuff struct object and add logging string + level;
            
            LogStuff logEntry;
            logEntry.timeStamp = getTimeStamp();
            logEntry.loggingMessage = "[Info] " + message;
            logEntry.level = LogStuff::INFO;
            log.push_back(logEntry);
            // print info message to logger
            // ImGui::Text(message);
        }
        void Logger::LogGameEvent(string event){
            // print event message to logger
            LogStuff logEntry;
            logEntry.timeStamp = getTimeStamp();
            logEntry.loggingMessage = "[Game Event] " + event;
            logEntry.level = LogStuff::GAME_EVENT;
            log.push_back(logEntry);
            // ImGui::Text(event);
        }
        void Logger::LogError(string error){
            // print red  error message to logger
            LogStuff logEntry;
          
            logEntry.timeStamp = getTimeStamp();
            logEntry.loggingMessage = "[ERROR] " + error;
            logEntry.level = LogStuff::ERROR;
            log.push_back(logEntry);
           
            // Red color for errors
        }
        void Logger::LogWarning(string warning){
           // print vellow  error message to logger
            LogStuff logEntry; 
            logEntry.timeStamp = getTimeStamp();
            logEntry.loggingMessage = "[WARN] "+ warning;
            logEntry.level = LogStuff::WARNING;
            log.push_back(logEntry);
        }
    
