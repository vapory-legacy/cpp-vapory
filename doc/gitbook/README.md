cpp-vapory guide
======= 

This book is intended as a practical user guide for the cpp-vapory software distribution.

cpp-vapory is a distribution of software including a number of diverse tools. This book begins with the installation instructions, before proceeding to introductions, walk-throughs and references for the various tools that make up cpp-vapory.

The full software suite of cpp-vapory includes:

- **vap** (`vap`) The mainline CLI Vapory client. Run it in the background and it will connect to the Vapory network; you can mine, make transactions and inspect the blockchain.
- `vapkey` A key/wallet management tool for Vapory keys. This lets you add, remove and change your keys as well as *cold wallet device*-friendly transaction inspection and signing.
- `vapminer` A standalone miner. This can be used to check how fast you can mine and will mine for you in concert with `vap`, `gvap` and `pyvapory`.
- `vapvm` The Vapory virtual machine emulator. You can use this to run VVM code.
- `rlp` An serialisation/deserialisation tool for the Recursive Length Prefix format.
