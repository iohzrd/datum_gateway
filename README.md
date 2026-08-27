# DATUM Gateway
**Decentralized Alternative Templates for Universal Mining**
(c) 2024-2025 Bitcoin Ocean, LLC, Jason Hughes, and individual contributors

The DATUM Gateway implements lightweight efficient client side decentralized block template creation for true solo mining.

It reaches out to a local Bitcoin node for block templates, generates and distributes work for mining hardware, and submits solved blocks to the network directly.

For miners wanting to pool rewards, it facilitates communication with a DATUM-supporting pool in addition to the above.  The pool is responsible for coordinating the block reward split based on work done for the pool by the miner, but does not create work for the miner.

The work provided by the gateway to mining hardware is generated only from the local node generating templates for the miner. The real miner is always whoever is running the Bitcoin node. With DATUM, that's not the pool. As the protocol is intended solely for mining of decentralized block templates, the DATUM protocol has no mechanisms for the pool providing the information needed to construct work or a block template.

This build mines the BLAKE2b proof of work introduced by [bitcoinknots/bitcoin#359](https://github.com/bitcoinknots/bitcoin/pull/359), and no other. It serves version 2 (164-byte) block headers over the Siacoin dialect of Stratum v1, so that Siacoin BLAKE2b mining hardware can mine the chain that pull request creates. SHA256d work is not served, and `mining.configure` reports `version-rolling` as false: in a version 2 header the block version is fixed by the template and committed in the h1 commitment, so there is nothing for hardware to roll. See [BLAKE2b Proof of Work](#blake2b-proof-of-work) below for the two settings this requires.

Communication with the Bitcoin node is via RPC and must support GBT ("getblocktemplate").  Communication with the pool is via the DATUM protocol.

**Using Bitcoin Knots is highly recommended**. This gives miners fine controls over how they wish to construct their block templates.  Other node implementations that support GBT can also be used.  This includes Bitcoin Core, but it is severely lacking in template control options.  That is unfortunately a centralizing force which partly defeats the purpose of decentralizing block template creation in the first place.

The DATUM Gateway only supports mining Bitcoin.  Modifying the code to support non-Bitcoin is not straightforward, as many optimizations and design considerations are tightly tied to Bitcoin-specific restraints for efficiency.

## DATUM Protocol
The DATUM Gateway's communication with the mining pool is via the DATUM Protocol.  This is an encrypted communication link between the DATUM Gateway (client) and DATUM Prime (pool side).

The protocol itself was made from the ground up as a custom protocol.  Its specification is evolving, subject to change, and will be published elsewhere.

The core concepts of the protocol:

 - Encrypt communications between the Gateway and pool
 - Obfuscate the communications somewhat so a MITM is unable to glean useful or accurate insight into the miner's operation via analysis of the still-ciphered communications.
 - Retrieve proper generation transaction payout splits from the pool for locally constructed templates
 - Submit work to the pool with sufficient data to efficiently validate and accept the work for proper rewards
 - Communicate minimal guardrails and requirements for a valid template to earn pooled rewards

With the current version of the protocol, the pool does block validation after coordinating with the miner. This is strictly to ensure miners are not accidentally creating invalid blocks while DATUM is still undergoing testing. In a future version of the protocol, the pool will not be in charge of this function and will be almost completely blinded to the contents of the miner's block template.

The protocol is not specific to a pooled reward system, as the Gateway coordinates the appropriate generation transaction with the pool.  However, in the spirit of maximum decentralization, the pool should implement rewarding miners directly from generated payouts, such as with OCEAN's TIDES reward system.

![DATUM v0 2-beta recommended setup - network diagram](doc/DATUM_recommended_setup-network_diagram.svg)

## BLAKE2b Proof of Work

This build does not mine SHA256d. Every job it serves is a version 2 block header hashed with BLAKE2b, as specified by [bitcoinknots/bitcoin#359](https://github.com/bitcoinknots/bitcoin/pull/359), so it needs a node built from that pull request and two consensus values that have no defaults:

    "mining": {
        "blake2b_activation_height": <height at which the fork activates>,
        "blake2b_headline": "<exact headline text>"
    }

Both are required and the Gateway will not start without them. They are properties of the network being mined, not preferences, so take them from that network rather than copying them from the example configuration file, whose values are placeholders.

 - `blake2b_activation_height` is the first height at which a version 2 block is valid. Below it the Gateway serves no work at all and logs why, once per height: a version 2 block would be rejected there, and this build produces nothing else.
 - `blake2b_headline` must appear in the coinbase scriptSig of the block at the activation height, or the network rejects that block as `bad-headline`. The Gateway puts it there in place of the cosmetic coinbase tags, since a coinbase scriptSig holds at most 100 bytes. It is limited to 86 bytes and is checked at startup, because a headline that does not fit could otherwise only fail while building the one block that cannot be mined again later.

### The reduced_data rule

Knots activates the RDTS deployment (BIP 110) as a flag day at the same height as the BLAKE2b hardfork, and enforces its rules until the parent block's median time past reaches the deployment's expiry. The node reports `reduced_data` in the `rules` array of every template it enforces it on, and the Gateway takes that as the signal:

 - The consensus block weight limit drops from 4,000,000 to 700,000. The node already reports the lower figure in the template's `weightlimit`, which is what the Gateway sizes the coinbase against, so this needs no configuration.
 - Every output of the generation transaction is limited to a 34-byte scriptPubKey, or 83 bytes if it begins with `OP_RETURN`. A block carrying a larger one is rejected as `bad-txns-vout-script-toolarge`. An output script from `mining.pool_address` is at most 34 bytes and always fits. A payout script from a DATUM pool is not bounded by the protocol: the Gateway leaves an oversized miner payout output out of the coinbase, and refuses to serve work for the block if the pool's own payout script is oversized, since that output carries the rest of the generation value and cannot be left out.

Consequences for mining hardware:

 - Hardware must speak the Siacoin dialect of Stratum v1. `mining.notify` sends `coinb1` as `000000` followed by the h2 commitment, an empty `coinb2`, an empty merkle branch list, an empty and unused version field, a tagged hash of the previous block hash with its first six bytes cleared in place of the previous block hash itself (what proof-of-work profile 0 hashes), and `ntime` as eight raw header bytes. `mining.submit` carries `ntime` and `nonce` as eight raw header bytes each, spliced into the header rather than parsed as numbers. A miner builds its merkle leaf as `BLAKE2b(0x00 || coinb1 || extranonce1 || extranonce2)`, which reproduces the hash1 of the pull request's construction exactly.
 - `mining.subscribe` reports an 8-byte extranonce1 and an 8-byte extranonce2. In a version 2 header the extranonce is a header field rather than part of the coinbase.
 - Version rolling (aka "ASICBoost") is refused. A miner that asks for it is told `"version-rolling": false` and should submit five parameters to `mining.submit`, as the Siacoin dialect does.
 - The block time is fixed by the Gateway and committed in h1. The field hardware treats as its 64-bit timestamp is nonce space, so hardware that rolls a real timestamp there cannot push the block time forward.

## Requirements

 - 64-bit AMD or Intel system. Other systems may work, but at this time it is at your own risk.
 - Linux-based operating system. Other OSs will be supported in the future.
 - Bitcoin full node built from [bitcoinknots/bitcoin#359](https://github.com/bitcoinknots/bitcoin/pull/359), fully synced with the network being mined. This build serves only the BLAKE2b proof of work that pull request introduces, so a node without it will reject the blocks the Gateway submits.
 - Fast storage recommended for the Bitcoin node.
 - Stable internet connection for both the Bitcoin node and Gateway's communication with the pool.
 - CPU powerful enough to run the Bitcoin node without validation delays.
 - Approximately 1GB/RAM, plus 1GB/RAM per 1000 Stratum clients, plus Bitcoin node RAM requirements.
 - Siacoin BLAKE2b mining hardware able to reach the system running the DATUM Gateway. SHA256d hardware cannot mine this build's work.

This list is not extensive, but the main goal is the have a stable system for your Bitcoin node and the Gateway such that your node is processing new incoming blocks and getting templates to the Gateway as quickly as possible.  While this may all work on relatively low end hardware, your mileage may vary.

No modifications to the Bitcoin node source code is required for the Gateway, as it uses the standard GBT mechanism for template fetch.

The following external libraries are required:
 - libcurl
 - libjansson
 - libmicrohttpd
 - libsodium

## Node Configuration
Your Bitcoin node must be configured to construct blocks as you desire.  Bitcoin Knots provides many options for configuring your node's policy and is highly recommended.

At this time, you must also reserve some block space for the pool's generation transaction.  The following options are currently recommended:

    blockmaxsize=3985000
    blockmaxweight=3985000

Note: This reservation requirement will be removed for Bitcoin Knots users in a future version of the DATUM Gateway thanks to support for on-the-fly specification of these metrics by the client in Knots.

To avoid mining stale work, you will need to ensure the DATUM Gateway receives new block notifications from your node. It is suggested you run the DATUM Gateway as the same user as your full node and utilize the following configuration line in your bitcoin.conf:

    blocknotify=killall -USR1 datum_gateway

Ensure you have "killall" installed on your system (*psmisc* package on many OSs).

If the node and Gateway are on different systems, you may need to utilize the "NOTIFY" endpoint on the Gateway's dashboard/API instead.

Finally, the Gateway must have RPC access to your node, and you must add an RPC user to your configuration to facilitate this, as well as ensuring the service running the Gateway is whitelisted for RPC access (if not on the same machine).

Some additional recommendations:

    maxmempool=1000
    blockreconstructionextratxn=1000000

As a true miner, you'll most likely want as many valid transactions as possible in your mempool which meet your node's policies.

## Installation
Install and fully sync your Bitcoin full node. Instructions for this are beyond the scope of this document.

Configure your node to create block templates as you desire. Be sure to reserve some space for the generation transaction, otherwise your work will not be able to fit a reward split.  See node configuration recommendations above.

Install the required libraries and development packages for dependencies: cmake, pkgconf, libcurl, jansson, libsodium, and libmicrohttpd. You may also need psmisc for your node to send blocknotify signals to the DATUM Gateway.

For Debian/Ubuntu:

    sudo apt install cmake pkgconf libcurl4-openssl-dev libjansson-dev libsodium-dev libmicrohttpd-dev psmisc

For Fedora/Amazon Linux:

    sudo dnf install cmake pkgconf libcurl-devel jansson-devel libsodium-devel libmicrohttpd-devel psmisc

For Alma Linux:

    sudo dnf install epel-release dnf-plugins-core
    sudo dnf config-manager --set-enabled crb
    sudo dnf install cmake pkgconf libcurl-devel jansson-devel libsodium-devel libmicrohttpd-devel psmisc

For Oracle Linux:

    sudo dnf install epel-release dnf-plugins-core
    sudo dnf config-manager --set-enabled ol9_codeready_builder
    sudo dnf install cmake pkgconf libcurl-devel jansson-devel libsodium-devel libmicrohttpd-devel psmisc

For Alpine (also needs a standalone argp library):

    sudo apk add build-base cmake pkgconf argp-standalone curl-dev jansson-dev libsodium-dev libmicrohttpd-dev psmisc

For Arch:

    sudo pacman -Syu base-devel cmake pkgconf curl jansson libsodium libmicrohttpd psmisc

For Clear Linux:

    sudo swupd bundle-add c-basic cmake pkgconf devpkg-curl devpkg-jansson devpkg-libsodium devpkg-libmicrohttpd psmisc

For FreeBSD:

    sudo pkg install cmake pkgconf curl jansson libsodium libmicrohttpd argp-standalone libepoll-shim

Compile DATUM by running:

    cmake . && make

## Usage

Run the datum_gateway executable with the -? flag for detailed configuration information, descriptions, and required options.  Then construct a configuration file (defaults to "datum_gateway_config.json" in the current working directory). Be sure to also set your coinbase tags.  The primary tag setting is unused in pooled mining, however the secondary tag is intended to show on things like block explorers when you mine a block.

There is an [example configuration file included in the doc/ directory](doc/example_datum_gateway_config.json) you may wish to use as a template. Its `mining.blake2b_activation_height` and `mining.blake2b_headline` values are placeholders and will not start the Gateway; replace them with the real values for the network you are mining, as described under [BLAKE2b Proof of Work](#blake2b-proof-of-work).
Note that the API/web admin password is also used for preventing CSRF attacks, so it is crucial you set it to something reasonably secure (or disable the API/web interface entirely).

You should review the [documentation on usernames](doc/usernames.md) next.
Once you have everything running, you can point miners at the Gateway.

## Docker

The DATUM Gateway is also available as a Docker image.


### Building the Docker Image

To build the DATUM Gateway Docker image:

```bash
# From the root of the repository
docker build -t datum_gateway .
```

### Running the Container

To run the DATUM Gateway container:

```bash
# Run with default configuration
docker run -p 23334:23334 -p 7152:7152 --name datum-gateway datum_gateway
```

The container expects a configuration file at `/app/config/config.json`. Mount a volume to this path to use your own configuration:

```bash
docker run -v /path/to/your/config/directory:/app/config -p 23334:23334 -p 7152:7152 datum_gateway
```

You will need to disable the notify fallback in your configuration file if you are using Docker. And in bitcoin.conf, you will need to set the following:

```bash
blocknotify=wget -q -O /dev/null http://datum-gateway:7152/NOTIFY
```

### Connecting to a Bitcoin Node

When running the DATUM Gateway in Docker, you need to configure it to connect to your Bitcoin node. The connection method depends on where your Bitcoin node is running:

#### 1. Bitcoin Node Running in Docker (Same Network)

If your Bitcoin node is also running in a Docker container on the same network, use the container name as the hostname:

```json
{
  "rpc_host": "bitcoin-node",
  "rpc_port": 8332,
  "rpc_user": "your_rpc_user",
  "rpc_pass": "your_rpc_password"
}
```

In your `bitcoin.conf`, set the blocknotify to use the DATUM Gateway container name:

```
blocknotify=wget -q -O /dev/null http://datum-gateway:7152/NOTIFY
```

#### 2. Bitcoin Node Running on Host System

If your Bitcoin node is running directly on the host system or in a container that binds to host ports, you have two options:

**Option A: Using host.docker.internal (recommended)**
```json
{
  "rpc_host": "host.docker.internal",
  "rpc_port": 8332,
  "rpc_user": "your_rpc_user",
  "rpc_pass": "your_rpc_password"
}
```

**Option B: Using host networking mode**
Run the DATUM Gateway container with `--network host`:

```bash
docker run --network host -v /path/to/config:/app/config datum_gateway
```

Then configure using localhost:
```json
{
  "rpc_host": "localhost",
  "rpc_port": 8332,
  "rpc_user": "your_rpc_user",
  "rpc_pass": "your_rpc_password"
}
```

For blocknotify in `bitcoin.conf` when using host networking:
```
blocknotify=wget -q -O /dev/null http://localhost:7152/NOTIFY
```

#### 3. Bitcoin Node on Remote System

If your Bitcoin node is running on a different machine, use the hostname or IP address:

```json
{
  "rpc_host": "192.168.1.100",
  "rpc_port": 8332,
  "rpc_user": "your_rpc_user",
  "rpc_pass": "your_rpc_password"
}
```

In your remote Bitcoin node's `bitcoin.conf`:
```
blocknotify=wget -q -O /dev/null http://datum-gateway-host-ip:7152/NOTIFY
```

**Important Notes:**
- Ensure your Bitcoin node's RPC is configured to accept connections from the DATUM Gateway
- For remote connections, you may need to configure `rpcbind` and `rpcallowip` in your `bitcoin.conf`
- Always use strong RPC credentials and consider network security when exposing RPC endpoints
- Remember to disable the notify fallback in your DATUM Gateway configuration when using Docker

## Template/Share Requirements for Pooled Mining

 - Must be a valid block and conform to current Bitcoin consensus rules
 - Submitted work must be for the current latest block height, valid time, etc
 - Must include generation transaction outputs provided by the pool in the order provided
 - Must include the primary coinbase tag as provided by the pool
 - Must include the unique identifier provided by the pool
 - Work must include the work target and meet/exceed that target
 - Any additional requirements by pool documentation

## Notes/Known Issues/Limitations

- By default, if the connection with the pool is lost and fails to reconnect, the Gateway will disconnect all stratum clients. This way miners can use their built-in failover and switch to non-DATUM mining, or an alternate/backup Gateway.
- Accepted/rejected share counts on mining hardware may not perfectly match with the pool. The delta may vary depending on the Gateway's configuration. This is because shares are first accepted or rejected as valid for your local template based on your local node, and then again accepted or rejected based on the pool's requirements, latency to the pool (stale work), latency between your node and the network (stale work), etc.  Stratum v1 has no mechanism to report back to the miner that previously accepted work is now rejected, and it doesn't make sense to wait for the pool before responding, either.

**Most importantly**, please note that this is currently a public **BETA** release. While best efforts have been made to ensure this software is as stable and as useful as possible, you may still encounter issues.

This software is likely to undergo rapid development and revisions up until a v1.0 stable release. Some of these revisions may include changes, such as protocol changes, that require upgrading to the latest version with short or even no notice in order to continue using the software with a DATUM pool. Be sure to watch for important updates!

Be sure you have failover settings on your miners. As a best practice, when mining on a DATUM pool, set your miner's failover to use that pool's Stratum endpoint.

## License

The DATUM Gateway (including the DATUM Protocol) is free open source software and released under the terms of the MIT license.  See LICENSE.
