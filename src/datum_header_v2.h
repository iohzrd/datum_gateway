/*
 *
 * DATUM Gateway
 * Decentralized Alternative Templates for Universal Mining
 *
 * BLAKE2b proof-of-work block header (version 2), per
 * Bitcoin Knots v29.4.1.knots20260508.
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

#ifndef _DATUM_HEADER_V2_H_
#define _DATUM_HEADER_V2_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Serialized size of a version 2 (BLAKE2b) block header.
#define DATUM_HEADER_V2_SIZE 164
// Version-field bit marking a v2 header on the wire.
#define DATUM_HEADER_V2_FLAG 0x80000000UL
// m_flags bit 2: nTime = time_on_wire + m_time_offset.
#define DATUM_HEADER_FLAG_USE_TIME_OFFSET 4
// m_flags bits 0-1 select the layout the mining hardware hashes.
#define DATUM_HEADER_FLAG_PROFILE_MASK 3
// The largest ASIC input (profile 3 prepends 80 zero bytes to an 80-byte core).
#define DATUM_HEADER_V2_MAX_ASIC_INPUT 160
// Size of the header extranonce, which replaces the Sv1 in-coinbase one.
#define DATUM_HEADER_V2_EXTRANONCE_SIZE 16

// A version 2 block header. All 256/128-bit values are held in internal
// (little-endian) byte order, i.e. the order they are serialized in, which is
// the reverse of the "display" hex used by RPC and the PR's test vectors.
typedef struct {
	int32_t version;                       // low 31 bits; bit 31 is the v2 marker
	unsigned char prev_block[32];
	unsigned char merkle_root[32];
	uint32_t time;                         // real block time (nTime), not the wire time
	uint32_t bits;
	uint32_t nonce;
	uint32_t nonce2;
	uint32_t nonce3;
	unsigned char extranonce[DATUM_HEADER_V2_EXTRANONCE_SIZE];  // Sv1 extranonce1 || extranonce2
	uint32_t time_offset;
	uint16_t txcount;
	uint8_t flags;
	uint8_t xor_key_mask_clear_bits;
	unsigned char xor_key[16];             // pool's anti-withholding key; zero disables
	int32_t height;
	unsigned char mm_rhs[32];              // merge-mining hook
} T_DATUM_HEADER_V2;

// Every intermediate of the proof-of-work computation, for tests and logging.
typedef struct {
	unsigned char xor_key_hash[32];
	unsigned char prevblock_hidden[32];    // tagged hash of prev_block, first 6 bytes cleared
	unsigned char h1[32];
	unsigned char h2[32];
	unsigned char hash1[32];               // BLAKE2b(0000 0000 || h2 || extranonce)
	unsigned char hash2[32];               // BLAKE2b(asic_input)
	unsigned char mask[32];
	unsigned char result[32];              // hash2 ^ mask, in BLAKE2b output order
	unsigned char asic_input[DATUM_HEADER_V2_MAX_ASIC_INPUT];
	size_t asic_input_len;
	uint8_t asic_profile;
} T_DATUM_HEADER_V2_COMPONENTS;

// The time value that is serialized and committed, which differs from nTime
// when the time-offset flag is set.
uint32_t datum_header_v2_time_on_wire(const T_DATUM_HEADER_V2 *h);

// ASIC layout selector (m_flags & 3).
uint8_t datum_header_v2_asic_profile(const T_DATUM_HEADER_V2 *h);

// Serialize to the 164-byte wire form.
void datum_header_v2_serialize(const T_DATUM_HEADER_V2 *h, unsigned char out[DATUM_HEADER_V2_SIZE]);

// Parse a 164-byte wire header. Returns false if the v2 marker bit is absent.
bool datum_header_v2_deserialize(T_DATUM_HEADER_V2 *h, const unsigned char in[DATUM_HEADER_V2_SIZE]);

// The template-fixed parts: hash1 (what the hardware treats as its merkle
// root), h2 (which profiles 1-3 hash directly) and the XOR mask. None of them
// depend on the grinding fields, so a job can compute them once. Any output
// pointer may be NULL.
void datum_header_v2_precompute(const T_DATUM_HEADER_V2 *h, unsigned char hash1[32], unsigned char h2[32], unsigned char mask[32]);

// The 32 bytes the hardware receives in place of the previous block hash: a
// tagged hash of hashPrevBlock in display order, with its first six bytes
// cleared. Profile 0 hashes this rather than the previous block hash itself,
// so it is also what mining.notify must carry.
void datum_header_v2_prevblock_hidden(const unsigned char prev_block[32], unsigned char out[32]);

// Build the bytes the mining hardware hashes. Returns the length (80, 128 or
// 160 depending on the profile). Profile 0 consumes the hidden previous block
// hash; profiles 1-3 consume h2 instead.
size_t datum_header_v2_asic_input(const T_DATUM_HEADER_V2 *h, const unsigned char hash1[32], const unsigned char h2[32], unsigned char out[DATUM_HEADER_V2_MAX_ASIC_INPUT]);

// Full computation with all intermediates.
void datum_header_v2_components(const T_DATUM_HEADER_V2 *h, T_DATUM_HEADER_V2_COMPONENTS *c);

// The consensus block hash, in the same internal byte order as the SHA256d
// hashes elsewhere in this codebase, so it can be compared against a target
// with compare_hashes().
void datum_header_v2_pow(const T_DATUM_HEADER_V2 *h, unsigned char out[32]);

// Fill in a version 2 header from a job's template-derived fields plus the
// fields the mining hardware grinds.
//
// merkle_root is the block's SHA256d merkle root in internal byte order (the
// same value that occupies bytes 36..68 of a version 1 header). It commits to
// the coinbase and every other transaction, and travels inside h1 rather than
// being hashed by the hardware.
//
// extranonce is the 16-byte header extranonce, which replaces the Sv1 coinbase
// extranonce: in version 2 the coinbase is fixed for the whole job, so per-miner
// uniqueness lives in the header instead of in the coinbase.
//
// With DATUM_HEADER_FLAG_USE_TIME_OFFSET set in flags, the block time is
// job_time + time_offset, so hardware that rolls its time field rolls the
// block time.
void datum_header_v2_build(T_DATUM_HEADER_V2 *out,
                           int32_t version, const unsigned char prev_block[32], const unsigned char merkle_root[32],
                           uint32_t job_time, uint32_t bits, int32_t height, uint16_t txcount, uint8_t flags,
                           const unsigned char extranonce[DATUM_HEADER_V2_EXTRANONCE_SIZE],
                           uint32_t nonce, uint32_t nonce2, uint32_t nonce3, uint32_t time_offset);

// Byte offset of m_extranonce in the serialized header. The first 80 bytes
// deliberately share the version 1 field layout (version, prevhash, merkle
// root, time-on-wire, bits, nonce), so offsets 0, 4, 36, 68, 72 and 76 mean the
// same thing in both.
#define DATUM_HEADER_V2_EXTRANONCE_OFFSET 88

// Bytes of the header extranonce assigned by the server (extranonce1). The
// miner supplies the remainder as extranonce2. Siacoin hardware limits on
// extranonce2_size are not yet confirmed, so this is the one value to revisit
// once a real miner has been captured; everything that depends on the split
// derives from these two, so changing it does not need a search for hardcoded
// sizes.
#define DATUM_HEADER_V2_EXTRANONCE1_SIZE 8
#define DATUM_HEADER_V2_EXTRANONCE2_SIZE (DATUM_HEADER_V2_EXTRANONCE_SIZE - DATUM_HEADER_V2_EXTRANONCE1_SIZE)

// extranonce1 carries the 4-byte Stratum session id, right-aligned in its
// share, and cannot take the whole field or the miner would have no extranonce2
// to vary.
_Static_assert(DATUM_HEADER_V2_EXTRANONCE1_SIZE >= 4, "extranonce1 must hold the session id");
_Static_assert(DATUM_HEADER_V2_EXTRANONCE2_SIZE > 0, "the miner needs an extranonce2");

// The leading bytes of m_extranonce that are always zero: extranonce1 is the
// session id left-padded into its share, so everything before the id is padding.
// A share carries only what follows, and the server restores the rest.
#define DATUM_HEADER_V2_EXTRANONCE_PAD (DATUM_HEADER_V2_EXTRANONCE1_SIZE - 4)
#define DATUM_HEADER_V2_EXTRANONCE_WIRE (DATUM_HEADER_V2_EXTRANONCE_SIZE - DATUM_HEADER_V2_EXTRANONCE_PAD)

// The protocol carries twelve extranonce bytes for either proof of work, so the
// header layout has to leave exactly four bytes of padding in front of them.
_Static_assert(DATUM_HEADER_V2_EXTRANONCE_WIRE == 12, "a share carries twelve extranonce bytes");

// Length of the Sv1 "coinb1" a Siacoin-layout miner is given for a v2 job.
#define DATUM_HEADER_V2_COINB1_SIZE 35

// The per-job commitments h1 and h2, which depend only on template fields and
// so are computed once per job rather than per share. Either output may be NULL.
//
// job_time_on_wire is the serialized time, i.e. the block time minus the time
// offset when DATUM_HEADER_FLAG_USE_TIME_OFFSET is set. xor_key and mm_rhs may
// be NULL, meaning all zeroes.
void datum_header_v2_job_commitment(int32_t version, const unsigned char prev_block[32],
                                    const unsigned char merkle_root[32],
                                    uint32_t job_time_on_wire, uint32_t bits, int32_t height,
                                    uint16_t txcount, uint8_t flags,
                                    const unsigned char xor_key[16], uint8_t xor_key_mask_clear_bits,
                                    const unsigned char mm_rhs[32],
                                    unsigned char h1[32], unsigned char h2[32]);

// The Sv1 "coinb1" for a version 2 job: three zero bytes followed by h2.
//
// A Siacoin-layout miner builds its merkle leaf as
// BLAKE2b(0x00 || coinb1 || extranonce1 || extranonce2 || coinb2) with an empty
// coinb2, which reproduces hash1 exactly: the leaf's own 0x00 prefix supplies
// the fourth zero byte of the 52-byte preimage. This is what lets unmodified
// Siacoin hardware mine this chain.
void datum_header_v2_coinb1(const unsigned char h2[32], unsigned char out[DATUM_HEADER_V2_COINB1_SIZE]);

// Siacoin-dialect Stratum field mapping.
//
// Verified against the SiaMining reference implementations rather than prose:
//   spec:  https://github.com/SiaMining/Stratum/blob/master/Stratum.md
//   proxy: https://github.com/SiaMining/stratum-mining-proxy
//          mining_libs/jobs.py serialize_header() and submit()
//
// The Sia header is
//     prevhash(32) || nonce(8) || ntime(8) || merkleroot(32)
// and the proxy submits ntime and nonce as the *raw header bytes* hexlified
// (header[32:40] and header[40:48]), with no endianness conversion and no
// integer formatting. A server must therefore splice those eight bytes in
// verbatim; treating them as a number, as Bitcoin Stratum v1 does, is wrong.
//
// The 32 bytes the miner is sent in the prevhash position are not the previous
// block hash but datum_header_v2_prevblock_hidden of it, which is what profile
// 0 hashes.
//
// Profile 0 (Bitcoin Knots v29.4.1.knots20260508) places the header v2 grinding
// fields in exactly those positions:
//     nonce bytes  = nNonce (4, LE) || m_nonce2 (4, LE)
//     ntime bytes  = m_time_offset (4, LE) || m_nonce3 (4, LE)
// so hardware rolling its 64-bit nonce rolls nNonce and m_nonce2, and hardware
// rolling its 64-bit timestamp rolls the block time (with the time-offset flag
// set) and m_nonce3.
//
// h must already hold the job's fields, as datum_header_v2_build leaves them.
// The serialized time is taken to be the job's and is preserved across the
// splice; h->time is recomputed from it and the new offset, so the struct's
// time = time_on_wire + time_offset invariant still holds afterwards.
void datum_header_v2_from_sia_fields(T_DATUM_HEADER_V2 *h, const unsigned char nonce_bytes[8], const unsigned char ntime_bytes[8]);

// The inverse: the eight nonce bytes and eight ntime bytes a miner would send
// for this header.
void datum_header_v2_to_sia_fields(const T_DATUM_HEADER_V2 *h, unsigned char nonce_bytes[8], unsigned char ntime_bytes[8]);

#endif
