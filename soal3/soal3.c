#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

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
    char *nama = strrchr(filename, '/');
    char namafile[200] = "/home/daffainfo/modul3/";
    strcat(namafile, dirname);
    strcat(namafile, nama);

    //move file pake rename
    rename(filename, namafile);
}

//fungsi main
int main(int argc, char *argv[])
{
    DIR *dp;
    struct dirent *ep;
    struct stat buffer;
    char path[100];
    int n = 0;

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
            //membuka working directory sendiri
            dp = opendir("/home/daffainfo/modul3/");
            strcpy(path, "/home/daffainfo/modul3/");
        }

        //ketika ada argument -d
        else if (strcmp(argv[1], "-d") == 0)
        {
            //open direktori sesuai argument kedua
            dp = opendir(argv[2]);
            strcpy(path, argv[2]);
        }

        //buka direktori
        if (dp != NULL)
        {
            //check nanti berisi full path file
            char check[200];
            while (ep = readdir(dp))
            {
                strcpy(check, path);
                strcat(check, "/");
                strcat(check, ep->d_name);

                //ketika ditemukan file
                if (stat(check, &buffer) == 0 && S_ISREG(buffer.st_mode))
                {
                    //membuat thread untuk cek move
                    pthread_t thread;
                    int err = pthread_create(&thread, NULL, move, (void *)check);
                    pthread_join(thread, NULL);
                }
                ++n;
            }
            //Semisal direktori gaada file didalamnya, maka print gagal
            if (n <= 2)
                printf("Yah, gagal disimpan :(\n");
            (void)closedir(dp);
        }
        else
        {
            //Jika dir gaada print gagal
            printf("Yah, gagal disimpan :(\n");
        }
    }
}
