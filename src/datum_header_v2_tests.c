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
			.flags = 92,
			.xor_key_mask_clear_bits = 0,
			.xor_key = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
			.height = 840000,
			.mm_rhs = {0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab},
		},
		.serialized = "000000a01f1e1d1c1b1a191817161514131211100f0e0d0c0b0a0908070605040302010000112233445566778899aabbccddeeff00102030405060708090a0b0c0d0e0f0a8913577ffff001d0df0ad0b44332211efcdab89ffeeddccbbaa998877665544332211005802000003005c000000000000000000000000000000000040d10c008967452301efcdab8967452301efcdab8967452301efcdab8967452301efcdab",
		.xor_key_hash = "86e4855b51daf0932719011a6565a5908aef105fc6f8b85a23601de43865f4db",
		.h1 = "dae7762b2999ee4d30479ab8b52fb58f27e061cd7d080b7964aa5a3ec2d95f6f",
		.h2 = "42108894e3e4d6e3172e379412d4bf4e03c259f46d58fa419e44ec54620f1d54",
		.blake2b_1 = "c63e7f0d331b6b9b5974b8c798bc135979a2982a8ac1a8ff1e4fc4edc3dc790a",
		.blake2b_2 = "c80e70f7a4a0dbfef4b23c32c52a8e632115c7056b85efd495eadaf66bbc0aef",
		.mask = "0000000000000000000000000000000000000000000000000000000000000000",
		.block_hash = "c80e70f7a4a0dbfef4b23c32c52a8e632115c7056b85efd495eadaf66bbc0aef",
		.asic_profile = 0,
		.asic_input = "000000000000943aff74219e1f45899abfdf536373c0f2fc92e6fe58335cd0ad0df0ad0b4433221158020000efcdab89c63e7f0d331b6b9b5974b8c798bc135979a2982a8ac1a8ff1e4fc4edc3dc790a",
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
			.flags = 93,
			.xor_key_mask_clear_bits = 0,
			.xor_key = {0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01},
			.height = 840001,
			.mm_rhs = {0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab},
		},
		.serialized = "000000a01f1e1d1c1b1a191817161514131211100f0e0d0c0b0a0908070605040302010000112233445566778899aabbccddeeff00102030405060708090a0b0c0d0e0f0a8913577ffff001d0df0ad0b44332211efcdab89ffeeddccbbaa998877665544332211005802000001005d00efcdab8967452301efcdab896745230141d10c008967452301efcdab8967452301efcdab8967452301efcdab8967452301efcdab",
		.xor_key_hash = "985071be3e4adf9e7983f4d5480ff4f77d14f6dcb6d2d9623226d3f0ff7687c6",
		.h1 = "7b9a0f388499bdb0e2190a03675cb29567c3060f6b7c6a4eff63db2bec7c3c0f",
		.h2 = "4ae51736084aaae056338bc6d3c271b9d6ce7786844bf27f61d988546531aff6",
		.blake2b_1 = "7780757a72ffbbfa4ffcbd51c1b71ae5dc6138636f45705bf3ab6ffaadadb650",
		.blake2b_2 = "07bfd82ef1e496b7ca92310ca4786ab1cc1bb7eefe33d85af274b27138e64ae3",
		.mask = "58b901be52b9b42d058537f41dd321bdcff5ec1cfa01f899625937f7279af7b7",
		.block_hash = "5f06d990a35d229acf1706f8b9ab4b0c03ee5bf2043220c3902d85861f7cbd54",
		.asic_profile = 1,
		.asic_input = "0df0ad0b44332211efcdab89580200007780757a72ffbbfa4ffcbd51c1b71ae5dc6138636f45705bf3ab6ffaadadb6504ae51736084aaae056338bc6d3c271b9d6ce7786844bf27f61d988546531aff6",
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
			.flags = 94,
			.xor_key_mask_clear_bits = 7,
			.xor_key = {0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe, 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe},
			.height = 840000,
			.mm_rhs = {0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab},
		},
		.serialized = "000000a01f1e1d1c1b1a191817161514131211100f0e0d0c0b0a0908070605040302010000112233445566778899aabbccddeeff00102030405060708090a0b0c0d0e0f0a8913577ffff001d0df0ad0bddccbbaaefcdab89ffeeddccbbaa998877665544332211005802000003005e071032547698badcfe1032547698badcfe40d10c008967452301efcdab8967452301efcdab8967452301efcdab8967452301efcdab",
		.xor_key_hash = "4ead68aa92ceffdb2bc4a97b01c1ad61bcd4db55a8ce5520358024180c71f2f9",
		.h1 = "656685c4322513b7c1f79d38934b5cb9b50232dec8e878fe9fc4ef0a464265b5",
		.h2 = "a3e76c40212f55077dab4c6fbe9443b2074d84f4ddb77d7766b5a0fdc109fec7",
		.blake2b_1 = "494457efa822c2d1558c7001a42a7b4f303e7b6d449006c559fe2ec9e4033fcd",
		.blake2b_2 = "7d294afc786a5f4805ba22e8058d42097ab0c52b51cb49db55d4819fd846e7b2",
		.mask = "00aa74c7e80a7f4889d075e84d39086b99989dd8d7ba91ff6c69919317bc5895",
		.block_hash = "7d833e3b906020008c6a570048b44a62e32858f38671d82439bd100ccffabf27",
		.asic_profile = 2,
		.asic_input = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000a3e76c40212f55077dab4c6fbe9443b2074d84f4ddb77d7766b5a0fdc109fec70df0ad0bddccbbaa58020000efcdab89494457efa822c2d1558c7001a42a7b4f303e7b6d449006c559fe2ec9e4033fcd",
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
			.flags = 95,
			.xor_key_mask_clear_bits = 8,
			.xor_key = {0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe, 0x10, 0x32, 0x54, 0x76, 0x98, 0xba, 0xdc, 0xfe},
			.height = 840000,
			.mm_rhs = {0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab},
		},
		.serialized = "000000a01f1e1d1c1b1a191817161514131211100f0e0d0c0b0a0908070605040302010000112233445566778899aabbccddeeff00102030405060708090a0b0c0d0e0f0a8913577ffff001d0df0ad0b44332211040302010000000000000000ffffffffffffffff5802000003005f081032547698badcfe1032547698badcfe40d10c008967452301efcdab8967452301efcdab8967452301efcdab8967452301efcdab",
		.xor_key_hash = "4ead68aa92ceffdb2bc4a97b01c1ad61bcd4db55a8ce5520358024180c71f2f9",
		.h1 = "dfbafc72ea439239e12a6bc183c2dc77e473659805f4ca7dca9502e91ea1ecc3",
		.h2 = "05106bcfeffaf237574073e295358beb3c39fab8700b05b928f4114ec8f8634a",
		.blake2b_1 = "f83b9ec159531c8334936191110b1d0b0b30e324f32046b31b7b0649fff3357b",
		.blake2b_2 = "6d9996da99d1f3d70672038c895c40b9f82f2201079dedeb1cbbf779508b9f65",
		.mask = "00aa74c7e80a7f4889d075e84d39086b99989dd8d7ba91ff6c69919317bc5895",
		.block_hash = "6d33e21d71db8c9f8fa27664c46548d261b7bfd9d0277c1470d266ea4737c7f0",
		.asic_profile = 3,
		.asic_input = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005106bcfeffaf237574073e295358beb3c39fab8700b05b928f4114ec8f8634a0df0ad0b443322115802000004030201f83b9ec159531c8334936191110b1d0b0b30e324f32046b31b7b0649fff3357b",
	},
	{
		.name = "profile_0_offset_as_nonce_selector_255",
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
			.flags = 88,
			.xor_key_mask_clear_bits = 255,
			.xor_key = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
			.height = 840000,
			.mm_rhs = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		},
		.serialized = "000000a01f1e1d1c1b1a191817161514131211100f0e0d0c0b0a0908070605040302010000112233445566778899aabbccddeeff00102030405060708090a0b0c0d0e0f000943577ffff001dffffffff44332211efcdab89ffeeddccbbaa9988776655443322110088776655030058ff2222222222222222111111111111111140d10c000000000000000000000000000000000000000000000000000000000000000000",
		.xor_key_hash = "7bd0c09b8fe9cf1513cc141524b5bf46a4851ce06fe5bce05e04f9434f0a88a2",
		.h1 = "3dd10725a72a0f366e2f79f6842e71f76ab1d2518d05e211a85489155cb56280",
		.h2 = "05dead5273cd55b6f8b130944931cacaa028620bf0f89dd177d1e7becac70611",
		.blake2b_1 = "a3ebe3e3a78d4ca997fd6393199b78823ea9aa5a058d4b7100b0be4c69d4f228",
		.blake2b_2 = "5af72c67b0c15c88a49bfddd2554554c417a8c22e79ef69a816ef3efca92aed0",
		.mask = "0000000000000000000000000000000000000000000000000000000000000001",
		.block_hash = "5af72c67b0c15c88a49bfddd2554554c417a8c22e79ef69a816ef3efca92aed1",
		.asic_profile = 0,
		.asic_input = "000000000000943aff74219e1f45899abfdf536373c0f2fc92e6fe58335cd0adffffffff4433221188776655efcdab89a3ebe3e3a78d4ca997fd6393199b78823ea9aa5a058d4b7100b0be4c69d4f228",
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
