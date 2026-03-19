#!/bin/bash
set -eu

bitstream=$(realpath $1)
if [ ! -d $bitstream ]; then
  echo "Bitstream direcotry ($bitstream) is not found."
  exit 1
fi

cbwrite='/c/Program Files (x86)/Skyworks/ClockBuilder Pro/Bin/CBProDeviceWrite.exe'

if [ ! -f "$cbwrite" ]; then
  echo "CBProDeviceWrite.exe ($cbwrite) not found"
  exit 1
fi

cd `dirname $0`
edir=$(pwd)

cd ..

if [ ! -d build ]; then
  ./msys_build.sh
fi
mkdir -p result

exe=build/bin/voltage-measure.exe
repeat=1
echo "[INFO] Set Clock Board: 125MHz"
### "$cbwrite" --registers $edir/125_000MHz.reg.txt
"$cbwrite" --project $edir/125_000MHz.slabtimeproj
echo "[INFO] Run Test (2.5Gbps, 5.0Gbps, 10.0Gbps)"

"$exe" -y -b $bitstream -o result/voltage.csv -s 400k -r $repeat $edir/sample_voltage.csv



# echo "[INFO] Set Clock Board: 117.188MHz"
"$cbwrite" --registers $edir/117_188MHz.reg.txt
### "$cbwrite" --project $edir/117_188MHz.slabtimeproj

echo "[INFO] Run Test (7.5Gbps)"

"$exe" -y \
  -b $bitstream \
  -o result/voltage_7p5.csv \
  -O result/V_7p5 \
  -s 400k \
  -r $repeat \
  -c 117.188 \
  $edir/sample_voltage_7p5.csv

echo "success"
