#ifndef INTELHEX_PARSER_H
#define INTELHEX_PARSER_H

#include <stdbool.h>
#include <stdint.h>

struct intelhex_parser {
	//intelhex_parser_state state;
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

void intelhex_parser_init(struct intelhex_parser *parser);

/**
 * adds a new byte of data to the parser
 * @return < 0 if the data is invalid
 *				Subsequent calls to intelhex_parser_add_byte may result
 *				in invalid data after this has occurred.
 *         > 0 if the new byte results in a complete record
 *         == 0 if the byte is successfully absorbed
 */
int intelhex_parser_add_byte(struct intelhex_parser *parser, uint8_t byte);

/**
 * if intelparser_add_byte returns > 0, then calling intelhex_parser_get_data
 * will return the fully parsed record
 */
uint8_t *intelhex_parser_get_data(struct intelhex_parser *parser, uint32_t *address, uint32_t *length);

bool intelhex_parser_is_done(struct intelhex_parser *parser);

#endif
