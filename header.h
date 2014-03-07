#ifndef HEADER_INCLUDED
#define HEADER_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <time.h>
#include <regex.h>
#include <openssl/md5.h>
#include <sys/mman.h>
#include<fcntl.h>

int PORT;
#define BACKLOG 5
#define LENGTH 512


/* Defining Variables */
int sockfd;
int nsockfd;
int num;
int sin_size;
struct sockaddr_in addr_local; /* client addr */
struct sockaddr_in addr_remote; /* server addr */
struct sockaddr_in remote_addr;
char revbuf[LENGTH]; // Receiver buffer
int exec_mode=0;
char SERVER[1024],CLIENT[1024];

//Global Variables for client side
char complete_command[1000];
char command[100][100];
int command_count=0;


void send_receive_command();
void send_file(char *fs_name);
void receive_file(char *fr_name);
void send_latest_files();
void send_all_files();
void send_regex_files(char *regex_expr);
void filehash_verify(char *filename);
void filehash_checkall();
void file_download();
void file_upload();
void server(char *argv1);

void client_send_receive_command();
void client_send_file(char* fs_name);
void client_receive_file(char *fr_name);
void client(char *argv1,char *argv2);


#endif

