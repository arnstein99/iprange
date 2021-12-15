# Tuning
CXXFLAGS += \
    -O
    # -g -O0

SUFFIXES += .d
CXXFLAGS += \
    -std=c++2a -Wall -Wpedantic -Wextra -Wduplicated-cond \
    -Wlogical-op -Wnull-dereference \
    -Wdouble-promotion -Wshadow -Wformat=2 \
    -Wold-style-cast -Wuseless-cast

PROGRAMS = ipar_read ipar_subtract ipar_expand ipar_gap_analyzer \
    ipar_intersect ipar_interactive
SOURCES = \
    ipar_read.cpp ipar_subtract.cpp ipar_expand.cpp ipar_iplist.cpp \
    ipar_gap_analyzer.cpp ipar_common.cpp ipar_intersect.cpp \
    ipar_interactive.cpp
LIB_OBJECTS = ipar_iplist.o ipar_common.o

Q_ = @
ifdef VERBOSE
    Q_ =
endif

DEPFILES:=$(patsubst %.cpp,%.d,$(SOURCES))

all: $(PROGRAMS) $(DEPFILES)

%: %.o $(LIB_OBJECTS)
	@echo Linking $@
	$(Q_)$(LINK.cc) $(LDFLAGS) $@.o $(LIB_OBJECTS) $(LDLIBS) -o $@

%.d: %.cpp
	@echo Computing dependencies $@
	$(Q_)$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst %.cpp,%.o,$<)' $< -MF $@

%.o: %.cpp
	@echo Compiling $@
	$(Q_)$(CXX) $(CXXFLAGS) -c $< -o $@

install: $(PROGRAMS)
	cp $(PROGRAMS) $(HOME)/bin
clean:
	$(RM) *.o *.d $(PROGRAMS)

.PHONY: all install clean

-include $(DEPFILES)
