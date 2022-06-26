#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <string.h>
#include "parson.h"
#include "helpers.h"
#include "requests.h"
#include "Utils.h"
#include <stdbool.h>

#define IP "34.241.4.235"
#define PORT 8080
#define MAX_LENGTH 1000
#define TRUE 1
#define FALSE 0


int main(int argc, char *argv[])
{
    char *finalCookie = (char*)malloc(250);
    char *jwtToken = (char*)malloc(250);
    int sockfd;
    bool isLogged = FALSE;
    bool isJWT = FALSE;
    char* type = "application/json";
    char* endToken = (char*)malloc(5 * sizeof(char));
    strcpy(endToken, "\r\n");
        
    while(TRUE){
        char* command = malloc(30);
        fgets(command, 30, stdin);
        int nrCommand = verifyCommand(command);
        
        if(nrCommand != -1) {
            //register
            if (nrCommand == 0) {
                //open connection
                sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
                char* url = malloc(100);
                strcpy(url, "/api/v1/tema/auth/register");
              
                //create json string
                char *json_string = getUserInfo(url);
                
                char **arr = (char **) malloc(2);
                arr[0] = (char *) malloc(strlen(json_string) + 1);
                strcpy(arr[0], json_string);
                char *message = compute_post_request(IP, url, type, arr, 1, NULL, 0);
                
                //send message to the server
                send_to_server(sockfd, message);
                char *response = malloc(MAX_LENGTH);

                //receive response from the server
                strcpy(response, receive_from_server(sockfd));
          
                free(arr[0]);
                free(message);
        
                //verify errors
                int logic = findNrOfOccurences(response, "error");
                
                switch (logic > 0)
                {
                case TRUE:
                    printf("The username is already taken\n");
                    break;
                
                case FALSE:
                    printf("Successful registration!\n");
                    break;
                }
                close(sockfd);
                //login
            } else if (nrCommand == 1) {
                //open connection
                sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
                char* url = malloc(100);
                strcpy(url, "/api/v1/tema/auth/login");

                //create string in json format
                char *json_string = getUserInfo();
                char **arr = (char **) malloc(2);
                arr[0] = (char *) malloc(strlen(json_string) + 1);
                strcpy(arr[0], json_string);

                //create message
                char *message = compute_post_request(IP, url, type, arr, 1, NULL, 0);
                
                //send to the server
                send_to_server(sockfd, message);
                char *response = malloc(MAX_LENGTH);

                //receive response from the server
                strcpy(response, receive_from_server(sockfd));
               
                free(arr[0]);
                free(message);

                //verify for errors
                int logic = findNrOfOccurences(response, "error");
               
                switch (logic > 0)
                {
                case TRUE:
                    printf("Username or password is wrong\n");
                    break;
                
                case FALSE: ;
                    char *sessionCookie = strstr(response, "Set-Cookie: connect.sid");
                 
                    if (sessionCookie != NULL) {
                        finalCookie = textBetweenTwoStrings("Set-Cookie: ", "; Path", sessionCookie);
                        printf("Successful login\n");
                        printf("Cookie is %s\n", finalCookie);
                        isLogged = TRUE;
                    } else {
                        continue;
                    }
                    break;
                }
               close(sockfd);
               //enter_library
            } else if (nrCommand == 2) {
                //open connection
                sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
                char* url = malloc(100);
                strcpy(url, "/api/v1/tema/library/access");
                
                if(isLogged == TRUE) {
                    char **arr = (char **) malloc(2);
                    arr[0] = (char *) malloc(strlen(finalCookie) + 1);
                    strcpy(arr[0], finalCookie);

                    //compute message
                    char *message = compute_get_request(IP, url, NULL, arr, 1);

                    //send to server
                    send_to_server(sockfd, message);
                    char* response = receive_from_server(sockfd);

                    //verify errors
                    int logic = findNrOfOccurences(response, "error");
                
                    switch (logic > 0)
                    {
                    case TRUE:
                        printf("Found error\n");
                        break;
                    
                    case FALSE: ;
                        char *token = strstr(response, "{\"token\":");
                        jwtToken = textBetweenTwoStrings("{\"token\":\"", "\"}", token);
                        printf("JWT Token is %s\n", jwtToken);
                        printf("Enter the library successfully\n");
                        isJWT = TRUE; 
                        break;
                    }

                } else {
                    printf("You are not connected\n");
                }
                close(sockfd);
                //get_books
            } else if (nrCommand == 3) {
                //open connection
                sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
                if (isJWT == TRUE) {
                    char *message = (char *)malloc(600);
                    //compute message
                    sprintf(message, "GET /api/v1/tema/library/books HTTP/1.1\r\nHost: 34.241.4.235\r\nAuthorization: Bearer %s\r\nCookie: %s\r\n\r\n", jwtToken, finalCookie);
                    
                    //send to server
                    send_to_server(sockfd, message);

                    //receive response from the server
                    char *response = receive_from_server(sockfd);
                    
                    //verify errors
                    int logic = findNrOfOccurences(response, "error");
                    
                    switch (logic > 0)
                    {
                    case TRUE:
                        printf("Error found!\n");
                        break;
                    
                    case FALSE: ;
                        
                        char *json = strstr(response, "[{\"");
                        
                        printf("json is %s\n", json);
                        break;
                    }
                    close(sockfd);
                } else {
                    printf("You are not connected to the library!\n");
                }
                close(sockfd);
                //get_book
            } else if (nrCommand == 4) {
                //open connection
                sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
                if (isJWT == TRUE) {
                    char *id = (char *)malloc(100);
                    printf("id=");
                    fgets(id, 100, stdin);
                    id[strlen(id) - 1] = '\0';
                    char *message = (char *) malloc(600);
                    //compute message
                    strcat(message, "GET /api/v1/tema/library/books/");
                    strcat(message, id);
                    strcat(message," HTTP/1.1");
                    strcat(message, endToken);
                    strcat(message, "Host: ");
                    strcat(message, IP);
                    strcat(message, ":");
                    strcat(message, "8080");
                    strcat(message, endToken);
                    strcat(message, "Authorization: Bearer ");
                    strcat(message, jwtToken);
                    strcat(message, endToken);
                    strcat(message, "Cookie: ");
                    strcat(message, finalCookie);
                    strcat(message, endToken);
                    strcat(message, endToken);
                    //send to server
                    send_to_server(sockfd, message);
                    //receive response
                    char *response = receive_from_server(sockfd);
                    
                    //verify for errors
                    int logic = findNrOfOccurences(response, "error");
                   
                    switch (logic > 0)
                    {
                    case TRUE:
                        printf("There is not a book with the given ID!\n");
                        break;
                    
                    case FALSE: ;
                        char *json = textBetweenTwoStrings("[", "]", response);
                        printf("json is %s\n", json);
                        break;
                    }
                    
                } else {
                    printf("You are not connected to the library!\n");
                }
                close(sockfd);
                //add_book
            } else if (nrCommand == 5) {
                //open connection
                sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
                if (isJWT == TRUE) {
                    char *message = getBookInfo(jwtToken, finalCookie);
                    //send to server
                    send_to_server(sockfd, message);
                    //receive response
                    char *response = receive_from_server(sockfd);

                    //verify errors
                    int logic = findNrOfOccurences(response, "error");
               
                    switch (logic > 0)
                    {
                    case TRUE:
                        printf("Data is wrong!\n");
                        break;
                    
                    case FALSE:
                        printf("Book added successfully!\n");
                        break;
                    }
                } else {
                    printf("You are not connected to the library\n");
                }

                close(sockfd);
                //delete book
            } else if (nrCommand == 6) {
                if (isJWT == TRUE) {
                    //open connection
                    sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                    char *id = (char *)malloc(100 * sizeof(char));
                    
                    printf("id=");
                    fgets(id, 100, stdin);
                    id[strlen(id) - 1] = '\0';

                    sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                    char *message = (char *)malloc(500 * sizeof(char));
                    //Compute message
                    strcat(message, "DELETE /api/v1/tema/library/books/");
                    strcat(message, id);
                    strcat(message, " HTTP/1.1");
                    strcat(message, endToken);
                    strcat(message, "Host: ");
                    strcat(message, "34.241.4.235");
                    strcat(message, "8080");
                    strcat(message, endToken);
                    strcat(message, "Authorization: Bearer ");
                    strcat(message, jwtToken);
                    strcat(message, endToken);
                    strcat(message, "Cookie: ");
                    strcat(message, finalCookie);
                    strcat(message, endToken);
                    strcat(message, endToken);
                    
                   
                    send_to_server(sockfd, message);
                    char *response = receive_from_server(sockfd);
                    
                    int logic = findNrOfOccurences(response, "error");
               
                    switch (logic > 0)
                    {
                    case TRUE:
                        printf("There is no book eith the given id!\n");
                        break;
                    
                    case FALSE:
                        printf("Book removed successfully!\n");
                        break;
                    }

                    close(sockfd);
                } else {
                    printf("You are not connected to the library\n");
                }
            } else if (nrCommand == 7) {
                if (isLogged == TRUE) {
                    char* url = malloc(100);
                    strcpy(url, "/api/v1/tema/auth/logout");
                    //open connection
                    sockfd = open_connection("34.241.4.235", 8080, AF_INET, SOCK_STREAM, 0);
                    char *message = (char *)malloc(600 * sizeof(char));
                    //Compute message
                    strcat(message, "GET /api/v1/tema/auth/logout");
                    strcat(message, " HTTP/1.1");
                    strcat(message, endToken);
                    strcat(message, "Host: ");
                    strcat(message, "34.241.4.235");
                    strcat(message, ":");
                    strcat(message, "8080");
                    strcat(message, endToken);
                    strcat(message, "Authorization: Bearer ");
                    strcat(message, jwtToken);
                    strcat(message, endToken);
                    strcat(message, "Cookie: ");
                    strcat(message, finalCookie);
                    strcat(message, endToken);
                    strcat(message, endToken);

                    send_to_server(sockfd, message);
                    char *response = receive_from_server(sockfd);
                
                    int logic = findNrOfOccurences(response, "error");
                
                    switch (logic > 0)
                    {
                    case TRUE:
                        printf("Error found!\n");
                        break;
                        
                    case FALSE:
                        printf("Logout successfully!\n");
                        isLogged = FALSE;
                        isJWT = FALSE;
                        break;
                    }

                    close(sockfd);
                } else {
                    printf("No previous login found!\n");
                }
                //exit
            } else if (nrCommand == 8) {
                return 0;
            }
           
        } 
        else {
            printf("Wrong command!\n");
        }
        free(command);
    }

    return 0;
}