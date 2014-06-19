BUILDDIR := build
PREFIX := bin

CXXFLAGS += -std=c++0x -Wall -Werror -Ilib
LDFLAGS += -lportmidi `sdl2-config --libs`

CXXFLAGS += -g -O0

LIBS := $(shell find lib -name "*.cc")
OBJS := $(LIBS:%.cc=$(BUILDDIR)/%.o)

include tools/prc/Makefile

all: install

install:
	mkdir -p $(PREFIX)
	cp -r $^ $(PREFIX)

$(BUILDDIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $(CXXFLAGS) $<

$(BUILDDIR)/%: %.cc $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS) $(OBJS) $(LDLIBS)

clean:
	rm -rf $(BUILDDIR)