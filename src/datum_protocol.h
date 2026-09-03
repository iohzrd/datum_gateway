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
 * Copyright (c) 2024 Bitcoin Ocean, LLC & Jason Hughes
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

#ifndef _DATUM_PROTOCOL_H_
#define _DATUM_PROTOCOL_H_

#include <sodium.h>
#include <stdbool.h>
#include <stdint.h>

#include "datum_stratum.h"
#include "datum_header_v2.h"

// Job ids are a whole byte on the wire, so 256 is what the message format
// allows, and it is also what a server speaking header v2 tracks. How many of
// the slots this Gateway uses is datum.protocol_job_slots, which defaults to
// all of them and exists to be lowered for a server that tracks fewer.
//
// Nothing negotiates this. The server's config message carries the payout
// script, the prime id, the coinbase tag and the minimum difficulty, and no job
// capacity, so both ends hold the number as a static assumption.
//
// The ring matters because a slot is reused after this many new jobs, and the
// job it then describes is not the job an old share was mined on. A share for a
// job that far back cannot be expressed in the protocol at all, so the client
// drops it rather than reporting the wrong job -- see datum_protocol_pow_submit.
// A block takes that path before the staleness check rather than after it, so a
// ring shorter than the work a miner may still be holding costs the pool's
// record of a block that was found, not only a share.
#define MAX_DATUM_PROTOCOL_JOBS 256
// Only a sanity bound. The requirement that matters is that the ring outlast
// the window in which a share is still accepted, which datum_read_config checks
// against the configured work update and staleness intervals.
#define MIN_DATUM_PROTOCOL_JOB_SLOTS 1

#define DATUM_PROTOCOL_VERSION "v0.4.2-beta" // this is sent to the server as a UA
#define DATUM_PROTOCOL_CONNECT_TIMEOUT 30

#define DATUM_PROTOCOL_MAX_CMD_DATA_SIZE 4194304 // 2^22 - protocol limit!
#define DATUM_PROTOCOL_BUFFER_SIZE (DATUM_PROTOCOL_MAX_CMD_DATA_SIZE * 3)
#define DATUM_PROTOCOL_MAX_USERNAME_LEN 384

#define MAX_DATUM_CLIENT_EVENTS 32

// Header is only XOR'd with a rotating key.  This is NOT 100% secure, and makes the cmd# and length of the handshake message decipherable.
// This is not an issue for security, however, as all following packets use a negotiated XOR key.
// It's likely possible to brute force the XOR key to break packets down into individual commands, but the contents and nature of the
// cmd is still obfuscated and unrecoverable without the session keys.

typedef struct __attribute__((packed)) T_DATUM_PROTOCOL_HEADER
{
	uint32_t cmd_len : 22; // max cmd size is 2^22 (~4MB), which is roughly the max block size for a raw submission or a raw template validation
	uint8_t reserved : 2;  // save for later use
	bool is_signed : 1;
	bool is_encrypted_pubkey : 1;
	bool is_encrypted_channel : 1;
	uint8_t proto_cmd : 5; // 32 protocol level commands
} T_DATUM_PROTOCOL_HEADER;

typedef struct
{
	bool is_remote;

	// ed25519 key pair (signing)
	unsigned char pk_ed25519[crypto_sign_PUBLICKEYBYTES];
	unsigned char sk_ed25519[crypto_sign_SECRETKEYBYTES];

	// x25519 key pair (encyption)
	unsigned char pk_x25519[crypto_box_PUBLICKEYBYTES];
	unsigned char sk_x25519[crypto_box_SECRETKEYBYTES];
} DATUM_ENC_KEYS;

typedef struct
{
	DATUM_ENC_KEYS *local;
	DATUM_ENC_KEYS *remote;
	unsigned char precomp_remote[crypto_box_BEFORENMBYTES];
} DATUM_ENC_PRECOMP;

typedef struct T_DATUM_PROTOCOL_JOB
{
	unsigned char datum_job_id;
	T_DATUM_STRATUM_JOB *sjob;

	bool server_has_merkle_branches;

	bool server_has_coinbase[8];
	bool server_has_coinbase_empty;
	bool server_has_short_txnlist;

	bool server_has_validated_block;
} T_DATUM_PROTOCOL_JOB;

typedef struct
{
	unsigned char datum_job_id;
	// The job this share was actually mined on. The ring slot named by
	// datum_job_id can be recycled between the submitting thread checking it
	// and the protocol thread reading it, so the reader compares this against
	// the slot's current occupant under the same lock it reads it with.
	const T_DATUM_STRATUM_JOB *sjob_expected;
	unsigned char extranonce[DATUM_HEADER_V2_EXTRANONCE_SIZE];
	char username[384];
	unsigned char coinbase_id;
	bool subsidy_only;
	bool is_block;
	bool quickdiff;
	unsigned char target_byte;
	uint16_t target_byte_index;
	uint32_t ntime;
	uint32_t nonce;
	uint32_t version;
	// The three 32-bit fields above cannot hold a version 2 share's 64-bit
	// nonce and time, so they only restate the parts that still fit. The whole
	// of what the hardware controls travels in a 0x03 section: the raw eight-byte
	// Sia fields as mining.submit sent them, the job's serialized block time, and
	// the header flags. Everything else in the header is the pool's own job, so
	// the pool builds the header rather than being handed one.
	unsigned char sia_ntime[8];
	unsigned char sia_nonce[8];
	bool use_time_offset;
} T_DATUM_PROTOCOL_POW;

// Bit 0 of the first reserved byte of a submit-POW message: the header's
// time-offset selector. The ASIC profile is not sent at all, because profile 0
// is the only layout the protocol describes.
#define DATUM_POW_RESERVED_BLAKE2B_USE_TIME_OFFSET 0x01

int datum_protocol_init(void);
int datum_encrypt_generate_keys(DATUM_ENC_KEYS *keys);
bool datum_protocol_is_active(void);
void datum_increment_session_nonce(void *s);
int datum_protocol_fetch_coinbaser(uint64_t value);
int datum_protocol_coinbaser_fetch(void *s);
int datum_protocol_pow_submit(
	const T_DATUM_CLIENT_DATA *c,
	const T_DATUM_STRATUM_JOB *job,
	const char *username,
	const bool was_block,
	const bool subsidy_only,
	const bool quickdiff,
	const unsigned char *block_header,
	const uint64_t target_diff,
	const unsigned char *full_cb_tx,
	const T_DATUM_STRATUM_COINBASE *cb,
	unsigned char coinbase_index);

bool datum_protocol_thread_is_active(void);
void datum_protocol_start_connector(void);
unsigned char datum_protocol_setup_new_job_idx(void *sx);

extern uint64_t datum_accepted_share_count;
extern uint64_t datum_accepted_share_diff;
extern uint64_t datum_rejected_share_count;
extern uint64_t datum_rejected_share_diff;

#define DATUM_REJECT_BAD_JOB_ID 10
#define DATUM_REJECT_BAD_COINBASE_ID 11
#define DATUM_REJECT_BAD_EXTRANONCE_SIZE 12
#define DATUM_REJECT_BAD_TARGET 13
#define DATUM_REJECT_BAD_USERNAME 14
#define DATUM_REJECT_BAD_COINBASER_ID 15
#define DATUM_REJECT_BAD_MERKLE_COUNT 16
#define DATUM_REJECT_BAD_COINBASE_TOO_LARGE 17
#define DATUM_REJECT_COINBASE_MISSING 18
#define DATUM_REJECT_TARGET_MISMATCH 19
#define DATUM_REJECT_H_NOT_ZERO 20
#define DATUM_REJECT_HIGH_HASH 21
#define DATUM_REJECT_COINBASE_ID_MISMATCH 22
#define DATUM_REJECT_BAD_NTIME 23
#define DATUM_REJECT_BAD_VERSION 24
#define DATUM_REJECT_STALE_BLOCK 25
#define DATUM_REJECT_BAD_COINBASE 26
#define DATUM_REJECT_BAD_COINBASE_OUTPUTS 27
#define DATUM_REJECT_MISSING_POOL_TAG 28
#define DATUM_REJECT_DUPLICATE_WORK 29
#define DATUM_REJECT_OTHER 30

#define DATUM_POW_SHARE_RESPONSE_ACCEPTED 0x50
#define DATUM_POW_SHARE_RESPONSE_ACCEPTED_TENTATIVELY 0x55
#define DATUM_POW_SHARE_RESPONSE_REJECTED 0x66

#endif
