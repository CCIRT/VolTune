set host localhost
set port 33129
set target_rx 3
set target_tx 6

set test_pattern 2
# 0: All Zero
# 1: All Hi
# 2: Count Up

set test_size 10000000000
puts "Test size (aligned to 8 Byte)"
puts "      [expr (($test_size + 7) / 8) * 8] \[Byte\]"
puts ""

set test_voltage_rx 0x1000
set test_voltage_tx 0x1000

set i2cClockSelect 1
# Select PMBus clock
# 0: 100kHz, 1: 400kHz, 2: 1MHz

# Connect to hw_server
connect -host $host -port $port

# RX side
target $target_rx
## test_pattern
mwr 0x00001000 $test_pattern
## test_size
mwr 0x00001010 [expr $test_size & 0xFFFFFFFF]
mwr 0x00001014 [expr $test_size >> 32]
## test_voltage
mwr 0x00000020 $test_voltage_rx
## i2cClockSelect
mwr 0x00000010 $i2cClockSelect
## test start
mwr 0x00010000 0x0
mwr 0x00010000 0x1
mwr 0x00010000 0x0

# TX side
target $target_tx
## test_pattern
mwr 0x00001000 $test_pattern
## test_size
mwr 0x00001010 [expr $test_size & 0xFFFFFFFF]
mwr 0x00001014 [expr $test_size >> 32]
## test_voltage
mwr 0x00000020 $test_voltage_tx
## i2cClockSelect
mwr 0x00000010 $i2cClockSelect
## test start
mwr 0x00010000 0x0
mwr 0x00010000 0x1
mwr 0x00010000 0x0

# Wait for test ending
target $target_rx
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
target $target_rx
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

# RX side
target $target_rx
## Check Result
puts "RX Result"
puts [mrd 0x00002000]
puts "RX Min Power"
puts [mrd 0x00002030]
puts "RX Max Power"
puts [mrd 0x00002040]
puts "RX Sum Power   (lower 32 bit)"
puts [mrd 0x00002050]
puts "RX Sum Power   (upper 32 bit)"
puts [mrd 0x00002054]
puts "RX Sum Power 2 (lower 32 bit)"
puts [mrd 0x00002060]
puts "RX Sum Power 2 (upper 32 bit)"
puts [mrd 0x00002064]
puts "RX Sum Count"
puts [mrd 0x00002070]

# TX side
target $target_tx
## Check Result
puts "TX Result"
puts [mrd 0x00002000]
puts "TX Min Power"
puts [mrd 0x00002030]
puts "TX Max Power"
puts [mrd 0x00002040]
puts "TX Sum Power   (lower 32 bit)"
puts [mrd 0x00002050]
puts "TX Sum Power   (upper 32 bit)"
puts [mrd 0x00002054]
puts "TX Sum Power 2 (lower 32 bit)"
puts [mrd 0x00002060]
puts "TX Sum Power 2 (upper 32 bit)"
puts [mrd 0x00002064]
puts "TX Sum Count"
puts [mrd 0x00002070]
