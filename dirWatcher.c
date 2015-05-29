#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int itoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;
    int total;
    int tam1 = 0;
    int tam2 = 0;
    char * strRet = (char*)malloc(sizeof(char)+1);
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
    }
 
    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10)
	{
		isNegative = 1;
		num = -num;
	}
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator

    //Copying the str to reverse it
    total = strlen(str);
    tam1 = strlen(str);
    while (tam2 < total)
    {
    	if (str[tam1] != '\0')
    	{
    		strRet[tam2] = str[tam1];
    		tam2++;
    	}
    	tam1--;
    }

    strRet[tam2 + 1] = '\0';
    strRet[tam2 + 2] = '\0';

    tam1++;
    while (tam1 < total)
    {
    	str[tam1] = strRet[tam1];
    	tam1++;
    }

    str[tam1++] = '\0';

    return strlen(strRet);
}

void printEvent (char * type, char * action, struct tm *current, struct inotify_event * event)
{
	char * tmp;
	char month[3];
	char day[3];
	char hour[3];
	char minute[3];
	char second[3];
	time_t now = time(0);

	current = localtime(&now);

	if (current->tm_mon <= 9)
	{
		itoa((current->tm_mon + 1), tmp, 10);
		month[0] = '0';
		month[1] = tmp[0];
		month[2] = '\0';
	}
	else
		itoa((current->tm_mon + 1), month, 10);

	if (current->tm_mday <= 9)
	{
		itoa(current->tm_mday, tmp, 10);
		day[0] = '0';
		day[1] = tmp[0];
		day[2] = '\0';
	}
	else
		itoa(current->tm_mday, day, 10);

	if (current->tm_hour <= 9)
	{
		itoa(current->tm_hour, tmp, 10);
		hour[0] = '0';
		hour[1] = tmp[0];
		hour[2] = '\0';
	}
	else
		itoa(current->tm_hour, hour, 10);

	if (current->tm_min <= 9)
	{
		itoa(current->tm_min, tmp, 10);
		minute[0] = '0';
		minute[1] = tmp[0];
		minute[2] = '\0';
	}
	else
		itoa(current->tm_min, minute, 10);

	if (current->tm_sec <= 9)
	{
		itoa(current->tm_sec, tmp, 10);
		second[0] = '0';
		second[1] = tmp[0];
		second[2] = '\0';
	}
	else
		itoa(current->tm_sec, second, 10);

	printf( "[%i-%s-%s %s:%s:%s][%s] %s was %s.\n", (current->tm_year + 1900), month, day, hour, minute, second, type, event->name, action);
}

int main (int argc, char * argv[])
{
	time_t now;
	struct tm *current;

	int fd, wd, length;
	char buffer[BUF_LEN];
	char * dirName = argv[1];
	
	fd = inotify_init();
	if (fd < 0) {
		printf("[ERROR] Lib \"inotify.h\" not found!\n");
		return 1;
	}

	if (argc == 1) {
		printf("[ERROR] Usage: %s directory_to_monitor\n", argv[0]);
		return 1;
	}


	wd = inotify_add_watch(fd, dirName, IN_MODIFY | IN_CREATE | IN_DELETE);
	if (wd < 0) {
		printf("[ERRO] Can't watch dir \"%s\"\n", dirName);
		return 1;
	}

	while (1) {
		length = read( fd, buffer, BUF_LEN );  
		struct inotify_event *event = ( struct inotify_event * ) &buffer[ 0 ];
		if ( event->len ) 
		{
			if ( event->mask & IN_CREATE ) 
			{
				if ( event->mask & IN_ISDIR ) 
				{
					now = time(0);
					current = localtime(&now);
					printEvent ("Directory", "created", current, event);
				}
				else 
				{
					now = time(0);
					current = localtime(&now);
					printEvent ("File", "created", current, event);
				}
			}

			if ( event->mask & IN_MODIFY ) 
			{
				if ( event->mask & IN_ISDIR ) 
				{
					now = time(0);
					current = localtime(&now);
					printEvent ("Directory", "modified", current, event);

				}
				else
				{
					now = time(0);
					current = localtime(&now);
					printEvent ("File", "modified", current, event);
				} 
					
			}

			if ( event->mask & IN_DELETE ) 
			{
				if ( event->mask & IN_ISDIR ) 
				{
					now = time(0);
					current = localtime(&now);
					printEvent ("Directory", "deleted", current, event);
				}
				else 
				{
					now = time(0);
					current = localtime(&now);
					printEvent ("File", "deleted", current, event);
				}
			}
		}
	}
	return 0;
}
