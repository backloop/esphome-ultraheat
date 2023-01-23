#ifndef ULTRAHEAT_DEBUG
#define ULTRAHEAT_DEBUG

//
// This header must be listed first in ultraheat.yaml for this defines
// to be accepted without "redifinition warnings" as esphome includes
// all headers in the listed order. 
//

// statemachine
//#define SM_PRINTF(f_, ...) printf("%s ", timestamp()), printf("machine: "), printf((f_), ##__VA_ARGS__), printf("\n")
//#define SM_PRINTF(f_, ...) ESP_LOGD("machine: ", f_"\r\n", ##__VA_ARGS__)
// ultraheat_states
//#define US_PRINTF(f_, ...) printf("%s ", timestamp()), printf("states:  "), printf((f_), ##__VA_ARGS__), printf("\n")
// ultraheat_message
//#define UM_PRINTF(f_, ...) printf("%s ", timestamp()), printf("message: "), printf((f_), ##__VA_ARGS__), printf("\n")
//#define UM_PRINTF(f_, ...) ESP_LOGD("message: ", f_"\r\n", ##__VA_ARGS__)
// ultraheat_parser
//#define UP_PRINTF(f_, ...) printf("%s ", timestamp()), printf("parser:  "), printf((f_), ##__VA_ARGS__), printf("\n")


#endif // ULTRAHEAT_DEBUG
