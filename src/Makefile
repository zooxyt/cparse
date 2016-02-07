CC = gcc
CFLAGS = -Wall -Wextra -g -MMD -O0 -fPIC
INCLUDES = -I./
RM = rm -rf

BUILDPATH = .build
SRCS = $(wildcard *.c)
OBJS = $(addprefix $(BUILDPATH)/, $(notdir $(SRCS:.c=.o)))
DEPS = $(OBJS:.o=.d)

default: $(OBJS)

$(BUILDPATH)/%.o: %.c
	@-mkdir -p $(BUILDPATH)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

all: clean $(OBJS)

clean:
	@$(RM) $(BUILDPATH)/*.o $(BUILDPATH)/*.d
	@$(RM) $(BUILDPATH)

-include $(DEPS)
