#makefile
SOURCES = $(PLAIN_SOURCES) $(LIB_SOURCES)
PLAIN_SOURCES = main.cpp
LIB_SOURCES = Requests.cpp
TARG_LIB = libPro.so

CXX = g++
CXXFLAGS += -pipe -march=core2 -lfcgi -lfcgi++
LDFLAGS += 

%.o: %.cpp
	$(CXX) -MMD -shared -fpic -c -o $@ $< $(CXXFLAGS)

run:  $(PLAIN_SOURCES:%.c=%.O) $(TARG_LIB)
	sudo $(CXX) -o /var/www/$@ $^ $(CXXFLAGS) $(LDFLAGS) -L. -lPro -Wl,-rpath=.

$(TARG_LIB): $(LIB_SOURCES:%.cpp=%.o)
	$(CXX) -o $(TARG_LIB) $^ -shared -fpic $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(SOURCES:%.cpp=%.d) *.o *.so

-include $(SOURCES:%.cpp=%.d)
