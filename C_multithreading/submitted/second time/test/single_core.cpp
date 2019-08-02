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
#define STUDENT_NAME ""
#define STUDENT_CWID ""

//you may not change code in this section:
void print_name(void) {
	printf("\n%s\n", STUDENT_NAME);
	printf("%s\n", STUDENT_CWID);
}

int is_prime(long long num)
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


int main(int argc, char ** argv)
{
	//unsigned int n = std::thread::hardware_concurrency();
	//printf("%d", n);

	//don't change this name print section or you won't get points:
	if (argc == 2 && strncmp(argv[1], "-n", 2) == 0) {
		print_name();
		return 0;
	}
	//end of name print section

	char buffer[SIZE];
	int c;
	int i;
	
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
			long long num = atoll(buffer);
			if (is_prime(num)) {
				printf("\nreturn value: %d\n", PRIME);
				return PRIME;
			}		
		}
	} while(c != EOF);
	printf("\nreturn value: %d\n", NOPRIME);
	return NOPRIME;	
}

