#include "pipe.h"

#include <fcntl.h>
#include <unistd.h>
#include <limits.h> // For PIPE_BUF
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <signal.h>
#include <algorithm>

namespace pr {

// Place this in shared memory !    
struct PipeShm {
    char buffer[PIPE_BUF];
    size_t head; // write position
    size_t tail; // read position
    size_t count; // number of bytes in the pipe
    size_t nbReaders; // number of readers
    size_t nbWriters; // number of writers
    // TODO : semaphores
    sem_t semMut;
    sem_t canRead;
    sem_t canWrite;
};

// This is per-process handle, not in shared memory
struct Pipe {
    PipeShm *shm; // pointer to shared memory
    int oflags; // O_RDONLY or O_WRONLY
};

int pipe_create(const char *name) {
    // Construct shared memory name
    char shm_name[256];
    // add a '/' at the beginning for shm_open
    snprintf(shm_name, sizeof(shm_name), "/%s", name);
    
    int fd;
    //Création du segment 
    if((fd = shm_open(shm_name, O_RDWR|O_CREAT|O_EXCL, 0666)) == -1){
        perror("shm_open");
        exit(1);
    }

    //Allocation de la taille du segment ppur stocker un pipeshm
    if(ftruncate(fd, sizeof(PipeShm)) == -1){
        perror("ftruncate");
        exit(1);
    }
    
    //Mapper le segment en R/W partagé
    PipeShm* shm;
    if((shm=(PipeShm*)mmap(NULL, sizeof(PipeShm), PROT_READ|PROT_WRITE,MAP_SHARED,fd,0))== MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    memset(shm, 0, sizeof(PipeShm));

    //Initialisation des valeurs
    sem_init(&shm->semMut, 1, 1);
    sem_init(&shm->canRead, 1, 0);
    sem_init(&shm->canWrite, 1, 0);
    shm-> head = 0; // write position
    shm -> tail = 0; // read position
    shm -> count = 0; // number of bytes in the pipe
    shm ->  nbReaders = 0; // number of readers
    shm ->  nbWriters = 0;

    

    close(fd);
    munmap(shm, sizeof (PipeShm));
    
    
    // Try to create shared memory with O_CREAT|O_EXCL
    // Set size of shared memory
    // Map the shared memory 
    // Initialize the PipeShm structure
    // memset(shm, 0, sizeof(PipeShm));

    // Including semaphores

    // Unmap and close (setup persists in shared memory)
    
    return 0;
}

Pipe * pipe_open(const char *name, int oflags) {
    // Construct shared memory name
    char shm_name[256];
    snprintf(shm_name, sizeof(shm_name), "/%s", name);

    int fd;
    //Ouverture de la mémoire partagé
    if((fd = shm_open(shm_name, O_RDWR, 0666)) == -1){
        perror("shm_open");
        exit(1);
    }

    //Mapper le segment  partagé
    PipeShm* shm;
    if((shm=(PipeShm*)mmap(NULL, sizeof(PipeShm), PROT_READ|PROT_WRITE,MAP_SHARED,fd,0))== MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    close(fd);

    if(oflags == O_RDONLY){
        sem_wait(&shm -> semMut);
        shm -> nbReaders++;
        sem_post(&shm -> semMut);
    }
    else{
        sem_wait(&shm -> semMut);
        shm -> nbWriters++;
        sem_post(&shm -> semMut);
    }
    
    // Open shared memory (without O_CREAT)
    // Map the shared memory
    // Can close fd after mmap
    // Increment nbReaders or nbWriters
    
    // Create and return Pipe handle
    Pipe *handle = new Pipe{shm, oflags}; // new Pipe(...);
    return handle;
}

ssize_t pipe_read(Pipe *handle, void *buf, size_t count) {
    if (handle == nullptr || handle->oflags != O_RDONLY) {
        errno = EBADF;
        return -1;
    } else if (count > PIPE_BUF) {
        errno = EINVAL;
        return -1;
    }
    
    // wait until some data available or no writers
    while(true){    
        sem_wait(&handle->shm ->semMut);
        
        if(handle ->shm ->count > 0 || handle ->shm ->nbWriters == 0){
            break;
        }
        sem_post(&handle->shm->semMut); //tube vide & ecrivains actifs
        sem_wait(&handle->shm->canRead);
    }
    // Check if pipe is empty and no writers : EOF
    if (handle->shm->count == 0 && handle->shm->nbWriters == 0) {
        sem_post(&handle->shm->canRead); 
        sem_post(&handle->shm->semMut);
        return 0;
    }

    // Read min(count, shm->count) bytes
    PipeShm *shm = handle->shm;
    size_t to_read = std::min(count, shm->count);
    char *output = (char *)buf;
    
    // Handle circular buffer: may need to copy in two parts
    size_t first_chunk = std::min(to_read, PIPE_BUF - shm->tail);
    memcpy(output, &shm->buffer[shm->tail], first_chunk);
    
    if (first_chunk < to_read) {
        // Wrap around to beginning of buffer
        memcpy(output + first_chunk, &shm->buffer[0], to_read - first_chunk);
    }
    
    shm->tail = (shm->tail + to_read) % PIPE_BUF;
    shm->count -= to_read;
    
    // warn other readers/writers if needed
    sem_post(&handle->shm ->canWrite); //data consomme donc j'ai retirer -> on peut ecrire
    if(handle ->shm ->count > 0){
        sem_post(&handle->shm ->canRead); // au autre lecteur -> on peut lire
    }
    sem_post(&handle->shm ->semMut);
    return to_read;
}

ssize_t pipe_write(Pipe *handle, const void *buf, size_t count) {
    if (handle == nullptr || handle->oflags != O_WRONLY) {
        errno = EBADF;
        return -1;
    } else if (count > PIPE_BUF) {
        errno = EINVAL;
        return -1;
    }
    
    PipeShm *shm = handle->shm;
    
    // wait until *enough* space available or no readers
    while(true){    
        sem_wait(&shm ->semMut);
        size_t free_space = PIPE_BUF - shm->count; // espace libre restant dans le buffer du pipe
        if(free_space >= count || shm ->nbReaders == 0){
            break;
        }
        sem_post(&shm->semMut); //tube vide & lecteurs actifs
        sem_wait(&shm->canWrite);
    }
    // Check if no readers => SIGPIPE
    if (shm->nbReaders == 0) {
        sem_post(&shm->semMut);
        kill(getpid(), SIGPIPE);
        errno = EPIPE; 
        return -1;
    }
    // Write count bytes
    const char *input = (const char *)buf;
    
    // Handle circular buffer: may need to copy in two parts
    size_t first_chunk = std::min(count, PIPE_BUF - shm->head);
    memcpy(&shm->buffer[shm->head], input, first_chunk);
    
    if (first_chunk < count) {
        // Wrap around to beginning of buffer
        memcpy(&shm->buffer[0], input + first_chunk, count - first_chunk);
    }
    
    shm->head = (shm->head + count) % PIPE_BUF;
    shm->count += count;
    
    // warn other readers/writers if needed
    sem_post(&shm ->canRead); //data mis a dispo donc -> on peut lire
    if (PIPE_BUF - shm->count > 0) { // au autre ecrivains -> on peut ecrire
        sem_post(&shm->canWrite);
    }
    sem_post(&shm ->semMut);

    return count;
}

int pipe_close(Pipe *handle) {
    if (handle == nullptr) {
        errno = EBADF;
        return -1;
    }
    
    PipeShm *shm = handle->shm;
    
    // Decrement reader or writer count
    // Warn other process as needed (e.g. if last reader/writer)
        sem_wait(&shm -> semMut);
        
        if(handle->oflags == O_RDONLY){
            shm -> nbReaders--;
            if(shm -> nbReaders == 0){
                sem_post(&shm->canWrite);
            }
        }
        else{
            shm -> nbWriters--;
            if(shm -> nbWriters == 0){
                sem_post(&shm->canRead);
            }
        }
        
        sem_post(&shm -> semMut);
    
    // Unmap memory
    if (munmap(shm, sizeof(PipeShm)) == -1) {
        perror("munmap");
    }
    // Free handle
    delete handle;
    return 0;
}

int pipe_unlink(const char *name) {
    // Construct shared memory name
    char shm_name[256];
    snprintf(shm_name, sizeof(shm_name), "/%s", name);
    // Unlink shared memory (this also destroys the embedded semaphores)
    
    int fd;
    
    if((fd = shm_open(shm_name, O_RDWR, 0)) == -1){
        perror("shm_open");
        exit(1);
    }

    PipeShm* shm;
    if((shm=(PipeShm*)mmap(NULL, sizeof(PipeShm), PROT_READ|PROT_WRITE,MAP_SHARED,fd,0))== MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    sem_destroy(&shm->canRead);
    sem_destroy(&shm->canWrite);
    sem_destroy(&shm->semMut);

    close(fd);
    munmap(shm, sizeof (PipeShm));

    shm_unlink(shm_name);
    return 0;
}

} // namespace pr

