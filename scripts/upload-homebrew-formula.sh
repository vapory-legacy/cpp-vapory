#!/usr/bin/env bash
# author: Lefteris Karapetsas <lefteris@refu.co>
#
# Just upload the generated .rb file to homebrew vapory

echo ">>> Starting the script to upload .rb file to homebrew vapory"
rm -rf homebrew-vapory
git clone git@github.com:vapory/homebrew-vapory.git
cp webthree-umbrella/build/cpp-vapory.rb homebrew-vapory
cd homebrew-vapory
git add . -u
git commit -m "update cpp-vapory.rb"
git push origin
cd ..
rm -rf homebrew-vapory
echo ">>> Succesfully uploaded the .rb file to homebrew vapory"
