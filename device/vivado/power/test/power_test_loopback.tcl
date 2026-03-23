# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

set host localhost
set port 3121
set target 3

set test_pattern 2
# 0: All Zero
# 1: All Hi
# 2: Count Up

set test_size 10000000000
puts "Test size (aligned to 8 Byte)"
puts "      [expr (($test_size + 7) / 8) * 8] \[Byte\]"
puts ""

set test_voltage 0x1000

set i2cClockSelect 1
# Select PMBus clock
# 0: 100kHz, 1: 400kHz, 2: 1MHz

# Connect to hw_server
connect -host $host -port $port
target $target

## test_pattern
mwr 0x00001000 $test_pattern
## test_size
mwr 0x00001010 [expr $test_size & 0xFFFFFFFF]
mwr 0x00001014 [expr $test_size >> 32]
## test_voltage
mwr 0x00000020 $test_voltage
## i2cClockSelect
mwr 0x00000010 $i2cClockSelect
## test start
mwr 0x00010000 0x0
mwr 0x00010000 0x1
mwr 0x00010000 0x0

# Wait for test ending
puts "Wait for test ending..."
while {1} {
    after 1000;
    set status [mrd 0x00010000]
    puts $status
    set val [lindex [split $status " "] 6]
    if {$val == "00000004"} {
        break
    }
}

# Read error bit count & latency
puts "Recv data size \[Byte\] (lower 32 bit)"
puts [mrd 0x00002080]
puts "Recv data size \[Byte\] (upper 32 bit)"
puts [mrd 0x00002084]
puts "Error bit count (lower 32 bit)"
puts [mrd 0x00002010]
puts "Error bit count (upper 32 bit)"
puts [mrd 0x00002014]
puts "Latency"
puts [mrd 0x00002020]

## Check Result
puts "Result"
puts [mrd 0x00002000]
puts "Min Power"
puts [mrd 0x00002030]
puts "Max Power"
puts [mrd 0x00002040]
puts "Sum Power   (lower 32 bit)"
puts [mrd 0x00002050]
puts "Sum Power   (upper 32 bit)"
puts [mrd 0x00002054]
puts "Sum Power 2 (lower 32 bit)"
puts [mrd 0x00002060]
puts "Sum Power 2 (upper 32 bit)"
puts [mrd 0x00002064]
puts "Sum Count"
puts [mrd 0x00002070]
