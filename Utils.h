#ifndef _USEFUL_COMM_
#define _USEFUL_COMM_

//returns the index of the command or -1 if there is not a valid command
int verifyCommand(char *command);

char *getUserInfo();

int findNrOfOccurences(char *string, char *substring);

char *textBetweenTwoStrings(char *firstPattern, char *secondPattern, char *s);

char *getBookInfo(char *jwt, char *cookie);

#endif