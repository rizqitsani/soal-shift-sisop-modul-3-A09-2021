# Soal Shift Modul 3

## Daftar Isi
* [Daftar Isi](#daftar-isi)
* [Nomor 1](#nomor-1)
* [Nomor 2](#nomor-2)
* [Nomor 3](#nomor-3)

## Nomor 2

[Daftar Isi](#daftar-isi)

### **Soal 2A**

Source Code : [Click here!](https://github.com/rizqitsani/soal-shift-sisop-modul-3-A09-2021/blob/main/soal2/soal2a.c)

Membuat program perkalian dua matriks dengan ukuran 4x3 dan 3x6. Setelah itu menampilkan hasilnya.

Pertama digunakan beberapa konstan untuk membantu proses pengerjaan

```c
#define BARIS_KIRI 4
#define KOLOM_KIRI 3
#define BARIS_KANAN 3
#define KOLOM_KANAN 6
```

Karena hasil perkalian akan digunakan pada soal 2b, digunakan shared memory dengan value yang dibagikan diberi nama `sharedValue` yang berisi matriks hasil perkalian

```c
key_t key = 7890;
int (*sharedValue)[KOLOM_KANAN];
int shmid = shmget(key, sizeof(int[BARIS_KIRI][KOLOM_KANAN]), IPC_CREAT | 0666);
sharedValue = shmat(shmid, NULL, 0);
```

Pertama, pengguna diminta untuk memasukkan matriksnya

```c
printf("Input pertama:\n");
for(int i = 0; i < BARIS_KIRI; i++) {
    for(int j =0; j < KOLOM_KIRI; j++) {
        scanf("%d", &first[i][j]);
    }
}

printf("Input kedua:\n");
for(int i = 0; i < BARIS_KANAN; i++) {
    for(int j = 0; j < KOLOM_KANAN; j++) {
        scanf("%d", &second[i][j]);
    }
}
printf("\n");
```

Setelah itu dilakukan proses perkalian

```c
for (int i = 0; i < BARIS_KIRI; i++) {
  for (int j = 0; j < KOLOM_KANAN; j++) {
    for (int k = 0; k < KOLOM_KIRI; k++) {
      sum += first[i][k] * second[k][j];
    }

    sharedValue[i][j] = sum;
    result[i][j] = sum;
    sum = 0;
  }
}
```

Lalu, matriks hasil perkalian akan ditampilkan sekaligus disimpan kedalam `sharedValue`

```c
printf("Hasil Matriks:\n");
for (int i = 0; i < BARIS_KIRI; i++) {
  for (int j = 0; j < KOLOM_KANAN; j++) {
    sharedValue[i][j] = result[i][j];
    printf("%d\t", sharedValue[i][j]);
  }

  printf("\n");
}
```

### **Soal 2B**

Source Code : [Click here!](https://github.com/rizqitsani/soal-shift-sisop-modul-3-A09-2021/blob/main/soal2/soal2b.c)

Melakukan operasi unik yang telah ditentukan menggunakan matriks dari soal 2a dan matriks dari input pengguna

Pertama digunakan beberapa konstan untuk membantu proses pengerjaan

```c
#define BARIS 4
#define KOLOM 6
```

Dilakukan pengambilan nilai matriks yang dihasilkan dari soal 2a lalu disimpan kedalam variabel `sharedValue`

```c
key_t key = 7890;

int shmid = shmget(key, sizeof(int[BARIS][KOLOM]), IPC_CREAT | 0666);
sharedValue = shmat(shmid, NULL, 0);
```

Pertama, pengguna diminta memasukkan matriks baru baru matriks yang ditampung dalam `sharedValue` ditampilkan

```c
printf("Input kedua:\n");
for(int i = 0; i < BARIS; i++) {
  for(int j = 0; j < KOLOM; j++) {
    scanf("%d", &inputMatrix[i][j]);
  }
}

printf("Matriks yang diterima: \n");

for (int i = 0; i < BARIS; i++) {
  for (int j = 0; j < KOLOM; j++) {
    printf("%d\t", sharedValue[i][j]);
  }

  printf("\n");
}
```

Membuat thread yang akan menjalankan fungsi `threadFunction`

```c
pthread_create(&tid, NULL, &threadFunction, NULL);
```

Fungsi `threadFunction` ini akan menelusuri elemen pada kedua matriks sehingga dapat dimasukkan ke fungsi `getDesiredValue` untuk mendapat hasil operasinya

```c
void *threadFunction(void* argument) {
  for(int i = 0; i < BARIS; i++) {
    for(int j = 0; j < KOLOM; j++) {
      if (sharedValue[i][j] == 0 || inputMatrix[i][j] == 0) {
        resultMatrix[i][j] = 0;
      } else {
        resultMatrix[i][j] = getDesiredValue(sharedValue[i][j], inputMatrix[i][j]);
      }
    }
  }
}
```

Fungsi `getDesiredValue` berfungsi untuk melakukan operasi unik yang telah ditentukan

```c
int getDesiredValue(int value, int limit) {
  int result = 1, multiplier;

  if (limit >= value) {
    multiplier = 2;
  } else {
    multiplier = value - limit + 1;
  }

  for (int i = multiplier; i <= value; i++) {
    result *= i;
  }

  return result;
}
```

Terakhir thread di-join dan matriks hasil operasi ditampilkan

```c
pthread_join(tid, NULL);

printf("\nMatriks yang dihasilkan: \n");

for (int i = 0; i < BARIS; i++) {
  for (int j = 0; j < KOLOM; j++) {
    printf("%d\t", resultMatrix[i][j]);
  }

  printf("\n");
}
```

### **Soal 2C**

Source Code : [Click here!](https://github.com/rizqitsani/soal-shift-sisop-modul-3-A09-2021/blob/main/soal2/soal2c.c)

Membuat sebuah program untuk melihat 5 proses teratas yang memakan banyak resource

Proses pengerjaan mirip seperti pada soal latihan menggunakan fork untuk membantu proses piping

```c
int main() {
  pid_t child_id;
  int status;

  if (pipe(fd1) == -1) {
    fprintf(stderr, "Pipe Failed");
    return 1;
  }

  child_id = fork();
  if (child_id < 0) {
    exit(EXIT_FAILURE);
  }

  if (child_id == 0) {
    close(fd1[0]);
    dup2(fd1[1], STDOUT_FILENO);

    execlp("ps", "ps", "aux", NULL);
  }

  if (pipe(fd2) == -1) {
    fprintf(stderr, "Pipe Failed");
    return 1;
  }

  child_id = fork();
  if (child_id < 0)
  {
    exit(EXIT_FAILURE);
  }

  if (child_id == 0) {
    close(fd1[1]);
    dup2(fd1[0], STDIN_FILENO);

    close(fd2[0]);
    dup2(fd2[1], STDOUT_FILENO);

    execlp("sort", "sort", "-nrk", "3.3", NULL);
  }

  close(fd1[0]);
  close(fd1[1]);

  close(fd2[1]);
  dup2(fd2[0], STDIN_FILENO);
  close(fd2[0]);

  execlp("head", "head", "-5", NULL);
}
```

### **Kendala**

- Belum mengetahui cara yang efektif agar program 2b dapat menunggu sampai program 2a selesai dijalankan
- Belum terlalu paham dengan konsep piping sehingga sering terjadi error saat pengerjaan

### **Screenshot**

Testcase yang dijalankan pada soal 2A dan 2B:

```
2 3 1
1 2 3
1 3 3
2 1 2
1 2 5 4 1 1
1 5 1 3 2 2
1 4 3 3 2 4


1 2 2 4 1 3
2 1 3 3 1 0
1 2 4 4 2 1
1 3 1 1 2 2
```

Soal 2A dan 2B:

![Soal 2A & 2B](https://i.ibb.co/84rdQnQ/Screenshot-from-2021-05-23-21-44-54.png "Soal 2A & 2B")

Soal 2C:

![Soal 2C](https://i.ibb.co/RyDHvSz/Screenshot-from-2021-05-23-21-46-23.png "Soal 2C")

## Nomor 3

1. Keverk adalah orang yang cukup ambisius dan terkenal di angkatannya. Sebelum dia menjadi ketua departemen di HMTC, dia pernah mengerjakan suatu proyek dimana keverk tersebut meminta untuk membuat server database buku. Proyek ini diminta agar dapat digunakan oleh pemilik aplikasi dan diharapkan bantuannya dari pengguna aplikasi ini. 

Di dalam proyek itu, Keverk diminta: 

a. Pada saat client tersambung dengan server, terdapat dua pilihan pertama, yaitu register dan login. Jika memilih register, client akan diminta input id dan passwordnya untuk dikirimkan ke server. User juga dapat melakukan login. Login berhasil jika id dan password yang dikirim dari aplikasi client sesuai dengan list akun yang ada didalam aplikasi server. Sistem ini juga dapat menerima multi-connections. Koneksi terhitung ketika aplikasi client tersambung dengan server. Jika terdapat 2 koneksi atau lebih maka harus menunggu sampai client pertama keluar untuk bisa melakukan login dan mengakses aplikasinya. Keverk menginginkan lokasi penyimpanan id dan password pada file bernama __akun.txt__ dengan format :

b. Sistem memiliki sebuah database yang bernama files.tsv. Isi dari files.tsv ini adalah path file saat berada di server, publisher, dan tahun publikasi. Setiap penambahan dan penghapusan file pada folder file yang bernama  FILES pada server akan memengaruhi isi dari files.tsv. Folder FILES otomatis dibuat saat server dijalankan. 

c. Tidak hanya itu, Keverk juga diminta membuat fitur agar client dapat menambah file baru ke dalam server. Direktori FILES memiliki struktur direktori di bawah ini : 

d. Dan client dapat mendownload file yang telah ada dalam folder FILES di server, sehingga sistem harus dapat mengirim file ke client. Server harus melihat dari files.tsv untuk melakukan pengecekan apakah file tersebut valid. Jika tidak valid, maka mengirimkan pesan error balik ke client. Jika berhasil, file akan dikirim dan akan diterima ke client di folder client tersebut. 

e. Setelah itu, client juga dapat menghapus file yang tersimpan di server. Akan tetapi, Keverk takut file yang dibuang adalah file yang penting, maka file hanya akan diganti namanya menjadi ‘old-NamaFile.ekstensi’. Ketika file telah diubah namanya, maka row dari file tersebut di file.tsv akan terhapus.

f. Client dapat melihat semua isi files.tsv dengan memanggil suatu perintah yang bernama see. Output dari perintah tersebut keluar dengan format. 

g. Aplikasi client juga dapat melakukan pencarian dengan memberikan suatu string. Hasilnya adalah semua nama file yang mengandung string tersebut. Format output seperti format output f.

h. Dikarenakan Keverk waspada dengan pertambahan dan penghapusan file di server, maka Keverk membuat suatu log untuk server yang bernama running.log. Contoh isi dari log ini adalah

2. Crypto (kamu) adalah teman Loba. Suatu pagi, Crypto melihat Loba yang sedang kewalahan mengerjakan tugas dari bosnya. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:

a. Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).

b. Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (__Catatan!__: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user) sebagai berikut contoh perhitungan untuk matriks yang ada. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya (dari paling besar ke paling kecil) (__Catatan!__: gunakan thread untuk perhitungan di setiap cel). 
__Ketentuan__
```
If a >= b  -> a!/(a-b)!
If b > a -> a!
If 0 -> 0
```
c. Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command `“ps aux | sort -nrk 3,3 | head -5”` (__Catatan!__: Harus menggunakan IPC Pipes)

3. Seorang mahasiswa bernama Alex sedang mengalami masa gabut. Di saat masa gabutnya, ia memikirkan untuk merapikan sejumlah file yang ada di laptopnya. Karena jumlah filenya terlalu banyak, Alex meminta saran ke Ayub. Ayub menyarankan untuk membuat sebuah program C agar file-file dapat dikategorikan. Program ini akan memindahkan file sesuai ekstensinya ke dalam folder sesuai ekstensinya yang folder hasilnya terdapat di working directory ketika program kategori tersebut dijalankan.

Terdapat fungsi bernama `cfileexists` yang berfungsi untuk mengecek file tersebut ada atau tidak
```
int cfileexists(const char *filename)
{
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist == 0)
        return 1;
    else // -1
        return 0;
}
```
Kemudian terdapat fungsi `*move` yaitu adalah fungsi utama dari program ini
```
void *move(void *filename)
```
Fungsinya yang pertama ialah untuk mengkategorikan file sesuai extensinya, jika tidak terdapat ekstensi maka akan masuk folder "Unknown" dan juga bila file tersebut adalah hidden file maka masuk kedalam folder bernama "Hidden".
```
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
    
    int exist = cfileexists(existsfile);
    if (exist)
        mkdir(dirname, 0755);
```
Ini adalah salah satu fungsinya yaitu mengkategorikan file, yang pertama mengambil character pertama yaitu `.` maka akan membuat folder Hidden, kemudian mengambil extensinya dan membuat folder sesuai nama extensi, namun terdapat aturan jika nama extensi tidak case sensitive sehingga memakai fungsi `tolower` untuk membuat capital menjadi huruf kecil, dan kemudian jika tidak extensi maka akan membuat folder "Unknown"
```
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
```
Kemudian dibagian ini terdapat fungsi untuk mengambil nama file saja menggunakan fungsi `strrchr` dan memindahkan file yang dituju menggunakan fungsi c yaitu `rename()`

```
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
```
Terdapat juga fungsi untuk melist file secara rekursif dikarenakan untuk soal nomer 2 point b dan c diharuskan untuk melist file secara rekursif. Kemudian setelah dilist, dibuat juga thread dan fungsi `*move` tadi sebagai argument dan setelah dibuat thread dilanjutkan menggunakan `pthread_join`
```
int main(int argc, char *argv[])
{
    char cwd[PATH_MAX];
    //Ketika ada argument -f
    if (strcmp(argv[1], "-f") == 0)
    {
        pthread_t thread;
        int i, j;
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
        for (j = 0; j < argc; i++) {
            pthread_join(thread, NULL);
        }
        // pthread_join(thread, NULL);
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
```
- Kemudian di fungsi `main` yang didalamnya untuk mengecek argument-argument seperti `-f` kemudian dibuat fungsi untuk memindahkan file tujuan ke working directory. didalamnya terdapat create thread menggunkanan `pthread_create` dan terdapat print berhasil atau gagal dan juga setelah dibuat create dan membuat `pthread_join`
- Kemudian terdapat juga untuk mengecek `*` yaitu untuk mengkategorikan semua file sesuai file soal3 ini berasal, disini menggunakan `getcwd` untuk mengetahui lokasi file dan menggunakan fungsi yang sudah dibuat diatas yaitu `listFilesRecursively` untuk melist file secara rekursif
- Dan yang terakhir untuk cek argument `-d` yaitu untuk mengkategorikan semua file tergantung dari folder tujuan, disini kurang lebih sama seperti mengecek * namun di argument untuk fungsi `listFilesRecursively` menggunakan `argv[2]` untuk mengambil balue argument ke dua dan juga terdapat print output disini

## Kendala yang dialami
- Masih kurang terbiasa dengan konsep process dan thread
- Kadang-kadang terkena error segmentation fault

## Screenshoot
`mkdir testcase && cd testcase && touch root.JPG && touch root.jpg && touch .root && mkdir child && cd child && touch child.tar.gz && cd` Saya membuat testcase dengan command seperti ini yaitu membuat folder dan didalamnya terdapat file" dengan extensi yang berbeda-beda
![https://i.ibb.co/FVv6MLj/Screenshot-from-2021-05-11-15-18-40.png](https://i.ibb.co/FVv6MLj/Screenshot-from-2021-05-11-15-18-40.png)
Mencoba argument `-f` Untuk memindahkan dan kategori file ke tempat soal3 berada dan print berhasil atau gagal

![https://i.ibb.co/tm1YYLT/Screenshot-from-2021-05-11-16-25-30.png](https://i.ibb.co/tm1YYLT/Screenshot-from-2021-05-11-16-25-30.png)
Kemudian untuk argument `-d` Untuk memindahkan folder dan kategorikan file yang ada di tujuan ke tempat soal3 berada

![https://i.ibb.co/SQqCc2w/Screenshot-from-2021-05-11-16-28-21.png](https://i.ibb.co/SQqCc2w/Screenshot-from-2021-05-11-16-28-21.png)
Kemudian untuk argument `*` Untuk mengkategorikan folder sesuai dengan working directory di tempat soal3 berada
Dan kemudian screenshoot untuk tiap testcase seperti case sensitive atau terdapat extensi yang bertumpuk seperti screenshootnya
![https://i.ibb.co/4sRr5mZ/Screenshot-from-2021-05-11-16-28-24.png](https://i.ibb.co/4sRr5mZ/Screenshot-from-2021-05-11-16-28-24.png)
![https://i.ibb.co/LvwHT48/Screenshot-from-2021-05-11-16-36-39.png](https://i.ibb.co/LvwHT48/Screenshot-from-2021-05-11-16-36-39.png)
![https://i.ibb.co/nL1QfH5/Screenshot-from-2021-05-11-16-36-43.png](https://i.ibb.co/nL1QfH5/Screenshot-from-2021-05-11-16-36-43.png)
