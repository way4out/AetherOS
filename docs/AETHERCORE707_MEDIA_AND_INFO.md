# AetherCore 707 Media + Current Information Layer

AetherCore 707 adds a lawful content-discovery layer to Media Studio.

## Supported classes

- Public-domain and openly licensed media catalogs
- Official free-to-watch/watch-now sources
- Internet radio directories and authorized station streams
- Local TV directories and station-provided feeds
- Global public/rights-holder television feeds
- Refreshable news/RSS/web information sources

The DSi stores a compact directory/cache rather than attempting to bundle the Internet or copyrighted television catalogs into the ROM.

## Playback boundary

The Media Hub is a directory/stream contract. Actual playback depends on the feed format, network availability, HTTPS/TLS compatibility, bandwidth, and the DSi media decoder. It does not claim that every modern Web video stream is playable natively.

## Copyright boundary

AetherOS does not package unlicensed full episodes or rebroadcasts. It can point users toward lawful public-domain, Creative Commons, official/free, or otherwise authorized sources.

## Radio and TV

Radio and TV entries are represented as sources rather than pretending that the DSi contains every broadcast. A future gateway can translate compatible network feeds into a DSi-friendly format.

## Current information

Current information is network-refreshable. Offline mode exposes cached data and clearly labels it as cached rather than pretending it is live.

## RF safety

The existing RF/TinySA surface remains passive/authorized analysis. AetherCore 707 does not implement radio jamming or disruption.

## Automated quality gate

CI checks that the NDS is non-empty, DSi-targeted, structurally valid, and that Messaging and Media Hub integration remain present.