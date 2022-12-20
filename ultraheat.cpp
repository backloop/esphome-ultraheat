#include <string>
#include "ultraheat.h"
//#include "ultraheat_message.h"
//#include "ultraheat_parser.h"


//default to no debug prints
#ifndef DEBUG_PRINTF
#define DEBUG_PRINTF(...)
#endif

int Ultraheat::read_bytes_until(char c, uint8_t *buffer, const uint16_t buf_size) {

    uint8_t iterations = 0;

    while (iterations < MAX_ITERATIONS) {

        uint8_t len = available();
        if (len == 0) {
            //ESP_LOGD("rbu", "delay %d", iterations);
            delay(WAIT_TIME);
            iterations++;
        } else {
            // reset counter as data was found
            iterations = 0;

            //ESP_LOGD("rbu", "serbuf: %p, write: %p", serbuf, write);
            //ESP_LOGD("rbu", "available: %d", len);

            // avoid serbuf overrun
            uint8_t readlen = (write+len)-serbuf < BUF_SIZE ? len : (serbuf+BUF_SIZE)-write;
            //ESP_LOGD("rbu", "want len: %d", len);

            if (readlen == 0) {
                ESP_LOGE("rbu", "no space left in serbuf, needs extra: %ld", (write+len)-(serbuf+BUF_SIZE));
                return 0;
            }

            if (!read_array(write, readlen)) {
                // timeout
                iterations++;
                if (iterations >= MAX_ITERATIONS) {
                    ESP_LOGE("rbu", "timeout");
                    return 0;
                }
            }

            // set new write pointer
            write += readlen;

            // terminate string
            *write = 0;
            //ESP_LOGD("rbu", "read %ld", write-serbuf);
            //ESP_LOGD("rbu", "%s", serbuf);

            char *pos = strchr((char *)serbuf, c);
            if (pos != NULL) {
                int len = (pos+1)-(char *)serbuf;
                //ESP_LOGD("rbu", "output: %d", len);

                if (len > (buf_size-1)) { // account for termination char
                    ESP_LOGE("rbu", "destination buffer is too small");
                    return 0;
                }

                // copy to output buffer
                memcpy(buffer, serbuf, len);
                *(buffer+len) = 0;

                // cache remainder
                len = strlen(pos+1);
                memmove(serbuf, pos+1, len);
                write = serbuf + len;
                memset(write, 0, serbuf + BUF_SIZE - write);

                // return count of bytes read
                return strlen((char *)buffer);
            }
        }
    }

    ESP_LOGI("rwr", "timeout");
    return 0;
}

//
//        bool read_bytes_until_match(uint8_t *str, uint8_t *buffer, const uint16_t buf_size) {
//            // initialize buffer
//            memset (buffer, 0, buf_size);
//
//            uint8_t iterations = 0;
//
//            int old_cnt = 0;
//            uint8_t *write = buffer;
//            while (strcmp((char *)buffer, (char *)str) != 0) {
//
//                // wait for slow 300bps chars to arrive
//                // TODO: timeout of read_array()
//                //delay(100);
//
//                uint8_t len = available();
//
//                if (len > 0) {
//                    //ESP_LOGD("wakeup", "buffer: %p, write: %p", buffer, write);
//                    //ESP_LOGD("wakeup", "available: %d", len);
//
//                    // avoid buffer overrun
//                    len = (write+len)-buffer < buf_size ? len : (buffer+buf_size)-write;
//                    //ESP_LOGD("wakeup", "want len: %d", len);§yy
//
//                    if (!read_array(write, len)) {
//                        // timeout
//                        iterations++;
//                        if (iterations >= 10) {
//                            //ESP_LOGD("read_bytes_until", "timeout...");
//                            return false;
//                        }
//                    }
//
//                    // terminate string
//                    *(write + len) = 0;
//
//                    //ESP_LOGD("wakeup", "got len: %d", len);§yy
//                    //ESP_LOGD("wakeup", "new: <%s>", buffer);
//
//                    // find last ocurrence of start character
//                    char *start = strrchr((char *)buffer, str[0]);
//
//                    // find last ocurrence of end character
//                    char *end = strrchr((char *)buffer, str[strlen(str)-1]);
//
//                    //ESP_LOGD("wakeup", "buffer: %p, write: %p, start: %p, end: %p", buffer, write, start, end);
//
//                    if (start == NULL) {
//                        start = (char *)buffer;
//                    }
//
//                    if (end == NULL) {
//                        end = (char *)(write + len);
//                    }
//                    //ESP_LOGD("wakeup", "buffer: %p, write: %p, start: %p, end: %p, len: %lu", buffer, write, start, end, end - start);
//
//                    if (start > (char *)buffer) {
//                        // cut away preceeding noise
//                        memmove(buffer, start, end - start);
//                    }
//
//                    // set new write pointer
//                    write = (uint8_t *)(buffer + (size_t)(end - start));
//
//                    // terminate string
//                    *write = 0;
//                    ESP_LOGD("wakeup", "stripped: <%s>", buffer);
//                }
//
//                // check how far the matching has come
//                int cnt = 0;
//                for (cnt = 0; cnt < strlen(str); cnt++) {
//                    if (buffer[cnt] != str[cnt]) break;
//                }
//                if (cnt == old_cnt) {
//                    // no progress yet
//                    iterations++;
//                    if (iterations >= 10) {
//                        //ESP_LOGD("read_bytes_until", "timeout...");
//                        return false;
//                    }
//                } else {
//                    // assume matching chars don't decrease over time
//                    old_cnt = cnt;
//                    iterations = 0;
//                }
//            }
//
//            return true;
//        }
