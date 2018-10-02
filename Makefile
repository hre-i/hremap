#
#

TARGET  = hremap
SOURCES = keyboard.cpp conv.cpp hremap.cpp main.cpp
DEPFILE = Dependencies

OBJS = $(addsuffix .o, $(basename $(SOURCES)))

D ?= -DDEBUG
CXXFLAGS += -Wall -O2 -g $D
LDFLAGS += -Wall -O2 -g
LOADLIBES += #-lboost_regex

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPFILE) *~

depend:
	rm -f $(DEPFILE)
	g++ $(CXXFLAGS) -MM $(SOURCES) > $(DEPFILE)

install:
	install -m 755 -s $(TARGET) /usr/local/bin/

setup:
	(cd systemd && sudo ./setup.sh)

.PHONY: all clean install tarball depend test stop

$(TARGET): $(OBJS)
	g++ $(LDFLAGS) -o $@ $^ $(LOADLIBES)

%.o: %.cc
	g++ $(CXXFLAGS) -c -o $@ $<

-include $(DEPFILE)
