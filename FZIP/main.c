
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

//You must fill out your name and id below
char *studentName = (char *) "LIAM LE";
char *studentCWID = (char *) "898223490";
//Do not change this section in your submission

int dir = 0;
int file = 1;

char *usageString =
        (char *) "To archive a file: 		fzip -a INPUT_FILE_NAME  OUTPUT_FILE_NAME\n"
                "To archive a directory: 	fzip -a INPUT_DIR_NAME   OUTPUT_DIR_NAME\n"
                "To extract a file: 		fzip -x INPUT_FILE_NAME  OUTPUT_FILE_NAME\n"
                "To extract a directory: 	fzip -x INPUT_DIR_NAME   OUTPUT_DIR_NAME\n";

bool isExtract = false;

void parseArg(int argc, char *argv[], char **inputName, char **outputName) {
    if (argc >= 2 && strncmp("-n", argv[1], 2) == 0) {
        printf("Student Name: %s\n", studentName);
        printf("Student CWID: %s\n", studentCWID);
        exit(EXIT_SUCCESS);
    }

    if (argc != 4) {
        fprintf(stderr, "Incorrect arguements\n%s", usageString);
        exit(EXIT_FAILURE);
    }

    *inputName = argv[2];
    *outputName = argv[3];
    if (strncmp("-a", argv[1], 2) == 0) {
        isExtract = false;
    } else if (strncmp("-x", argv[1], 2) == 0) {
        isExtract = true;
    } else {
        fprintf(stderr, "Incorrect arguements\n%s", usageString);
        exit(EXIT_FAILURE);
    }
}
//END OF: Do not change this section

bool isDir(const char *target) {
    struct stat statbuf;
    stat(target, &statbuf);
    return S_ISDIR(statbuf.st_mode);
}

/**
 *
 * Your program should archive or extract based on the flag passed in.
 * Both when extracting and archiving, it should print the output file/dir path as the last line.
 *
 * @param argc the number of args
 * @param argv the arg string table
 * @return
 */

char* substring(const char* str, unsigned short begin, unsigned short len)
{
    if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len))
        return 0;

    return strndup(str + begin, len);
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


char* newPath(const char* originalPath, const char* topDir, const char* newOutput){
    unsigned short originalPathLength = strlen(originalPath);
    unsigned short topDirLength = strlen(topDir);
    return concat(newOutput, substring(originalPath, topDirLength, originalPathLength-topDirLength));
}


void archive(char *input, FILE *zip, int depth) {
    DIR *directory;
    struct dirent *entry;
    struct stat statbuf;
    int spaces = depth * 4;

    if ((directory = opendir(input)) == NULL) {
        fprintf(stderr, "cannot open directory: %s\n", input);
        return;
    }

    chdir(input); // change directory
    while ((entry = readdir(directory)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) { // IS A DIRECTORY
            /*  ignore . and .. directories */
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
            size_t l;

            l = fwrite(&dir, sizeof(dir), 1, zip);
            char *path = realpath(entry->d_name, NULL);

            unsigned short pathLength = strlen(path) + 1;
            l = fwrite(&pathLength, sizeof(unsigned short), 1, zip);
            l = fwrite(path, 1, pathLength, zip);
            free(path);

            // Recursively call archive function for next level
            archive(entry->d_name, zip, depth + 1);
        } else {  // IS A FILE
            FILE *source = fopen(entry->d_name, "r");
            size_t l1, l2;
            unsigned char buffer[statbuf.st_size];
            l1 = fread(buffer, 1, sizeof(buffer), source);
            l2 = fwrite(&file, sizeof(file), 1, zip);
            char *path = realpath(entry->d_name, NULL);

            unsigned short pathLength = strlen(path) + 1;
            l2 = fwrite(&pathLength, sizeof(unsigned short), 1, zip);
            l2 = fwrite(path, 1, pathLength, zip);
            l2 = fwrite(&l1, 1, sizeof(l1), zip);
            l2 = fwrite(buffer, 1, l1, zip);

            fclose(source);
            free(path);
        }
    }
    chdir("..");
    closedir(directory);
}

int main(int argc, char **argv) {

    char *inputName, *outputName;
    parseArg(argc, argv, &inputName, &outputName);

    if (isExtract) {
        printf("Extracting %s\n", inputName);
        //start extracting
        FILE *zip = fopen(inputName, "rb");
        int flag;
        fread(&flag, 1, sizeof(int), zip);
        if (flag == 1) { //extracting a file

            unsigned short pathLength = 0;
            fread(&pathLength, 1, sizeof(unsigned short), zip);

            char *fileName = malloc(sizeof(char) * pathLength);
            fread(fileName, 1, pathLength, zip);

            //FILE *destination = fopen(fileName, "w");
            FILE *destination = fopen(outputName, "w");
            size_t dataSize = 0;
            fread(&dataSize, 1, sizeof(size_t), zip);

            unsigned char buffer[dataSize];
            fread(buffer, 1, dataSize, zip);
            fwrite(buffer, 1, sizeof(buffer), destination);

            fclose(zip);
            fclose(destination);

        } else { //start extracting a directory

            unsigned int readBytes = sizeof(flag);
            struct stat fileStat;
            mkdir(outputName, 0777);
            char *newDir = realpath(outputName, NULL);
            if (stat(inputName, &fileStat) < 0)
                return 1;
            size_t zipSize = fileStat.st_size;

            unsigned short pathLength = 0;
            fread(&pathLength, 1, sizeof(unsigned short), zip);

            readBytes += sizeof(unsigned short);
            char* dirName = malloc(sizeof(char) * pathLength);
            fread(dirName, 1, pathLength, zip);
            readBytes += pathLength;

            mkdir(newPath(dirName, dirName, newDir), 0777);

            while (readBytes < zipSize) {
                readBytes += fread(&flag, 1, sizeof(int), zip);

                if (flag == 0) { // read dir
                    fread(&pathLength, 1, sizeof(unsigned short), zip);
                    readBytes += sizeof(unsigned short);
                    char *fileName = malloc(sizeof(char) * pathLength);
                    fread(fileName, 1, pathLength, zip);
                    readBytes += pathLength;

                    mkdir(newPath(fileName, dirName, realpath(outputName, NULL)), 0777);

                } else { //read file
                    unsigned short pathLength = 0;
                    fread(&pathLength, 1, sizeof(unsigned short), zip);
                    readBytes += sizeof(unsigned short);

                    char *fileName = malloc(sizeof(char) * pathLength);
                    fread(fileName, 1, pathLength, zip);
                    readBytes += pathLength;

                    FILE *destination = fopen(newPath(fileName, dirName, realpath(outputName, NULL)), "w");
                    size_t dataSize = 0;
                    fread(&dataSize, 1, sizeof(size_t), zip);
                    readBytes += sizeof(size_t);

                    unsigned char buffer[dataSize];
                    fread(buffer, 1, dataSize, zip);
                    readBytes += dataSize;
                    fwrite(buffer, 1, sizeof(buffer), destination);
                    fclose(destination);
                }
            }
            fclose(zip);
            free(newDir);
        }

        printf("%s\n", outputName);

    } else { // making zipped file
        printf("Archiving %s\n", inputName);

        if (isDir(inputName)) { //ARCHIVING A DIRECTORY
            FILE *zip;
            size_t l;
            struct stat fileStat;
            zip = fopen(outputName, "w");
            l = fwrite(&dir, sizeof(dir), 1, zip);
            char *path = realpath(inputName, NULL);

            unsigned short pathLength = strlen(path) + 1;
            l = fwrite(&pathLength, sizeof(unsigned short), 1, zip);
            l = fwrite(path, 1, pathLength, zip);
            free(path);
            archive(inputName, zip, 0);
            fclose(zip);

        } else { // ARCHIVING a FILE
            FILE *source = fopen(inputName, "r");
            FILE *zip = fopen(outputName, "w");
            struct stat fileStat;
            if (stat(inputName, &fileStat) < 0)
                return 1;

            size_t l1, l2;
            unsigned char buffer[fileStat.st_size];
            l1 = fread(buffer, 1, sizeof(buffer), source);
            l2 = fwrite(&file, sizeof(file), 1, zip);
            char *path = realpath(inputName, NULL);
            unsigned short pathLength = strlen(path) + 1;
            l2 = fwrite(&pathLength, sizeof(unsigned short), 1, zip);
            l2 = fwrite(path, 1, pathLength, zip);
            l2 = fwrite(&l1, 1, sizeof(l1), zip);
            l2 = fwrite(buffer, 1, l1, zip);

            fclose(source);
            fclose(zip);
            free(path);
        }
        printf("%s\n", outputName);
    }
    // TEST COMMAND: diff -rqb file1 file2
    return EXIT_SUCCESS;
}


