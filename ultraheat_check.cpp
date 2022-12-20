#include <string>
#include <cstring>

std::string response[] = {
"6.8(0002.039*MWh)6.26(00040.62*m3)9.21(71422831)",
"6.26*01(00000.00*m3)6.8*01(0000.000*MWh)",
"F(0)9.20(71422831)6.35(60*m)",
"6.6(0008.9*kW)6.6*01(0000.0*kW)6.33(000.144*m3ph)9.4(084.2*C&040.4*C)",
"6.31(0001067*h)6.32(0000000*h)9.22(R)9.6(000&71422831&0&000&71422831&0)",
"9.7(20000)6.32*01(0000000*h)6.36(01-01&00:00)6.33*01(000.000*m3ph)",
"6.8.1()6.8.2()6.8.3()6.8.4()6.8.5()",
"6.8.1*01()6.8.2*01()6.8.3*01()",
"6.8.4*01()6.8.5*01()",
"9.4*01(000.0*C&000.0*C)",
"6.36.1(2022-11-26)6.36.1*01(2000-00-00)",
"6.36.2(2022-10-19)6.36.2*01(2000-00-00)",
"6.36.3(2022-11-20)6.36.3*01(2000-00-00)",
"6.36.4(2022-10-28)6.36.4*01(2000-00-00)",
"6.36.5()6.36*02(01&00:00)9.36(2022-12-02&22:41:34)9.24(1.5*m3ph)",
"9.17(0)9.18()9.19()9.25()",
"9.1(0&1&0&0017&CECV&CECV&1&5.24&5.24&F&081008&040404&08&0&00&:5&00&20)",
"9.2(&&)9.29()9.31(0000759*h)",
"9.0.1(00000000)9.0.2(00000000)9.34.1(000.00000*m3)9.34.2(000.00000*m3)",
"8.26.1(00000000*m3)8.26.2(00000000*m3)",
"8.26.1*01(00000000*m3)8.26.2*01(00000000*m3)",
"6.26.1()6.26.4()6.26.5()",
"6.26.1*01()6.26.4*01()6.26.5*01()0.0(71422831)",
};

#define ESP_LOGD(tag, format, ...)
//#define ESP_LOGD(tag, format, ...) printf(format "\r\n", __VA_ARGS__);
#define DEBUG_PRINTF(format, ...) printf(format"\r\n", __VA_ARGS__)

#include "ultraheat_functions.h"

class UltraheatParserTest : public UltraheatParser {

    public:
        void testSerialDataRowParsing() {
            printf("hello world\n");

            UltraheatMessage message;

            int len = sizeof(response) / sizeof(response[0]);
            printf("len: %d\n", len);

            for (int i = 0; i < len; i++) {
                // https://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
                //char *row = (char *)malloc(strlen(response[i].c_str())+1);
                //memcpy(row, response[i].c_str(), strlen(response[i].c_str())+1);
                char * row = response[i].data();
                printf("%s\n", row);

                parseResponseRow(&message, row);
                //free(row);
            }

            message.print();
        }
};

int main(int argc, char **argv) {

    UltraheatParserTest t;
    t.testSerialDataRowParsing();

    return 0;
}
