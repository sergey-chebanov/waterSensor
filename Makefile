#############################################################################
#
# Makefile for librf24 examples on Raspberry Pi
#
# License: GPL (General Public License)
# Author:  gnulnulf <arco@appeltaart.mine.nu>
# Date:    2013/02/07 (version 1.0)
#
# Description:
# ------------
# use make all and make install to install the examples
# You can change the install directory by editing the prefix line
#
prefix := /usr/local

# The recommended compiler flags for the Raspberry Pi
#CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
CCFLAGS=-g -std=c++11

# define all programs
#PROGRAMS = scanner pingtest gettingstarted
PROGRAMS = read_on_interruption
#PROGRAMS = read_on_interruption gettingstarted gettingstarted_call_response transfer pingpair_dyn
SOURCES = read_on_interruption.cpp DB.cpp

all: ${PROGRAMS}

DB: DB.cpp
	g++ ${CCFLAGS} -Wall $@.cpp -lboost_date_time -lsqlite3 -o $@

${PROGRAMS}: ${SOURCES}
	g++ ${CCFLAGS} -Wall ${SOURCES} -lrf24 -lbcm2835 -lwiringPi -lpthread -lboost_date_time -lsqlite3 -o $@

clean:
	rm -rf $(PROGRAMS)

install: all
	test -d $(prefix) || mkdir $(prefix)
	test -d $(prefix)/bin || mkdir $(prefix)/bin
	for prog in $(PROGRAMS); do \
	  install -m 0755 $$prog $(prefix)/bin; \
	done

.PHONY: install
