#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include <string.h>
#include "parson.h"

#define MAX_NR_COMMANDS 9
#define IP "34.241.4.235"

char commandArray[10][30] =
{ "register",
  "login",
  "enter_library",
  "get_books",
  "get_book",
  "add_book",
  "delete_book",
  "logout",
  "exit"
};

//verify the given command
int verifyCommand(char *command) {
    for (int i = 0; i < MAX_NR_COMMANDS; i++) {
        if (strncmp(command, commandArray[i], strlen(commandArray[i])) == 0) {
            return i;
        }     
    }
    return -1;
}

//return a json string with users's credentials
char* getUserInfo() {
    char* username = malloc(50);
    char* password = malloc(50);
    printf("%s", "username=");
    fgets(username, 30, stdin);
    username[strlen(username) - 1] = '\0';

    printf("%s", "password=");
    fgets(password, 30, stdin);
    password[strlen(password) - 1] = '\0';

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
            
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    char *serialized_string = json_serialize_to_string_pretty(root_value);

    return serialized_string;
}

//find the number of the occurences of a substring in a string 
int findNrOfOccurences(char *string, char *substring) {
    int i, j, l1, l2;
    int count = 0;
    int found = 0;

    l1 = strlen(string);
    l2 = strlen(substring);

    for(i = 0; i < l1 - l2 + 1; i++) {
        found = 1;
        for(j = 0; j < l2; j++) {
        if(string[i+j] != substring[j]) {
            found = 0;
            break;
        }
        }

        if(found) {
        count++;
        i = i + l2 -1;
        }
    }

    return count;
}

//returns the string between two pattern strings
char *textBetweenTwoStrings(char *firstPattern, char *secondPattern, char *s) {
    char *target = NULL;
    char *start, *end;

    if (start = strstr(s, firstPattern))
    {
        start += strlen(firstPattern);
        if (end = strstr(start, secondPattern))
        {
            target = (char *)malloc(end - start + 1);
            memcpy(target, start, end - start);
            target[end - start] = '\0';
        }
    }
    return target;
}

//returns message
char* getBookInfo(char *jwt, char *cookie) {
   
    char *title = (char *)malloc(1000 * sizeof(char));
    char *author = (char *)malloc(1000 * sizeof(char));
    char *genre = (char *)malloc(1000 * sizeof(char));
    char *publisher = (char *)malloc(1000 * sizeof(char)); 
    char *page_count =  (char *)malloc(1000 * sizeof(char));

    printf("title=");
    fgets(title, 999, stdin);
    title[strlen(title) - 1] = '\0';

    printf("author=");
    fgets(author, 999, stdin);
    author[strlen(author) - 1] = '\0';

    printf("genre=");
    fgets(genre, 999, stdin);
    genre[strlen(genre) - 1] = '\0';


    printf("page_count=");
    fgets(page_count, 999, stdin);
    page_count[strlen(page_count) - 1] = '\0';

    printf("publisher=");
    fgets(publisher, 999, stdin);
    publisher[strlen(publisher) - 1] = '\0';
    
    char *serialized_string = (char *)malloc(1000 * sizeof(char));

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    
    json_object_set_string(root_object, "title", title); 
    json_object_set_string(root_object, "author", author); 
    json_object_set_string(root_object, "genre", genre);
    json_object_set_string(root_object, "publisher", publisher);
    json_object_set_number(root_object, "page_count", atoi(page_count));
    strcpy(serialized_string,json_serialize_to_string_pretty(root_value));
    char *message = calloc(1000,sizeof(char));
    sprintf(message, "POST /api/v1/tema/library/books HTTP/1.1\r\nHost: %s:8080\r\nContent-Type: application/json\r\nAuthorization: Bearer %s\r\nCookie: %s\r\nContent-Length: %d\r\n\r\n%s\r\n", IP, jwt, cookie, (int) strlen(serialized_string), serialized_string);
   
    return message;
}