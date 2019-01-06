#!/usr/bin/env bash

workdir=$(pwd)

#Clean the previous build
rm -rf cpp-vapory 2>/dev/null || true
rm -rf tests 2>/dev/null || true
exec &> $workdir/buildlog.txt
export VAPORY_TEST_PATH="$(pwd)/tests"

#Clonning Repositories
echo "Cloning Repositories"
git clone --depth 1 --single-branch https://github.com/vaporyco/tests.git
git clone --recursive --depth 1 --single-branch https://github.com/vaporyco/cpp-vapory.git
cd tests
testHead=$(git rev-parse HEAD)
cd ..
cd tests/RPCTests
npm install
cd $workdir/cpp-vapory
cppHead=$(git rev-parse HEAD)

#Prepare test results
mkdir build
cd build
echo "Make cpp-vapory develop:"
cmake .. -DCOVERAGE=On
make -j8
echo "Running all tests:"
echo "cpp-vapory repository at commit $cppHead"
echo "tests repository at commit $testHead"
exec 2> $workdir/testlog.txt
timestart=$(date +%s.%N)
test/testvap -- --all --exectimelog
cd $workdir/tests/RPCTests
echo "#--------------RPC TESTS--------------"
node main.js $workdir/cpp-vapory/build/vap/vap
timeend=$(date +%s.%N)
date=$(date +%Y-%m-%d)

# Upload coverage report
if [ -z "$CODECOV_TOKEN" ]; then
    echo "Warning! CODECOV_TOKEN not set. See https://codecov.io/gh/vapory/cpp-vapory/settings."
else
    bash <(curl -s https://codecov.io/bash) -n alltests -b "$date" -F alltests -a '>/dev/null 2>&1'
fi

# Make report
cd $workdir
(
echo "REPORT"
exectime=$(echo "$timeend - $timestart" | bc)
echo "Test execution time: $exectime s"
echo "Coverage: https://codecov.io/gh/vapory/cpp-vapory/commit/$cppHead"
cat testlog.txt
cat buildlog.txt
) > report.txt

# Send mail
RECIPIENTS="dimitry@vapory.co pawel@vapory.co chris@vapory.co andrei@vapory.co yoichi@vapory.co"
mail < report.txt -s "cpp-vapory alltests $date" $RECIPIENTS
