#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <string.h>
#include <sys/time.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
#define UDP_PORT 8888


#define TIMES 10
#define MULTIPLICAND_START  (3)
#define MULTIPLICAND_END    (9)
#define PASS 19
#define TIME_OUT 17
#define CHECK_TIME 10
#define TIME_TBL_VER "TIME_TBL_V1.1"
#define TMP_FILE_NAME "tmp"

enum EMENU_SET_CMD {
    EMENU_TIME_TBL,
    EMENU_TEST,
    EMENU_SVR,
    EMENU_CLIENT,
    EMENU_SET_CMD_NUM
};

enum TIMES_STAT {
    STAT,         
    REL,          
    TIMEOUT_VAL,
    NUM_OF_TIMES_STAT
};

#define STR_LEN 256
#define BUF_LEN 10
#define TIMES_END 9
#define NUM_OF_TIMES (MULTIPLICAND_END - MULTIPLICAND_START + 1)

struct pri_data {
	uint32_t cmd;
	uint32_t times;
	uint32_t pass_cnt;
	uint32_t fail_cnt;
	uint8_t is_output_file;
	char output_file[STR_LEN];
};

int tbl[NUM_OF_TIMES][TIMES_END][NUM_OF_TIMES_STAT] = {0};

void _show_total(FILE *fd)
{
    int i, k;

    fprintf(fd, "num|01|02|03|04|05|06|07|08|09|\n");
    for (i = 0; i< MULTIPLICAND_END - MULTIPLICAND_START + 1; i++) {
    	fprintf(fd, "  %d|", i+MULTIPLICAND_START);
    	for (k = 0; k < TIMES_END; k++) {
	    if (tbl[i][k][REL] == PASS)
    		fprintf(fd, "%02s|", "--");
	    else
    		fprintf(fd, "%02d|", tbl[i][k][REL]);
	}
	fprintf(fd, "\n");
    }
}

void _show_timeout(FILE *fd)
{
    int i, k;
    for (i = 0; i< MULTIPLICAND_END - MULTIPLICAND_START + 1; i++) {
    	for (k = 0; k < TIMES_END; k++) {
//	    if (tbl[i][k][TIMEOUT_VAL] > CHECK_TIME || tbl[i][k][REL] != PASS)
	    	fprintf(fd, "=> %02d x %02d time spend: %d, result = %s\n", 
		i + MULTIPLICAND_START, k+1, tbl[i][k][TIMEOUT_VAL], tbl[i][k][REL] == PASS?"pass":"fail");
	}
    }
    return ;
}

static void _print_time(FILE *fd)
{
    time_t t = time(NULL);

    struct tm tm = *localtime(&t);

    fprintf(fd, "test finish at: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, 
        tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

}

static int _input_check(int argc, const char *argv[], struct pri_data *p_data)
{
	int opt;
	int opt_count;
	char in[STR_LEN];
	
	while ((opt = getopt(argc, (char *const *)argv, "c:ef:m:h")) != -1) {
		switch (opt) {
		case 'c':
			sscanf(optarg, "%s", in);
			printf("with input arg %s\n", in);
			return -1;
		case 'e':
			printf("with no input arg\n");
			return -1;
		case 'f':
			sscanf(optarg, "%s", p_data->output_file);
			p_data->is_output_file = 1;
			break;
		case 'm':
			sscanf(optarg, "%s", in);
			p_data->cmd = atoi(in);
			printf("cmd is %d\n", p_data->cmd);
			break;
		case 'h':
		default:
			printf("-c test with input arg\n");
			printf("-e test with no input arg\n");
			return -1;
		}
	}

	return 0;
}

static void _print_result(struct pri_data *pri_data)
{
    FILE *fd;

    if (pri_data->is_output_file)
        fd=fopen(pri_data->output_file, "w");
    else
        fd=fopen(TMP_FILE_NAME, "ab+");

    fprintf(fd, "\n=========================================\n");
    fprintf(fd, "Times table: version: %s\n", TIME_TBL_VER);
    _print_time(fd);

    if (pri_data->pass_cnt == pri_data->times) {
        fprintf(fd, "*************************************\n");
        fprintf(fd, "*************************************\n");
	fprintf(fd, "ALL PASS\n");
        fprintf(fd, "*************************************\n");
        fprintf(fd, "*************************************\n");
    }

    fprintf(fd, "--------------------------------------\n");
    fprintf(fd, "pass: %d \n", pri_data->pass_cnt);
    fprintf(fd, "fail: %d \n", pri_data->times - pri_data->pass_cnt);
    fprintf(fd, "--------------------------------------\n");
    _show_total(fd);
    fprintf(fd, "--------------------------------------\n");
    _show_timeout(fd);
    fprintf(fd, "=========================================\n\n\n");

    fclose(fd);
}

static void _output_to_stdout(struct pri_data *pri_data)
{
    FILE *fd;
    char line[STR_LEN]; 

    if (pri_data->is_output_file)
        fd=fopen(pri_data->output_file, "r");
    else
        fd=fopen(TMP_FILE_NAME, "r");

    while (!feof(fd)) 
    { 
       fgets(line, STR_LEN, fd);
       printf("%s", line); 
    } 

    fclose(fd);
}

static void _show_times_of_two(int multiplicand, int multiplier)
{
    uint8_t i, j;
    for (i=1; i<=multiplier; i++) {
        for (j=0; j<multiplicand; j++) {
    	printf("O");
        }
        printf("\n");
        if (i == 2 | i == 4 | i == 8)
            printf("\n");
    }
}

static void times_table(struct pri_data *pri_data)
{
    int k = 0;
    int ans = 0;
    int multiplicand, multiplier; 
    struct  timeval    t_start, t_end;
    char buf[BUF_LEN];
    char *end;
    int pass_cnt=0;
    int t_diff = 0;
    int times = (MULTIPLICAND_END-MULTIPLICAND_START+1)*9;

    srand(time(0));
    for (k=0; k<times; k++)
    {
        printf("*************************************\n");

	while (1) {
            multiplicand = (rand() % (MULTIPLICAND_END - MULTIPLICAND_START + 1)) 
	                 + MULTIPLICAND_START;
            multiplier = (rand() % TIMES_END) + 1;

	    if (!tbl[multiplicand - MULTIPLICAND_START][multiplier-1][STAT]) {
	        tbl[multiplicand - MULTIPLICAND_START][multiplier-1][STAT] = 1;
	    	break;
	    } else { 
	    	continue;
	    }
	}
        printf("%d x %d = ? \n", multiplicand, multiplier);
        
	_show_times_of_two(multiplicand, multiplier);

        gettimeofday(&t_start, NULL);

        do {
            if (!fgets(buf, sizeof buf, stdin))
                break;

            buf[strlen(buf) - 1] = 0;

            ans = strtol(buf, &end, BUF_LEN);
        } while (end != buf + strlen(buf));

        gettimeofday(&t_end, NULL);

	t_diff = t_end.tv_sec - t_start.tv_sec;
//	printf("spend time: %d\n", t_diff);
	
	if (ans == multiplicand * multiplier) { 
	    pass_cnt += 1;
	    tbl[multiplicand - MULTIPLICAND_START][multiplier - 1][REL] = PASS;
	} else {
	    tbl[multiplicand - MULTIPLICAND_START][multiplier - 1][REL] = ans;
	}
	   
	tbl[multiplicand - MULTIPLICAND_START][multiplier - 1][TIMEOUT_VAL] = t_diff;
        printf("*************************************\n");
    }
	
    pri_data->times = times;
    pri_data->pass_cnt = pass_cnt;
    pri_data->fail_cnt = times - pass_cnt;

    _print_result(pri_data);
    _output_to_stdout(pri_data);

}

static int svr(struct pri_data *pri_data)
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "Hello from server";
  
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
        == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
 /*
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
*/
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
  
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    
  // closing the connected socket
    close(new_socket);
  // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}

static int client(struct pri_data *pri_data)
{
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
  
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if ((client_fd
         = connect(sock, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
  
    // closing the connected socket
    close(client_fd);
    return 0;
}

static int svr_udp(struct pri_data *pri_data)
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    int so_broadcast = 1;
    char buf[1024];
   
    /*Create an IPv4 UDP socket*/
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0))<0){
        perror("socket");
        return -1;
    }
   
    /*SO_BROADCAST: broadcast attribute*/
    if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast))<0){
        perror("setsockopt");
        return -1;
    }
   
    server_addr.sin_family = AF_INET; /*IPv4*/
    server_addr.sin_port = htons(INADDR_ANY); /*All the port*/
    server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); /*Broadcast address*/
    server_addr.sin_addr.s_addr = inet_addr("192.168.0.114"); /*Broadcast address*/
   
    if((bind(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr))) != 0){
        perror("bind");
        return -1;
    }
   
    client_addr.sin_family = AF_INET; /*IPv4*/
    client_addr.sin_port = htons(UDP_PORT);  /*Set port number*/
    client_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST); /*The broadcast address*/
    int clientlen = sizeof(client_addr);
   
    while(1){
        printf("Please input your word :> ");
        //scanf("%s", buf);
        fgets(buf, sizeof(buf), stdin); /*U can enter string by yourself*/
   
        /*Use sendto() to send messages to client*/
        /*sendto() doesn't need to be connected*/
        if((sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&client_addr, (socklen_t)clientlen)) < 0){
            perror("sendto");
            return -1;
        } else {
            printf("send msg %s\n", buf);
        }
    }
    close(sockfd);
    return 0;
}

static int client_udp(struct pri_data *pri_data)
{
    int sockfd;
    struct sockaddr_in serv_addr, client_addr;
    int yes = 1;
    ssize_t size;
    socklen_t addrlen = sizeof(client_addr);
    char buf[200];
   
    //Create an IPv4 and UDP socket
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0))<0){
        perror("socket");
        return -1;
    }
   
    //Set the struct of sockaddr_in
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(UDP_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); /*All the host*/
   
    /*Set communication address can be reused*/
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))<0){
        perror("setsockopt");
        return -1;
    }
   
    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) != 0){
        perror("bind");
        return -1;
    }
   
    while(1){
        memset(buf, 0, 200); /*Clean up the buffer*/
        size = 0;
   
        /*Use recvfrom() to receive the messages from server*/
        size = recvfrom(sockfd, buf, 200, 0, (struct sockaddr *)&client_addr, &addrlen);
        if(size<0){
            perror("recvfrom");
            return -1;
        }
   
        //buf[size] = '\0'; /* '\0' means final character*/
        printf("IP:%s msg: %s\n", inet_ntoa(client_addr.sin_addr), buf);
   
        /*If the buffer message is "quit", we will close socket fd and end the process*/
        if(strcmp(buf, "quit") == 0){
            printf("system quit!\n");
            close(sockfd);
            return 0;
        }
    }
   
    close(sockfd);
    return 0;
}

static void menu_set(struct pri_data *pri_data)
{
    switch (pri_data->cmd) {
    case EMENU_TEST:
    	printf("test %d\n", EMENU_TEST);
        break;
    case EMENU_SVR:
	printf("server udp:\n");
	svr_udp(pri_data);
    	break;
    case EMENU_CLIENT:
	printf("client udp:\n");
	client_udp(pri_data);
    	break;
    case EMENU_TIME_TBL:

    default:
        times_table(pri_data);
    }
}

void main(int argc, const char *argv[])
{
    struct pri_data pri_data;

    memset(&pri_data, 0, sizeof(pri_data));

    if (_input_check(argc, argv, &pri_data))
	return;

    menu_set(&pri_data);
    //times_table(&pri_data);

}
