#include <stdio.h>
#include <string.h>

#include "acutest.h"
#include "intelhex_parser.h"

static void test_line(void)
{
    char *line = ":0B0010006164647265737320676170A7";
    uint8_t known_data[] = {0x61, 0x64, 0x64, 0x72, 0x65, 0x73,
                            0x73, 0x20, 0x67, 0x61, 0x70};
    struct intelhex_parser p;
    intelhex_parser_init(&p);
    while (*line) {
        intelhex_parser_add_byte(&p, *line);
        line++;
    }
    uint32_t address;
    uint32_t len;
    uint8_t *data = intelhex_parser_get_data(&p, &address, &len);

    TEST_ASSERT(address == 0x10);
    TEST_ASSERT(len == 0xb);
    TEST_CHECK(memcmp(data, known_data, len) == 0);
    TEST_DUMP("expected", known_data, sizeof(known_data));
    TEST_DUMP("received", data, len);
}

static void test_simple(void)
{
    char *data = ":10010000214601360121470136007EFE09D2190140\n"
                 ":100110002146017E17C20001FF5F16002148011928\n"
                 ":10012000194E79234623965778239EDA3F01B2CAA7\n"
                 ":100130003F0156702B5E712B722B732146013421C7\n"
                 ":00000001FF\n";
    struct intelhex_parser p;
    intelhex_parser_init(&p);
    int nlines = 0;
    while (*data) {
        int e = intelhex_parser_add_byte(&p, *data);
        TEST_ASSERT(e >= 0);
        if (e > 0) {
            uint32_t address;
            uint32_t len;
            uint8_t *hex_data = intelhex_parser_get_data(&p, &address, &len);

            printf("hex_data: %p address: 0x%x len: 0x%x\n", hex_data,
                   address, len);
            TEST_ASSERT(address == 0x100 + 0x10 * nlines);
            TEST_ASSERT(len == 0x10);
            nlines++;
        }
        data++;
    }
    TEST_ASSERT(intelhex_parser_is_done(&p));
}

static void test_crc_fail(void)
{
    // Data from test_line, but CRC has been broken
    char *line = ":0B0010006164647265737320676170A8";
    struct intelhex_parser p;
    uint32_t address;
    uint32_t len;
    intelhex_parser_init(&p);
    while (*line) {
        intelhex_parser_add_byte(&p, *line);
        line++;
    }
    TEST_ASSERT(intelhex_parser_get_data(&p, &address, &len) == NULL);
}

static void test_file(void)
{
    FILE *fp = fopen("hello_world.ihex", "rb");
    struct intelhex_parser p;
    intelhex_parser_init(&p);

    TEST_ASSERT(fp != NULL);
    while (!feof(fp)) {
        int ch = fgetc(fp);
        if (ch >= 0) {
            int e = intelhex_parser_add_byte(&p, ch);
            TEST_ASSERT(e >= 0);
            if (e > 0) {
                uint32_t address;
                uint32_t len;
                uint8_t *data = intelhex_parser_get_data(&p, &address, &len);

                printf("data: %p address: 0x%x len: 0x%x\n", data, address,
                       len);
            }
        }
    }
    fclose(fp);
}

TEST_LIST = {{"line", test_line},
             {"simple", test_simple},
             {"crc_fail", test_crc_fail},
             {"file", test_file},
             {NULL, NULL}};