#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>

//fungsi buat cek eksis file
int cfileexists(const char *filename)
{
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist == 0)
        return 1;
    else // -1
        return 0;
}

//fungsi move
void *move(void *filename)
{
    char cwd[PATH_MAX];
    char dirname[200], hidden[100], hiddenname[100], file[100], existsfile[100];
    int i;
    strcpy(existsfile, filename);
    strcpy(hiddenname, filename);
    char *namaa = strrchr(hiddenname, '/');
    strcpy(hidden, namaa);

    //Untuk file yang hidden, awalan .
    if (hidden[1] == '.')
    {
        strcpy(dirname, "Hidden");
    }
    //File biasa
    else if (strstr(filename, ".") != NULL)
    {
        strcpy(file, filename);
        strtok(file, ".");
        char *token = strtok(NULL, "");
        //nggak case sensitive
        for (i = 0; token[i]; i++)
        {
            token[i] = tolower(token[i]);
        }
        strcpy(dirname, token);
    }
    //file gaada extensi
    else
    {
        strcpy(dirname, "Unknown");
    }

    //cek file ada ato ga, kalo ga dibuat folder
    int exist = cfileexists(existsfile);
    if (exist)
        mkdir(dirname, 0755);

    //dapet nama file
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        char *nama = strrchr(filename, '/');
        char namafile[200];
        strcpy(namafile, cwd);
        strcat(namafile, "/");
        strcat(namafile, dirname);
        strcat(namafile, nama);

        //move file pake rename
        rename(filename, namafile);
    }
}

//rekursif list
void listFilesRecursively(char *basePath)
{
    char path[1000];
    struct dirent *dp;
    struct stat buffer;
    DIR *dir = opendir(basePath);
    int n = 0;

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            if (stat(path, &buffer) == 0 && S_ISREG(buffer.st_mode))
            {
                //membuat thread untuk cek move
                pthread_t thread;
                int err = pthread_create(&thread, NULL, move, (void *)path);
                pthread_join(thread, NULL);
            }

            listFilesRecursively(path);
        }
    }
    closedir(dir);
}

//fungsi main
int main(int argc, char *argv[])
{
    char cwd[PATH_MAX];
    //Ketika ada argument -f
    if (strcmp(argv[1], "-f") == 0)
    {
        pthread_t thread;
        int i;
        //buat thread dan print msg
        for (i = 2; i < argc; i++)
        {
            char printmsg[1000];
            int exist = cfileexists(argv[i]);
            if (exist)
            {
                sprintf(printmsg, "File %d : Berhasil Dikategorikan", i - 1);
            }
            else
            {
                sprintf(printmsg, "File %d : Sad, gagal :(", i - 1);
            }
            printf("%s\n", printmsg);
            int err = pthread_create(&thread, NULL, move, (void *)argv[i]);
        }
        //join semua thread
        pthread_join(thread, NULL);
    }
    else
    {
        //ketika ada *
        if (strcmp(argv[1], "*") == 0)
        {
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                //membuka working directory sendiri
                listFilesRecursively(cwd);
            }
        }

        //ketika ada argument -d
        else if (strcmp(argv[1], "-d") == 0)
        {
            //open direktori sesuai argument kedua
            listFilesRecursively(argv[2]);
            struct stat buffer;
            int err = stat(argv[2], &buffer);
            if (err == -1)
            {
                printf("Yah, gagal disimpan :(\n");
            }
            else
            {
                printf("Direktori sukses disimpan!\n");
            }
        }
    }
}
