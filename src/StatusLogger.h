#pragma once

// configs
#ifndef LOGGING_config_h
#include <configs/LOGGING_config.h>
#endif

#ifndef BRICKS_config_h
#include <configs/BRICKS_config.h>
#endif

// libs
#include <LoopbackStream.h>
#include <TimeLib.h>
#include <Arduino.h>

// defs
#ifndef LOG_LEVEL_SAVE
#define LOG_LEVEL_SAVE 2 // Save these logs to the buffer to be retrieved later
#endif
#ifndef LOG_LEVEL_PRINT
#define LOG_LEVEL_PRINT 2 // Print these logs to the console (can't all be retrieved later)
#endif
#ifndef LOG_SIZE
#define LOG_SIZE 4 * 1024 // 4 kb of log buffer
#endif
#define MAX_BRICK_STATUSES 20
#define MAX_TRACKED_PINGS 12

#define EOL "\n"

namespace StatusLogger
{
    namespace // "private" namespace. Makes it cleaner for humans (that's you!) to read.
    {
        typedef String LOG_FUNCTIONALITY_td;
        typedef int LOG_LEVEL_td;
        typedef String LOG_NAME_td;

        typedef struct
        {
            String device_ID = "";
            String session_ID;
            time_t session_start_time;
            time_t PINGS_SUCCESSFUL[MAX_TRACKED_PINGS] = {};
            time_t PINGS_UNSUCCESSFUL[MAX_TRACKED_PINGS] = {};
            bool session_reported_on_strapi = false;
        } SESSION_DETAILS;

        typedef struct
        {
            time_t timestamp;
            char device[20] = "";
            char functionality[10];
            char msg[60];
        } BRICK_STATUS;

        namespace DeviceHealth
        {
            int NumberOfFilesOnSD = 0;
            int NumberOfFilesUnuploaded = 0;
            BRICK_STATUS BrickStatuses[MAX_BRICK_STATUSES]; // Allow up to 20 brick reports
        }

        /**
         * @brief Print a brick status into a Serial readible format.
         *
         * @param BrickStatus The status of the brick (in a brick status object)
         * @returns a human-formatted String unpacking the brick status
         */
        String deconstructBrickStatus(BRICK_STATUS BrickStatus)
        {
            String str = String(BrickStatus.timestamp);
            str += " : ";
            str += BrickStatus.device;
            str += " : ";
            str += BrickStatus.functionality;
            if (strcmp(BrickStatus.msg, "") != 0)
            {
                str += " : ";
                str += BrickStatus.msg;
            }
            return str;
        }
    }

    const LOG_LEVEL_td LEVEL_GOOD_NEWS = 0;            // Used for reporting errors, the highest priority of content
    const LOG_LEVEL_td LEVEL_ERROR = 1;                // Used for reporting errors, the highest priority of content
    const LOG_LEVEL_td LEVEL_STATS = 2;                // Used for statistics reporting. Almost as important as errors
    const LOG_LEVEL_td LEVEL_N2k_ALERT = 2;            // Used for anything to do with the n2k network
    const LOG_LEVEL_td LEVEL_WARNING = 3;              // Used for warnings. Device should still be partly working (and making do).
    const LOG_LEVEL_td LEVEL_VERBOSE = 4;              // Used for dumping everything to print. Also check
    const LOG_LEVEL_td LEVEL_INTRUSIVELY_VERBOSE = 5;  // Used for dumping everything to print. Also check
    const LOG_LEVEL_td LEVEL_BRICK_STATUS_UPDATE = 10; // Used for dumping everything to print. Also check

    const LOG_FUNCTIONALITY_td FUNCTIONALITY_PARTIAL = "PARTIAL"; // It's behaving strangely, but still reachable
    const LOG_FUNCTIONALITY_td FUNCTIONALITY_FULL = "FULL";       // It's behaving strangely, but still reachable
    const LOG_FUNCTIONALITY_td FUNCTIONALITY_OFFLINE = "OFFLINE"; // It's behaving strangely, but still reachable

    LoopbackStream LogsCacher(LOG_SIZE);

    SESSION_DETAILS session_details;

    /**
     * @brief Log something to the StatusLogger system.
     *  If the (log) level is less than LOG_LEVEL_SAVE then we'll print both to Serial and to the cacher to be retrieved later (use sparingly)!

     *
     * @param level The log level (e.g. LEVEL_ERROR or LEVEL_VERBOSE)
     * @param device The device that is reporting the log (e.g. NAME_ACCEL)
     * @param msg The message that you want to log (e.g. "Successfully uploaded X")
     * @param print_to_serial An optional override to force to print to serial
     * @param stream  An optional override to change where this log is sent
     */
    void log(LOG_LEVEL_td level, String device, String msg, bool print_to_serial = false, Stream *stream = &Serial)
    {
        String str = String(millis());
        str += " : ";
        str += device;
        str += " : ";
        str += msg;
        String color_format;
        switch (level)
        {
        case LEVEL_GOOD_NEWS:
            color_format = "\033[1;32m";
            break;
        case LEVEL_ERROR:
            color_format = "\033[1;31m";
            break;
        case LEVEL_WARNING:
            color_format = "\033[1;33m";
            break;
        case LEVEL_BRICK_STATUS_UPDATE:
            color_format = "\033[1;36m";
            break;
        default:
            color_format = "\033[0;39;49m";
            break;
        }

        if (level <= LOG_LEVEL_PRINT || print_to_serial == true)
        {
            if (stream == &Serial)
            {
                stream->println(color_format + str + "\033[0;39;49m");
            }
            else
            {
                stream->println(str);
            }
        }
        if (level <= LOG_LEVEL_SAVE)
        {
            LogsCacher.println(str);
        }
    }

    /**
     * @brief Retrieve logs previously cached (good for log dumps of important logs as opposed to just Serial.print).
     *
     * @param stream An optional override to print the retrievedLogs elsewhere (like an opened file)
     */
    void retrieveLogs(Stream *stream = &Serial)
    {
        stream->println("\nRetrieving previous logs\n");
        while (LogsCacher.available())
        {
            Serial.write(LogsCacher.read());
        }
        stream->println("\nFinished retrieving previous logs\n");
    }

    /**
     * @brief Update a brick's status (for example if internet failed).
     *  Automatically makes a log status update.
     * @param device The device/brick that you are reporting on (e.g. NAME_ACCEL)
     * @param functionality How functional the brick is (e.g. FUNCTIONALITY_FULL)
     * @param msg An optional message to explain how functional/unfunctional it is
     */
    void setBrickStatus(String device, LOG_FUNCTIONALITY_td functionality, String msg = "")
    {
        // Check if the device already exists in the log
        int i;
        for (i = 0; device.compareTo(DeviceHealth::BrickStatuses[i].device) != 0 && strcmp(DeviceHealth::BrickStatuses[i].device, "") != 0; i++)
        {
            ;
        }

        // Unpack info
        device.toCharArray(DeviceHealth::BrickStatuses[i].device, sizeof(DeviceHealth::BrickStatuses[i].device)); // Doesn't matter if we re-write
        functionality.toCharArray(DeviceHealth::BrickStatuses[i].functionality, sizeof(DeviceHealth::BrickStatuses[i].functionality));
        msg.toCharArray(DeviceHealth::BrickStatuses[i].msg, sizeof(DeviceHealth::BrickStatuses[i].msg));
        DeviceHealth::BrickStatuses[i].timestamp = now();

        // Set this in the logs too, so we can retrieve later if need be
        if (msg.compareTo("") != 0)
        {
            msg = ", " + msg;
        }
        log(LEVEL_BRICK_STATUS_UPDATE, device, DeviceHealth::BrickStatuses[i].functionality + msg, true);
    }

    /**
     * @brief Retrieve all of the latest brick statuses and print to logs
     *
     * @param stream An optional override to print the retrievedLogs elsewhere (like an opened file)
     */
    void printBrickStatuses(Stream *stream = &Serial)
    {
        log(LEVEL_GOOD_NEWS, StatusLogger::NAME_ESP32, "-- BRICK STATUSES --", false, stream);
        for (int i = 0; strcmp(DeviceHealth::BrickStatuses[i].device, "") != 0; i++)
        {
            log(LEVEL_GOOD_NEWS, "BRICK STATUS", deconstructBrickStatus(DeviceHealth::BrickStatuses[i]), false, stream);
        }
        log(LEVEL_GOOD_NEWS, StatusLogger::NAME_ESP32, "-- END OF BRICK STATUSES --", false, stream);
    }

    /**
     * @brief Set the session details at the beginning of operation. Set once and forget!
     *
     * @param device_ID The id of this device (e.g. Jennifer)
     * @param session_FILENAME The filename of this session (e.g. Jennifer-{data}-0)
     * @param session_start_time The start time of the session
     */
    void setSessionDetails(String device_ID, String session_FILENAME, time_t session_start_time)
    {
        if (session_details.device_ID != "")
        {
            log(LEVEL_ERROR, NAME_ESP32, "Session details have already been set, you can not set them again!");
        }
        session_details.device_ID = device_ID;
        session_details.session_ID = device_ID + "-" + String(session_start_time) + "-" + 0;
        session_details.session_start_time = session_start_time;
    }

    /**
     * @brief Print only the session details to log
     */
    void printSessionDetails()
    {
        log(LEVEL_STATS, StatusLogger::NAME_ESP32, String(EOL) + "-- SESSION DETAILS --", true);
        log(LEVEL_STATS, StatusLogger::NAME_ESP32, "Device ID: " + session_details.device_ID, true);
        log(LEVEL_STATS, StatusLogger::NAME_ESP32, "Session name: " + session_details.session_ID, true);
        log(LEVEL_STATS, StatusLogger::NAME_ESP32, "Session start time: " + String(session_details.session_start_time), true);
        log(LEVEL_STATS, StatusLogger::NAME_ESP32, "-- END OF SESSION DETAILS --");
    }
};
