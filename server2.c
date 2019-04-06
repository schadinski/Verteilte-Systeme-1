#include "chatLib.h"

int main()
{
  int readBytes;
  int events;
  fd_set readset;
  char* nickname; 
  int serverFD = 0;
  int peerFD = 0;
  unsigned int nameLen;
  size_t nameChars = 0;
  unsigned int len;
  
  nickname = malloc( 13*sizeof(char));
  //server part
  if ((	serverFD = socket( AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    perror("socket: ");
  }
  
  struct sockaddr_in localAddr;
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons(2000);
  localAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
      
  int rc = bind(serverFD, (struct sockaddr*)&localAddr, sizeof(localAddr));
  if (rc < 0)
  {
    printf("Error: Bind FD\n");
    perror("bind()");
    rc = 0;
  }
  //server part end
  
  
  //peer part
  peerFD = socket( AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in peerAddr;
  memset(&peerAddr, 0, sizeof(peerAddr));
  peerAddr.sin_family = AF_INET;
  peerAddr.sin_port = htons(3000);
  peerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //peer part end

  //get user input
  printf("Enter name:");
  nameChars = getline(&nickname, &nameLen, stdin);
  //printf("length of name is %d\n", nameChars);
  
  sendEntry(serverFD, nickname, peerAddr);
  
  FD_ZERO(&readset);

   while(1)
   {
     char* buf2 = malloc(2096* sizeof(char));
     FD_SET(serverFD, &readset);
     FD_SET(0, &readset);
     
     events = select(serverFD+1, &readset, 0, 0, 0);
     if(FD_ISSET(serverFD,&readset))
     {    
	recvPeerMsg(serverFD); 
     }
     if(FD_ISSET(STDIN_FILENO,&readset))
     {
       size_t chars = 0;
       chars = getline(&buf2, &len, stdin);
       //printf("chars no %d\n", chars);
       if(strstr(buf2, "!Exit"))
       {
	 sendExit(serverFD, nickname, peerAddr);
	 break;
       }
       else
       {
       sendMsg(serverFD, nickname, buf2, peerAddr);
       }
     }
     free(buf2);
   }
 free(nickname);
 return 0;
}

