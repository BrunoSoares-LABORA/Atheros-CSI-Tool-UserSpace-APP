#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>

#include "csi_collector.h"
#include "csi_fun.h"

#define FILE_TIME_SEC 1
#define BUFSIZE 4096
#define MAX_FILES 60

unsigned char buf_addr[BUFSIZE];
int ready_to_write = FALSE;
FILE* write_file;
char stored_files[MAX_FILES][512];
int file_index = 0;

void *collect_csi_data(void *vargp) {
    csi_struct* csi_status = (csi_struct*)malloc(sizeof(csi_struct));
    int csi_cnt_bytes, csi_device_fd;
    u_int16_t   buf_len;
    pthread_t tid;

    /* Create file manager */
    pthread_create(&tid, NULL, file_manager, NULL);

    /* Create device reading */
    csi_device_fd = open_csi_device();
    if (csi_device_fd < 0) {
        printf("Failed to open the CSI device...\n");
        exit(0);
    }

    while(TRUE) {
        /* keep listening the kernel until csi report */
        do {
            csi_cnt_bytes = read_csi_buf(buf_addr, csi_device_fd, BUFSIZE);
        } while(!csi_cnt_bytes);

        record_status(buf_addr, csi_cnt_bytes, csi_status);

        /* wait until is ready to write */
        while(!ready_to_write);

        buf_len = csi_status->buf_len;
        fwrite(&buf_len, 1, 2, write_file);
        fwrite(buf_addr, 1, buf_len, write_file);
    }

    close_csi_device(csi_device_fd);
    free(csi_status);
}

void *file_manager(void *vargp) {
    char file_name[512];

    while(TRUE) {
        sprintf(file_name, "%s/outputs/%d", get_selfpath(), (int)time(NULL));
        write_file = fopen(file_name,"w");
        if (!write_file) {
            printf("Fail to open <output_file>, are you root?\n");
            fclose(write_file);
            exit(0);
        }

        ready_to_write = 1;
        sleep(FILE_TIME_SEC);
        /* close file and add to the not recovered array */
        ready_to_write = 0;
        fclose(write_file);

        strcpy(stored_files[file_index%MAX_FILES], file_name);
        file_index++;
    }
}

char* get_selfpath() {
    char *path;
    char buff[512];
    path = (char*)malloc(sizeof(char) * 255);
    ssize_t len = readlink("/proc/self/exe", buff, sizeof(buff)-1);

    if (len != -1) {
      // -9 because we need to remove the executable name
      buff[len-9] = '\0';
      sprintf(path, "%s",buff);
      return path;
    }
}

