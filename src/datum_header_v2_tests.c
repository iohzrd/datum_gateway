/*
 *
 * DATUM Gateway
 * Decentralized Alternative Templates for Universal Mining
 *
 * This file is part of OCEAN's Bitcoin mining decentralization
 * project, DATUM.
 *
 * https://ocean.xyz
 *
 * ---
 *
 * Copyright (c) 2025 Bitcoin Ocean, LLC & Luke Dashjr
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "datum_header_v2.h"
#include "datum_utils.h"

// Test vectors from bitcoinknots/bitcoin PR #359, src/test/data/block_header_v2.json.
// This file is generated from that JSON; do not edit it by hand.

typedef struct {
	const char *name;
	T_DATUM_HEADER_V2 header;
	const char *serialized;
	const char *xor_key_hash;
	const char *h1;
	const char *h2;
	const char *blake2b_1;
	const char *blake2b_2;
	const char *mask;
	const char *block_hash;
	uint8_t asic_profile;
	const char *asic_input;
} T_DATUM_BLAKE2B_TEST_VECTOR;

static const T_DATUM_BLAKE2B_TEST_VECTOR datum_blake2b_test_vectors[] = {
	{
		.name = "profile_0_time_offset",
		.header = {
			.version = 536870912,
			.prev_block = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00},
			.merkle_root = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0},
			.time = 2000000000U,
			.bits = 486604799U,
			.nonce = 195948557U,
			.nonce2 = 287454020U,
			.nonce3 = 2309737967U,
			.extranonce = {0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00},
			.time_offset = 600U,
			.txcount = 3,
			.flags = 28,
			.xor_key_mask_clear_bits = 0,
			.xor_key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
			.height = 840000,
			.mm_rhs = {0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab},
		},
		.serialized = "000000a01f1e1d1c1b1a191817161514131211100f0e0d0c0b0a0908070605040302010000112233445566778899aabbccddeeff00102030405060708090a0b0c0d0e0f0a8913577ffff001d0df0ad0b44332211efcdab89ffeeddccbbaa998877665544332211005802000003001c000000000000000000000000000000000040d10c008967452301efcdab8967452301efcdab8967452301efcdab8967452301efcdab",
		.xor_key_hash = "86e4855b51daf0932719011a6565a5908aef105fc6f8b85a23601de43865f4db",
		.h1 = "4ff7ec7f24f6935064cb962ec8cc0c947d60621cc22c5ba8516b0b995cd0c01b",
		.h2 = "ab5becb2336a3701557b0f6e33de39bd333072b8494c7c60952a8e8a636565e3",
		.blake2b_1 = "7e6326906eaa52fe59e03a14f1dfb8dd5d6e78497e56a8a6e4f4fb4d385e43db",
		.blake2b_2 = "4b495dcf05d70a49785b799b22284fbcd9dd1209237c53c87e4674b15587d704",
		.mask = "0000000000000000000000000000000000000000000000000000000000000000",
		.block_hash = "4b495dcf05d70a49785b799b22284fbcd9dd1209237c53c87e4674b15587d704",
		.asic_profile = 0,
		.asic_input = "000000000000943aff74219e1f45899abfdf536373c0f2fc92e6fe58335cd0ad0df0ad0b4433221158020000efcdab897e6326906eaa52fe59e03a14f1dfb8dd5d6e78497e56a8a6e4f4fb4d385e43db",
	},
	{
		.name = "profile_1_time_offset_nonzero_key",
		.header = {
			.version = 536870912,
			.prev_block = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00},
			.merkle_root = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0},
			.time = 2000000000U,
			.bits = 486604799U,
			.nonce = 195948557U,
			.nonce2 = 287454020U,
			.nonce3 = 2309737967U,
			.extranonce = {0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00},
			.time_offset = 600U,
			.txcount = 1,
			.flags = 29,
			.xor_key_mask_clear_bits = 0,
			.xor_key = {0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01},
			.height = 840001,
			.mm_rhs = {0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab},
		},
		.serialized = "000000a01f1e1d1c1b1a191817161514131211100f0e0d0c0b0a0908070605040302010000112233445566778899aabbccddeeff00102030405060708090a0b0c0d0e0f0a8913577ffff001d0df0ad0b44332211efcdab89ffeeddccbbaa998877665544332211005802000001001d00efcdab8967452301efcdab896745230141d10c008967452301efcdab8967452301efcdab8967452301efcdab8967452301efcdab",
		.xor_key_hash = "985071be3e4adf9e7983f4d5480ff4f77d14f6dcb6d2d9623226d3f0ff7687c6",
		.h1 = "b1fd91c55ba79811b18bb0b0d84d30c670c65243624cdddc510a74e0dd5a0123",
		.h2 = "be70c7fd6151172efb761561f3087bd61d97ccf070a1b05ae4c5d458686523d7",
		.blake2b_1 = "43cfb1efb395515321be80f8ff1961bcec5cb5b9780d552ccc46e878e27250a1",
		.blake2b_2 = "1c0a823c4f532ed5d25d2c5369cf151143f2479d5109e51209a960ad5fe0e958",
		.mask = "58b901be52b9b42d058537f41dd321bdcff5ec1cfa01f899625937f7279af7b7",
		.block_hash = "44b383821dea9af8d7d81ba7741c34ac8c07ab81ab081d8b6bf0575a787a1eef",
		.asic_profile = 1,
		.asic_input = "0df0ad0b44332211efcdab895802000043cfb1efb395515321be80f8ff1961bcec5cb5b9780d552ccc46e878e27250a1be70c7fd6151172efb761561f3087bd61d97ccf070a1b05ae4c5d458686523d7",
	},
	{
		.name = "profile_2_time_offset_selector_7",
		.header = {
			.version = 536870912,
			.prev_block = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00},
			.merkle_root = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0},
			.time = 2000000000U,
			.bits = 486604799U,
			.nonce = 195948557U,
			.nonce2 = 2864434397U,
			.nonce3 = 2309737967U,
			.extranonce = {0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00},
			.time_offset = 600U,
			.txcount = 3,
			.flags = 30,
			.xor_key_mask_clear_bits = 7,
			.xor_key = {0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe, 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe},
			.height = 840000,
			.mm_rhs = {0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab},
		},
		.serialized = "000000a01f1e1d1c1b1a191817161514131211100f0e0d0c0b0a0908070605040302010000112233445566778899aabbccddeeff00102030405060708090a0b0c0d0e0f0a8913577ffff001d0df0ad0bddccbbaaefcdab89ffeeddccbbaa998877665544332211005802000003001e071032547698badcfe1032547698badcfe40d10c008967452301efcdab8967452301efcdab8967452301efcdab8967452301efcdab",
		.xor_key_hash = "4ead68aa92ceffdb2bc4a97b01c1ad61bcd4db55a8ce5520358024180c71f2f9",
		.h1 = "c66fb3a62935bf95f948da68c7a6fc7f898ce507c15e493eaafa6d141ba787dd",
		.h2 = "dbe5fcacfe44b437da5612f21eb86c75e33868b40bfd7f458a9c3ee10a2be540",
		.blake2b_1 = "21e015cef9a6a323f1a5c1cdf791562447a78d2c7693ee0af9b553b80a574d19",
		.blake2b_2 = "0657ae2302c06ff0d557d62fa048791441a1057bf85baeb8ec1bba8c4a3476e3",
		.mask = "00aa74c7e80a7f4889d075e84d39086b99989dd8d7ba91ff6c69919317bc5895",
		.block_hash = "06fddae4eaca10b85c87a3c7ed71717fd83998a32fe13f4780722b1f5d882e76",
		.asic_profile = 2,
		.asic_input = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000dbe5fcacfe44b437da5612f21eb86c75e33868b40bfd7f458a9c3ee10a2be5400df0ad0bddccbbaa58020000efcdab8921e015cef9a6a323f1a5c1cdf791562447a78d2c7693ee0af9b553b80a574d19",
	},
	{
		.name = "profile_3_time_offset_selector_8",
		.header = {
			.version = 536870912,
			.prev_block = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00},
			.merkle_root = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0},
			.time = 2000000000U,
			.bits = 486604799U,
			.nonce = 195948557U,
			.nonce2 = 287454020U,
			.nonce3 = 16909060U,
			.extranonce = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
			.time_offset = 600U,
			.txcount = 3,
			.flags = 31,
			.xor_key_mask_clear_bits = 8,
			.xor_key = {0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe, 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe},
			.height = 840000,
			.mm_rhs = {0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab},
		},
		.serialized = "000000a01f1e1d1c1b1a191817161514131211100f0e0d0c0b0a0908070605040302010000112233445566778899aabbccddeeff00102030405060708090a0b0c0d0e0f0a8913577ffff001d0df0ad0b44332211040302010000000000000000ffffffffffffffff5802000003001f081032547698badcfe1032547698badcfe40d10c008967452301efcdab8967452301efcdab8967452301efcdab8967452301efcdab",
		.xor_key_hash = "4ead68aa92ceffdb2bc4a97b01c1ad61bcd4db55a8ce5520358024180c71f2f9",
		.h1 = "151989e9cec8011a56257e54ca73f3b11e904b024597c3fb214c51ab8f9cf5bf",
		.h2 = "9b10300a354d18ccf225314cae9a628c404edf068656236ecd0978875f9afa2e",
		.blake2b_1 = "b7b7fa4483321ad668a2e9a98489d6d82c3aca5ce5a78db4d824b7cf06e5e217",
		.blake2b_2 = "e69a31e0bb651ed5b3076ef46c9b27b5609e19407b7ff04b7c6292d0641428cd",
		.mask = "00aa74c7e80a7f4889d075e84d39086b99989dd8d7ba91ff6c69919317bc5895",
		.block_hash = "e6304527536f619d3ad71b1c21a22fdef9068498acc561b4100b034373a87058",
		.asic_profile = 3,
		.asic_input = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000009b10300a354d18ccf225314cae9a628c404edf068656236ecd0978875f9afa2e0df0ad0b443322115802000004030201b7b7fa4483321ad668a2e9a98489d6d82c3aca5ce5a78db4d824b7cf06e5e217",
	},
	{
		.name = "profile_0_time_offset_disabled_selector_255",
		.header = {
			.version = 536870912,
			.prev_block = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00},
			.merkle_root = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0},
			.time = 2000000000U,
			.bits = 486604799U,
			.nonce = 4294967295U,
			.nonce2 = 287454020U,
			.nonce3 = 2309737967U,
			.extranonce = {0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00},
			.time_offset = 1432778632U,
			.txcount = 3,
			.flags = 24,
			.xor_key_mask_clear_bits = 255,
			.xor_key = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
			.height = 840000,
			.mm_rhs = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		},
		.serialized = "000000a01f1e1d1c1b1a191817161514131211100f0e0d0c0b0a0908070605040302010000112233445566778899aabbccddeeff00102030405060708090a0b0c0d0e0f000943577ffff001dffffffff44332211efcdab89ffeeddccbbaa9988776655443322110088776655030018ff2222222222222222111111111111111140d10c000000000000000000000000000000000000000000000000000000000000000000",
		.xor_key_hash = "7bd0c09b8fe9cf1513cc141524b5bf46a4851ce06fe5bce05e04f9434f0a88a2",
		.h1 = "f06e36d21af2982442bff107cf6546b4440421f841a49ed027d4fa11e3e504e6",
		.h2 = "eae5d77dab38f5094ad95e848237614bd734f7fbc66f306422eccd2f009dc91e",
		.blake2b_1 = "544a71e01a4c041c727e86ec7cb2c68c62d9dcab0ee9b07cdaf1a59bf2e5d40b",
		.blake2b_2 = "c31b24420d67f86e524f980a24a18e88f36c821046d5288251b5d88998c69f87",
		.mask = "0000000000000000000000000000000000000000000000000000000000000001",
		.block_hash = "c31b24420d67f86e524f980a24a18e88f36c821046d5288251b5d88998c69f86",
		.asic_profile = 0,
		.asic_input = "000000000000943aff74219e1f45899abfdf536373c0f2fc92e6fe58335cd0adffffffff4433221188776655efcdab89544a71e01a4c041c727e86ec7cb2c68c62d9dcab0ee9b07cdaf1a59bf2e5d40b",
	},
};
static void datum_header_v2_tests_hexcmp(const char *what, const char *name, const unsigned char *actual, size_t len, const char *expected) {
	// The longest value compared here is the 164-byte serialized header, not
	// the ASIC input, whose maximum is 160.
	char buf[(DATUM_HEADER_V2_SIZE * 2) + 1];
	_Static_assert(DATUM_HEADER_V2_MAX_ASIC_INPUT <= DATUM_HEADER_V2_SIZE, "hex buffer is sized for the serialized header");
	size_t i;

	for (i = 0; i < len; i++) {
		uchar_to_hex(&buf[i << 1], actual[i]);
	}
	buf[len << 1] = 0;
	if (strcmp(buf, expected)) {
		fprintf(stderr, "BLAKE2b header vector \"%s\": %s mismatch\n  expected %s\n  actual   %s\n", name, what, expected, buf);
	}
	datum_test(strcmp(buf, expected) == 0);
}

void datum_header_v2_tests_sia_leaf(void);
void datum_header_v2_tests_sia_fields(void);

void datum_header_v2_tests(void) {
	unsigned char serialized[DATUM_HEADER_V2_SIZE];
	unsigned char pow[32], reversed[32];
	T_DATUM_HEADER_V2_COMPONENTS c;
	T_DATUM_HEADER_V2 roundtrip;
	size_t n = sizeof(datum_blake2b_test_vectors) / sizeof(datum_blake2b_test_vectors[0]);
	size_t i;
	int j;

	for (i = 0; i < n; i++) {
		const T_DATUM_BLAKE2B_TEST_VECTOR * const v = &datum_blake2b_test_vectors[i];

		datum_header_v2_serialize(&v->header, serialized);
		datum_header_v2_tests_hexcmp("serialized", v->name, serialized, DATUM_HEADER_V2_SIZE, v->serialized);

		// A serialize/deserialize round trip must preserve every field.
		datum_test(datum_header_v2_deserialize(&roundtrip, serialized));
		datum_test(memcmp(&roundtrip, &v->header, sizeof(roundtrip)) == 0);

		datum_header_v2_components(&v->header, &c);
		datum_test(c.asic_profile == v->asic_profile);
		datum_header_v2_tests_hexcmp("xor_key_hash", v->name, c.xor_key_hash, 32, v->xor_key_hash);
		datum_header_v2_tests_hexcmp("h1", v->name, c.h1, 32, v->h1);
		datum_header_v2_tests_hexcmp("h2", v->name, c.h2, 32, v->h2);
		datum_header_v2_tests_hexcmp("blake2b_1", v->name, c.hash1, 32, v->blake2b_1);
		datum_header_v2_tests_hexcmp("asic_input", v->name, c.asic_input, c.asic_input_len, v->asic_input);
		datum_header_v2_tests_hexcmp("blake2b_2", v->name, c.hash2, 32, v->blake2b_2);
		datum_header_v2_tests_hexcmp("mask", v->name, c.mask, 32, v->mask);
		datum_header_v2_tests_hexcmp("block_hash", v->name, c.result, 32, v->block_hash);

		// datum_header_v2_pow returns the internal byte order used for target
		// comparisons, which is the reverse of the vector's display form.
		datum_header_v2_pow(&v->header, pow);
		for (j = 0; j < 32; j++) reversed[j] = pow[31 - j];
		datum_header_v2_tests_hexcmp("pow", v->name, reversed, 32, v->block_hash);

		// datum_header_v2_build must reconstruct the same header from the job
		// fields plus the fields mining hardware grinds. It does not take the
		// XOR key or the merge-mining commitment, so those are copied in
		// afterwards -- which is why every vector can be checked here, whatever
		// key or selector it carries.
		{
			T_DATUM_HEADER_V2 built;
			// The job supplies the base (wire) time; the hardware's offset is
			// what turns it into the block time.
			uint32_t job_time = datum_header_v2_time_on_wire(&v->header);
			datum_header_v2_build(&built,
				v->header.version, v->header.prev_block, v->header.merkle_root,
				job_time,
				v->header.bits, v->header.height, v->header.txcount, v->header.flags,
				v->header.extranonce,
				v->header.nonce, v->header.nonce2, v->header.nonce3, v->header.time_offset);
			memcpy(built.xor_key, v->header.xor_key, 16);
			built.xor_key_mask_clear_bits = v->header.xor_key_mask_clear_bits;
			memcpy(built.mm_rhs, v->header.mm_rhs, 32);
			datum_header_v2_serialize(&built, serialized);
			datum_header_v2_tests_hexcmp("built serialized", v->name, serialized, DATUM_HEADER_V2_SIZE, v->serialized);
		}
	}

	// A job whose height is below the activation height must keep using the
	// legacy header; that decision lives in the stratum job builder, so here we
	// only check the header-size constant the serializer switches on.
	datum_test(DATUM_HEADER_V2_SIZE == 164);

	datum_header_v2_tests_sia_leaf();
	datum_header_v2_tests_sia_fields();
}

// The Siacoin dialect carries ntime and nonce as raw header bytes, so the
// mapping to the header v2 grinding fields is a plain splice. Checked against
// the ASIC input the vectors publish: for profile 0 the eight nonce bytes and
// eight ntime bytes appear at offsets 32 and 40 of that input, which is the
// Sia header layout.
void datum_header_v2_tests_sia_fields(void) {
	unsigned char nonce_bytes[8], ntime_bytes[8];
	unsigned char serialized[DATUM_HEADER_V2_SIZE];
	T_DATUM_HEADER_V2 h;
	T_DATUM_HEADER_V2_COMPONENTS c;
	size_t n = sizeof(datum_blake2b_test_vectors) / sizeof(datum_blake2b_test_vectors[0]);
	size_t i;

	for (i = 0; i < n; i++) {
		const T_DATUM_BLAKE2B_TEST_VECTOR * const v = &datum_blake2b_test_vectors[i];

		datum_header_v2_to_sia_fields(&v->header, nonce_bytes, ntime_bytes);

		// Exactly what client_mining_submit does: build the header from the
		// job's fields with the grinding fields zero, then splice in the eight
		// nonce bytes and eight ntime bytes the miner sent. The result must be
		// the vector's header, byte for byte.
		//
		// The block time is the part that can go wrong. m_time_offset is one of
		// the spliced fields, and with the time-offset flag set the block time
		// is derived from it, so a splice that leaves h.time at the value it had
		// before serializes the wrong time on the wire and commits it in h1.
		datum_header_v2_build(&h,
			v->header.version, v->header.prev_block, v->header.merkle_root,
			datum_header_v2_time_on_wire(&v->header),
			v->header.bits, v->header.height, v->header.txcount, v->header.flags,
			v->header.extranonce,
			0, 0, 0, 0);
		memcpy(h.xor_key, v->header.xor_key, 16);
		h.xor_key_mask_clear_bits = v->header.xor_key_mask_clear_bits;
		memcpy(h.mm_rhs, v->header.mm_rhs, 32);

		datum_header_v2_from_sia_fields(&h, nonce_bytes, ntime_bytes);
		datum_test(h.nonce == v->header.nonce);
		datum_test(h.nonce2 == v->header.nonce2);
		datum_test(h.nonce3 == v->header.nonce3);
		datum_test(h.time_offset == v->header.time_offset);
		datum_test(h.time == v->header.time);
		datum_test(datum_header_v2_time_on_wire(&h) == datum_header_v2_time_on_wire(&v->header));

		datum_header_v2_serialize(&h, serialized);
		datum_header_v2_tests_hexcmp("build+splice serialized", v->name, serialized, DATUM_HEADER_V2_SIZE, v->serialized);

		// For profile 0 the bytes must land where the Sia header puts them:
		// prevhash(32) || nonce(8) || ntime(8) || merkleroot(32).
		if (v->asic_profile == 0) {
			datum_header_v2_components(&v->header, &c);
			datum_test(c.asic_input_len == 80);
			datum_test(memcmp(&c.asic_input[32], nonce_bytes, 8) == 0);
			datum_test(memcmp(&c.asic_input[40], ntime_bytes, 8) == 0);
		}
	}
}

// The property the whole Siacoin-hardware plan rests on: a Sia-layout miner,
// given coinb1 = 000000 || h2 and an empty coinb2, builds a merkle leaf that is
// byte-identical to the hash1 of PR #359's construction. If this ever stops
// holding, unmodified Siacoin ASICs can no longer mine this chain.
void datum_header_v2_tests_sia_leaf(void) {
	unsigned char coinb1[DATUM_HEADER_V2_COINB1_SIZE];
	unsigned char arbtx[1 + DATUM_HEADER_V2_COINB1_SIZE + 16];
	unsigned char leaf[32], h1[32], h2[32];
	T_DATUM_HEADER_V2_COMPONENTS c;
	size_t n = sizeof(datum_blake2b_test_vectors) / sizeof(datum_blake2b_test_vectors[0]);
	size_t i;

	for (i = 0; i < n; i++) {
		const T_DATUM_BLAKE2B_TEST_VECTOR * const v = &datum_blake2b_test_vectors[i];

		// The per-job commitment must match what the full computation derives.
		datum_header_v2_job_commitment(v->header.version, v->header.prev_block, v->header.merkle_root,
			datum_header_v2_time_on_wire(&v->header), v->header.bits, v->header.height,
			v->header.txcount, v->header.flags,
			v->header.xor_key, v->header.xor_key_mask_clear_bits, v->header.mm_rhs,
			h1, h2);
		datum_header_v2_tests_hexcmp("job commitment h1", v->name, h1, 32, v->h1);
		datum_header_v2_tests_hexcmp("job commitment h2", v->name, h2, 32, v->h2);

		// Now mine it the way a Siacoin miner would: leaf = BLAKE2b(0x00 ||
		// coinb1 || extranonce), with coinb2 empty and no merkle branches.
		datum_header_v2_coinb1(h2, coinb1);
		arbtx[0] = 0x00;
		memcpy(&arbtx[1], coinb1, DATUM_HEADER_V2_COINB1_SIZE);
		memcpy(&arbtx[1 + DATUM_HEADER_V2_COINB1_SIZE], v->header.extranonce, 16);
		my_blake2b(leaf, arbtx, sizeof(arbtx));

		datum_header_v2_components(&v->header, &c);
		datum_header_v2_tests_hexcmp("sia leaf == hash1", v->name, leaf, 32, v->blake2b_1);
		datum_test(memcmp(leaf, c.hash1, 32) == 0);
	}
}
