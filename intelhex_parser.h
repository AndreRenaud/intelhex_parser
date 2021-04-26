#ifndef INTELHEX_PARSER_H
#define INTELHEX_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * Structure describing the state of the parser
 * Note: There should be no need to access the members of this structure
 * directly. It is present in the header file to make it easier
 * to inline the structure in other modules to avoid dynamic allocation,
 * but the values in it should be considered private
 */
struct intelhex_parser {
    int pos;
    uint8_t buffer[256];
    uint8_t last_val;
    bool seen_eof;
    uint8_t crc;
    uint8_t line_length;
    uint8_t record_type;
    uint16_t line_address;
    uint32_t base_address;
};

/**
 * Initialise the parser struct
 * @param parser IntelHex parser object
 */
static inline void intelhex_parser_init(struct intelhex_parser *parser)
{
    memset(parser, 0, sizeof(*parser));
}

/**
 * Adds a new byte of data to the parser
 * @param parser IntelHex parser object
 * @param byte The byte of data to parser
 * @return < 0 if the data is invalid
 *				Subsequent calls to intelhex_parser_add_byte may
 *result in invalid data after this has occurred. > 0 if the new byte results
 *in a complete record
 *         == 0 if the byte is successfully absorbed
 */
int intelhex_parser_add_byte(struct intelhex_parser *parser, uint8_t byte);

/**
 * If intelparser_add_byte returns > 0, then calling intelhex_parser_get_data
 * will return the fully parsed record
 * @param parser IntelHex parser object
 * @param address Pointer to be set to the address where the return data is
 * for
 * @param length Pointer to be set to the number of bytes available in the
 * return value
 * @return pointer to *length bytes of raw data
 */
uint8_t *intelhex_parser_get_data(struct intelhex_parser *parser,
                                  uint32_t *address, uint32_t *length);

/**
 * Returns true if the parser has hit the EOF record in the hex data
 * @param parser IntelHex parser object
 */
bool intelhex_parser_is_done(struct intelhex_parser *parser);

#ifdef __cplusplus
}
#endif
#endif
