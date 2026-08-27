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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "datum_blocktemplates.h"
#include "datum_conf.h"
#include "datum_jsonrpc.h"
#include "datum_stratum.h"
#include "datum_utils.h"

void datum_stratum_mod_username_tests() {
	const char * const s_umods = "{\"x\":{\"addrA\": 0.3}, \"abc\":{\"addrB\":0.3,\"addrC\":0.3},\":)\":{\"\":0.5}}";
	json_error_t err;
	json_t * const j_umods = JSON_LOADS(s_umods, &err);
	assert(j_umods);
	struct datum_username_mod *umods = NULL;
	int ret = datum_config_parse_username_mods(&umods, j_umods, false);
	assert(ret == 1);
	json_decref(j_umods);
	datum_config.stratum_username_mod = umods;
	
	char buf[0x100];
	char * const pool_addr = datum_config.mining_pool_address;
	char *s, *modname;
	const char *res, *a1, *a2;
	
	strcpy(pool_addr, "dummy");
	
	s = "def~G";
	modname = &s[4];
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0, modname, 1) == s);
	
	s = "def~x";
	modname = &s[4];
	res = datum_stratum_mod_username(s, buf, sizeof(buf), 0, modname, 1);
	datum_test(0 == strcmp(res, "addrA"));
	memset(buf, 0, 5);
	res = datum_stratum_mod_username(s, buf, sizeof(buf), 0x4ccc, modname, 1);
	datum_test(0 == strcmp(res, "addrA"));
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0x4ccd, modname, 1) == pool_addr);
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0xffff, modname, 1) == pool_addr);
	
	s = "def~abc";
	modname = &s[4];
	res = datum_stratum_mod_username(s, buf, sizeof(buf), 0, modname, 3);
	if (0 == strcmp(res, "addrB")) {  // jansson doesn't order keys'
		a1 = "addrB";
		a2 = "addrC";
	} else {
		a1 = "addrC";
		a2 = "addrB";
	}
	datum_test(0 == strcmp(res, a1));
	memset(buf, 0, 5);
	res = datum_stratum_mod_username(s, buf, sizeof(buf), 0x4ccc, modname, 3);
	datum_test(0 == strcmp(res, a1));
	memset(buf, 0, 5);
	res = datum_stratum_mod_username(s, buf, sizeof(buf), 0x4ccd, modname, 3);
	datum_test(0 == strcmp(res, a2));
	memset(buf, 0, 5);
	res = datum_stratum_mod_username(s, buf, sizeof(buf), 0x9999, modname, 3);
	datum_test(0 == strcmp(res, a2));
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0x999a, modname, 3) == pool_addr);
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0xffff, modname, 3) == pool_addr);
	
	s = "def.ghi~abc";
	modname = &s[8];
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0, modname, 3) == buf);
	datum_test(0 == strncmp(buf, a1, 5));
	datum_test(0 == strcmp(&buf[5], ".ghi"));
	memset(buf, 0, 8);
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0x4ccc, modname, 3) == buf);
	datum_test(0 == strncmp(buf, a1, 5));
	datum_test(0 == strcmp(&buf[5], ".ghi"));
	memset(buf, 0, 8);
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0x4ccd, modname, 3) == buf);
	datum_test(0 == strncmp(buf, a2, 5));
	datum_test(0 == strcmp(&buf[5], ".ghi"));
	memset(buf, 0, 8);
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0x9999, modname, 3) == buf);
	datum_test(0 == strncmp(buf, a2, 5));
	datum_test(0 == strcmp(&buf[5], ".ghi"));
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0x999a, modname, 3) == pool_addr);
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0xffff, modname, 3) == pool_addr);
	
	s = "def.ghi~:)";
	modname = &s[8];
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0, modname, 2) == buf);
	datum_test(0 == strcmp(buf, "def.ghi"));
	memset(buf, 0, 7);
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0x7fff, modname, 2) == buf);
	datum_test(0 == strcmp(buf, "def.ghi"));
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0x8000, modname, 2) == pool_addr);
	datum_test(datum_stratum_mod_username(s, buf, sizeof(buf), 0xffff, modname, 2) == pool_addr);
	
	// Intentionally overflow buf with address: we lose the worker name, but get the full address via its umod buffer
	s = "def.ghi~x";
	modname = &s[8];
	memset(buf, 0x0e, 8);
	res = datum_stratum_mod_username(s, buf, 2, 0, modname, 1);
	datum_test(res != buf);
	datum_test(res != pool_addr);
	datum_test(buf[2] == 0x0e);
	datum_test(0 == strcmp(res, "addrA"));
	res = datum_stratum_mod_username(s, buf, 2, 0x4ccc, modname, 1);
	datum_test(0 == strcmp(res, "addrA"));
	datum_test(datum_stratum_mod_username(s, buf, 2, 0x4ccd, modname, 1) == pool_addr);
	datum_test(datum_stratum_mod_username(s, buf, 2, 0xffff, modname, 1) == pool_addr);
	datum_test(buf[2] == 0x0e);
	datum_test(buf[6] == 0x0e);
	res = datum_stratum_mod_username(s, buf, 6, 0, modname, 1);
	datum_test(res == buf);
	datum_test(res != pool_addr);
	datum_test(buf[6] == 0x0e);
	datum_test(0 == strcmp(res, "addrA"));
	memset(buf, 0x0e, 9);
	datum_test(datum_stratum_mod_username(s, buf, 7, 0, modname, 1) == buf);
	datum_test(buf[8] == 0x0e);
	datum_test(0 == strcmp(res, "addrA."));
	memset(buf, 0x0e, 10);
	datum_test(datum_stratum_mod_username(s, buf, 8, 0, modname, 1) == buf);
	datum_test(buf[9] == 0x0e);
	datum_test(0 == strcmp(res, "addrA.g"));
	memset(buf, 0x0e, 11);
	datum_test(datum_stratum_mod_username(s, buf, 9, 0, modname, 1) == buf);
	datum_test(buf[10] == 0x0e);
	datum_test(0 == strcmp(res, "addrA.gh"));
	memset(buf, 0x0e, 12);
	datum_test(datum_stratum_mod_username(s, buf, 10, 0, modname, 1) == buf);
	datum_test(buf[11] == 0x0e);
	datum_test(0 == strcmp(res, "addrA.ghi"));
	s = "def.ghi~:)";
	modname = &s[8];
	memset(buf, 0x0e, 9);
	datum_test(datum_stratum_mod_username(s, buf, 2, 0, modname, 2) == buf);
	datum_test(buf[2] == 0x0e);
	datum_test(0 == strcmp(res, "d"));
	datum_test(datum_stratum_mod_username(s, buf, 6, 0, modname, 2) == buf);
	datum_test(buf[6] == 0x0e);
	datum_test(0 == strcmp(res, "def.g"));
	datum_test(datum_stratum_mod_username(s, buf, 7, 0, modname, 2) == buf);
	datum_test(buf[7] == 0x0e);
	datum_test(0 == strcmp(res, "def.gh"));
	datum_test(datum_stratum_mod_username(s, buf, 8, 0, modname, 2) == buf);
	datum_test(buf[8] == 0x0e);
	datum_test(0 == strcmp(res, "def.ghi"));
}

void datum_stratum_request_id_tests(void) {
	char buf[MAX_STRATUM_REQUEST_ID_CHARS + 1];
	json_t *id;

	// The type that arrives is the type that goes back. BLAKE2b miners send
	// strings, and the quotes are part of the text so the reply is valid JSON
	// whichever type it was.
	id = json_integer(42);
	datum_test(datum_stratum_request_id(id, buf, sizeof(buf)));
	datum_test(0 == strcmp(buf, "42"));
	json_decref(id);

	id = json_string("abc");
	datum_test(datum_stratum_request_id(id, buf, sizeof(buf)));
	datum_test(0 == strcmp(buf, "\"abc\""));
	json_decref(id);

	id = json_null();
	datum_test(datum_stratum_request_id(id, buf, sizeof(buf)));
	datum_test(0 == strcmp(buf, "null"));
	json_decref(id);

	// The longest id that still fits: 62 characters plus two quotes.
	char longest[MAX_STRATUM_REQUEST_ID_CHARS - 1];
	memset(longest, 'a', sizeof(longest) - 1);
	longest[sizeof(longest) - 1] = 0;
	id = json_string(longest);
	datum_test(datum_stratum_request_id(id, buf, sizeof(buf)));
	datum_test(strlen(buf) == MAX_STRATUM_REQUEST_ID_CHARS);
	json_decref(id);

	// One character more is refused rather than truncated.
	char over[MAX_STRATUM_REQUEST_ID_CHARS];
	memset(over, 'a', sizeof(over) - 1);
	over[sizeof(over) - 1] = 0;
	id = json_string(over);
	datum_test(!datum_stratum_request_id(id, buf, sizeof(buf)));
	json_decref(id);

	// Far over, which is the case that mattered: json_dumpb returns the length
	// the encoding would need, so a test for zero alone would write a NUL past
	// the end of buf at an offset the client picks.
	char way_over[512];
	memset(way_over, 'a', sizeof(way_over) - 1);
	way_over[sizeof(way_over) - 1] = 0;
	id = json_string(way_over);
	datum_test(json_dumpb(id, buf, sizeof(buf) - 1, JSON_COMPACT | JSON_ENCODE_ANY) > sizeof(buf) - 1);
	datum_test(!datum_stratum_request_id(id, buf, sizeof(buf)));
	json_decref(id);

	// A buffer with no room for even an empty encoding.
	id = json_integer(1);
	datum_test(!datum_stratum_request_id(id, buf, 1));
	datum_test(!datum_stratum_request_id(NULL, buf, sizeof(buf)));
	json_decref(id);
}

void datum_stratum_pot_byte_tests(void) {
	// The byte the gateway commits when it builds work and the byte it commits
	// when it rebuilds that work to check a share have to be the same one. A
	// quickdiff job records its difficulty in quickdiff_value and deliberately
	// leaves the job's own slot alone, so taking it from the slot yields a
	// different coinbase, merkle root, h2 and hash1 than the miner was given,
	// and every share on the job is rejected as high-hash.
	T_DATUM_MINER_DATA * const m = calloc(1, sizeof(*m));
	datum_test(m != NULL);
	m->stratum_job_diffs[3] = 1024;   // 2^10
	m->quickdiff_value = 65536;       // 2^16

	datum_test(stratum_client_pot_byte(m, false, 3) == 10);
	datum_test(stratum_client_pot_byte(m, true, 3) == 16);

	// A quickdiff bump is at least four times the difficulty it replaces, so
	// the two byte values never coincide and the mistake is never masked.
	m->quickdiff_value = m->stratum_job_diffs[3] << 2;
	datum_test(stratum_client_pot_byte(m, false, 3) == 10);
	datum_test(stratum_client_pot_byte(m, true, 3) == 12);

	free(m);
}

static void datum_gbt_rules_tests(void) {
	const int saved_height = datum_config.blake2b_activation_height;
	json_error_t error;
	json_t *gbt;
	
	datum_config.blake2b_activation_height = 100;
	
	// Below the activation height the node must not list !blake2b.
	gbt = json_loads("{\"rules\":[\"segwit\"]}", 0, &error);
	datum_test(gbt != NULL);
	datum_test(datum_gbt_check_blake2b_rules(gbt, 99));
	datum_test(!datum_gbt_check_blake2b_rules(gbt, 100));
	datum_test(!datum_gbt_check_blake2b_rules(gbt, 101));
	json_decref(gbt);
	
	// From the activation height on it must.
	gbt = json_loads("{\"rules\":[\"segwit\",\"!blake2b\"]}", 0, &error);
	datum_test(gbt != NULL);
	datum_test(!datum_gbt_check_blake2b_rules(gbt, 99));
	datum_test(datum_gbt_check_blake2b_rules(gbt, 100));
	datum_test(datum_gbt_check_blake2b_rules(gbt, 101));
	json_decref(gbt);
	
	// Only the required form "!blake2b" counts; the bare name is the client
	// side capability, which the node never echoes.
	gbt = json_loads("{\"rules\":[\"blake2b\"]}", 0, &error);
	datum_test(gbt != NULL);
	datum_test(!datum_gbt_check_blake2b_rules(gbt, 100));
	json_decref(gbt);
	
	// A missing or malformed rules array is a version 1 template.
	gbt = json_loads("{}", 0, &error);
	datum_test(gbt != NULL);
	datum_test(datum_gbt_check_blake2b_rules(gbt, 99));
	datum_test(!datum_gbt_check_blake2b_rules(gbt, 100));
	json_decref(gbt);
	gbt = json_loads("{\"rules\":\"!blake2b\"}", 0, &error);
	datum_test(gbt != NULL);
	datum_test(!datum_gbt_check_blake2b_rules(gbt, 100));
	json_decref(gbt);
	
	datum_config.blake2b_activation_height = saved_height;
}

static void datum_gbt_reduced_data_tests(void) {
	json_error_t error;
	json_t *gbt;
	unsigned char script[128];
	
	// The reduced_data rule is reported unprefixed, alongside the others.
	gbt = json_loads("{\"rules\":[\"segwit\",\"!blake2b\",\"reduced_data\"]}", 0, &error);
	datum_test(gbt != NULL);
	datum_test(datum_gbt_rule_present(gbt, "reduced_data"));
	datum_test(datum_gbt_rule_present(gbt, "!blake2b"));
	datum_test(!datum_gbt_rule_present(gbt, "blake2b"));
	json_decref(gbt);
	
	gbt = json_loads("{\"rules\":[\"segwit\",\"!blake2b\"]}", 0, &error);
	datum_test(gbt != NULL);
	datum_test(!datum_gbt_rule_present(gbt, "reduced_data"));
	json_decref(gbt);
	
	gbt = json_loads("{}", 0, &error);
	datum_test(gbt != NULL);
	datum_test(!datum_gbt_rule_present(gbt, "reduced_data"));
	json_decref(gbt);
	
	// A non-OP_RETURN script is limited to 34 bytes: P2TR and P2WSH reach it
	// exactly, and one byte more is over.
	memset(script, 0x00, sizeof(script));
	script[0] = 0x51; // OP_1 <32 byte program>
	script[1] = 0x20;
	datum_test(datum_rdts_output_script_ok(script, 34));
	datum_test(!datum_rdts_output_script_ok(script, 35));
	datum_test(!datum_rdts_output_script_ok(script, 64));
	
	// An OP_RETURN script is limited to 83 bytes. The witness commitment the
	// node supplies is 38 bytes, and the extranonce output is 16.
	script[0] = 0x6a;
	datum_test(datum_rdts_output_script_ok(script, 16));
	datum_test(datum_rdts_output_script_ok(script, 38));
	datum_test(datum_rdts_output_script_ok(script, 83));
	datum_test(!datum_rdts_output_script_ok(script, 84));
	
	// An empty script has no limit.
	datum_test(datum_rdts_output_script_ok(script, 0));
}

void datum_stratum_tests(void) {
	datum_stratum_mod_username_tests();
	datum_stratum_request_id_tests();
	datum_stratum_pot_byte_tests();
	datum_gbt_rules_tests();
	datum_gbt_reduced_data_tests();
}
