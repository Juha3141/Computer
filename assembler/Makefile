SOURCESFOLDER = Sources
HEADERSFOLDER = Headers
BINARYFOLDER = Binary

CCOPTIONS = -m64 -W -Wall -g -I$(HEADERSFOLDER)

TARGET = assembler

clean:
	rm -rf $(TARGET)
	rm -rf $(BINARYFOLDER)/*.*

all: $(notdir $(wildcard $(SOURCESFOLDER)/*.cpp))
	g++ $(CCOPTIONS) $(patsubst %.cpp,%.obj,$(subst $(SOURCESFOLDER),$(BINARYFOLDER),$(wildcard $(SOURCESFOLDER)/*.cpp))) -o $(TARGET)

run:
	./$(TARGET) test.asm

%.cpp:
	g++ $(CCOPTIONS) -c $(SOURCESFOLDER)/$@ -o $(BINARYFOLDER)/$(subst .cpp,.obj,$@) $(CCOPTIONS)
