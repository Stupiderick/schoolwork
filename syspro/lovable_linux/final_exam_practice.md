# Angrave's 2018 Acme-CS241-Exam Prep		
## AKA Preparing for the Final Exam & Beyond CS241...

Some of the questions require research (wikibook; websearch; wikipedia).
It is ok to work together & discuss answers!
Be ready to discuss and clear confusions & misconceptions in your last discussion section.
The final will also include pthreads, fork-exec-wait questions and virtual memory address translation.
Be awesome. Angrave.

## 1. C


1.	What are the differences between a library call and a system call? Include an example of each.
    * Library calls are implemented in user space, while system calls are defined in kernel. When calling system calls, some special system instructions are used.
    * Library call example:
    ```c
    printf("hello world\n");
    ```
    * System call example:
    ```c
    write(1, "hello world\n", 13);
    ```


2.	What is the `*` operator in C? What is the `&` operator? Give an example of each.
    * `*` means dereference of a pointer, it can get the value the pointer pointing at.
    ```c
    int a = 7;
    int *b = &a;
    printf("val of b is: %d\n", *b);
    ```
    * `&` can get the address of the variable.
    ```c
    int a = 5;
    printf("the address of a is %p\n", &a);
    ```


3.	When is `strlen(s)` != `1+strlen(s+1)` ?
    * When `\0` is at idx 0, e.g.:
    ```c
    char *str = "\0abcdefg";
    // strlen(str) == 0;
    // 1 + strlen(str + 1) == 8;
    ```


4.	How are C strings represented in memory? What is the wrong with `malloc(strlen(s))` when copying strings?
    * In C, strings are represented as char pointers pointing to the first char, and use null bytes as tails.
    * strlen(s) does not count the null byte in the end. Thus, calling `malloc(strlen(s))` will not reserve enough seats for a string. `malloc(strlen(s) + 1)` will work instead.


5.	Implement a truncation function `void trunc(char*s,size_t max)` to ensure strings are not too long with the following edge cases.
```
if (length < max)
    strcmp(trunc(s, max), s) == 0
else if (s is NULL)
    trunc(s, max) == NULL
else
    strlen(trunc(s, max)) <= max
    // i.e. char s[]="abcdefgh; trunc(s,3); s == "abc".
```


6.	Complete the following function to create a deep-copy on the heap of the argv array. Set the result pointer to point to your array. The only library calls you may use are malloc and memcpy. You may not use strdup.

    ```java
    void duplicate(char **argv, char ***result) {
        int trav = 0;
        result = malloc(sizeof(char **));
        *result = malloc(sizeof(char *) * argc);
        char **ret = *result;
        while (argv[trav] != NULL) {
            **ret = malloc(strlen(argv[trav]) + 1);
            memcpy(argv[trav], **ret, strlen(argv[trav]) + 1);
            *ret++;
        }
    }
    ```


7.	Write a program that reads a series of lines from `stdin` and prints them to `stdout` using `fgets` or `getline`. Your program should stop if a read error or end of file occurs. The last text line may not have a newline char.
```c
#include <stdio.h>
int main() {
    char p;
    while (fgets(&p, 1, stdin)) != -1) {
        if (p != '\n') {
            printf("%c", p);
        } else {
            continue;
        }
    }
    return 0;
}
```

## 2. Memory

1.	Explain how a virtual address is converted into a physical address using a multi-level page table. You may use a concrete example e.g. a 64bit machine with 4KB pages
    * There is a MMU (memory management unit) in CPUs, and it takes care of the memory conversion. Memory is stored in blocks, and the unit can use multiple levels of page tables to store the virtual memory. There is also a TLB (translation lookaside buffer) to keep the just-found conversion for the sake of shorter running time.

2.	Explain Knuth's and the Buddy allocation scheme. Discuss internal & external Fragmentation.
    * Buddy allocator first use 64kb spaces, then if the requested memory is smaller than the allocated spaces, the algorithm will break down the allocated spaces for the request.

3.	What is the difference between the MMU and TLB? What is the purpose of each?
    * MMU converts virtual memory addresses to physical memory addresses. It will also interrupt CPU if processes try to access read-only memory.
    * TLB is a cache. When there are requests to converting a virtual address to physical address, it will be queried to find if the conversion has been cached. If so, it will make the translation faster, because seeking times are reduced.

4.	Assuming 4KB page tables what is the page number and offset for virtual address 0x12345678?
    * 4KB is 2^12 bytes, so 12345 is the page number and offset is 678.

5.	What is a page fault? When is it an error? When is it not an error?
    * Page faults will raise when running programs are trying to access a virtual address that is not mapped to a physical address.
    * Page faults are not an error when there is no mapping for the page, but it is a valid address.

6.	What is Spatial and Temporal Locality? Swapping? Swap file? Demand Paging?
    * temporal locality is when you access the same memory address multiple times within a relatively small time period
    * spatial locality is accessing elements in close storage locations (addresses close to each other) (like traversing an array)
    * swapping – when a process is swapped out of main memory to a backing store (to the disk) and vice versa
    * swap file – a space on a hard disk used as the virtual memory extension of a computer's real memory (where the swapped out page goes to on disk)
    * demand paging – a type of swapping where pages are not copied from disk to memory until needed


## 3. Processes and Threads

1.	What resources are shared between threads in the same process?
    * Address space
    * Global variables
    * Heap
    * File descriptors

2.	Explain the operating system actions required to perform a process context switch


3.	Explain the actions required to perform a thread context switch to a thread in the same process
    * When you have more threads than CPU, each threads will be run in a short time by one CPU.

4.	How can a process be orphaned? What does the process do about it?

5.	How do you create a process zombie?

6.	Under what conditions will a multi-threaded process exit? (List at least 4)

## 4. Scheduling
1.	Define arrival time, pre-emption, turnaround time, waiting time and response time in the context of scheduling algorithms. What is starvation?  Which scheduling policies have the possibility of resulting in starvation?
    * arrival time: when the process arrives at the queue.
    * pre-emption: the process can be forcefully taken out from CPU before it is finished.
    * turnaround time: end time - arrival time.
    * waiting time: turnaround - runtime.
    * response time: start time - arrival time.

2.	Which scheduling algorithm results the smallest average wait time?
    * pre-emptive SJF

3.	What scheduling algorithm has the longest average response time? shortest total wait time?
    * Longest avg response time: FCFS
    * Shortest total wait time: Pre-emptive SJF

4.	Describe Round-Robin scheduling and its performance advantages and disadvantages.
    * advantages: it can be fair, and it can avoid convey effect. Short waiting time.
    * disadvantages: it has large overhead for switches.

5.	Describe the First Come First Serve (FCFS) scheduling algorithm. Explain how it leads to the convoy effect.

6.	Describe the Pre-emptive and Non-preemptive SJF scheduling algorithms.

7.	How does the length of the time quantum affect Round-Robin scheduling? What is the problem if the quantum is too small? In the limit of large time slices Round Robin is identical to _____?
    * FCFS

8.	What reasons might cause a scheduler switch a process from the running to the ready state?

## 5. Synchronization and Deadlock

1.	Define circular wait, mutual exclusion, hold and wait, and no-preemption. How are these related to deadlock?
    * circular wait: loops in resource allocation graphs;
    * mutual exclusion: resource cannot be shared by multiple threads;
    * hold and wait: when a thread grabs a resource, it will hold it and wait for the next resource.
    * no preemption: once a process gets a resource, it cannot be taken away forcefully.
    * If all four conditions are true, a deadlock can appear.

2.	What problem does the Banker's Algorithm solve?
    * break circular wait.

3.	What is the difference between Deadlock Prevention, Deadlock Detection and Deadlock Avoidance?

4.	Sketch how to use condition-variable based barrier to ensure your main game loop does not start until the audio and graphic threads have initialized the hardware and are ready.
```c
pthread_mutex_lock(&m);
if (initialized_machine) {
    pthread_cond_broadcast(&cv);
} else {
    pthread_cond_wait(&cv, &m);
}
pthread_mutex_unlock(&m);
```

5.	Implement a producer-consumer fixed sized array using condition variables and mutex lock.

6.	Create an incorrect solution to the CSP for 2 processes that breaks: i) Mutual exclusion. ii) Bounded wait.

7.	Create a reader-writer implementation that suffers from a subtle problem. Explain your subtle bug.

## 6. IPC and signals

1.	Write brief code to redirect future standard output to a file.

2.	Write a brief code example that uses dup2 and fork to redirect a child process output to a pipe

3.	Give an example of kernel generated signal. List 2 calls that can a process can use to generate a SIGUSR1.

4.	What signals can be caught or ignored?

5.	What signals cannot be caught? What is signal disposition?
    * SIGKILL;
    * SIGSTOP

6.	Write code that uses sigaction and a signal set to create a SIGALRM handler.
```c
void setoff_alarm() {
    // do smth
}

int main() {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = setoff_alarm;
    if (sigaction(SIGALRM, &act, NULL) < 0) {
        perror(“sigaction”);
        return 1;
    }
    return 0;
}

```

7.	Why is it unsafe to call printf, and malloc inside a signal handler?
    * printf: operates on global data (global buffer on output stream), so is not re-entrant safe
    * malloc: operates on global data, so 2 different invocations could return the same memory block

## 7. Networking

1.	Explain the purpose of `socket`, `bind`, `listen`, and `accept` functions
    * socket: To create a endpoint for networking communication;
    * bind: associates an abstract socket with an actual network interface and port;
    * listen:  specifies the queue size for the number of incoming, unhandled connections;
    * accept: to wait for new connections.

2.	Write brief (single-threaded) code using `getaddrinfo` to create a UDP IPv4 server. Your server should print the contents of the packet or stream to standard out until an exclamation point "!" is read.
```c
struct addrinfo hints, *result;
memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_DGRAM;
hints.ai_flags = AI_PASSIVE;

int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

getaddrinfo(NULL; "300", &hints, &result);
if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
    perror("bind()");
    exit(1);
}

struct sockaddr_storage addr;
int addrlen = sizeof(addr);

while (1) {
    char buf[1024];
    ssize_t byte_count = recvfrom(sockfd, buf, sizeof(buf), 0, &addr, &addrlen);
    buf[byte_count] = '\0';

    for (size_t i = 0; i < byte_count; i++) {
        if (buf[i] == '!') {
            break;
        }
        printf("%c", buf[i]);
    }
}

return 0;
```

3.	Write brief (single-threaded) code using `getaddrinfo` to create a TCP IPv4 server. Your server should print the contents of the packet or stream to standard out until an exclamation point "!" is read.
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
struct addrinfo hints, *result;
memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;

getaddrinfo(NULL, "8080", &hints, &result);
bind(sockfd, result->ai_addr, result->ai_addrlen);
listen(sockfd, 10);
int clientfd = accept(sockfd, NULL, NULL);

while (1) {
    char buf[1024];
    ssize_t byte_count = read(sockfd, buf, sizeof(buf), 0, &addr, &addrlen);
    buf[byte_count] = '\0';

    for (size_t i = 0; i < byte_count; i++) {
        if (buf[i] == '!') {
            break;
        }
        printf("%c", buf[i]);
    }
}

return 0;
```

4.	Explain the main differences between using `select` and `epoll`. What are edge- and level-triggered epoll modes?
    * Edge-triggered: you will only be notified when it's available to read and it was not read yet.
    * Level-triggered: you will be notified when there it's available to read.

5.	Describe the services provided by TCP but not UDP.
    * TCP: chatroom

6.	How does TCP connection establishment work? And how does it affect latency in HTTP1.0 vs HTTP1.1?
    * TCP uses three-way handshake. (SYN, SYN-ACK, ACK)

7.	Wrap a version of read in a loop to read up to 16KB into a buffer from a pipe or socket. Handle restarts (`EINTR`), and socket-closed events (return 0).
```c
ssize_t read_all_from_socket(int sockfd, char *buffer, size_t count) {
    ssize_t counter = 0;
    while (counter != (ssize_t)count) {
        int ret_code = read(sockfd, buffer + counter, count - counter);
        if (ret_code == 0) {
            return counter;
        } else if (ret_code > 0) {
            counter += ret_code;
        } else if (ret_code == -1 && errno == EINTR) {
            continue;
        } else {
            return -1;
        }
    }
    return counter;
}
```

8.	How is Domain Name System (DNS) related to IP and UDP? When does host resolution not cause traffic?
    * DNS servers receive UDP packets, and it wll send back IP addresses. If the hostname is in the cache.

9.	What is NAT and where and why is it used?
    * It is Network Address Translation. It is a method to remapping one IP address space into another by modifying network address information in the IP header.

## 8. Files

1.	Write code that uses `fseek`, `ftell`, `read` and `write` to copy the second half of the contents of a file to a `pipe`.
```java
ssize_t read_all_from_file(int fd, char *buffer, size_t count) {
    ssize_t counter = 0;
    while (counter != (ssize_t)count) {
        int ret_code = read(fd, buffer + counter, count - counter);
        if (ret_code == 0) {
            return counter;
        } else if (ret_code > 0) {
            counter += ret_code;
        } else if (ret_code == -1 && errno == EINTR) {
            continue;
        } else {
            return -1;
        }
    }
    return counter;
}

ssize_t write_all_to_pipe(int fd, const char *buffer, size_t count) {
    ssize_t counter = 0;
    while (counter != (ssize_t)count) {
        ssize_t ret_code = write(fd, buffer + counter, count - counter);
        if (ret_code == 0) {
            return counter;
        } else if (ret_code > 0) {
            counter += ret_code;
        } else if (ret_code == -1 && errno == EINTR) {
            continue;
        } else {
            return -1;
        }
    }
    return counter;
}

int filedes[2];
pipe(filedes);
FILE *myfile = fopen(filename, "r+");
int fd = fileno(myfile);
fseek(myfile, 0, SEEK_END);
long filesize = ftell(myfile);
long halfsize = filesize / 2;
fseek(myfile, halfsize, SEEK_SET);
for (size_t i = halfsize; i < filesize; i++) {
    size_t count = 0;
    void *buf;
    while (count != filesize - halfsize) {
        buf = malloc(23000);
        size_t bytes_read = read_all_from_file(fd, buf, 23000);
        count += bytes_read;
        write_all_to_pipe(filedes[1], buf, bytes_read);
        free(buf);
    }
}
fclose(myfile);
```

2.	Write code that uses `open`, `fstat`, `mmap` to print in reverse the contents of a file to `stderr`.

3.	Write brief code to create a symbolic link and hard link to the file /etc/password
```c
symlink("/etc/password", "/my/path/");
```
```c
link("/etc/password", "/my/path/");
```

4.	"Creating a symlink in my home directory to the file /secret.txt succeeds but creating a hard link fails" Why?

5.	Briefly explain permission bits (including sticky and setuid bits) for files and directories.
    * sticky bit is used for setting permission of renaming or deleting files to owners or root user.

6.	Write brief code to create a function that returns true (1) only if a path is a directory.
```c
int is_dir(char *path) {
    struct stat s;
    stat(path, &s);
    return S_ISDIR(s.st_mode);
}
```

7.	Write brief code to recursive search user's home directory and sub-directories (use `getenv`) for a file named "xkcd-functional.png' If the file is found, print the full path to stdout.

8.	The file 'installmeplz' can't be run (it's owned by root and is not executable). Explain how to use sudo, chown and chmod shell commands, to change the ownership to you and ensure that it is executable.
    * `sudo ./installmeplz`
    * `chown user_id installmeplz`
    * `chmod 4000 installmeplz`

## 9. File system
Assume 10 direct blocks, a pointer to an indirect block, double-indirect, and triple indirect block, and block size 4KB.

1.	A file uses 10 direct blocks, a completely full indirect block and one double-indirect block. The latter has just one entry to a half-full indirect block. How many disk blocks does the file use, including its content, and all indirect, double-indirect blocks, but not the inode itself? A sketch would be useful.
    * Direct blocks: 10;
    * Indirect block: 4KB/4B * 1/2 = 2K;
    * total usage: 2K + 10 blocks

2.	How many inode reads are required to fetch the file access time at /var/log/dmesg ? Assume the inode of (/) is cached in memory. Would your answer change if the file was created as a symbolic link? Hard link?
    * 3 inode reads.

3.	What information is stored in an i-node?  What file system information is not?
    * file size;
    * permission;
    * change/access/modification time;
    * uid;
    * gid;
    * i-node number

4.	Using a version of stat, write code to determine a file's size and return -1 if the file does not exist, return -2 if the file is a directory or -3 if it is a symbolic link.
```c
    ssize_t find_file_size(char *filename) {
        struct stat buf;
        if (lstat(filename, &buf) != 0) {
            return -1;
        }
        if (S_IFDIR(buf.st_mode)) {
            return -2;
        }
        if (S_ISLNK(buf.st_mode)) {
            return -3;
        }
        return buf.st_size;
    }
```

5.	If an i-node based file uses 10 direct and n single-indirect blocks (1 <= n <= 1024), what is the smallest and largest that the file contents can be in bytes? You can leave your answer as an expression.
    * Smallest: 0 bytes
    * Largest: 10 * 4KB + 1K * 4K * 1K = 4G + 40K

6.	When would `fstat(open(path,O_RDONLY), &s)` return different information in s than `lstat(path, &s)`?
    * when `&s` is refering to a symbolic link.

## 10. "I know the answer to one exam question because I helped write it"

Create a hard but fair 'spot the lie/mistake' multiple choice or short-answer question. Ideally, 50% can get it correct.
