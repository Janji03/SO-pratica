#include <stdlib.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

void copy_file(char* src_path, char* dest_path) {
    int src_fd = open(src_path, O_RDONLY);
    if(src_fd < 0) {
        perror("open");
        exit(1);
    }
    int dest_fd = open(dest_path, O_WRONLY | O_APPEND, 0644);
    if(dest_fd < 0) {
        perror("open dest");
        close(src_fd);
        exit(1);
    }

    char buffer[4096];
    ssize_t bytes_read, bytes_written;
    while((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if(bytes_written != bytes_read) {
            perror("write");
            close(src_fd);
            close(dest_fd);
            exit(1);
        }
    }

    if(bytes_read < 0) {
        perror("read");
    }

    close(src_fd);
    close(dest_fd);
}


void add_to_file(char* dir_path, char* file_path, char* event_name) {
    int file_fd = open(file_path, O_WRONLY | O_APPEND,  0644);
    if(file_fd < 0) {
        perror("open");
        exit(1);
    }
    
    char event_header[256];
    snprintf(event_header, sizeof(event_header), "%s\n", event_name);
    if(write(file_fd, event_header, strlen(event_header)) < 0) {
        perror("write header");
        close(file_fd);
        exit(1);
    }

    char event_path[1024];
    snprintf(event_path, sizeof(event_path), "%s/%s", dir_path, event_name);
    copy_file(event_path, file_path);
    
    if(remove(event_path) < 0) {
        perror("remove");
        close(file_fd);
        exit(1);
    }

    close(file_fd);
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Numero  di argomenti sbagliato");
        return 1;
    }

    char* dir_path = argv[1];
    char* file_path = argv[2];

    int fd = inotify_init();
    if(fd == -1) {
        perror("inotify_init");
        return 1;
    }

    int wd = inotify_add_watch(fd, dir_path, IN_CLOSE_NOWRITE);
    if(wd == -1) {
        perror("inotify_add_watch");
        return 1;
    }

    char buf[BUF_LEN];
    while(1) {
        ssize_t len = read(fd, buf, sizeof(buf));
        if(len == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        ssize_t i = 0;
        while(i < len) {
            struct inotify_event *event = (struct inotify_event *) &buf[i];
            if(event->len) {
                if(event->mask & IN_CLOSE_NOWRITE) {
                    add_to_file(dir_path, file_path, event->name);
                } 
            }
            i += EVENT_SIZE + event->len;
        }
    }

    if(inotify_rm_watch(fd, wd) == -1) {
        perror("inotify_rm_watch");
    }
    close(fd);

    return 0;
}