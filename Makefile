PROJECT = panel
BUILD_DIR = bin

SHARED_DIR =
CFILES = panel.c

# TODO - you will need to edit these two lines!
DEVICE=stm32f303ret6
OOCD_FILE = board/st_nucleo_f3.cfg

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
# OPENCM3_DIR=../../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk