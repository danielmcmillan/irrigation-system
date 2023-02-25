#ifndef _LOGGING_H
#define _LOGGING_H

#ifndef DISABLE_LOGGING
#define LOG_INFO(msg) Serial.println(F("INFO " msg))
#define LOG_ERROR(msg) Serial.println(F("ERROR " msg))
#else
#define LOG_INFO(msg)
#define LOG_ERROR(msg)
#endif

#endif
