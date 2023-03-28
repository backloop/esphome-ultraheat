#ifndef ULTRAHEAT_IO
#define ULTRAHEAT_IO

#define DATA_SIZE (128)
#define LINE_SIZE (DATA_SIZE - 1) // make space for string termination

#ifndef IO_PRINTF
#define IO_PRINTF(...)
#endif

namespace Ultraheat {

    class IO {

        public:

            IO()
                : data()
                , write(data)
            {}

            virtual void setup() {};
            virtual int available() = 0;
            virtual bool read_array(uint8_t *data, size_t len) = 0;
            //virtual void write_array(const std::vector<uint8_t> &data) {};
            virtual void write_array(const uint8_t *data, size_t len) = 0;
            virtual void clear_buffers(void) {}

            // 0 - line is complete
            // 1 - nothing to read
            // 2 - buffer full
            // 3 - read timeout
            // 4 - return data buffer is too small
            // 5 - line is not complete
            int read_line_nonblock(uint8_t* data, size_t max_size) {

                // find newline
                char c = '\n';
                char *pos = strchr((char *)this->data, c);

                if (pos == nullptr) {
                    // now newline found in buffer, read mmore data

                    int available = this->available();

                    if (!available) {
                        return 1;
                    }

                    int space_remaining = (this->data + LINE_SIZE) - this->write;
                    if (space_remaining <= 0) {
                        return 2;
                    }
                    IO_PRINTF("space_remaining: %d", space_remaining);

                    int possible = available < space_remaining ? available : space_remaining;

                    if (!this->read_array(this->write, possible)) {
                        return 3;
                    }
                    this->write += possible;
                    *this->write = '\0';
                    IO_PRINTF("data: <%s>", this->data);

                    // find newline
                    char *pos = strchr((char *)this->data, c);

                    if (pos == nullptr) {
                        return 5;
                    }
                }

                // include newline
                pos++;

                // line length
                size_t len = pos  - (char *)this->data;

                // will it fit the output buffer?
                if (len > (max_size-1)) { // account for string termination
                    return 4;
                }

                // copy to output buffer
                memcpy(data, this->data, len);
                *(data+len) = 0;

                // move remainder to beginning
                len = (this->write - this->data) - len;
                memmove(this->data, pos, len);
                this->write = this->data + len;
                memset(this->write, 0, this->data + DATA_SIZE - this->write);

                // remainder
                IO_PRINTF("remainder: <%s>", this->data);

                return 0;
            }

        private:
            uint8_t data[DATA_SIZE];
            uint8_t *write;
    };
};

#endif
