NAME	= ircserv
CFLAGS	= -Wall -Wextra -Werror  -std=c++98 #-Wshadow -Wno-shadow
CC		= c++

SRC	= 	main.cpp \
		Authentication.cpp \
		Channel.cpp \
		Client.cpp \
		Clients_Commands.cpp \
		Commands.cpp \
		Join.cpp \
		Mode.cpp \
		Privmsg.cpp \
    	Server.cpp \
		Tools_Errors.cpp \

OBJ_DIR	= obj
OBJ		= $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

# Colors
RESET = \033[0m
BOLD = \033[1m
RED = \033[91m
GREEN = \033[92m
YELLOW = \033[33m
ORANGE = \033[93m
BLUE = \033[94m

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(BOLD)$(RED)    🛠️ 🚧      Compiling ircserv      🚧🛠️$(RESET)"
	@echo "\n"
	@echo "$(BOLD)$(BLUE)-----------      -----------      -----------$(RESET)"
	@echo "\n"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(BOLD)$(GREEN) 💾🎆🌐💬     ircserv compiled     💬🌐🎆💾 $(RESET)"
	@echo "\n"

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(BOLD)$(ORANGE)🧹🧼    .o files deleted    🧼🧹$(RESET)"

fclean: clean
	@rm -rf $(NAME)
	@echo "$(BOLD)$(ORANGE)🧹🧼      exec deleted      🧼🧹$(RESET)"

re: fclean all

.PHONY: all clean fclean re
