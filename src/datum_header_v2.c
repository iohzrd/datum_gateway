/*
 *
 * DATUM Gateway
 * Decentralized Alternative Templates for Universal Mining
 *
 * BLAKE2b proof-of-work block header (version 2), per
 * Bitcoin Knots v29.4.1.knots20260508, reproducing
 * CBlockHeader::GetHash() from src/primitives/block.cpp.
 *
 * Copyright (c) 2026 the DATUM Gateway contributors
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

#include "datum_header_v2.h"

#include <assert.h>
#include <string.h>

#include "datum_utils.h"

uint32_t datum_header_v2_time_on_wire(const T_DATUM_HEADER_V2 *h) {
	if (!(h->flags & DATUM_HEADER_FLAG_USE_TIME_OFFSET)) {
		return h->time;
	}
	return h->time - h->time_offset;
}

uint8_t datum_header_v2_asic_profile(const T_DATUM_HEADER_V2 *h) {
	return h->flags & DATUM_HEADER_FLAG_PROFILE_MASK;
}

void datum_header_v2_serialize(const T_DATUM_HEADER_V2 *h, unsigned char out[DATUM_HEADER_V2_SIZE]) {
	size_t i = 0;

	pk_u32le(out, i, DATUM_HEADER_V2_FLAG | (((uint32_t)h->version) & ~DATUM_HEADER_V2_FLAG)); i += 4;
	memcpy(&out[i], h->prev_block, 32); i += 32;
	memcpy(&out[i], h->merkle_root, 32); i += 32;
	pk_u32le(out, i, datum_header_v2_time_on_wire(h)); i += 4;
	pk_u32le(out, i, h->bits); i += 4;
	pk_u32le(out, i, h->nonce); i += 4;
	pk_u32le(out, i, h->nonce2); i += 4;
	pk_u32le(out, i, h->nonce3); i += 4;
	memcpy(&out[i], h->extranonce, 16); i += 16;
	pk_u32le(out, i, h->time_offset); i += 4;
	pk_u16le(out, i, h->txcount); i += 2;
	out[i++] = h->flags;
	out[i++] = h->xor_key_mask_clear_bits;
	memcpy(&out[i], h->xor_key, 16); i += 16;
	pk_u32le(out, i, (uint32_t)h->height); i += 4;
	memcpy(&out[i], h->mm_rhs, 32); i += 32;
	assert(i == DATUM_HEADER_V2_SIZE);
}

bool datum_header_v2_deserialize(T_DATUM_HEADER_V2 *h, const unsigned char in[DATUM_HEADER_V2_SIZE]) {
	size_t i = 0;
	uint32_t v, time_on_wire;

	memset(h, 0, sizeof(*h));
	v = upk_u32le(in, i); i += 4;
	if (!(v & DATUM_HEADER_V2_FLAG)) return false;
	h->version = (int32_t)(v & ~DATUM_HEADER_V2_FLAG);
	memcpy(h->prev_block, &in[i], 32); i += 32;
	memcpy(h->merkle_root, &in[i], 32); i += 32;
	time_on_wire = upk_u32le(in, i); i += 4;
	h->bits = upk_u32le(in, i); i += 4;
	h->nonce = upk_u32le(in, i); i += 4;
	h->nonce2 = upk_u32le(in, i); i += 4;
	h->nonce3 = upk_u32le(in, i); i += 4;
	memcpy(h->extranonce, &in[i], 16); i += 16;
	h->time_offset = upk_u32le(in, i); i += 4;
	h->txcount = upk_u16le(in, i); i += 2;
	h->flags = in[i++];
	h->xor_key_mask_clear_bits = in[i++];
	memcpy(h->xor_key, &in[i], 16); i += 16;
	h->height = (int32_t)upk_u32le(in, i); i += 4;
	memcpy(h->mm_rhs, &in[i], 32); i += 32;
	assert(i == DATUM_HEADER_V2_SIZE);

	h->time = time_on_wire;
	if (h->flags & DATUM_HEADER_FLAG_USE_TIME_OFFSET) {
		h->time = time_on_wire + h->time_offset;
	}
	return true;
}

// The mask applied to the BLAKE2b output: a tagged hash of the pool's key with
// its leading bits cleared, so the miner can still judge share difficulty but
// cannot tell a block from a share. A null key disables it entirely.
static void datum_header_v2_xor_mask(const T_DATUM_HEADER_V2 *h, unsigned char mask[32]) {
	unsigned int clear_bytes;
	int i;
	bool null_key = true;

	for (i = 0; i < 16; i++) {
		if (h->xor_key[i]) {
			null_key = false;
			break;
		}
	}
	if (null_key) {
		memset(mask, 0, 32);
		return;
	}

	datum_tagged_sha256("Bitcoin block hash PoW XOR mask", h->xor_key, 16, mask);
	clear_bytes = h->xor_key_mask_clear_bits / 8;
	if (clear_bytes > 32) clear_bytes = 32;
	if (clear_bytes) memset(mask, 0, clear_bytes);
	if (clear_bytes < 32) {
		mask[clear_bytes] &= 0xff >> (h->xor_key_mask_clear_bits % 8);
	}
}

// The commitment chain h1 -> h2. Every argument is a template field, so the
// result is fixed for a job. prev_block, merkle_root and mm_rhs arrive in
// internal byte order; h1 commits to the previous block hash in display order.
// Any of the three outputs may be NULL.
static void datum_header_v2_commitment_fields(int32_t version, const unsigned char prev_block[32],
                                              const unsigned char merkle_root[32],
                                              uint32_t time_on_wire, uint32_t bits, int32_t height,
                                              uint16_t txcount, uint8_t flags,
                                              const unsigned char xor_key[16], uint8_t xor_key_mask_clear_bits,
                                              const unsigned char mm_rhs[32],
                                              unsigned char xor_key_hash[32], unsigned char h1[32], unsigned char h2[32]) {
	unsigned char xkh[32], h1_local[32];
	unsigned char buf[119];
	size_t i = 0;
	int j;

	datum_tagged_sha256("Bitcoin block hash PoW XOR key", xor_key, 16, xkh);
	if (xor_key_hash) memcpy(xor_key_hash, xkh, 32);

	// h1 commits to everything the mining hardware never sees, so the hardware
	// cannot be broken by a future change to any of these fields. The hardware
	// does see the previous block hash, but only as the tagged hash that
	// datum_header_v2_prevblock_hidden derives, so h1 commits to it as well.
	// The node hashes the complete version with the v2 flag set (block.cpp
	// GetHash: h1 << GetCompleteVersion()); version holds it flag-stripped.
	pk_u32le(buf, i, DATUM_HEADER_V2_FLAG | ((uint32_t)version & ~DATUM_HEADER_V2_FLAG)); i += 4;
	for (j = 0; j < 32; j++) buf[i + j] = prev_block[31 - j];
	i += 32;
	pk_u32le(buf, i, (uint32_t)height); i += 4;
	memcpy(&buf[i], merkle_root, 32); i += 32;
	pk_u32le(buf, i, time_on_wire); i += 4;
	buf[i++] = 0;  // reserved for a 40-bit block time
	pk_u32le(buf, i, bits); i += 4;
	pk_u32le(buf, i, (uint32_t)txcount); i += 4;
	buf[i++] = flags;
	buf[i++] = xor_key_mask_clear_bits;
	memcpy(&buf[i], xkh, 32); i += 32;
	assert(i == 119);
	datum_tagged_sha256("Bitcoin block header 1", buf, i, h1_local);
	if (h1) memcpy(h1, h1_local, 32);
	if (!h2) return;

	// The 32 zero bytes between h1 and the merge-mining right-hand side are the
	// unused left-hand side of the hook.
	memcpy(buf, h1_local, 32);
	memset(&buf[32], 0, 32);
	memcpy(&buf[64], mm_rhs, 32);
	datum_tagged_sha256("Merge-mining hook", buf, 96, h2);
}

// What Sv1 carries as "coinb1 || extranonce", hashed into the value the
// hardware treats as its merkle root.
static void datum_header_v2_hash1(const unsigned char h2[32], const unsigned char extranonce[DATUM_HEADER_V2_EXTRANONCE_SIZE], unsigned char hash1[32]) {
	unsigned char buf[52];

	memset(buf, 0, 4);
	memcpy(&buf[4], h2, 32);
	memcpy(&buf[36], extranonce, DATUM_HEADER_V2_EXTRANONCE_SIZE);
	my_blake2b(hash1, buf, sizeof(buf));
}

void datum_header_v2_prevblock_hidden(const unsigned char prev_block[32], unsigned char out[32]) {
	unsigned char display[32];
	int i;

	for (i = 0; i < 32; i++) display[i] = prev_block[31 - i];
	datum_tagged_sha256("Bitcoin prevblock header, hashed", display, 32, out);
	// Profile 0 is the only consumer and it clears the first six bytes before
	// hashing them, so the value is only ever used in this form.
	memset(out, 0, 6);
}

void datum_header_v2_precompute(const T_DATUM_HEADER_V2 *h, unsigned char hash1[32], unsigned char h2_out[32], unsigned char mask[32]) {
	unsigned char h2[32];

	if (mask) datum_header_v2_xor_mask(h, mask);
	if (!hash1 && !h2_out) return;

	datum_header_v2_commitment_fields(h->version, h->prev_block, h->merkle_root,
		datum_header_v2_time_on_wire(h), h->bits, h->height, h->txcount, h->flags,
		h->xor_key, h->xor_key_mask_clear_bits, h->mm_rhs, NULL, NULL, h2);
	if (h2_out) memcpy(h2_out, h2, 32);
	if (hash1) datum_header_v2_hash1(h2, h->extranonce, hash1);
}

size_t datum_header_v2_asic_input(const T_DATUM_HEADER_V2 *h, const unsigned char hash1[32], const unsigned char h2[32], unsigned char out[DATUM_HEADER_V2_MAX_ASIC_INPUT]) {
	size_t i = 0, zeros = 0;

	if (datum_header_v2_asic_profile(h) == 1) {
		pk_u32le(out, i, h->nonce); i += 4;
		pk_u32le(out, i, h->nonce2); i += 4;
		pk_u32le(out, i, h->nonce3); i += 4;
		pk_u32le(out, i, h->time_offset); i += 4;
		memcpy(&out[i], hash1, 32); i += 32;
		memcpy(&out[i], h2, 32); i += 32;
		return i;
	}

	// Profile 0 hashes the hidden previous block hash in the leading 32 bytes,
	// which is the position the Siacoin layout puts the parent id in. Profiles
	// 2 and 3 put h2 there instead, behind 48 or 80 zero bytes.
	switch (datum_header_v2_asic_profile(h)) {
		case 2: zeros = 48; break;
		case 3: zeros = 80; break;
		default: zeros = 0; break;
	}
	if (zeros) {
		memset(out, 0, zeros); i = zeros;
		memcpy(&out[i], h2, 32); i += 32;
	} else {
		datum_header_v2_prevblock_hidden(h->prev_block, &out[i]); i += 32;
	}
	pk_u32le(out, i, h->nonce); i += 4;
	pk_u32le(out, i, h->nonce2); i += 4;
	pk_u32le(out, i, h->time_offset); i += 4;
	pk_u32le(out, i, h->nonce3); i += 4;
	memcpy(&out[i], hash1, 32); i += 32;
	return i;
}

void datum_header_v2_components(const T_DATUM_HEADER_V2 *h, T_DATUM_HEADER_V2_COMPONENTS *c) {
	int j;

	memset(c, 0, sizeof(*c));
	c->asic_profile = datum_header_v2_asic_profile(h);

	datum_header_v2_commitment_fields(h->version, h->prev_block, h->merkle_root,
		datum_header_v2_time_on_wire(h), h->bits, h->height, h->txcount, h->flags,
		h->xor_key, h->xor_key_mask_clear_bits, h->mm_rhs, c->xor_key_hash, c->h1, c->h2);
	datum_header_v2_prevblock_hidden(h->prev_block, c->prevblock_hidden);
	datum_header_v2_hash1(c->h2, h->extranonce, c->hash1);

	c->asic_input_len = datum_header_v2_asic_input(h, c->hash1, c->h2, c->asic_input);
	my_blake2b(c->hash2, c->asic_input, c->asic_input_len);

	datum_header_v2_xor_mask(h, c->mask);
	for (j = 0; j < 32; j++) c->result[j] = c->hash2[j] ^ c->mask[j];
}

void datum_header_v2_pow(const T_DATUM_HEADER_V2 *h, unsigned char out[32]) {
	T_DATUM_HEADER_V2_COMPONENTS c;
	int j;

	datum_header_v2_components(h, &c);
	// The consensus uint256 is the byte reversal of the BLAKE2b output, which
	// puts it in the same internal order as the SHA256d hashes used elsewhere.
	for (j = 0; j < 32; j++) out[j] = c.result[31 - j];
}

void datum_header_v2_build(T_DATUM_HEADER_V2 *out,
                           int32_t version, const unsigned char prev_block[32], const unsigned char merkle_root[32],
                           uint32_t job_time, uint32_t bits, int32_t height, uint16_t txcount, uint8_t flags,
                           const unsigned char extranonce[16],
                           uint32_t nonce, uint32_t nonce2, uint32_t nonce3, uint32_t time_offset) {
	memset(out, 0, sizeof(*out));
	out->version = version;
	memcpy(out->prev_block, prev_block, 32);
	memcpy(out->merkle_root, merkle_root, 32);
	out->bits = bits;
	out->height = height;
	out->txcount = txcount;
	out->flags = flags;
	if (extranonce) memcpy(out->extranonce, extranonce, sizeof(out->extranonce));
	out->nonce = nonce;
	out->nonce2 = nonce2;
	out->nonce3 = nonce3;
	out->time_offset = time_offset;
	// The job supplies the base time; with the time-offset flag the hardware's
	// offset moves the block time itself, otherwise the offset is pure nonce.
	out->time = job_time;
	if (flags & DATUM_HEADER_FLAG_USE_TIME_OFFSET) {
		out->time = job_time + time_offset;
	}
	// The pool's anti-withholding key is not used yet: with a null key the mask
	// is zero, so the miner sees true block hashes and can submit its own blocks.
}

void datum_header_v2_job_commitment(int32_t version, const unsigned char prev_block[32],
                                    const unsigned char merkle_root[32],
                                    uint32_t job_time_on_wire, uint32_t bits, int32_t height,
                                    uint16_t txcount, uint8_t flags,
                                    const unsigned char xor_key[16], uint8_t xor_key_mask_clear_bits,
                                    const unsigned char mm_rhs[32],
                                    unsigned char h1[32], unsigned char h2[32]) {
	unsigned char zero16[16], zero32[32];

	memset(zero16, 0, sizeof(zero16));
	memset(zero32, 0, sizeof(zero32));
	if (!xor_key) xor_key = zero16;
	if (!mm_rhs) mm_rhs = zero32;

	datum_header_v2_commitment_fields(version, prev_block, merkle_root, job_time_on_wire, bits,
		height, txcount, flags, xor_key, xor_key_mask_clear_bits, mm_rhs, NULL, h1, h2);
}

void datum_header_v2_coinb1(const unsigned char h2[32], unsigned char out[DATUM_HEADER_V2_COINB1_SIZE]) {
	memset(out, 0, 3);
	memcpy(&out[3], h2, 32);
}

void datum_header_v2_from_sia_fields(T_DATUM_HEADER_V2 *h, const unsigned char nonce_bytes[8], const unsigned char ntime_bytes[8]) {
	// The serialized time is the job's and does not change; the offset the
	// hardware sends is what moves the block time. So read the wire time before
	// overwriting m_time_offset and restore time = time_on_wire + time_offset
	// after. Without this the struct's invariant breaks: h->time still belongs
	// to the offset the header was built with, so with the time-offset flag set
	// datum_header_v2_serialize would write time - new_offset as the wire time
	// instead of the job's, and h1 would commit that wrong value.
	const uint32_t time_on_wire = datum_header_v2_time_on_wire(h);

	h->nonce = upk_u32le(nonce_bytes, 0);
	h->nonce2 = upk_u32le(nonce_bytes, 4);
	h->time_offset = upk_u32le(ntime_bytes, 0);
	h->nonce3 = upk_u32le(ntime_bytes, 4);

	h->time = time_on_wire;
	if (h->flags & DATUM_HEADER_FLAG_USE_TIME_OFFSET) {
		h->time = time_on_wire + h->time_offset;
	}
}

void datum_header_v2_to_sia_fields(const T_DATUM_HEADER_V2 *h, unsigned char nonce_bytes[8], unsigned char ntime_bytes[8]) {
	pk_u32le(nonce_bytes, 0, h->nonce);
	pk_u32le(nonce_bytes, 4, h->nonce2);
	pk_u32le(ntime_bytes, 0, h->time_offset);
	pk_u32le(ntime_bytes, 4, h->nonce3);
}
