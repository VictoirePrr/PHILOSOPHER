# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vicperri <vicperri@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/11 15:32:31 by vicperri          #+#    #+#              #
#    Updated: 2025/04/30 11:23:47 by vicperri         ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

# Project Name
NAME    = philo

# Compiler and Flags
CC := cc
CPPFLAGS = -MMD -MP
CCFLAGS := -Wall -Wextra -Werror -g3
SRC_DIR := src/
INCLUDES := include/
SRC := $(addprefix $(SRC_DIR), \
	main.c \
	parsing.c \
	routine.c \
	utils.c \
)

# Object Files and Directories
OBJ_DIR := .obj/
OBJ := $(SRC:$(SRC_DIR)%.c=$(OBJ_DIR)%.o)

# Dependencies Files
DEPS := $(OBJ:%.o=%.d)

# Include Directories
HEADERS := -I $(INCLUDES)

# Color Definitions
DEF_COLOR = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m
PURPLE = \033[35m

# Targets

all: greet $(NAME)

$(NAME): $(OBJ)
	@echo "$(CYAN)[INFO] $(NAME) Building...$(DEF_COLOR)"
	$(CC) $(CCFLAGS) $(OBJ) -o $(NAME)
	@echo "$(PURPLE)✨ $(NAME) compiled successfully! ✨$(DEF_COLOR)"
	$(MAKE) kitty

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(OBJ_DIR)
	@echo "$(BLUE)...Compiling: $<$(DEF_COLOR)"
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(HEADERS) -c $< -o $@

# Kitty Art
kitty:
	@echo "$(GREEN)"
	@echo "                                 /|"
	@echo "                               ///"
	@echo "                              / /"
	@echo "                             ||\\"
	@echo "                             /   \\"
	@echo "                             |   |"
	@echo "                       r     \\   \\     r^-"
	@echo "                       |\\---- |   |---/|"
	@echo "                       /      |   | |  \\"
	@echo "          - - - - -   (      /   /      ) - - - - - - -"
	@echo "            - - - - - \\      |   |      / - - - - -"
	@echo "                 __-----^----|   |----^--#----+"
	@echo "               /             |   |             #"
	@echo "              /             /     \\             \\"
	@echo "             /         /       *                 \\"
	@echo "            -/       /                  \\         \\-"
	@echo "            /        |                   |        |"
	@echo "            |        |                   |        |"
	@echo "            |        |---__-----____-----|        |"
	@echo "            __________                   __________"
	@echo "$(DEF_COLOR)"

# Display custom greeting
greet:
	@echo "$(GREEN)Welcome to $(NAME) Makefile!$(DEF_COLOR)"
	@echo "$(CYAN)Current Time: $(shell date +'%T')$(DEF_COLOR)"
	@echo "$(MAGENTA)Let the build begin!$(DEF_COLOR)"

clean:
	@echo "$(RED)Cleaning object files...$(DEF_COLOR)"
	rm -rf $(OBJ_DIR)
	@echo "$(GRAY)🧹 Object files cleaned! 🧹$(DEF_COLOR)"

fclean: clean
	@echo "$(YELLOW)🧽 $(NAME) successfully cleaned 🧽$(DEF_COLOR)"
	rm -f $(NAME)

re: fclean all

info:
	@echo "DEPS: $(DEPS)"
	@echo "$(DEF_COLOR)"

-include $(DEPS)

.PHONY: all clean fclean re greet kitty info
