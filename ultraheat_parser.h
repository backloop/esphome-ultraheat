#ifndef ULTRAHEAT_PARSER
#define ULTRAHEAT_PARSER

#include <string>
#include <cstring>
#include "ultraheat_message.h"

//default to no debug prints
#ifndef UP_PRINTF
#define UP_PRINTF(...)
#endif

class UltraheatParser {

    public:
        void parseResponseRow(UltraheatMessage& message, char* row) {
            char* saveptr = NULL;

            // cannot use strtok() below as strtok() won't handle empty values
            // like "()" as one would want (it consumes both delimiters simultaneously)

            // while not end of string
            while (*row != 0) {
                char* obisCode = row;
                char* next = strchr(row, '(');
                *next = 0;
                row = next + 1;

                char* value = row;
                next = strchr(row, ')');
                *next = 0;
                row = next + 1;

                parseResponseBlock(message, obisCode, value);
            }
        }

        void parseResponseBlock(UltraheatMessage& message, char* obisCode, char* value) {
            //UP_PRINTF("%s, %s", obisCode, value);

            if (strncmp(obisCode, "F", 1) == 0) {
                message.error_number = atof(value);

            } else if (strcmp(obisCode, "6.6") == 0) {
                message.power_max_kw = atof(value);

            } else if (strcmp(obisCode, "6.6*01") == 0) {
                message.power_max_previous_year_kw = atof(value);

            } else if (strcmp(obisCode, "6.8") == 0) {
                message.heat_usage_mwh = atof(value);

            } else if (strcmp(obisCode, "6.8*01") == 0) {
                message.heat_previous_year_mwh = atof(value);

            } else if (strcmp(obisCode, "6.26") == 0) {
                message.volume_usage_m3 = atof(value);

            } else if (strcmp(obisCode, "6.26*01") == 0) {
                message.volume_previous_year_m3 = atof(value);

            } else if (strcmp(obisCode, "6.31") == 0) {
                message.operating_hours = atof(value);

            } else if (strcmp(obisCode, "6.32") == 0) {
                message.fault_hours = atof(value);

            } else if (strcmp(obisCode, "6.32*01") == 0) {
                message.fault_hours_previous_year = atof(value);

            } else if (strcmp(obisCode, "6.33") == 0) {
                message.flowrate_max_m3ph = atof(value);

            } else if (strcmp(obisCode, "6.33*01") == 0) {
                message.flowrate_max_previous_year_m3ph = atof(value);

            } else if (strcmp(obisCode, "6.35") == 0) {
                message.measurement_period_minutes = atof(value);

            } else if (strcmp(obisCode, "6.36") == 0) {
                // format not understood, send raw string
                message.yearly_set_day = value;

            } else if (strcmp(obisCode, "6.36*02") == 0) {
                // format not understood, send raw string
                message.monthly_set_day = value;

            //6.36.1
            //6.36.1*01
            //6.36.2
            //6.36.2*01
            //6.36.3
            //6.36.3*01
            //6.36.4
            //6.36.4*01
            //6.36.5

            } else if (strcmp(obisCode, "9.1") == 0) {
                // format not understood, send raw string
                message.settings_and_firmware = value;

            //9.2

            } else if (strcmp(obisCode, "9.4") == 0) {
                // must split value
                char* val = strtok(value, "*");
                message.flow_temperature_max_c = atof(val);
                val = strtok(NULL, "*C&"); //ignore leading delimiters "C&", and terminate on "*"
                message.return_temperature_max_c = atof(val);
            } else if (strcmp(obisCode, "9.4*01") == 0) {
                // must split value
                char* val = strtok(value, "*");
                message.flow_temperature_max_previous_year_c = atof(val);
                val = strtok(NULL, "*C&"); //ignore leading delimiters "C&", and terminate on "*"
                message.return_temperature_max_previous_year_c = atof(val);

            //9.6
            //9.7

            } else if (strcmp(obisCode, "9.20") == 0) {
                message.device_number = value;

            } else if (strcmp(obisCode, "9.21") == 0) {
                message.ownership_number = value;

            //9.22

            } else if (strcmp(obisCode, "9.24") == 0) {
                message.measuring_range_m3ph = atof(value);

            } else if (strcmp(obisCode, "9.31") == 0) {
                message.flow_hours = atof(value);

            } else if (strcmp(obisCode, "9.36") == 0) {
                message.meter_date_time = value;
            }
        }
};

#endif // ULTRAHEAT_PARSER
