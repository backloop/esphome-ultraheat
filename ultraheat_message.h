#ifndef ULTRAHEAT_MESSAGE
#define ULTRAHEAT_MESSAGE

#include <string>

//default to no debug prints
#ifndef UM_PRINTF
#define UM_PRINTF(...)
#endif

class UltraheatMessage {

    public:
        //double model;
        double power_max_kw;
        double power_max_previous_year_kw;

        double heat_usage_mwh;
        double heat_previous_year_mwh;

        double volume_usage_m3;
        double volume_previous_year_m3;

        double error_number;
        double fault_hours;
        double fault_hours_previous_year;

        double flowrate_max_m3ph;
        double flowrate_max_previous_year_m3ph;
        double measuring_range_m3ph;

        std::string meter_date_time;
        double measurement_period_minutes;
        std::string yearly_set_day;
        std::string monthly_set_day;
        double operating_hours;
        double flow_hours;

        double flow_temperature_max_c;
        double return_temperature_max_c;
        double flow_temperature_max_previous_year_c;
        double return_temperature_max_previous_year_c;

        std::string device_number;
        std::string ownership_number;
        std::string settings_and_firmware;

        void print() {
            UM_PRINTF("power_max_kw                            %f", power_max_kw);
            UM_PRINTF("power_max_previous_year_kw              %f", power_max_previous_year_kw);

            UM_PRINTF("heat_usage_mwh                          %f", heat_usage_mwh);
            UM_PRINTF("heat_previous_year_mwh                  %f", heat_previous_year_mwh);

            UM_PRINTF("volume_usage_m3                         %f", volume_usage_m3);
            UM_PRINTF("volume_previous_year_m3                 %f", volume_previous_year_m3);

            UM_PRINTF("error_number                            %f", error_number);
            UM_PRINTF("fault_hours                             %f", fault_hours);
            UM_PRINTF("fault_hours_previous_year               %f", fault_hours_previous_year);

            UM_PRINTF("flowrate_max_m3ph                       %f", flowrate_max_m3ph);
            UM_PRINTF("flowrate_max_previous_year_m3ph         %f", flowrate_max_previous_year_m3ph);
            UM_PRINTF("measuring_range_m3ph                    %f", measuring_range_m3ph);

            UM_PRINTF("meter_date_time                         %s", meter_date_time.c_str());
            UM_PRINTF("measurement_period_minutes              %f", measurement_period_minutes);
            UM_PRINTF("yearly_set_day                          %s", yearly_set_day.c_str());
            UM_PRINTF("monthly_set_day                         %s", monthly_set_day.c_str());
            UM_PRINTF("operating_hours                         %f", operating_hours);
            UM_PRINTF("flow_hours                              %f", flow_hours);

            UM_PRINTF("flow_temperature_max_c                  %f", flow_temperature_max_c);
            UM_PRINTF("return_temperature_max_c                %f", return_temperature_max_c);
            UM_PRINTF("flow_temperature_max_previous_year_c    %f", flow_temperature_max_previous_year_c);
            UM_PRINTF("return_temperature_max_previous_year_c  %f", return_temperature_max_previous_year_c);

            UM_PRINTF("device_number                           %s", device_number.c_str());
            UM_PRINTF("ownership_number                        %s", ownership_number.c_str());
            UM_PRINTF("settings_and_firmware                   %s", settings_and_firmware.c_str());
        }
};

#endif // ULTRAHEAT_MESSAGE
