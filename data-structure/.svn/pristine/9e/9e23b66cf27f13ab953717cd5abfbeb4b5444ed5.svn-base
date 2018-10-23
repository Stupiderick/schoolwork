EXENAME1 = testimage
EXENAME2 = testscene
OBJS1 = testimage.o png.o rgbapixel.o image.o
OBJS2 = testscene.o png.o rgbapixel.o image.o scene.o

CXX = clang++
CXXFLAGS = -std=c++1y -stdlib=libc++ -c -g -O0 -Wall -Wextra -pedantic
LD = clang++
LDFLAGS = -std=c++1y -stdlib=libc++ -lpng -lc++abi -lpthread

all : $(EXENAME1) $(EXENAME2)

testimage : $(EXENAME1)

testscene : $(EXENAME2)

$(EXENAME1) : $(OBJS1)
	$(LD) $(OBJS1) $(LDFLAGS) -o $(EXENAME1)

$(EXENAME2) : $(OBJS2)
	$(LD) $(OBJS2) $(LDFLAGS) -o $(EXENAME2)

testimage.o : testimage.cpp png.h rgbapixel.h image.h
	$(CXX) $(CXXFLAGS) testimage.cpp

testscene.o : testscene.cpp png.h rgbapixel.h image.h scene.h
	$(CXX) $(CXXFLAGS) testscene.cpp

png.o : png.cpp png.h rgbapixel.h image.h
	$(CXX) $(CXXFLAGS) png.cpp

rgbapixel.o : rgbapixel.cpp rgbapixel.h
	$(CXX) $(CXXFLAGS) rgbapixel.cpp

image.o : image.cpp image.h png.h rgbapixel.h
	$(CXX) $(CXXFLAGS) image.cpp

scene.o : scene.h scene.cpp image.h png.h rgbapixel.h
	$(CXX) $(CXXFLAGS) scene.cpp

clean :
	-rm -rf *.o $(EXENAME1) $(EXENAME2)
