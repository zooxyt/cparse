MAKE = make
AR = ar
AR_FLAGS = -cr
CP = cp
RM = rm -rf
MKDIR = mkdir -p
TARGET = libcparse.a
SRCS = $(wildcard ./src/*.c)
HDRS = $(wildcard ./src/*.h)
OBJS = $(patsubst ./src/%.c,./src/.build/%.o,$(wildcard ./src/*.c))

default: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(MAKE) -C ./src
	$(AR) $(AR_FLAGS) -o $(TARGET) $(OBJS)

clean:
	$(MAKE) -C ./src clean
	$(RM) $(TARGET)

install:
	$(CP) $(TARGET) /usr/lib/
	$(MKDIR) /usr/include/cparse/
	$(CP) ./src/*.h /usr/include/cparse

uninstall:
	$(RM) /usr/lib/$(TARGET)
	$(RM) /usr/include/cparse/


