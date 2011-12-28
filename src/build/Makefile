.PHONY: all all-before all-after clean clean-custom

all: all-before lib BRIDGE STP CLI PORT GDD  RSTP VLAN DHCP IPV4 VRRP LIB PLATFORM SWITCH all-after

include make.h

lib:
	mkdir -p obj
BRIDGE:
	make -C $(CODE_DIR)/bridge
STP:
	make -C $(CODE_DIR)/bridge/stp
PLATFORM:
	make -C $(CODE_DIR)/platform
CLI:
	make -C $(CODE_DIR)/cli
PORT:
	make -C $(CODE_DIR)/ifmgmt
GDD:
	make -C $(CODE_DIR)/gdd
VLAN:
	make -C $(CODE_DIR)/vlan

RSTP:
	make -C $(CODE_DIR)/bridge/rstp
IPV4:
	make -C $(CODE_DIR)/ipv4

DHCP:
	make -C $(CODE_DIR)/dhcp

LIB:
	make -C $(CODE_DIR)/lib
VRRP:
	make -C $(CODE_DIR)/vrrp


SWITCH:  lib BRIDGE STP PLATFORM CLI PORT GDD  RSTP VLAN IPV4 VRRP
	gcc -o Switch.exe obj/*.o -lpthread -lrt

clean: clean-custom
	make clean -C $(CODE_DIR)/bridge
	make clean -C $(CODE_DIR)/bridge/stp
	make clean -C $(CODE_DIR)/cli
	make clean -C $(CODE_DIR)/ifmgmt
	make clean -C $(CODE_DIR)/gdd
	make clean -C $(CODE_DIR)/bridge/rstp
	make clean -C $(CODE_DIR)/vlan
	make clean -C $(CODE_DIR)/lib
	make clean -C $(CODE_DIR)/dhcp
	make clean -C $(CODE_DIR)/ipv4
	make clean -C $(CODE_DIR)/platform
	make clean -C $(CODE_DIR)/vrrp
