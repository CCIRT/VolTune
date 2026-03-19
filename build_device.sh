#!/bin/bash
TARGET_VIVADO_VERSION=2022.1

# Target List
# BER/Power
ber=(
  impl_tx_10g
  impl_rx_10g
  impl_loopback_10g
  impl_tx_7p5g
  impl_rx_7p5g
  impl_loopback_7p5g
  impl_tx_5g
  impl_rx_5g
  impl_loopback_5g
  impl_tx_2p5g
  impl_rx_2p5g
  impl_loopback_2p5g
  bit_tx_10g_swpmbus_vitis
  bit_rx_10g_swpmbus_vitis
  bit_loopback_10g_swpmbus_vitis
  bit_tx_7p5g_swpmbus_vitis
  bit_rx_7p5g_swpmbus_vitis
  bit_loopback_7p5g_swpmbus_vitis
  bit_tx_5g_swpmbus_vitis
  bit_rx_5g_swpmbus_vitis
  bit_loopback_5g_swpmbus_vitis
  bit_tx_2p5g_swpmbus_vitis
  bit_rx_2p5g_swpmbus_vitis
  bit_loopback_2p5g_swpmbus_vitis
)

# Voltage
voltage=(
  impl_voltage_n000
  impl_voltage_tx_10g
  impl_voltage_rx_10g
  impl_voltage_loopback_10g
  impl_voltage_tx_7p5g
  impl_voltage_rx_7p5g
  impl_voltage_loopback_7p5g
  impl_voltage_tx_5g
  impl_voltage_rx_5g
  impl_voltage_loopback_5g
  impl_voltage_tx_2p5g
  impl_voltage_rx_2p5g
  impl_voltage_loopback_2p5g
  bit_voltage_n000_swpmbus_vitis
  bit_voltage_tx_10g_swpmbus_vitis
  bit_voltage_rx_10g_swpmbus_vitis
  bit_voltage_loopback_10g_swpmbus_vitis
  bit_voltage_tx_7p5g_swpmbus_vitis
  bit_voltage_rx_7p5g_swpmbus_vitis
  bit_voltage_loopback_7p5g_swpmbus_vitis
  bit_voltage_tx_5g_swpmbus_vitis
  bit_voltage_rx_5g_swpmbus_vitis
  bit_voltage_loopback_5g_swpmbus_vitis
  bit_voltage_tx_2p5g_swpmbus_vitis
  bit_voltage_rx_2p5g_swpmbus_vitis
  bit_voltage_loopback_2p5g_swpmbus_vitis
)


#---------------------------------------------------
ber_targets="${ber[@]}"
vol_targets="${voltage[@]}"
all_targets="$ber_targets $vol_targets"

_usage() {
    echo -e "Usage: $0 [-a] [-b <Dir>] [-h] [-V <Vivado Root Dir>] [-H <Vitis HLS Root Dir>] <make targets>..."
    echo -e " Options:"
    echo -e "\t-a, --mgtavcc  : support MGTAVCC lane only"
    echo -e "\t-b, --build    : build directory"
    echo -e "\t-V, --vivado   : Vivado Root Directory"
    echo -e "\t-H, --vitis-hls: Vitis HLS Root Directory"
    echo -e "\t-I, --vitis    : Vitis Root Directory"
    echo -e "\t-h, --help     : show this help"
    echo ""
    echo -e "\t<make targets>: Make target list"
    echo -e "\t                Special targets"
    echo -e "\t                       all: build all voltage and ber design"
    echo -e "\t                   voltage: build all voltage design"
    echo -e "\t                       ber: build all ber/power design"
}
# -------------------------------------------------
set -e
build=build-device
mgtavcc=""
evmmode=""
vroot=""
hroot=""
vitisroot=""
make_target=""

for OPT in "$@"
do
  case $OPT in
    -h | --help)
      _usage
      exit
      ;;

    -a | --mgtavcc)
      mgtavcc="-DMGTAVCC=ON"
      shift 1
      ;;

    -b | --build)
      build="$1"
      shift 2
      ;;

    # Hidden option. (for Fixstars)
    -e | --evm)
      evmmode="-DEVM=ON"
      build="build-evm"
      shift 1
      ;;

    -V | --vivado)
      vroot="$1"
      shift 2
      ;;

    -H | --vitis-hls)
      hroot="$1"
      shift 2
      ;;

    -I | --vitis)
      vitisroot="$1"
      shift 2
      ;;

    *)
      make_target="$make_target $1"
      shift 1
      ;;
  esac
done

# -------------------------------------------------
cd `dirname $0`
if [ ! -d $build ]; then
  mkdir -p $build
fi
cd $build
echo $build
if [ ! -f CMakeCache.txt ]; then
  #
  # Run CMake
  #

  # find default vivado path
  if [ ! -z $vroot ]; then
    if [ ! -d $vroot ]; then
      echo "Vivado directory: $vroot does not exist."
      exit 1
    fi
    if [ $(basename $vroot) != $TARGET_VIVADO_VERSION ]; then
      echo "Vivado directory: $vroot is not Vivado $TARGET_VIVADO_VERSION ."
      exit 1
    fi
  fi

  if [ ! -z $hroot ]; then
    if [ ! -d $hroot ]; then
      echo "Vitis HLS directory: $hroot does not exist."
      exit 1
    fi
    if [ $(basename $hroot) != $TARGET_VIVADO_VERSION ]; then
      echo "Vitis HLS directory: $hroot is not Vitis HLS $TARGET_VIVADO_VERSION ."
      exit 1
    fi
  fi

  if [ -z $vroot ] && [ ! -z $XILINX_VIVADO ]; then
    if [ $(basename $XILINX_VIVADO) != $TARGET_VIVADO_VERSION ]; then
      vroot=$XILINX_VIVADO
    fi
  fi
  if [ -z $hroot ] && [ ! -z $XILINX_HLS ]; then
    if [ $(basename $XILINX_HLS) != $TARGET_VIVADO_VERSION ]; then
      hroot=$XILINX_HLS
    fi
  fi

  if [ -z $vroot ]; then
    if [ -d /opt/Xilinx/Vivado/$TARGET_VIVADO_VERSION ]; then
      vroot=/opt/Xilinx/Vivado/$TARGET_VIVADO_VERSION
    elif [ -d /tools/Xilinx/Vivado/$TARGET_VIVADO_VERSION ]; then
      vroot=/tools/Xilinx/Vivado/$TARGET_VIVADO_VERSION
    else
      echo "Vivado $TARGET_VIVADO_VERSION is not found."
    fi
  fi

  if [ -z $hroot ]; then
    if [ -d /opt/Xilinx/Vitis_HLS/$TARGET_VIVADO_VERSION ]; then
      hroot=/opt/Xilinx/Vitis_HLS/$TARGET_VIVADO_VERSION
    elif [ -d /tools/Xilinx/Vitis_HLS/$TARGET_VIVADO_VERSION ]; then
      hroot=/tools/Xilinx/Vitis_HLS/$TARGET_VIVADO_VERSION
    else
      echo "Vitis HLS $TARGET_VIVADO_VERSION is not found."
    fi
  fi

  if [ -z $vitisroot ]; then
    if [ -d /opt/Xilinx/Vitis/$TARGET_VIVADO_VERSION ]; then
      vitisroot=/opt/Xilinx/Vitis/$TARGET_VIVADO_VERSION
    elif [ -d /tools/Xilinx/Vitis/$TARGET_VIVADO_VERSION ]; then
      vitisroot=/tools/Xilinx/Vitis/$TARGET_VIVADO_VERSION
    else
      echo "Vitis $TARGET_VIVADO_VERSION is not found."
    fi
  fi

  echo $vitisroot

  cmake ../device \
    $mgtavcc $evmmode \
    -DVIVADO_ROOT=$vroot \
    -DVITIS_HLS_ROOT=$hroot \
    -DVITIS_ROOT=$vitisroot
fi

# trim space
make_target=$(echo $make_target)

if [ ! -z $make_target ]; then
  if [ "$make_target" = "all" ]; then
    make $all_targets
  elif [ "$make_target" = "voltage" ]; then
    make $vol_targets
  elif [ "$make_target" = "power" ] || [ "$make_target" = "ber" ]; then
    make $ber_targets
  else
    make $make_target
  fi
else
  echo ""
  echo "  Please work in $(pwd)"
  echo ""
  echo "   $ cd $(pwd)"
  echo "   $ make *target*"
  echo ""
fi
