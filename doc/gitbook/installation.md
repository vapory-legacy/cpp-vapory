# Installation

Installation is a different process dependent on which platform you run. At present, cpp-vapory supports three platforms: Ubuntu, Mac OS X and Windows.

For installing the desktop tools on Windows and Mac, just grab the latest release. (For Windows you might also need [this](http://www.microsoft.com/en-US/download/details.aspx?id=40784).)

For installing on Ubuntu or Homebrew, instructions follow.

# Installing on Ubuntu 14.04 and later (64-bit)

**Warning: The `vapory-qt` PPA will upgrade your system-wide Qt5 installation, from 5.2 on Trusty and 5.3 on Utopic, to 5.5.**

For the latest stable version:
```
sudo add-apt-repository ppa:vapory/vapory-qt
sudo add-apt-repository ppa:vapory/vapory
sudo apt-get update
sudo apt-get install cpp-vapory
```

If you want to use the cutting edge developer version:
```
sudo add-apt-repository ppa:vapory/vapory-qt
sudo add-apt-repository ppa:vapory/vapory
sudo add-apt-repository ppa:vapory/vapory-dev
sudo apt-get update
sudo apt-get install cpp-vapory
```

# Installing on OS X and Homebrew

If you want the full suite of CLI tools, include `vap` and `vapminer`, you'll need [Homebrew](brew.sh).

Once you've got Homebrew installed, tap the vapory brew:
```
brew tap vapory/vapory
```

Then, for the stable version:
```
brew install cpp-vapory
brew linkapps cpp-vapory
```

or, for the latest cutting edge developer version:
```
brew reinstall cpp-vapory --devel
brew linkapps cpp-vapory
```

For options and patches, see: https://github.com/vaporyco/homebrew-vapory
