#include <stdio.h>
#include <string.h>

#include "intelhex_parser.h"

#define ERR(a, b...) fprintf(stderr, "ihex:%s:%d: " a "\n", __func__, __LINE__ , ##b)

enum {
	IHEX_RECORD_DATA,
	IHEX_RECORD_EOF,
	IHEX_RECORD_EXTENDED_SEGMENT_ADDRESS,
	IHEX_RECORD_START_SEGMENT_ADDRESS,
	IHEX_RECORD_EXTENDED_LINEAR_ADDRESS,
	IHEX_RECORD_START_LINEAR_ADDRESS,
};

void intelhex_parser_init(struct intelhex_parser *parser)
{
	memset(parser, 0, sizeof(*parser));
}

static int from_hex(uint8_t byte)
{
	if (byte >= '0' && byte <= '9')
		return byte - '0';
	if (byte >= 'a' && byte <= 'f')
		return byte - 'a' + 10;
	if (byte >= 'A' && byte <= 'F')
		return byte - 'A' + 10;
	return -1;
}

int intelhex_parser_add_byte(struct intelhex_parser *parser, uint8_t byte)
{
	bool parsed_line = false;
	if (parser->seen_eof) {
		if (byte == '\n' || byte == '\r')
			return 0;
		return -1;
	}
	if (parser->pos == 0) {
		if (byte == ':')
			parser->pos++;
		else if (byte != '\n' && byte != '\r')
			return -1;
		return 0;
	}

	int val = from_hex(byte);
	if (val < 0)
		return -1;
	if (parser->pos % 2 == 0) {
		parser->crc += parser->last_val << 4 | val;
		//ERR("crc: 0x%x 0x%x 0x%x", (~parser->crc) & 0xff, parser->crc, parser->last_val << 4 | val);
	}
	switch (parser->pos) {
		case 1: // byte count
		case 2:
			parser->line_length <<= 4;
			parser->line_length |= val;
			//if (parser->pos == 2) ERR("line_length: 0x%x", parser->line_length);
			break;

		case 3: // addr
		case 4:
		case 5:
		case 6:
			parser->line_address <<= 4;
			parser->line_address |= val;
			//if (parser->pos == 6) ERR("line address: 0x%x", parser->line_address);
			break;

		case 7:
		case 8:
			parser->record_type <<= 4;
			parser->record_type |= val;
			//if (parser->pos == 8) ERR("record type: %d", parser->record_type);
			break;

		default: {
			// Is it a data byte?
			int data_pos = parser->pos - 9;

			if (data_pos / 2 < parser->line_length) {
				if (data_pos % 2 != 0) {
					parser->buffer[data_pos / 2] = parser->last_val << 4 | val;
					//ERR("data %d/%d: 0x%x", data_pos / 2, parser->line_length, parser->buffer[data_pos / 2]);
				}
			} else if (data_pos == parser->line_length * 2) {
				;
			} else if (data_pos == parser->line_length * 2 + 1) {
				//uint8_t crc = parser->last_val << 4 | val;
				//ERR("crc: 0x%x 0x%x", crc, parser->crc);
				// The CRC will have evaluated to 0 if all went well
				if (parser->crc != 0)
					return -1;
				// TODO: Check CRC
				parsed_line = true;
			} else {
				ERR("pos: %d", parser->pos);
				return -1;
			}
			break;
		}
	}

	if (parsed_line) {
		switch (parser->record_type) {
			case IHEX_RECORD_DATA: // normal data
				break;
			case IHEX_RECORD_EOF:
				parser->seen_eof = true;
				parsed_line = false;
				break;
			default:
				ERR("unhandled record %d", parser->record_type);
				return -1;
		}
		parser->pos = 0;
	}
	else
		parser->pos++;

	parser->last_val = val;

	return parsed_line ? 1 : 0;
}

uint8_t *intelhex_parser_get_data(struct intelhex_parser *parser, uint32_t *address, uint32_t *length)
{
	// TODO: Better check
	if (parser->pos != 0)
		return NULL;
	//ERR("line_length: 0x%x", parser->line_length);
	//ERR("line_address: 0x%x", parser->line_address);
	*address = parser->line_address | parser->base_address;
	*length = parser->line_length;

	// reset the parser...
	parser->last_val = 0;
	parser->line_address = 0;
	parser->base_address = 0;
	parser->record_type = 0;
	parser->crc = 0;

	return &parser->buffer[0];
}

bool intelhex_parser_is_done(struct intelhex_parser *parser)
{
	return parser->seen_eof;
}
