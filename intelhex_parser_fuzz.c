#include "intelhex_parser.c"

int LLVMFuzzerTestOneInput(const char *data, int size)
{
    struct intelhex_parser p;

    intelhex_parser_init(&p);

    for (int i = 0; i < size; i++) {
        int e = intelhex_parser_add_byte(&p, data[i]);
        if (e < 0)
            return 0;
        if (e > 0) {
            uint32_t address, length;
            uint8_t *data;
            data = intelhex_parser_get_data(&p, &address, &length);
            // Just access the data to make sure it's ok
            for (uint32_t i = 0; i < length; i++) {
                data[i] ^= data[i];
            }
        }
    }

    return 0;
}
