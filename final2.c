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


void set_nonblock(int socket) {
	int flags;
	flags = fcntl(socket,F_GETFL,0);
	// assert(flags != -1);
	fcntl(socket, F_SETFL, flags | O_NONBLOCK);
	fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
}




char SERVER[1024],CLIENT[1024];
void error(const char *msg)
{
	perror(msg);
	exit(1);
}


int main (int argc,char *argv[])
{
printf("0-Server TCP\n");
printf("1-Client TCP\n2-Server UDP\n3-Client UDP\n");
scanf("%d",&exec_mode);

printf("Enter Your SHARED directory\n");
if(exec_mode==0 || exec_mode==2)
	scanf("%*c%[^\n]",SERVER);
else
	scanf("%*c%[^\n]",CLIENT);

if(exec_mode==0 || exec_mode==2)
	server(argv[2]);
else if(exec_mode==1 || exec_mode==3)
	client(argv[1],argv[2]);
else
    printf("Entered Wrong Choice\n");
    return 0;
}


void scan_input(char *str){
	char c;
	int count=0;
	
	strcpy(complete_command,str);
	command_count=0;
	int count2=0;
	int i;
	for(i=0;i<strlen(complete_command);i++){
		if(complete_command[i]==' '){
			command[command_count][count2++]='\0';
			command_count++;
			count2=0;
			continue;
		}
		command[command_count][count2]=complete_command[i];
		count2++;
	}
	command[command_count][count2++]='\0';
	command_count++;
}

void server(char *argv2)
{
/* Get the Socket file descriptor */

    if (exec_mode == 2 || exec_mode == 3)
    {
        printf("Server has started in UDP mode\n");
    }
	PORT=atoi(argv2);
	printf("port= %d\n",PORT);
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor. (errno = %d)\n", errno);
		exit(1);
	}
	else 
		printf("[Server] Obtaining socket descriptor successfully.\n");

	/* Fill the client socket address struct */
	addr_local.sin_family = AF_INET; // Protocol Family
	addr_local.sin_port = htons(PORT); // Port number
	printf("INADDR = %d\n",INADDR_ANY);
	addr_local.sin_addr.s_addr = INADDR_ANY; // AutoFill local address
	bzero(&(addr_local.sin_zero), 8); // Flush the rest of struct

	/* Bind a special Port */
	if( bind(sockfd, (struct sockaddr*)&addr_local, sizeof(struct sockaddr)) == -1 )
	{
		fprintf(stderr, "ERROR: Failed to bind Port. (errno = %d)\n", errno);
		exit(1);
	}
	else 
		printf("[Server] Binded tcp port %d in addr 127.0.0.1 sucessfully.\n",PORT);

	/* Listen remote connect/calling */
	if(listen(sockfd,BACKLOG) == -1)
	{
		fprintf(stderr, "ERROR: Failed to listen Port. (errno = %d)\n", errno);
		exit(1);
	}
	else
		printf ("[Server] Listening the port %d successfully.\n", PORT);


    sin_size = sizeof(struct sockaddr_in);

    /* Wait a connection, and obtain a new socket file despriptor for single connection */
    if ((nsockfd = accept(sockfd, (struct sockaddr *)&addr_remote, &sin_size)) == -1)
    {
        fprintf(stderr, "ERROR: Obtaining new Socket Despcritor. (errno = %d)\n", errno);
        exit(1);
    }
    else
        printf("[Server] Server has got connected from %s.\n", inet_ntoa(addr_remote.sin_addr));

   while(1)
   {
   send_receive_command();
    }
 


    printf("[Server] Connection with Client closed. Server will wait now...\n");

}

void send_receive_command()
{
    int n;
    char buffer[1024];
    bzero(buffer,256);
    n = read(nsockfd,buffer,255);
    scan_input(buffer);
    
    if (strcmp(command[0],"IndexGet") == 0 )
    {
        if (strcmp(command[1], "ShortList") == 0)
        {
		char pch1[1024],pch2[1024];

		printf("%s\n",buffer);
		int i,space_count=0;
		char c;
		int pch1_count=0,pch2_count=0;
		while(command[2][pch1_count]!='\0')
		{
			pch1[pch1_count]=command[2][pch1_count];
			pch1_count++;
		}
		int count=0;
		while(command[3][count]!='\0')
		{
			pch1[pch1_count]=command[2][count];
			pch1_count++;
			count++;
		}
		while(command[4][pch2_count]!='\0')
		{
			pch2[pch2_count]=command[2][pch2_count];
			pch2_count++;
		}
		count=0;
		while(command[5][count]!='\0')
		{
			pch2[pch2_count]=command[2][count];
			pch2_count++;
			count++;
		}
		pch1[pch1_count]='\0';
		pch2[pch2_count]='\0';
           	send_latest_files(pch1,pch2);
        }
        else if (strcmp(command[1], "LongList") ==0)
        {
            send_all_files();
        }
        else if (strcmp(command[1], "RegEx") ==0)
        {
           
            send_regex_files(command[2]);
        }
    }
    else if (strcmp(command[0], "FileHash") ==0)
    {
        if (strcmp(command[1], "Verify") == 0)
        {
          
            filehash_verify(command[2]);
        }
        else if (strstr(buffer, "CheckAll") != NULL)
        {
            filehash_checkall();
        }
    }
    else if (strcmp(command[0], "FileDownload") == 0)
    {
       // file_download();
	char file_path[1024];
         
	    strcpy(file_path,SERVER);
	    strcat(file_path,command[1]);
	    printf("%s\n",file_path);
	    
	    char permission[10];
	    printf("Provide Permission to Client for Download write y or n\n");
	    scanf("%s",permission);
    	    write(nsockfd,permission, strlen(permission));
	    if(permission[0]=='y')
	 	   send_file(file_path);
		
    }
    else if (strcmp(command[0], "FileUpload") == 0)
    {
		char file_path[1024];
        
	    strcpy(file_path,SERVER);
	    strcat(file_path,command[1]);
	    printf("%s\n",file_path);
	    receive_file(file_path);
    }
}

void send_file(char* fs_name)
{
    /* Send File to Client */

    struct stat st;
    stat(fs_name, &st);
    int size = st.st_size;

    char sdbuf[LENGTH]; // Send buffer
    printf("[Server] Sending %s to the Client...", fs_name);

    FILE *fs = fopen(fs_name, "rb");
    if(fs == NULL)
    {
        fprintf(stderr, "ERROR: File %s not found on server. (errno = %d)\n", fs_name, errno);
      //  exit(1);
	return;
    }
    bzero(sdbuf, LENGTH);
    int fs_block_sz;
    

    int total_size=0;
    while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))>0)
    {
        if(send(nsockfd, sdbuf, fs_block_sz, 0) < 0)
        {
            fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
            exit(1);
        }
	sleep(1);
	total_size+=fs_block_sz;
	if(total_size>=size)
		break;
	bzero(sdbuf, LENGTH);
    }
    write(nsockfd,"Done\n", strlen("Done\n"));

}

void receive_file(char* fr_name)
{
    /*Receive File from Client */
    FILE *fr = fopen(fr_name, "wb");
    if(fr == NULL)
        printf("File %s Cannot be opened file on server.\n", fr_name);
    else
    {
        bzero(revbuf, LENGTH);
        int fr_block_sz = 0;
        while((fr_block_sz = recv(nsockfd, revbuf, LENGTH, 0)) > 0)
        {
            int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
            if(write_sz < fr_block_sz)
            {
                error("File write failed on server.\n");
            }
            bzero(revbuf, LENGTH);
            if (fr_block_sz == 0 || fr_block_sz==5)
            {
                break;
            }
        }
        if(fr_block_sz < 0)
        {
            if (errno == EAGAIN)
            {
                printf("recv() timed out.\n");
            }
            else
            {
                fprintf(stderr, "recv() failed due to errno = %d\n", errno);
                exit(1);
            }
        }
        printf("Ok received from client!\n");
        fclose(fr);
    }

}


void send_latest_files(char *pch1,char *pch2)
{
    write(nsockfd, "\n---LIST START---", strlen("\n---LIST START---"));
    int n;
    struct stat buf;
    DIR *dp;
    struct dirent *ep;

    dp = opendir(SERVER);

    if (dp != NULL)
    {
        while(ep = readdir(dp))
        {

            char full_path[1024], date[100];
            strcpy(full_path, SERVER);
            strcat(full_path, ep->d_name);
            stat(full_path, &buf);
            strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&(buf.st_mtime)));
            	printf("The file %s was last modified at %s\n", ep->d_name, date);
           if((strcmp(date,pch1)>0 && strcmp(date,pch2)<0)&&(strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0))
           {
               char permissions[1024], trash[1024],date[100];
               strcpy(permissions, (S_ISDIR(buf.st_mode)) ? "d" : "-");
               strcat(permissions, (buf.st_mode & S_IRUSR) ? "r" : "-");
               strcat(permissions, (buf.st_mode & S_IWUSR) ? "w" : "-");
               strcat(permissions, (buf.st_mode & S_IXUSR) ? "x" : "-");
               strcat(permissions, (buf.st_mode & S_IRGRP) ? "r" : "-");
               strcat(permissions, (buf.st_mode & S_IWGRP) ? "w" : "-");
               strcat(permissions, (buf.st_mode & S_IXGRP) ? "x" : "-");
               strcat(permissions, (buf.st_mode & S_IROTH) ? "r" : "-");
               strcat(permissions, (buf.st_mode & S_IWOTH) ? "w" : "-");
               strcat(permissions, (buf.st_mode & S_IXOTH) ? "x" : "-");
               strcat(permissions, " ");
               sprintf(trash, "%8d", buf.st_size);
               strcat(permissions, trash);
               strcat(permissions, " ");
               strftime(date, 20, "%Y-%m-%d %H:%M", localtime(&(buf.st_mtime)));
               strcat(permissions, date);
               strcat(permissions, " ");
               strcat(permissions, ep->d_name);
               strcat(permissions, "\n");
               n = write(nsockfd, permissions, strlen(permissions));
           }

            if (n < 0)
                error("ERROR writing to socket");
        }
        closedir(dp);
    }
    else
    {
        perror("Couldn't open the directory\n");
        exit(1);
    }
    write(nsockfd, "---LIST END---\n", strlen("---LIST END---\n"));
}

void send_all_files()
{
    write(nsockfd, "\n---LIST START---", strlen("\n---LIST START---"));
    int n;
    struct stat buf;
    DIR *dp;
    struct dirent *ep;

    dp = opendir(SERVER);

    if (dp != NULL)
    {
        while(ep = readdir(dp))
        {


            char full_path[1024], permissions[1024], trash[1024],date[100];
            strcpy(full_path, SERVER);
            strcat(full_path, ep->d_name);
            stat(full_path, &buf);
            strcpy(permissions, (S_ISDIR(buf.st_mode)) ? "d" : "-");
            strcat(permissions, (buf.st_mode & S_IRUSR) ? "r" : "-");
            strcat(permissions, (buf.st_mode & S_IWUSR) ? "w" : "-");
            strcat(permissions, (buf.st_mode & S_IXUSR) ? "x" : "-");
            strcat(permissions, (buf.st_mode & S_IRGRP) ? "r" : "-");
            strcat(permissions, (buf.st_mode & S_IWGRP) ? "w" : "-");
            strcat(permissions, (buf.st_mode & S_IXGRP) ? "x" : "-");
            strcat(permissions, (buf.st_mode & S_IROTH) ? "r" : "-");
            strcat(permissions, (buf.st_mode & S_IWOTH) ? "w" : "-");
            strcat(permissions, (buf.st_mode & S_IXOTH) ? "x" : "-");
            strcat(permissions, " ");
            sprintf(trash, "%8d", buf.st_size);
            strcat(permissions, trash);
            strcat(permissions, " ");
            strftime(date, 20, "%Y-%m-%d %H:%M", localtime(&(buf.st_mtime)));
            strcat(permissions, date);
            strcat(permissions, " ");
            strcat(permissions, ep->d_name);
            n = write(nsockfd, strcat(permissions,"\n"), strlen(permissions)+1);
            if (n < 0)
                error("ERROR writing to socket");
        }
        closedir(dp);
    }
    else
    {
        perror("Couldn't open the directory\n");
        exit(1);
    }
    write(nsockfd, "---LIST END---\n", strlen("---LIST END---\n"));
}

void send_regex_files(char *regex_expr)
{
    write(nsockfd, "\n---LIST START---", strlen("\n---LIST START---"));
    regex_expr++;
    regex_expr[strlen(regex_expr)-1] = 0;
    int i;
    for(i=strlen(regex_expr);i>=1;i--)
	    regex_expr[i]=regex_expr[i-1];
    regex_expr[0]='.';
    printf("The RegEx is = %s\n", regex_expr);
    int n;
    struct stat buf;
    DIR *dp;
    struct dirent *ep;

    dp = opendir(SERVER);

    if (dp != NULL)
    {
        while(ep = readdir(dp))
        {
//            printf("%s\n", ep->d_name);

            regex_t regex;
            int reti;
            reti = regcomp(&regex, regex_expr, 0);
            if (reti)
                printf("The regex couldn't be compiled\n");
            reti = regexec(&regex, ep->d_name, 0, NULL, 0);
            printf("For %s, reti = %d\n", ep->d_name, reti);
            if(!reti)
            {
                char full_path[1024], permissions[1024], trash[1024],date[100];
                strcpy(full_path, SERVER);
                strcat(full_path, ep->d_name);
                stat(full_path, &buf);
                strcpy(permissions, (S_ISDIR(buf.st_mode)) ? "d" : "-");
                strcat(permissions, (buf.st_mode & S_IRUSR) ? "r" : "-");
                strcat(permissions, (buf.st_mode & S_IWUSR) ? "w" : "-");
                strcat(permissions, (buf.st_mode & S_IXUSR) ? "x" : "-");
                strcat(permissions, (buf.st_mode & S_IRGRP) ? "r" : "-");
                strcat(permissions, (buf.st_mode & S_IWGRP) ? "w" : "-");
                strcat(permissions, (buf.st_mode & S_IXGRP) ? "x" : "-");
                strcat(permissions, (buf.st_mode & S_IROTH) ? "r" : "-");
                strcat(permissions, (buf.st_mode & S_IWOTH) ? "w" : "-");
                strcat(permissions, (buf.st_mode & S_IXOTH) ? "x" : "-");
                strcat(permissions, " ");
                sprintf(trash, "%8d", buf.st_size);
                strcat(permissions, trash);
                strcat(permissions, " ");
                strftime(date, 20, "%Y-%m-%d %H:%M", localtime(&(buf.st_mtim)));
                strcat(permissions, date);
                strcat(permissions, " ");
                strcat(permissions, ep->d_name);
                strcat(permissions, "\n");
                n = write(nsockfd, permissions, strlen(permissions));
                if (n < 0)
                    error("ERROR writing to socket");
            }
        }
        closedir(dp);
    }
    else
    {
        perror("Couldn't open the directory\n");
        exit(1);
    }
    write(nsockfd, "---LIST END---\n", strlen("---LIST END---\n"));
}

void filehash_verify(char *filename)
{
    write(nsockfd, "\n---LIST START---", strlen("\n---LIST START---"));


    char temp_path[100];
    strcpy(temp_path, SERVER);
    strcat(temp_path, filename);
    int file_descript;
  //  printf("mera path=%s\n",temp_path);
    file_descript = open(temp_path, O_RDONLY);
    printf("file descriptor = %d\n", file_descript);
    if(file_descript < 0)
    {
        char *error_str = "ERROR: Could not open the file\n";

        write(nsockfd, "---LIST END---\n", strlen("---LIST END---\n"));
        return;
    }


    unsigned char result[MD5_DIGEST_LENGTH];
    struct stat buf;
    stat(temp_path, &buf);
    char *file_buffer;
    file_buffer = mmap(0, buf.st_size, PROT_READ, MAP_SHARED, file_descript, 0);
    MD5((unsigned char*) file_buffer, buf.st_size, result);

    int i;
    char md5_ans[1024], temp[100];

    for(i=0; i <MD5_DIGEST_LENGTH; i++)
    {
        sprintf(temp, "%02x", result[i]);
        strcat(md5_ans, temp);
        printf("%02x\t%s\n", result[i], md5_ans);
    }


    printf("String = %s\n", md5_ans);
    char ans[1024], date[100];
    strcpy(ans, filename);
    strcat(ans, " ");
    strcat(ans, md5_ans);
    strcat(ans, " ");
    strftime(date, 20, "%Y-%m-%d %H:%M", localtime(&(buf.st_mtim)));
    strcat(ans, date);
    strcat(ans, "\n");

    write(nsockfd, ans, strlen(ans));
    write(nsockfd, "---LIST END---\n", strlen("---LIST END---\n"));
}

void filehash_checkall()
{
    write(nsockfd, "\n---LIST START---", strlen("\n---LIST START---"));
    int n;
    DIR *dp;
    struct dirent *ep;

    dp = opendir(SERVER);

    if (dp != NULL)
    {
        while(ep = readdir(dp))
        {
            if (strcmp(ep->d_name, ".") && strcmp(ep->d_name, ".."))
            {
                char temp_path[1000];
                strcpy(temp_path, SERVER);
                strcat(temp_path, ep->d_name);
                int file_descript;
                file_descript = open(temp_path, O_RDONLY);
                if(file_descript < 0)
                {
                    
                    write(nsockfd, "---LIST END---\n", strlen("---LIST END---\n"));
                    return;
                }

                unsigned char result[MD5_DIGEST_LENGTH];
                struct stat buf;
                stat(temp_path, &buf);
                char *file_buffer;
                file_buffer = mmap(0, buf.st_size, PROT_READ, MAP_SHARED, file_descript, 0);
                MD5((unsigned char*) file_buffer, buf.st_size, result);

                int i;
                char md5_ans[1024], temp[100];
                bzero(md5_ans, 1024);
                for(i=0; i <MD5_DIGEST_LENGTH; i++)
                {
                    sprintf(temp, "%02x", result[i]);
                    strcat(md5_ans, temp);
                    printf("%02x\t%s\n", result[i], md5_ans);
                }

                char ans[1024], date[100];
                strcpy(ans, ep->d_name);
                strcat(ans, " ");
                strcat(ans, md5_ans);
                strcat(ans, " \n");
                strftime(date, 20, "%Y-%m-%d %H:%M", localtime(&(buf.st_mtim)));
                strcat(ans, date);
                strcat(ans, " \n");
                write(nsockfd, ans, strlen(ans));
                if (n < 0)
                    error("ERROR writing to socket");
            }
        }
        closedir(dp);
    }
    else
    {
        perror("Couldn't open the directory\n");
        exit(1);
    }
    write(nsockfd, "---LIST END---\n", strlen("---LIST END---\n"));
}
void client(char *argv1,char *argv2)
{
	/* Get the Socket file descriptor */
	PORT=atoi(argv2);
	printf("%d\n",PORT);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to obtain Socket Descriptor! (errno = %d)\n",errno);
		exit(1);
	}


	remote_addr.sin_family = AF_INET; 
	remote_addr.sin_port = htons(PORT); 
	inet_pton(AF_INET,argv1, &remote_addr.sin_addr); 
	bzero(&(remote_addr.sin_zero), 8);


	if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to connect to the host! (errno = %d)\n",errno);
		exit(1);
	}
	else 
		printf("[Client] Connected to server at port %d...ok!\n", PORT);

	while(1)
	{
    client_send_receive_command();

 
	}

}

void client_send_receive_command()
{
    int n;
    int break_while=0;
    char r_w_buffer[1024];
    int file_flag=0;
    printf("Please enter the message:");
    bzero(r_w_buffer,256);
    if(strcmp(r_w_buffer,"BYE")==0)
	    exit(1);
    scanf("%*c%[^\n]",r_w_buffer);
    n = write(sockfd, r_w_buffer, strlen(r_w_buffer));
    scan_input(r_w_buffer);
    char buffer[1024];
    strcpy(buffer,r_w_buffer);
    if (strcmp(command[0], "FileDownload") == 0)
    {
	    break_while=1;
		char file_path[1024];
	
	    strcpy(file_path,CLIENT);
	    strcat(file_path,command[1]);
	    printf("%s\n",file_path);
	    printf("Waiting for Server to provide Permission\n");
	    char r_w_buffer[1024];
	    read(sockfd, r_w_buffer, 1);
	    if(r_w_buffer[0]=='y')
	    {	    
		printf("Downloading.. for you\n");
	    	client_receive_file(file_path);
	    }
	    else
	    {
	    	printf("Download denied\n");
	    }


    }
    else if (strcmp(command[0], "FileUpload") ==0)
    {
	    break_while=1;
	    printf("buffer=%s\n",buffer);
		char file_path[1024];
	 
	    strcpy(file_path,CLIENT);
	    printf("file_path=%s\n",file_path);
	    strcat(file_path,command[1]);
	    printf("file_path=%s\n",file_path);
	    client_send_file(file_path);
    }

    if (n < 0)
        error("ERROR writing to socket");

    bzero(r_w_buffer, 1024);
    while(1 && !break_while)
    {
        bzero(r_w_buffer, 1024);
        n = read(sockfd, r_w_buffer, 1024);
        if (n < 0)
	{
		break;

	}
	
		printf("%s ", r_w_buffer);

        if (strstr(r_w_buffer, "---LIST END---\n") != NULL)
            break;
    }
}
void client_send_file(char* fs_name)
{
    /* Send File to Server */

    char sdbuf[LENGTH];
    printf("[Client] Sending %s to the Server... ", fs_name);
    FILE *fs = fopen(fs_name, "rb");
    if(fs == NULL)
    {
        printf("ERROR: File %s not found.\n", fs_name);
        exit(1);
    }

    bzero(sdbuf, LENGTH);
    int fs_block_sz;
    while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0)
    {
        if(send(sockfd, sdbuf, fs_block_sz, 0) < 0)
        {
            fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
            break;
        }
	sleep(1);
        bzero(sdbuf, LENGTH);
    }
    write(sockfd, "Done\n", strlen("Done\n"));
    printf("Ok File %s from Client was Sent!\n", fs_name);
    fclose(fs);
}

void client_receive_file(char* fr_name)
{
    /* Receive File from Server */
    printf("[Client] Receiving file from Server and saving it ...");

    FILE *fr = fopen(fr_name, "wb");
    if(fr == NULL)
        printf("File %s Cannot be opened.\n", fr_name);
    else
    {
        bzero(revbuf, LENGTH);
        int fr_block_sz = 0;
        while((fr_block_sz = recv(sockfd, revbuf, LENGTH, 0)) > 0)
        {

            int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
            if(write_sz < fr_block_sz)
            {
                error("File write failed.\n");
            }
            bzero(revbuf, LENGTH);
            if (fr_block_sz == 0 || fr_block_sz==5)
			break;
        }
        if(fr_block_sz < 0)
        {
            if (errno == EAGAIN)
            {
                printf("recv() timed out.\n");
            }
            else
            {
                fprintf(stderr, "recv() failed due to errno = %d\n", errno);
            }
        }
        printf("Ok received from server!\n");
        fclose(fr);
    }
}
