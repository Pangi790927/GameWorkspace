SOURCES = imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp \
		imgui_widgets.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))


UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

CXXFLAGS = -g -Wall -Wformat

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: libimgui.a

libimgui.a: $(OBJS)
	ar rvs $@ $^

clean:
	rm -f libimgui.a $(OBJS)
