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

install-scripts:
	cp hremap.sh /usr/local/etc/hremap.sh
	chmod 755 /usr/local/etc/hremap.sh
	[ -d /usr/lib/systemd/system ] || mkdir /usr/lib/systemd/system
	cp hremap.service /usr/lib/systemd/system/hremap.service

setup:
	[ -d /usr/lib/systemd/system ] || mkdir /usr/lib/systemd/system
	[ -f /usr/lib/systemd/system/hremap.service ] || cp systemd/hremap.service /usr/lib/systemd/system/hremap.service
	systemctl enable hremap
	systemctl start hremap

#setup:
#	(cd systemd && sudo ./setup.sh)

.PHONY: all clean install tarball depend test stop

$(TARGET): $(OBJS)
	g++ $(LDFLAGS) -o $@ $^ $(LOADLIBES)

%.o: %.cc
	g++ $(CXXFLAGS) -c -o $@ $<

-include $(DEPFILE)
