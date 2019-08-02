#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <thread>

#define PRIME 4
#define NOPRIME 2
#define SIZE 18

//example of what you need to do:
//#define STUDENT_NAME "George P. Burdell"
//#define STUDENT_CWID "123456789"

//put your info here or you won't get points
#define STUDENT_NAME "LIAM LE"
#define STUDENT_CWID "898223490"

//you may not change code in this section:
void print_name(void) {
    printf("\n%s\n", STUDENT_NAME);
    printf("%s\n", STUDENT_CWID);
}

int is_prime(long num)
{
    if (num <= 1) return 0;
    if (num % 2 == 0 && num > 2) return 0;
    for(long long i = 3; i < num / 2; i+= 2)
    {
        if (num % i == 0)
            return 0;
    }
    return 1;
}
//end of section
char buffer[SIZE];
int c;
int i;
//int flag =0;
void *task(void *argument){


    do {

        c = getchar();
        if (isspace(c)) {
            buffer[i] = '\0';
            i = 0;
        } else if(isdigit(c)) {
            buffer[i] = c;
            i++;
        }

        if (i == SIZE) {
            buffer[i] = '\0';
            i = 0;
        }
        if (i == 0 && strnlen(buffer, SIZE) > 0) {
            puts(buffer);
            long num = atoll(buffer);
            if (is_prime(num)) {
                printf("\nreturn value: %d\n", PRIME);
                exit(0);
            }
        }
    } while(c != EOF );
    printf("\nreturn value: %d\n", NOPRIME);
    exit(0);

}

int main(int argc, char ** argv)
{
    unsigned int n = std::thread::hardware_concurrency();
    //printf("%d", n);

    //don't change this name print section or you won't get points:
    if (argc == 2 && strncmp(argv[1], "-n", 2) == 0) {
        print_name();
        return 0;
    }
    //end of name print section
    pthread_t threads[n];
    int i;

    for(i=0; i<n; i++){
        pthread_create(&threads[i], NULL, task, NULL);
    }

    for (i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;

}
