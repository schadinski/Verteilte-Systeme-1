#include "chatLib.h"

int main(int argc, char *argv[])
{
//  printf("first line \n");
  if ( argv[1] == 0 )
    printf("no ip specified\n");

  int events;
  fd_set readset;
  char* nickname;
  size_t nameLen;
  size_t nameChars = 0;
  size_t len;
  unsigned int localPort =50123;
  int i;
  int peerFDs[MAXPEERS];
  struct sockaddr_in allPeerAddrs[MAXPEERS];
  char* myIP;
  int localFD;
  
//  printf("after var init\n");
  nickname = malloc( 13*sizeof(char));
//  printf("start\n");
  // setup all peer addresses
  for(i = 0; i < MAXPEERS; i++)
  {
    peerFDs[i] = socket( AF_INET, SOCK_DGRAM, 0);
    if(peerFDs[i] < 0)
    {
      printf("Error: socket peerFDs[%d]\n", i);
      perror("socket peerFDs:");
    }
    
    allPeerAddrs[i] = *(struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    memset(&allPeerAddrs[i], 0, sizeof(allPeerAddrs[i]));
    
    allPeerAddrs[i].sin_family = AF_INET;
    allPeerAddrs[i].sin_port = htons(localPort);
    allPeerAddrs[i].sin_addr.s_addr = inet_addr(ips[i]);
    // filter local ip and bind it
    if(strcmp(ips[i],argv[1]) == 0)
    {
      localFD = peerFDs[i];
      int rc = bind(localFD, (struct sockaddr*)&allPeerAddrs[i], sizeof(allPeerAddrs[i]));
      if (rc < 0)
      {
	printf("Error: Bind local FD\n");
	perror("bind()");
      }
    }
  }

  //get user input
  while(nameChars == 0)
  {
    printf("Enter name:");
    nameChars = getline(&nickname, &nameLen, stdin);
  }
  
  for(i=0; i<(MAXPEERS); i++)
  {
  sendEntry(localFD, nickname, allPeerAddrs[i]);
  //printf("after sendEntry in main\n");
  }
  FD_ZERO(&readset);
  
  // leave with !Exit
   while(1)
   {
     char* buf2 = malloc(2096* sizeof(char));
     FD_SET(localFD, &readset);
     FD_SET(0, &readset);
     
     events = select(localFD+1, &readset, 0, 0, 0);
     if(FD_ISSET(localFD,&readset))
     {    
	recvPeerMsg(localFD);
     }
     if(FD_ISSET(STDIN_FILENO,&readset))
     {
       getline(&buf2, &len, stdin);
       if(strstr(buf2, "!Exit"))
       {
	 for(i=0;i<(MAXPEERS);i++)
	 {
	 sendExit(localFD, nickname, allPeerAddrs[i]);
	 }
	 break;
       }
       else
       {
	  for(i=0;i<(MAXPEERS);i++)
	  { 
	    sendMsg(localFD, nickname, buf2, allPeerAddrs[i]);
	  }
       }
     }
     free(buf2);
   }
 for(i=0; i<(MAXPEERS);i++)
 {
   close(peerFDs[i]);
 }
 close(localPort);
 free(nickname);
 return 0;
}