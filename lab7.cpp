#include <stdio.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    struct utsname info;

    // Fetch system info
    if (uname(&info) == -1) {
        perror("uname");
        return 1;
    }

    printf("System Information:\n");
    printf("  Sysname : %s\n", info.sysname);
    printf("  Nodename: %s\n", info.nodename);
    printf("  Release : %s\n", info.release);
    printf("  Version : %s\n", info.version);
    printf("  Machine : %s\n\n", info.machine);

    // Open file
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Write message
    char buffer[100];
    int len = sprintf(buffer, "Hello from PID = %d\n", getpid());
    write(fd, buffer, len);
    close(fd);

    // Display file permissions
    struct stat st;
    if (stat("output.txt", &st) == -1) {
        perror("stat");
        return 1;
    }

    printf("File: output.txt\n");
    printf("Permissions: ");

    printf( (st.st_mode & S_IRUSR) ? "r" : "-");
    printf( (st.st_mode & S_IWUSR) ? "w" : "-");
    printf( (st.st_mode & S_IXUSR) ? "x" : "-");

    printf( (st.st_mode & S_IRGRP) ? "r" : "-");
    printf( (st.st_mode & S_IWGRP) ? "w" : "-");
    printf( (st.st_mode & S_IXGRP) ? "x" : "-");

    printf( (st.st_mode & S_IROTH) ? "r" : "-");
    printf( (st.st_mode & S_IWOTH) ? "w" : "-");
    printf( (st.st_mode & S_IXOTH) ? "x" : "-");

    printf("\n");

    return 0;
}
