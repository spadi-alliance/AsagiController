BINDIR		= ../bin
CXXFLAGS	+= -Wall -g -MMD -std=c++1y

CORELIB_DIR	= ../../hul_software.git/CoreLib
COMMON_DIR	= ../../hul_software.git/Common
CXXFLAGS 	+= -I. -I$(CORELIB_DIR)/src -I$(COMMON_DIR)/src
LDFLAGS		+= -L$(CORELIB_DIR)/lib -lhul

SRCALL		= $(wildcard *.cc)
DEPENDS		= $(SRCALL:.cc=.d)

SRC		= $(filter-out %main.cc, $(SRCALL))
OBJ		= $(SRC:.cc=.o)

TARGET		= set_asic_register

echo		:= /bin/echo -e
mkdir		:= mkdir -p

.PHONY: clean
.PRECIOUS: %.o

all: $(TARGET)

%: %_main.o $(OBJ)
	@if [ ! -d $(BINDIR) ]; then \
	  $(echo) "mkdir -p $(BINDIR)"; $(mkdir) $(BINDIR); \
	fi
	$(CXX) -o $(BINDIR)/$@ $< $(OBJ) $(LDFLAGS)

clean:
	$(RM) $(BINDIR)/* *.o *.d *~

show:
	@ $(echo) "CXXFLAGS = $(CXXFLAGS)"
	@ $(echo) "LDFLAGS  = $(LDFLAGS)"
	@ $(echo) "SRCALL   = $(SRCALL)"
	@ $(echo) "DEPENDS  = $(DEPENDS)"
	@ $(echo) "SRC      = $(SRC)"
	@ $(echo) "OBJ      = $(OBJ)"

-include $(DEPENDS)
