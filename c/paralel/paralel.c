#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

const char SPACE = ' ';

const int BUFFER_SIZE = 256;

int finalcount = 0;

int THREAD_LIMIT = 10;

int wc(char* words)
{
    int count = 0;
    int indexch = 0;

    char ch = words[0];

    bool pastletter = false,
         pastspace  = true;

    while (BUFFER_SIZE > indexch && ch != '\0' && ch != '\n')
    {
        if (ch != SPACE)
        {
            pastletter = true;
            if (pastletter && pastspace)
                count++;
            pastspace = false;
        }
        else
        {
            pastspace = true;
        }
        
        indexch++;
        ch = words[indexch];
    }

    return count;
}

int wcfile(char* filepath)
{
    int count = 0;

    FILE *file = fopen(filepath, "r");

    char words[BUFFER_SIZE];

    while (fgets(words, BUFFER_SIZE, file) != NULL)
        count += wc(words);

    return count;
}

void wcdir(char* dirpath) {
    char newPath[1024];
    
    int count = 0;

    struct dirent *entry;

    DIR *dir = opendir(dirpath);

    if (dir == NULL) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0)
            continue;
        
        snprintf(newPath, sizeof(newPath), "%s/%s", dirpath, entry->d_name);

        if (entry -> d_type == 4) {
            wcdir(newPath);
        }

        if (entry -> d_type == 8) {
            count += wcfile(newPath);
        }
    }

    finalcount += count;
}

int main(int argc, char* argv[])
{
    char* filepath = argv[1];

    wcdir(filepath);

    printf("Quantidade de palavras: %d\n", finalcount);

    return EXIT_SUCCESS;
}