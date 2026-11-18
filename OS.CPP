#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void displayFileSize(int fd, const char *filename) {
    struct stat fileStat;
    if (fstat(fd, &fileStat) == 0) {
        printf("[Info] Current size of '%s': %ld bytes\n", filename, fileStat.st_size);
    } else {
        perror("Error retrieving file size");
    }
}

void createFile() {
    char filename[256];
    printf("Enter new file name to create: ");
    scanf("%255s", filename);
    clearInputBuffer();

    int fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd == -1) {
        perror("Error creating file");
        return;
    }

    printf("Success: File '%s' created successfully (FD: %d).\n", filename, fd);
    close(fd);
}

void writeFile() {
    char filename[256];
    char buffer[BUFFER_SIZE];

    printf("Enter file name to write to: ");
    scanf("%255s", filename);
    clearInputBuffer();

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file for writing");
        return;
    }

    printf("Enter text to write (max %d chars):\n> ", BUFFER_SIZE - 1);
    if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        ssize_t bytesWritten = write(fd, buffer, strlen(buffer));
        if (bytesWritten == -1) {
            perror("Error writing to file");
        } else {
            printf("Success: %zd bytes written to '%s'.\n", bytesWritten, filename);
            displayFileSize(fd, filename);
        }
    }

    close(fd);
}

void readFile() {
    char filename[256];
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    printf("Enter file name to read: ");
    scanf("%255s", filename);
    clearInputBuffer();

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return;
    }

    printf("\n--- Contents of '%s' ---\n", filename);
    while ((bytesRead = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytesRead] = '\0';
        printf("%s", buffer);
    }

    if (bytesRead == -1) {
        perror("\nError reading file");
    } else {
        printf("\n--- End of File ---\n");
    }

    close(fd);
}

void appendFile() {
    char filename[256];
    char buffer[BUFFER_SIZE];

    printf("Enter file name to append to: ");
    scanf("%255s", filename);
    clearInputBuffer();

    int fd = open(filename, O_WRONLY | O_APPEND);
    if (fd == -1) {
        perror("Error opening file for appending");
        return;
    }

    printf("Enter text to append (max %d chars):\n> ", BUFFER_SIZE - 1);
    if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        ssize_t bytesWritten = write(fd, buffer, strlen(buffer));
        if (bytesWritten == -1) {
            perror("Error appending to file");
        } else {
            printf("Success: %zd bytes appended to '%s'.\n", bytesWritten, filename);
            displayFileSize(fd, filename);
        }
    }

    close(fd);
}

void copyFile() {
    char srcName[256], destName[256];
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead, bytesWritten;

    printf("Enter source file name: ");
    scanf("%255s", srcName);
    printf("Enter destination file name: ");
    scanf("%255s", destName);
    clearInputBuffer();

    int srcFd = open(srcName, O_RDONLY);
    if (srcFd == -1) {
        perror("Error opening source file");
        return;
    }

    int destFd = open(destName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (destFd == -1) {
        perror("Error opening/creating destination file");
        close(srcFd);
        return;
    }

    while ((bytesRead = read(srcFd, buffer, BUFFER_SIZE)) > 0) {
        bytesWritten = write(destFd, buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            perror("Error writing during file copy");
            close(srcFd);
            close(destFd);
            return;
        }
    }

    if (bytesRead == -1) {
        perror("Error reading source file during copy");
    } else {
        printf("Success: Copied '%s' to '%s'.\n", srcName, destName);
    }

    close(srcFd);
    close(destFd);
}

void renameFile() {
    char oldName[256], newName[256];

    printf("Enter current file name: ");
    scanf("%255s", oldName);
    printf("Enter new file name: ");
    scanf("%255s", newName);
    clearInputBuffer();

    if (rename(oldName, newName) == 0) {
        printf("Success: Renamed '%s' to '%s'.\n", oldName, newName);
    } else {
        perror("Error renaming file");
    }
}

void deleteFile() {
    char filename[256];

    printf("Enter file name to delete: ");
    scanf("%255s", filename);
    clearInputBuffer();

    if (unlink(filename) == 0) {
        printf("Success: File '%s' deleted permanently.\n", filename);
    } else {
        perror("Error deleting file");
    }
}

void changePermissions() {
    char filename[256];
    char modeStr[10];

    printf("Enter file name: ");
    scanf("%255s", filename);
    printf("Enter octal permission mode (e.g., 0644, 0755, 0400): ");
    scanf("%9s", modeStr);
    clearInputBuffer();

    mode_t mode = (mode_t)strtol(modeStr, NULL, 8);

    if (chmod(filename, mode) == 0) {
        printf("Success: Permissions of '%s' changed to %s.\n", filename, modeStr);
    } else {
        perror("Error changing file permissions");
    }
}

int main() {
    int choice;

    while (1) {
        printf("\n========================================\n");
        printf("   LINUX FILE MANAGEMENT SYSTEM (C)\n");
        printf("========================================\n");
        printf("1. Create File\n");
        printf("2. Write to File\n");
        printf("3. Read File\n");
        printf("4. Append to File\n");
        printf("5. Copy File\n");
        printf("6. Rename File\n");
        printf("7. Delete File\n");
        printf("8. Change File Permissions (chmod)\n");
        printf("9. Exit\n");
        printf("----------------------------------------\n");
        printf("Enter your choice (1-9): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: createFile(); break;
            case 2: writeFile(); break;
            case 3: readFile(); break;
            case 4: appendFile(); break;
            case 5: copyFile(); break;
            case 6: renameFile(); break;
            case 7: deleteFile(); break;
            case 8: changePermissions(); break;
            case 9:
                printf("Exiting File Management System. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice! Select between 1 and 9.\n");
        }
    }

    return 0;
}