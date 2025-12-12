#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "util/rsleep.h"
#include <cstring>
using namespace std;

volatile sig_atomic_t PV;
volatile bool isVador=true;
pid_t pid;


void onUSRatk(int sig){
    PV=PV-1;

    if(isVador){
        const char* msg = "Vador : ...\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    }
    else{
        const char* msg ="Luke : ARRRGHHH\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    }
}

void onUSRdef(int sig){
    const char* msg =" Luke : Coup paré\n";
    write(STDOUT_FILENO, msg, strlen(msg));
}

void attaque (pid_t adversaire){
    struct sigaction sa;
    sa.sa_handler=&onUSRatk;
    sigfillset(&sa.sa_mask);
    sa.sa_flags=0;
    if(sigaction(SIGUSR1,&sa,nullptr)<0){
        perror("sigaction");
        exit(2);
    }
    
    if(kill(adversaire,SIGUSR1)==-1){
        //useless maintenant
        if(isVador){
        const char* msg ="Vador : Luke, I am your father\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        }
        else{
            const char* msg = "Luke : NOOOOOOOOO\n";
            write(STDOUT_FILENO, msg, strlen(msg));
        }
        exit(0);
    }
    pr::randsleep();
}

void defenseVador(){
    struct sigaction sa;
    sa.sa_handler=SIG_IGN;
    sigfillset(&sa.sa_mask);
    sa.sa_flags=0;
    if(sigaction(SIGUSR1,&sa,nullptr)<0){
        perror("sigaction");
        exit(2);
    }
    const char* msg ="Vador is defending\n ";
    write(STDOUT_FILENO, msg, strlen(msg));
    pr::randsleep();
}

void defenseLuke(){
    struct sigaction sa;
    sa.sa_handler=&onUSRdef;
    sigfillset(&sa.sa_mask);
    sa.sa_flags=0;
    if(sigaction(SIGUSR1,&sa,nullptr)<0){           //Avantage de la defense de Luke par rapport à la defense normal
        perror("sigaction");                        //La defense normal protege pour une duree random puis se met en mode vulnerable(attaque)
        exit(2);                                    //La defense de Luke attend qu'il recoit un coup pour pouvoir passer en attaque
    }                                               //Donc, le combat n'est plus equitable Luke a clairement l'avantage.
    // masquage
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    
    // 3. sleep
    pr::randsleep();

    // 4. Suspendre pour recevoir le signal en attente
    sigset_t suspendMask;
    sigemptyset(&suspendMask);
    // Si Vador atk pendantsleep, 
    // onUSRdef s'exécute.
    sigsuspend(&suspendMask);

    // 5. Restaurer le masque d'origine
    sigprocmask(SIG_SETMASK, &oldmask, nullptr);
}

void combat(pid_t adversaire){
    PV=3;
    int status;
    while(PV>0){
        if(isVador){
            defenseVador();
        }else{
            defenseLuke();
        }
        attaque(adversaire);
        if(isVador){
            if(waitpid(pid, &status, WNOHANG)>0)exit(0);
        }
    }
    if(!isVador){
        const char* msg = "Luke : This can't be..., pas d'autre Star Wars apres ca.\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    }
    else{
        const char* msg = "Vador : uuhhhh....., c'est ainsi qu'Anakin perit.\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        //forcer luke a crever car il casse le terminal.
        kill(pid,SIGKILL);
        wait(NULL);
    }
    exit(0);
}

int main() {
    pid=fork();
    if (pid <0) perror("fork");
    cout<<"je suis pid ="<< getpid() << "fork rend=" << pid << endl;
    if(pid==0){
        //fils
        cout<<"je suis Luke"<<endl;
        isVador=false;
        combat(getppid());
        return 0;
    }
   // pause();
    combat(pid);

    if(isVador){
        wait(nullptr);
    }
    return 0;
}