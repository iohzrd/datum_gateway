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
 * Copyright (c) 2024-2025 Bitcoin Ocean, LLC & Jason Hughes
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <jansson.h>
#include <inttypes.h>
#include <curl/curl.h>
#include <stdatomic.h>
#include <signal.h>

#include "datum_gateway.h"
#include "datum_jsonrpc.h"
#include "datum_utils.h"
#include "datum_blocktemplates.h"
#include "datum_conf.h"
#include "datum_stratum.h"
#include "datum_protocol.h"

volatile sig_atomic_t new_notify = 0;
atomic_int new_notify_threadsafe = 0;
atomic_int notify_othercause = 0;
static pthread_mutex_t new_notify_lock = PTHREAD_MUTEX_INITIALIZER;
volatile char new_notify_blockhash[256] = { 0 };
volatile int new_notify_height = 0;

void datum_blocktemplates_notifynew_sighandler() {
	new_notify = 1;
}

void datum_blocktemplates_notifynew(const char * const prevhash, const int height) {
	if (prevhash && *prevhash) pthread_mutex_lock(&new_notify_lock);
	new_notify_threadsafe = 1;
	if (prevhash) {
		if (prevhash[0]) {
			strncpy((char *)new_notify_blockhash, prevhash, 66);
			if (height > new_notify_height) {
				new_notify_height = height;
			}
			pthread_mutex_unlock(&new_notify_lock);
		}
	}
}

void datum_blocktemplates_notify_othercause() {
	notify_othercause = 1;
}

T_DATUM_TEMPLATE_DATA *template_data = NULL;

int next_template_index = 0;

const char *datum_blocktemplates_error = NULL;

int datum_template_init(void) {
	char *temp = NULL, *ptr = NULL;
	int i,j;
	
	template_data = (T_DATUM_TEMPLATE_DATA *)calloc(sizeof(T_DATUM_TEMPLATE_DATA),MAX_TEMPLATES_IN_MEMORY+1);
	if (!template_data) {
		DLOG_FATAL("Could not allocate RAM for in-memory template data. :( (1)");
		return -1;
	}
	
	// TODO: Be smarter about dependent RAM data and size
	// we're storing both binary and ascii hex versions of all txns for both processing and submitblock speedups
	j = (sizeof(T_DATUM_TEMPLATE_TXN)*16384) + (MAX_BLOCK_SIZE_BYTES*3) + 2000000;
	temp = calloc(j, MAX_TEMPLATES_IN_MEMORY);
	if (!temp) {
		DLOG_FATAL("ERROR: Could not allocate RAM for in-memory template data. :( (2)");
		return -2;
	}
	
	ptr = temp;
	for(i=0;i<MAX_TEMPLATES_IN_MEMORY;i++) {
		template_data[i].local_data = ptr;
		ptr+=j;
		template_data[i].local_data_size = j;
		template_data[i].local_index = i;
	}
	
	DLOG_DEBUG("Allocated %d MB of RAM for template memory", (j*MAX_TEMPLATES_IN_MEMORY)/(1024*1024));
	
	return 1;
}

void datum_template_clear(T_DATUM_TEMPLATE_DATA* p) {
	p->coinbasevalue = 0;
	p->txn_total_fee = 0;
	p->txn_count = 0;
	p->txn_total_size = 0;
	p->txn_data_offset = 0;
	p->txn_total_weight = 0;
	p->txn_total_sigops = 0;
	p->txns = p->local_data;
}

T_DATUM_TEMPLATE_DATA *get_next_template_ptr(void) {
	T_DATUM_TEMPLATE_DATA *p;
	
	if (!template_data) return NULL;
	
	p = &template_data[next_template_index];
	
	datum_template_clear(p);
	
	next_template_index++;
	if (next_template_index >= MAX_TEMPLATES_IN_MEMORY) {
		next_template_index = 0;
	}
	
	return p;
}

// Check the two BLAKE2b consensus settings against the node that will validate
// the block.
//
// getblocktemplate's coinbaseaux carries the headline the node will enforce
// (rpc/mining.cpp), and only in the template for the activation height.
// Nothing else exposes either value over RPC: DEPLOYMENT_BLAKE2B is a buried
// deployment and getblockchaininfo and getdeploymentinfo do not list it. This
// is therefore the only chance the Gateway gets to check its configuration
// against the node, and it comes exactly at the height where being wrong costs
// the one block that cannot be mined again.
//
// Returns false to refuse the template. Serving no work for that block is the
// better failure: with the wrong headline the block is rejected as
// bad-headline, and at the wrong height as bad-version-sha256d.
//
// The node's value is deliberately not adopted in place of the configured one.
// A DATUM pool carries its own copy and checks the coinbase for it, so a
// Gateway that quietly switched would have its shares rejected by the pool
// instead of its block rejected by the network.
static bool datum_gbt_check_blake2b_activation(json_t *gbt, uint64_t height) {
	static uint64_t announced_height = UINT64_MAX;
	const size_t headline_len = strlen(datum_config.blake2b_headline);
	char node_headline[sizeof(datum_config.blake2b_headline)];
	const char *hex;
	size_t hex_len, i;
	
	hex = json_string_value(json_object_get(json_object_get(gbt, "coinbaseaux"), "blake2b_headline"));
	
	if (!hex) {
		// Absent at every other height, so its absence only means anything when
		// we have been told this is the activation height.
		if (height == (uint64_t)datum_config.blake2b_activation_height) {
			DLOG_FATAL("Node published no BLAKE2b headline for block %"PRIu64", which mining.blake2b_activation_height names as the activation height. Either the node activates at a different height, or it was not built from bitcoinknots/bitcoin#359. Serving no work for this block.", height);
			return false;
		}
		return true;
	}
	
	// Present, so this is the node's activation height whatever we were told.
	if (height != (uint64_t)datum_config.blake2b_activation_height) {
		DLOG_FATAL("Node says block %"PRIu64" activates BLAKE2b, but mining.blake2b_activation_height is %d. Serving no work for this block.", height, datum_config.blake2b_activation_height);
		return false;
	}
	
	hex_len = strlen(hex);
	if ((hex_len & 1) || ((hex_len >> 1) >= sizeof(node_headline))) {
		DLOG_FATAL("Node published a BLAKE2b headline of %zu hex characters, which this Gateway cannot build a coinbase from. Serving no work for this block.", hex_len);
		return false;
	}
	for (i = 0; i < (hex_len >> 1); i++) {
		node_headline[i] = (char)hex2bin_uchar(&hex[i << 1]);
	}
	node_headline[hex_len >> 1] = 0;
	
	if (((hex_len >> 1) != headline_len) || memcmp(node_headline, datum_config.blake2b_headline, headline_len)) {
		DLOG_FATAL("BLAKE2b headline mismatch at the activation block: the node will enforce \"%s\", mining.blake2b_headline is \"%s\". A block carrying the wrong text is rejected as bad-headline, so no work is being served for this block.", node_headline, datum_config.blake2b_headline);
		return false;
	}
	
	// Once per height, not once per template poll.
	if (announced_height != height) {
		announced_height = height;
		DLOG_INFO("Block %"PRIu64" activates BLAKE2b, and the configured headline is the one the node will enforce.", height);
	}
	return true;
}

// Whether the node listed the named rule for this template. GBT reports the
// rules in force for the block the template builds, prefixing with "!" those a
// client must understand to use the template.
bool datum_gbt_rule_present(json_t *gbt, const char *name) {
	json_t *rules, *rule;
	size_t i;
	
	rules = json_object_get(gbt, "rules");
	if (!json_is_array(rules)) return false;
	
	json_array_foreach(rules, i, rule) {
		const char *s = json_string_value(rule);
		if (s && !strcmp(s, name)) return true;
	}
	return false;
}

// Whether an output script the Gateway is about to put in the coinbase passes
// the RDTS (BIP 110) size limit that Consensus::CheckOutputSizes applies to the
// generation transaction while the deployment is active. An empty script is
// unrestricted; an OP_RETURN script may reach RDTS_MAX_OUTPUT_DATA_SIZE bytes;
// every other script is limited to RDTS_MAX_OUTPUT_SCRIPT_SIZE bytes.
bool datum_rdts_output_script_ok(const unsigned char *script, int len) {
	if (len <= 0) return true;
	if (script[0] == 0x6a) return len <= RDTS_MAX_OUTPUT_DATA_SIZE;
	return len <= RDTS_MAX_OUTPUT_SCRIPT_SIZE;
}

// Check the pool payout output script against the RDTS (BIP 110) size limit.
//
// This script carries whatever of the generation value is not paid to miners,
// so unlike a miner payout output it cannot be left out of the coinbase. If it
// is too large the block is rejected as bad-txns-vout-script-toolarge, so the
// template is refused and no work is served for that block.
//
// The script comes either from mining.pool_address, where addr_2_output_script
// produces at most 34 bytes and the limit is never reached, or from the DATUM
// server's client configuration message, where it is arbitrary.
//
// Returns false to refuse the template.
static bool datum_gbt_check_reduced_data_payout(uint64_t height) {
	static uint64_t reported_height = UINT64_MAX;
	unsigned char script[sizeof(((T_DATUM_STRATUM_JOB *)NULL)->pool_addr_script)] = { 0 };
	int script_len;
	
	if (datum_protocol_is_active()) {
		script_len = datum_config.override_mining_pool_scriptsig_len;
		// A longer script is rejected by datum_protocol_client_conf, so this
		// cannot be reached; report it as over the limit rather than reading
		// past the end of the copy.
		if (script_len > (int)sizeof(script)) script_len = sizeof(script) + 1;
		else memcpy(script, datum_config.override_mining_pool_scriptsig, script_len);
	} else {
		script_len = addr_2_output_script(datum_config.mining_pool_address, script, sizeof(script));
	}
	
	if (datum_rdts_output_script_ok(script, script_len)) return true;
	
	// Once per height, not once per template poll.
	if (reported_height != height) {
		reported_height = height;
		DLOG_FATAL("Pool payout output script is %d bytes, but the node enforces the reduced_data rule for block %"PRIu64", which limits a non-OP_RETURN coinbase output script to %d bytes. A block carrying it is rejected as bad-txns-vout-script-toolarge, so no work is being served for this block.", script_len, height, RDTS_MAX_OUTPUT_SCRIPT_SIZE);
	}
	return false;
}

// Check that the node's proof-of-work rule for this template agrees with
// mining.blake2b_activation_height.
//
// rpc/mining.cpp puts "!blake2b" into rules for every template whose header
// is version 2, and only those. This gateway serves version 2 work from the
// configured activation height on, so the two must agree at every height. If
// the configured height is too low, the blocks built in between would be
// rejected as bad-version-sha256d; if too high, no work would be served for
// blocks the node would accept. Neither case is reported by the headline
// check, which only runs at the one height where the node publishes it.
//
// Returns false to refuse the template.
bool datum_gbt_check_blake2b_rules(json_t *gbt, uint64_t height) {
	static uint64_t reported_height = UINT64_MAX;
	const bool expected = (height >= (uint64_t)datum_config.blake2b_activation_height);
	const bool node_v2 = datum_gbt_rule_present(gbt, "!blake2b");
	
	if (node_v2 == expected) return true;
	
	// Once per height, not once per template poll.
	if (reported_height != height) {
		reported_height = height;
		if (node_v2) {
			DLOG_FATAL("Node requires the BLAKE2b (version 2) header for block %"PRIu64", but mining.blake2b_activation_height is %d, below which this Gateway serves no work. The configured height is too high. Serving no work for this block.", height, datum_config.blake2b_activation_height);
		} else {
			DLOG_FATAL("Node does not list the !blake2b rule for block %"PRIu64", but mining.blake2b_activation_height is %d, so this Gateway would build a version 2 header that the node rejects as bad-version-sha256d. Either the configured height is too low, or the node was not built from bitcoinknots/bitcoin#359. Serving no work for this block.", height, datum_config.blake2b_activation_height);
		}
	}
	return false;
}

T_DATUM_TEMPLATE_DATA *datum_gbt_parser(json_t *gbt) {
	T_DATUM_TEMPLATE_DATA *tdata;
	const char *s;
	int i,j;
	json_t *tx_array, *jval;
	
	tdata = get_next_template_ptr();
	if (!tdata) {
		DLOG_ERROR("Could not get a template pointer.");
		return NULL;
	}
	
	tdata->height = json_integer_value(json_object_get(gbt, "height"));
	if (!tdata->height) {
		DLOG_ERROR("Missing data from GBT JSON (height)");
		return NULL;
	}
	
	if (!datum_gbt_check_blake2b_activation(gbt, tdata->height)) {
		return NULL;
	}
	
	if (!datum_gbt_check_blake2b_rules(gbt, tdata->height)) {
		return NULL;
	}
	
	// RDTS (BIP 110) reduces the block weight limit, which arrives in
	// weightlimit below, and limits the size of every coinbase output script.
	// The node lists the rule for exactly the blocks it enforces it on.
	tdata->reduced_data = datum_gbt_rule_present(gbt, "reduced_data");
	
	if (tdata->reduced_data && !datum_gbt_check_reduced_data_payout(tdata->height)) {
		return NULL;
	}
	
	tdata->coinbasevalue = json_integer_value(json_object_get(gbt, "coinbasevalue"));
	if (!tdata->coinbasevalue) {
		DLOG_ERROR("Missing data from GBT JSON (coinbasevalue)");
		return NULL;
	}
	
	tdata->mintime = json_integer_value(json_object_get(gbt, "mintime"));
	if (!tdata->mintime) {
		DLOG_ERROR("Missing data from GBT JSON (mintime)");
		return NULL;
	}
	
	tdata->sigoplimit = json_integer_value(json_object_get(gbt, "sigoplimit"));
	if (!tdata->sigoplimit) {
		DLOG_ERROR("Missing data from GBT JSON (sigoplimit)");
		return NULL;
	}
	
	tdata->curtime = json_integer_value(json_object_get(gbt, "curtime"));
	if (!tdata->curtime) {
		DLOG_ERROR("Missing data from GBT JSON (curtime)");
		return NULL;
	}
	
	tdata->sizelimit = json_integer_value(json_object_get(gbt, "sizelimit"));
	if (!tdata->sizelimit) {
		DLOG_ERROR("Missing data from GBT JSON (sizelimit)");
		return NULL;
	}
	
	tdata->weightlimit = json_integer_value(json_object_get(gbt, "weightlimit"));
	if (!tdata->weightlimit) {
		DLOG_ERROR("Missing data from GBT JSON (weightlimit)");
		return NULL;
	}
	
	tdata->version = json_integer_value(json_object_get(gbt, "version"));
	if (!tdata->version) {
		DLOG_ERROR("Missing data from GBT JSON (version)");
		return NULL;
	}
	
	jval = json_object_get(gbt, "bits");
	if (json_string_length(jval) != 8) {
		DLOG_ERROR("Wrong bits length from GBT JSON");
		return NULL;
	}
	s = json_string_value(jval);
	strcpy(tdata->bits, s);
	
	jval = json_object_get(gbt, "previousblockhash");
	if (json_string_length(jval) != 64) {
		DLOG_ERROR("Missing data from GBT JSON (previousblockhash)");
		return NULL;
	}
	s = json_string_value(jval);
	strcpy(tdata->previousblockhash, s);
	
	jval = json_object_get(gbt, "target");
	if (json_string_length(jval) != 64) {
		DLOG_ERROR("Missing data from GBT JSON (target)");
		return NULL;
	}
	s = json_string_value(jval);
	strcpy(tdata->block_target_hex, s);
	
	jval = json_object_get(gbt, "default_witness_commitment");
	if (json_string_length(jval) < 38 || json_string_length(jval) > 95) {
		DLOG_ERROR("Missing data from GBT JSON (default_witness_commitment)");
		return NULL;
	}
	s = json_string_value(jval);
	strcpy(tdata->default_witness_commitment, s);
	
	// "20000000", "192e17d5", "66256be5"
	// version, bits, time
	// 192e17d5 // gbt format matches stratum for bits
	
	// stash useful binary versions of prevblockhash and nbits
	for(i=0;i<64;i+=2) {
		tdata->previousblockhash_bin[31-(i>>1)] = hex2bin_uchar(&tdata->previousblockhash[i]);
	}
	for(i=0;i<4;i++) {
		tdata->bits_bin[3-i] = hex2bin_uchar(&tdata->bits[i<<1]);
	}
	tdata->bits_uint = upk_u32le(tdata->bits_bin, 0);
	nbits_to_target(tdata->bits_uint, tdata->block_target);
	
	// store binary default witness commitment
	j = strlen(tdata->default_witness_commitment);
	for(i=0;i<j;i+=2) {
		tdata->default_witness_commitment_bin[(i>>1)] = hex2bin_uchar(&tdata->default_witness_commitment[i]);
	}
	
	// Get the txns
	tx_array = json_object_get(gbt, "transactions");
	if (!json_is_array(tx_array)) {
		DLOG_ERROR("Missing data from GBT JSON (transactions)");
		return NULL;
	}
	
	tdata->txn_count = json_array_size(tx_array);
	if (tdata->txn_count > 16383) {
		// Truncating the list would leave the template inconsistent with
		// itself: coinbasevalue covers the fees of every transaction the node
		// selected, so a coinbase built for a shortened list claims more than
		// the block it is in collects, and the node rejects it as
		// bad-cb-amount. stratum_calculate_merkle_branches does not handle more
		// than this either. Refuse the template and keep serving the previous
		// one until the node offers one that fits.
		DLOG_ERROR("DATUM Gateway does not support blocks with more than 16383 transactions! %d txns in template. Ignoring this template.", (int)tdata->txn_count);
		return NULL;
	}
	tdata->txn_data_offset = sizeof(T_DATUM_TEMPLATE_TXN)*tdata->txn_count;
	if (tdata->txn_count > 0) {
		for(i=0;i<tdata->txn_count;i++) {
			json_t *tx = json_array_get(tx_array, i);
			if (!tx) {
				DLOG_ERROR("transaction %d not found!", i);
				return NULL;
			}
			if (!json_is_object(tx)) {
				DLOG_ERROR("transaction %d is not an object!", i);
				return NULL;
			}
			
			// index (1 based, like GBT depends)
			tdata->txns[i].index_raw = i+1;
			
			// txid
			jval = json_object_get(tx, "txid");
			if (json_string_length(jval) != 64) {
				DLOG_ERROR("Missing data from GBT JSON transactions[%d] (txid)",i);
				return NULL;
			}
			s = json_string_value(jval);
			strcpy(tdata->txns[i].txid_hex, s);
			hex_to_bin_le(tdata->txns[i].txid_hex, tdata->txns[i].txid_bin);
			
			// hash
			jval = json_object_get(tx, "hash");
			if (json_string_length(jval) != 64) {
				DLOG_ERROR("Missing data from GBT JSON transactions[%d] (hash)",i);
				return NULL;
			}
			s = json_string_value(jval);
			strcpy(tdata->txns[i].hash_hex, s);
			hex_to_bin_le(tdata->txns[i].hash_hex, tdata->txns[i].hash_bin);
			
			// fee
			//
			// The subsidy paid by a coinbase that collects no fees is derived
			// from coinbasevalue less this sum, so a fee that is absent or
			// negative -- which GBT uses to mean "unknown" -- cannot be read as
			// zero. Doing so would put more than the subsidy in that coinbase.
			jval = json_object_get(tx, "fee");
			if (!json_is_integer(jval) || json_integer_value(jval) < 0) {
				DLOG_ERROR("Missing or unknown fee in GBT JSON transactions[%d]; the coinbase value cannot be derived without it",i);
				return NULL;
			}
			tdata->txns[i].fee_sats = json_integer_value(jval);
			tdata->txn_total_fee += tdata->txns[i].fee_sats;
			
			// sigops
			tdata->txns[i].sigops = json_integer_value(json_object_get(tx, "sigops"));
			
			// weight
			tdata->txns[i].weight = json_integer_value(json_object_get(tx, "weight"));
			
			// data
			s = json_string_value(json_object_get(tx, "data"));
			if (!s) {
				DLOG_ERROR("Missing data from GBT JSON transactions[%d] (data)",i);
				return NULL;
			}
			
			// size
			tdata->txns[i].size = strlen(s)>>1;
			
			// raw txn data
			tdata->txns[i].txn_data_binary = &((uint8_t *)tdata->local_data)[tdata->txn_data_offset];
			tdata->txn_data_offset += tdata->txns[i].size+1;
			tdata->txns[i].txn_data_hex = &((char *)tdata->local_data)[tdata->txn_data_offset];
			tdata->txn_data_offset += (tdata->txns[i].size*2)+2;
			if (tdata->txn_data_offset >= tdata->local_data_size) {
				DLOG_ERROR("Exceeded template local size with txn data!");
				return NULL;
			}
			strcpy(tdata->txns[i].txn_data_hex, s);
			hex_to_bin(s, tdata->txns[i].txn_data_binary);
			
			// tallies
			tdata->txn_total_weight+=tdata->txns[i].weight;
			tdata->txn_total_size+=tdata->txns[i].size;
			tdata->txn_total_sigops+=tdata->txns[i].sigops;
		}
	}
	
	return tdata;
}

void *datum_gateway_fallback_notifier(void *args) {
	CURL *tcurl = NULL;
	char req[512];
	char p1[72];
	p1[0] = 0;
	json_t *gbbh, *res_val;
	const char *s;
	
	tcurl = curl_easy_init();
	if (!tcurl) {
		DLOG_FATAL("Could not initialize cURL");
		panic_from_thread(__LINE__);
	}
	DLOG_DEBUG("Fallback notifier thread ready.");
	
	while(1) {
		snprintf(req, sizeof(req), "{\"jsonrpc\":\"1.0\",\"id\":\"%"PRIu64"\",\"method\":\"getbestblockhash\",\"params\":[]}", current_time_millis());
		gbbh = bitcoind_json_rpc_call(tcurl, &datum_config, req);
		if (gbbh) {
			res_val = json_object_get(gbbh, "result");
			if (!res_val) {
				DLOG_ERROR("ERROR: Could not decode getbestblockhash result!");
			} else {
				s = json_string_value(res_val);
				if (s) {
					if (strlen(s) == 64) {
						if (p1[0] == 0) {
							strncpy(p1,s,70);
						} else {
							if (strcmp(s, p1) != 0) {
								// new block?!?!?!
								datum_blocktemplates_notifynew(s,0);
								strncpy(p1,s,70);
							}
						}
					}
				}
			}
			json_decref(gbbh);
			gbbh = NULL;
		}
		sleep(1);
	}
}

void *datum_gateway_template_thread(void *args) {
	CURL *tcurl = NULL;
	json_t *gbt = NULL, *res_val;
	uint64_t i = 0;
	char gbt_req[1024];
	int j;
	T_DATUM_TEMPLATE_DATA *t;
	bool was_notified = false;
	int wnc = 0;
	uint64_t last_block_change = 0;
	pthread_t pthread_datum_gateway_fallback_notifier;
	tcurl = curl_easy_init();
	if (!tcurl) {
		DLOG_FATAL("Could not initialize cURL");
		panic_from_thread(__LINE__);
	}
	
	if (datum_template_init() < 1) {
		DLOG_FATAL("Couldn't setup template processor.");
		panic_from_thread(__LINE__);
	}
	
	{
		unsigned char dummy[64];
		if (!addr_2_output_script(datum_config.mining_pool_address, &dummy[0], 64)) {
			if (datum_config.api_modify_conf) {
				DLOG_ERROR("Could not generate output script for pool addr! Perhaps invalid? Configure via API/dashboard.");
			} else {
				DLOG_FATAL("Could not generate output script for pool addr! Perhaps invalid? This is bad.");
				panic_from_thread(__LINE__);
			}
		}
		while (!addr_2_output_script(datum_config.mining_pool_address, &dummy[0], 64)) {
			usleep(50000);
		}
	}
	
	if (datum_config.bitcoind_notify_fallback) {
		// start getbestblockhash poller thread as a backup for notifications
		DLOG_DEBUG("Starting fallback block notifier");
		pthread_create(&pthread_datum_gateway_fallback_notifier, NULL, datum_gateway_fallback_notifier, NULL);
	}
	
	DLOG_DEBUG("Template fetcher thread ready.");
	
	char p1[72];
	p1[0] = 0;
	
	while(1) {
		i++;
		
		// fetch latest template
		snprintf(gbt_req, sizeof(gbt_req), "{\"method\":\"getblocktemplate\",\"params\":[{\"rules\":[\"segwit\",\"blake2b\"]}],\"id\":%"PRIu64"}",(uint64_t)((uint64_t)time(NULL)<<(uint64_t)8)|(uint64_t)(i&255));
		gbt = bitcoind_json_rpc_call(tcurl, &datum_config, gbt_req);
		
		if (!gbt) {
			datum_blocktemplates_error = "Could not fetch new template!";
			DLOG_ERROR("Could not fetch new template from %s!", datum_config.bitcoind_rpcurl);
			sleep(1);
			continue;
		} else {
			res_val = json_object_get(gbt, "result");
			if (!res_val) {
				datum_blocktemplates_error = "Could not decode GBT result!";
				DLOG_ERROR("%s", datum_blocktemplates_error);
			} else {
				DLOG_DEBUG("DEBUG: calling datum_gbt_parser (new=%d)", was_notified?1:0);
				t = datum_gbt_parser(res_val);
				
				if (t) {
					datum_blocktemplates_error = NULL;
					DLOG_DEBUG("height: %lu / value: %"PRIu64, (unsigned long)t->height, t->coinbasevalue);
					DLOG_DEBUG("--- prevhash: %s", t->previousblockhash);
					DLOG_DEBUG("--- txn_count: %u / sigops: %u / weight: %u / size: %u", t->txn_count, t->txn_total_sigops, t->txn_total_weight, t->txn_total_size);
					
					// If the previous block hash changed, or work is no longer valid, we should push clean work
					const bool new_block = strcmp(t->previousblockhash, p1);
					if (new_block || notify_othercause) {
						notify_othercause = 0;
						const bool empty_job_sent = update_stratum_job(t,true,JOB_STATE_EMPTY_PLUS);
						if (new_block) {
							last_block_change = current_time_millis();
							strcpy(p1, t->previousblockhash);
							was_notified = false;
							DLOG_INFO("NEW NETWORK BLOCK: %s (%lu)", t->previousblockhash, (unsigned long)t->height);
						} else {
							DLOG_DEBUG("Urgent work update triggered");
						}
						
						// Below the BLAKE2b activation height no job was published, so
						// there is no empty work in flight to wait on and no full job
						// to follow it with. Waiting would burn four seconds per block
						// on a completion signal that is never set.
						if (empty_job_sent) {
							// sleep for a milisecond
							// this will let other threads churn for a moment.  we wont get all the empty jobs blasted out in a milisecond anyway
							usleep(1000);
							
							// wait for the empties to complete
							DLOG_DEBUG("Waiting on empty work send completion...");
							for(j=0;j<4000;j++) {
								if (stratum_latest_empty_check_ready_for_full()) break;
								usleep(1001);
							}
							DLOG_DEBUG("Empty sends done!");
							
							// use this template to setup for a coinbaser wait job while the empty + full w/blank jobs are blasted
							// then this job will get blasted when its ready.
							i = datum_stratum_v1_global_subscriber_count();
							DLOG_INFO("Updating priority stratum job for block %lu: %.8f BTC, %lu txns, %lu bytes (Sent to %llu stratum client%s)", (unsigned long)t->height, (double)t->coinbasevalue / (double)100000000.0, (unsigned long)t->txn_count, (unsigned long)t->txn_total_size, (unsigned long long)i, (i!=1)?"s":"");
							update_stratum_job(t,false,JOB_STATE_FULL_PRIORITY_WAIT_COINBASER);
						}
					} else {
						if (was_notified) {
							// we got a notification of a new block, but there doesn't seem to actually be a new block.
							// we should quickly check again instead of actually updating the stratum job
							
							pthread_mutex_lock(&new_notify_lock);
							if ((new_notify_blockhash[0] > 0) && (!strcmp(t->previousblockhash,(char *)new_notify_blockhash))) {
								// we got notified for work we already knew about
								if (new_notify_height <= 0) {
									was_notified = false;
									wnc = 0;
								} else {
									if (new_notify_height == t->height) {
										was_notified = false;
										wnc = 0;
									}
								}
							}
							if (!was_notified) {
								DLOG_DEBUG("Multi notified for block we knew details about. (%s)", new_notify_blockhash);
							} else {
								DLOG_DEBUG("Notified, however new = %s, t->previousblockhash = %s, t->height = %lu, new_notify_height = %d", new_notify_blockhash, t->previousblockhash, (unsigned long)t->height, new_notify_height);
								
								// Sometimes we call GBT before we get the signal from a blocknotify.  It's a bit of a race condition.
								// Instead of freaking out, we'll just ignore things when we get a signal that results in the same block if it was
								// within 2.5s of a previous block change.
								// absolute worst case scenario here is that there's a reverse race condition of some kind where we get our notify early and GBT is still
								// returning the old block data... then we'd be one work change delay behind things.
								// that shouldn't be possible, though, if the notify comes from the same bitcoind that we're getting our templates from
								if ((current_time_millis()-2500) < last_block_change) {
									DLOG_DEBUG("This is probably a duplicate signal, since we just changed blocks less than 2.5s ago");
									was_notified = false;
								}
								
								if (((t->height < 800000) || (t->height > 2980000)) && (new_notify_blockhash[0] == 'T')) { // some hardcoded guardrails that should last for quite some time for testnet3 and testnet4
									DLOG_DEBUG("DEBUG: TESTNET FAST FORWARD HACK!!!");
									
									// set diff 1
									strcpy(t->bits, "1d00ffff");
									for(j=0;j<4;j++) {
										t->bits_bin[3-j] = hex2bin_uchar(&t->bits[j<<1]);
									}
									t->bits_uint = upk_u32le(t->bits_bin, 0);
									nbits_to_target(t->bits_uint, t->block_target);
									// ff 20 min
									if (new_notify_height > t->curtime) {
										t->curtime = new_notify_height;
										new_notify_height = -1;
									} else {
										t->curtime += 1200;
									}
									
									DLOG_DEBUG("t->curtime = %llu", (unsigned long long)t->curtime);
									
									update_stratum_job(t,true,JOB_STATE_FULL_PRIORITY_WAIT_COINBASER);
									new_notify_blockhash[0] = 0;
									was_notified = false;
								}
							}
							pthread_mutex_unlock(&new_notify_lock);
						} else {
							i = datum_stratum_v1_global_subscriber_count();
							DLOG_INFO("Updating standard stratum job for block %lu: %.8f BTC, %lu txns, %lu bytes (Sent to %llu stratum client%s)", (unsigned long)t->height, (double)t->coinbasevalue / (double)100000000.0, (unsigned long)t->txn_count, (unsigned long)t->txn_total_size, (unsigned long long)i, (i!=1)?"s":"");
							update_stratum_job(t,false,JOB_STATE_FULL_NORMAL_WAIT_COINBASER);
						}
					}
				}
			}
			json_decref(gbt);
		}
		gbt = NULL;
		
		if ((!was_notified) || (new_notify || new_notify_threadsafe)) {
			for(i=0;i<(((uint64_t)datum_config.bitcoind_work_update_seconds*(uint64_t)1000000)/(uint64_t)2500);i++) {
				usleep(2500);
				if (new_notify || new_notify_threadsafe) {
					new_notify = 0;
					new_notify_threadsafe = 0;
					was_notified = 1;
					wnc = 0;
					DLOG_INFO("NEW NETWORK BLOCK NOTIFICATION RECEIVED");
					break;
				}
			}
		} else {
			usleep(250000);
			wnc++;
			if (wnc > 16) { // 4 seconds
				// something is weird.
				DLOG_WARN("We received a new block notification, however after 16 attempts we did not see a new block.");
				was_notified = false;
				wnc = 0;
			}
		}
	}
	// this thread is never intended to exit unless the application dies
	
	// TODO: Clean things up
}
