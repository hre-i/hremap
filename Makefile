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
	sudo install -m 755 -s $(TARGET) /usr/local/bin/
	[ -f /usr/local/etc/hremap.sh ] || sudo cp systemd/hremap.sh /usr/local/etc/hremap.sh
	sudo chmod 755 /usr/local/etc/hremap.sh
	[ -d /usr/lib/systemd/system ] || sudo mkdir /usr/lib/systemd/system
	[ -f /usr/lib/systemd/system/hremap.service ] || sudo cp systemd/hremap.service /usr/lib/systemd/system/hremap.service

setup:
	systemctl enable hremap
	systemctl start hremap

.PHONY: all clean install tarball depend test stop

$(TARGET): $(OBJS)
	g++ $(LDFLAGS) -o $@ $^ $(LOADLIBES)

%.o: %.cc
	g++ $(CXXFLAGS) -c -o $@ $<

-include $(DEPFILE)
