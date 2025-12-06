#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
int main(int argc, char **argv) {

  // Parse command line arguments: find the pipe separator "|"
  // Format: ./pipe cmd1 [args...] | cmd2 [args...]
  char* buff1[argc];
  char* buff2[argc];

  bool isPiped = false;

  char* func1 = argv[1];  //recupere la cmd 1
  buff1[0] = func1;
  int a1 = 1;

  char* func2 = nullptr;
  int a2 = 0;
  
  for(int i = 2; i < argc; i++){
    if(!strcmp(argv[i],"|")){
      isPiped = true;           // on rencontre le pipe
      buff1[a1] = nullptr;      // donc on s'arrete de remplir le buffer 1 (isPiped passe a 1)
      if(i+1 < argc){           // et on recupere la cmd 2 comme au debut
        func2 = argv[i+1];
        buff2[0] = func2;
        a2 = 1;
        i++;
      }
      continue;
    }

    if(!isPiped){
      buff1[a1] = argv[i];      // on remplie le buffer d'argument de
      a1++;                     // la premiere commande
    }
    else{
      buff2[a2] = argv[i];      // on remplie le buffer d'argument de 
      a2++;                     // la deuxieme commande
    }
  }

  if(isPiped){                  // on a fini de remplir le buffer 2
    buff2[a2] = nullptr;        // donc on le bouchonne avec un nullptr
  }
  else{
    cerr<<"pas de pipe dans la ligne, IDIOT"<<endl;
    exit(1);
  }

  if(func1 == nullptr ||func2 == nullptr ){
    cerr<<"sah abandonne"<<endl;
    exit(1);
  }
  

  // Creation du pipe
  int pipefd[2];
  if(pipe(pipefd)==-1){
    cerr<<"pb d'ouverture pipe"<<endl;
    exit(1);
  }

  // Fork pour enfant 1 ; child redirige son OUT vers le WRITE du pipe, puis exec
  int f1 = fork();
  if(f1==0){//fils 1
    close(pipefd[0]);
    if(dup2(pipefd[1], STDOUT_FILENO)==-1){
      cerr<<"pb de changement de sotie pipe"<<endl;
      exit(1);
    }
    close(pipefd[1]);
    execvp(func1,buff1);
  }

  // Fork pour enfant 2 ; child redirige le READ du pipe vers son IN, puis exec
  int f2 = fork();
  if(f2==0){//fils 1
    close(pipefd[1]);
    if(dup2(pipefd[0], STDIN_FILENO)==-1){
      cerr<<"pb de changement de sortie pipe"<<endl;
      exit(1);
    }
    close(pipefd[0]);
    execvp(func2,buff2);
  }

  // On oublie pas de ferme le pipe puis d'attendre les fils
  close(pipefd[0]);
  close(pipefd[1]);
  wait(0);
  wait(0);

  return 0;
}