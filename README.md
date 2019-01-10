# cpp-vapory - Vapory C++ client

This repository contains [cpp-vapory](http://cpp-vapory.co), the [Vapory](https://vapory.co) C++ client.

It is the third most popular of the Vapory clients, behind [gvap](https://github.com/vaporyco/go-vapory) (the [go](https://golang.org)
client) and [Parity](https://github.com/vapcore/parity) (the [rust](https://www.rust-lang.org/) client).  The code is exceptionally
[portable](http://cpp-vapory.co/portability.html) and has been used successfully on a very broad range
of operating systems and hardware.


## Contact

[![Gitter](https://img.shields.io/gitter/room/nwjs/nw.js.svg)](https://gitter.im/vapory/cpp-vapory)
[![GitHub Issues](https://img.shields.io/github/issues-raw/badges/shields.svg)](https://github.com/vaporyco/cpp-vapory/issues)

- Chat in [cpp-vapory channel on Gitter](https://gitter.im/vapory/cpp-vapory).
- Report bugs, issues or feature requests using [GitHub issues](issues/new).


## Getting Started

The Vapory Documentation site hosts the **[cpp-vapory homepage](http://cpp-vapory.co)**, which
has a Quick Start section.


Operating system | Status
---------------- | ----------
Ubuntu and macOS | [![TravisCI](https://img.shields.io/travis/vapory/cpp-vapory/develop.svg)](https://travis-ci.org/vapory/cpp-vapory)
Windows          | [![AppVeyor](https://img.shields.io/appveyor/ci/vapory/cpp-vapory/develop.svg)](https://ci.appveyor.com/project/vapory/cpp-vapory)


## Building from source

### Get the source code

Git and GitHub is used to maintain the source code. Clone the repository by:

```shell
git clone --recursive https://github.com/vaporyco/cpp-vapory.git
cd cpp-vapory
```

The `--recursive` option is important. It orders git to clone additional 
submodules which are required to build the project.
If you missed it you can correct your mistake with command 
`git submodule update --init`.

### Install CMake

CMake is used to control the build configuration of the project. Quite recent 
version of CMake is required 
(at the time of writing [3.4.3 is the minimum](CMakeLists.txt#L25)).
We recommend installing CMake by downloading and unpacking the binary 
distribution  of the latest version available on the 
[**CMake download page**](https://cmake.org/download/).

The CMake package available in your operating system can also be installed
and used if it meets the minimum version requirement.

> **Alternative mvapod**
>
> The repository contains the
[scripts/install_cmake.sh](scripts/install_cmake.sh) script that downloads 
> a fixed version of CMake and unpacks it to the given directory prefix. 
> Example usage: `scripts/install_cmake.sh --prefix /usr/local`.

### Install dependencies (Linux, macOS)

The following *libraries* are required to be installed in the system in their
development variant:

- leveldb

They usually can be installed using system-specific package manager.
Examples for some systems:

Operating system | Installation command
---------------- | --------------------
Debian-based     | `sudo apt-get install libleveldb-dev`
RedHat-based     | `dnf install leveldb-devel`
macOS            | `brew install leveldb`


We also support a "one-button" shell script 
[scripts/install_deps.sh](scripts/install_deps.sh)
which attempts to aggregate dependencies installation instructions for Unix-like
operating systems. It identifies your distro and installs the external packages.
Supporting the script is non-trivial task so please [inform us](#contact)
if it does not work for your use-case.

### Install dependencies (Windows)

We provide prebuilt dependencies required to build the project. Download them
with the [scripts/install_deps.bat](scripts/install_deps.bat) script.

```shell
scripts/install_deps.bat
```

### Build

Configure the project build with the following command. It will create the 
`build` directory with the configuration.

```shell
mkdir build; cd build  # Create a build directory.
cmake ..               # Configure the project.
cmake --build .        # Build all default targets.
```

On **Windows** Visual Studio 2015 is required. You should generate Visual Studio 
solution file (.sln) for 64-bit architecture by adding 
`-G "Visual Studio 14 2015 Win64"` argument to the CMake configure command.
After configuration is completed the `cpp-vapory.sln` can be found in the
`build` directory.

```shell
cmake .. -G "Visual Studio 14 2015 Win64"
```

## Contributing

[![Contributors](https://img.shields.io/github/contributors/vapory/cpp-vapory.svg)](https://github.com/vaporyco/cpp-vapory/graphs/contributors)
[![Gitter](https://img.shields.io/gitter/room/nwjs/nw.js.svg)](https://gitter.im/vapory/cpp-vapory)
[![up-for-grabs](https://img.shields.io/github/issues-raw/vapory/cpp-vapory/up-for-grabs.svg)](https://github.com/vaporyco/cpp-vapory/labels/up-for-grabs)

The current codebase is the work of many, many hands, with nearly 100
[individual contributors](https://github.com/vaporyco/cpp-vapory/graphs/contributors) over the course of its development.

Our day-to-day development chat happens on the
[cpp-vapory](https://gitter.im/vapory/cpp-vapory) Gitter channel.

All contributions are welcome! We try to keep a list of tasks that are suitable
for newcomers under the tag 
[up-for-grabs](https://github.com/vaporyco/cpp-vapory/labels/up-for-grabs).
If you have any questions, please just ask.

Please read [CONTRIBUTING](CONTRIBUTING.md) and [CODING_STYLE](CODING_STYLE.md) 
thoroughly before making alterations to the code base.

All development goes in develop branch.


## Mining

This project is **not suitable for Vapory mining**. The support for GPU mining 
has been dropped some time ago including the vapminer tool. Use the vapminer tool from https://github.com/vapory-mining/vapminer.

## Testing

To run the tests, make sure you clone https://github.com/vaporyco/tests and point the environment variable
`VAPORY_TEST_PATH` to that path.

## Documentation

- [Internal documentation for developers](doc/index.rst).
- [Outdated documentation for end users](http://www.vapdocs.org/en/latest/vapory-clients/cpp-vapory/).


## License

[![License](https://img.shields.io/github/license/vapory/cpp-vapory.svg)](LICENSE)

All contributions are made under the [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0.en.html). See [LICENSE](LICENSE).
