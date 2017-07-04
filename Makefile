NAME = objviewer

CC = g++

CFLAGS = -Wall -Wextra -Werror -O0 -g -march=native #-flto=8

INCLUDES_PATH = include -I lib/glfw/include -I lib/glad/include -I lib/librender/include -I lib/glm -I lib/libformat/include

SRCS_PATH = src/

SRCS_NAME = Main.cpp \
	ObjParser.cpp \
	MtlParser.cpp \
	FpsManager.cpp \

SRCS = $(addprefix $(SRCS_PATH), $(SRCS_NAME))

OBJS_PATH = obj/

OBJS_NAME = $(SRCS_NAME:.cpp=.o)

OBJS = $(addprefix $(OBJS_PATH), $(OBJS_NAME))

LIBRARY = -L lib/glfw/src -l:libglfw3.a -L lib/glad -l:libglad.a -L lib/librender -l:librender.a -L lib/libformat -l:libformat.a -Llib/libpng/.libs -l:libpng16.a -Llib/zlib -l:libz.a -lGL -lX11 -lXrandr -lXinerama -lXcursor -lXxf86vm -ldl -lrt -lm -lpthread

all: odir $(NAME)

$(NAME): $(OBJS)
	@echo " - Making $(NAME)"
	@$(CC) $(CFLAGS) -o $(NAME) $^ $(LIBRARY)

$(OBJS_PATH)%.o: $(SRCS_PATH)%.cpp
	@echo " - Compiling $<"
	@$(CC) $(CFLAGS) -o $@ -c $< -I$(INCLUDES_PATH)

odir:
	@mkdir -p $(OBJS_PATH)

clean:
	@echo " - Cleaning objs"
	@rm -f $(OBJS)

fclean: clean
	@echo " - Cleaning executable"
	@rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re odir
