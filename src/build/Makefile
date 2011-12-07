.PHONY: all all-before all-after clean clean-custom

all: all-before lib BRIDGE STP SIM CLI PORT GDD  RSTP VLAN SWITCH all-after

include make.h

lib:
	mkdir -p obj
BRIDGE:
	make -C $(CODE_DIR)/bridge
STP:
	make -C $(CODE_DIR)/bridge/stp
SIM:
	make -C $(CODE_DIR)/sim
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



SWITCH:  lib BRIDGE STP SIM CLI PORT GDD  RSTP VLAN
	gcc -o Switch.exe obj/*.o -lpthread -lrt

clean: clean-custom
	make clean -C $(CODE_DIR)/sim
	make clean -C $(CODE_DIR)/bridge
	make clean -C $(CODE_DIR)/bridge/stp
	make clean -C $(CODE_DIR)/cli
	make clean -C $(CODE_DIR)/ifmgmt
	make clean -C $(CODE_DIR)/gdd
	make clean -C $(CODE_DIR)/bridge/rstp
	make clean -C $(CODE_DIR)/vlan
