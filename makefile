all: chat.cpp
			gcc -g -Wall -o chat chat.cpp

clean:
	 	$(RM) chat
