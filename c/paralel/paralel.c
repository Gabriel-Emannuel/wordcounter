#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>

const char SPACE = ' ';

const char EL = '\n';

const int PATH_SIZE = 512;

const int DIR_LIST_SIZE = 50;

const int BUFFER_LIST_SIZE = 50;

const int DIR_TYPE = 4;

const int FILE_TYPE = 8;

const int THREAD_LIMIT = 10;

pthread_mutex_t lock;

int wccounter = 0;

typedef struct metadata {
    char** list;
    int index;
    int actualsize;
} metadata;

pthread_t* createpool()
{
    pthread_t* pool = (pthread_t*) malloc(THREAD_LIMIT*sizeof(pthread_t));

    return pool;
}

void* wcfile(void* filepath)
{
    int count = 0;

    FILE *file = fopen(filepath, "r");

    int byte;

    char character;
    
    bool pastletter = false,
         pastspace  = true;
    
    while ((byte = fgetc(file)) != EOF)
    {
        character = (char) byte;
        if (character != SPACE && character != EL)
        {
            pastletter = true;
            if (pastletter && pastspace)
            {
                count++;
            }
            pastspace = false;
        }
        else
        {
            pastspace = true;
        }
    }

    pthread_mutex_lock(&lock);
    wccounter += count;
    pthread_mutex_unlock(&lock);

    return NULL;

}

char** createlist()
{
    char** dirlist = (char**) malloc(DIR_LIST_SIZE * sizeof(char*));

    for (int i = 0; i < DIR_LIST_SIZE; i++)
    {
        dirlist[i] = (char*) malloc(PATH_SIZE * sizeof(char));
    }

    return dirlist;
}

void freedir(char** list, int actualsize)
{
    for (int i = 0; i < actualsize; i++)
    {
        free(list[i]);
    }

    free(list);
}

char** increaselist(char** actualdirlist, int actualsize)
{
    int finalsize = actualsize + BUFFER_LIST_SIZE;

    char** newdirlist = (char**) malloc(finalsize*sizeof(char*));

    for (int i = 0; i < actualsize; i++)
    {
        newdirlist[i] = actualdirlist[i];
    }

    free(actualdirlist);

    for (int i = actualsize; i < finalsize; i++) {
        newdirlist[i] = (char*) malloc(PATH_SIZE * sizeof(char));
    }

    return newdirlist;
}

char* createpath()
{
    char* dirpath = (char*) malloc(PATH_SIZE*sizeof(char));

    return dirpath;
}

void generatedirs(metadata* metadatadir, char* dirpath)
{
    if (strlen(dirpath) == 0)
    {
        return;
    }

    int begin = metadatadir->index,
        final;

    char* finalpath;

    char path[PATH_SIZE];

    struct dirent *entry;

    DIR *dir = opendir(dirpath);

    if (dir == NULL)
    {
        printf("Não foi encontrado o diretório: %s\n", dirpath);
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry -> d_name, ".") == 0 
            || strcmp(entry -> d_name, "..") == 0)
            continue;
        
        snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);

        if (entry -> d_type == DIR_TYPE)
        {
            if (metadatadir->index == metadatadir->actualsize)
            {
                metadatadir->list = increaselist(metadatadir->list, metadatadir->actualsize);
                metadatadir->actualsize += BUFFER_LIST_SIZE;
            }
            finalpath = createpath();
            strcpy(finalpath, path);
            metadatadir->list[metadatadir->index] = finalpath;
            metadatadir->index++;
        }
    }

    final = metadatadir->index;

    for (int i = begin; i <= final; i++)
    {
        generatedirs(metadatadir, metadatadir->list[i]);
    }
}

void generatefiles(metadata* metadatadir, metadata* metadatafiles)
{
    char path[PATH_SIZE];

    char* dirpath;
    char* finalpath;

    struct dirent *entry;

    DIR *dir;

    for (int i = 0; i < metadatadir->index; i++)
    {
        dirpath = metadatadir->list[i];

        dir = opendir(dirpath);
        
        while ((entry = readdir(dir)) != NULL) 
        {
            if (strcmp(entry -> d_name, ".") == 0 
            || strcmp(entry -> d_name, "..") == 0)
            continue;
            
            snprintf(path, sizeof(path), "%s/%s", dirpath, entry->d_name);

            if (entry->d_type == FILE_TYPE)
            {
                if (metadatafiles->index == metadatafiles->actualsize)
                {
                    metadatafiles->list = increaselist(metadatafiles->list, metadatafiles->actualsize);
                    metadatafiles->actualsize = metadatafiles->actualsize + BUFFER_LIST_SIZE;
                }
                finalpath = createpath();
                strcpy(finalpath, path);
                metadatafiles->list[metadatafiles->index] = finalpath;
                metadatafiles->index++;
            }
        }
    }

    freedir(metadatadir->list, metadatadir->index);
   
    free(metadatadir);

}

void execpool(pthread_t* pool, int threadsize, int begin, char** filelist)
{
    for (int i = 0; i < threadsize; i++)
    {
        pthread_create(&pool[i], NULL, wcfile, (void*) filelist[begin+i]);
    }
}

void waitpool(pthread_t* pool, int threadsize)
{
    for (int i = 0; i < threadsize; i++)
    {
        pthread_join(pool[i], NULL);
    }
}

void freepool(pthread_t* pool, int threadsize)
{
    for (int i = 0; i < threadsize; i++)
    {
        free(pool[i]);
    }
    free(pool);
}

int main(int argc, char* argv[])
{
    char* filepath = argv[1];

    metadata* metadatadir = (metadata*) malloc(sizeof(metadata));

    metadatadir->index = 0;
    metadatadir->actualsize = DIR_LIST_SIZE;
    metadatadir->list=createlist();

    generatedirs(metadatadir, filepath);

    metadata* metadatafile = (metadata*) malloc(sizeof(metadata));

    metadatafile->index = 0;
    metadatafile->actualsize = DIR_LIST_SIZE;
    metadatafile->list=createlist();
    
    generatefiles(metadatadir, metadatafile);

    int i = 0, poolsize;

    int total = metadatafile->index + 1;

    pthread_t* pool = createpool();
    
    while (i < total)
    {
        poolsize = total - i;

        if (poolsize > THREAD_LIMIT)
        {
            poolsize = THREAD_LIMIT;
        }

        execpool(pool, poolsize, i, metadatafile->list);

        waitpool(pool, poolsize);

        freepool(pool, poolsize);

        i += poolsize;
    }

    printf("Existem %d palavras no dataset\n", wccounter);

    return EXIT_SUCCESS;
}