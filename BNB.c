/* EL2208 Praktikum Pemecahan Masalah dengan C 2023/2024
* Modul            : Tugas Besar - Travelling Salesmen Problem (TSP)
* Hari dan Tanggal : Senin, 20 Mei 2024
* Nama (NIM)       : Aliya Shafa I. (18321009)
* Asisten (NIM)    : Emmanuella Pramudita Rumanti (13220031)
* Nama File        : main.c
* Deskripsi        : Program yang memecahkan masalah TSP dengan algoritma Branch and Bound
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <ctype.h>

#define MAX_CITIES 100

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

City cities[MAX_CITIES];
int numCities = 0;
double distMatrix[MAX_CITIES][MAX_CITIES];
double bestRoute[MAX_CITIES + 1];
double minDist = DBL_MAX;
int startIndex = -1;

// Fungsi untuk menghitung jarak antara dua kota menggunakan rumus Haversine
double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dlat = (lat2 - lat1) * M_PI / 180.0;
    double dlon = (lon1 - lon2) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) *
               sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double R = 6371.0; // Earth radius in kilometers
    return R * c;
}

// Fungsi untuk men-delete tanda kutip dari string
char* trimQuotes(char *str) {
    char *end;
    while (isspace((unsigned char)*str) || *str == '"') str++;
    if (*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while (end > str && (isspace((unsigned char)*end) || *end == '"')) end--;
    *(end + 1) = '\0';
    return str;
}

// Fungsi untuk membaca daftar kota dari file
int readCities(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 0;
    }
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",\t");
        if (token != NULL) {
            strcpy(cities[numCities].name, trimQuotes(token));
        }
        token = strtok(NULL, ",\t");
        if (token != NULL) {
            cities[numCities].latitude = atof(trimQuotes(token));
        }
        token = strtok(NULL, ",\t");
        if (token != NULL) {
            cities[numCities].longitude = atof(trimQuotes(token));
        }
        numCities++;
    }
    fclose(file);
    return 1;
}

// Fungsi untuk mengeluarkan rute terbaik
void printBestRoute() {
    printf("Best route found:\n");
    for (int i = 0; i <= numCities; i++) {
        printf("%s", cities[(int)bestRoute[i]].name);
        if (i < numCities) {
            printf(" -> ");
        }
    }
    printf("\nBest route distance: %.5f km\n", minDist);
}

// Fungsi rekursif algoritma Branch and Bound
void tsp(int level, double currDist, int visited[], double currRoute[]) {
    // Jika semua kota telah dikunjungi (level sama dengan jumlah kota)
    if (level == numCities) {
        // Tambahkan jarak dari kota terakhir ke kota awal untuk menyelesaikan tur
        currDist += distMatrix[(int)currRoute[level - 1]][startIndex];
        // Jika jarak saat ini lebih kecil daripada jarak minimum yang ditemukan sejauh ini
        if (currDist < minDist) {
            // Perbarui jarak minimum dengan jarak saat ini
            minDist = currDist;
            // Salin rute saat ini ke dalam rute terbaik yang ditemukan sejauh ini
            memcpy(bestRoute, currRoute, (numCities + 1) * sizeof(double));
            // Setel kota awal sebagai kota terakhir dalam rute terbaik
            bestRoute[numCities] = startIndex;
        }
        // Kembali dari fungsi setelah menyelesaikan satu kemungkinan rute
        return;
    }
    // Iterasi melalui semua kota untuk memilih kota berikutnya yang akan dikunjungi
    for (int i = 0; i < numCities; i++) {
        // Jika kota belum dikunjungi
        if (!visited[i]) {
            // Tandai kota ini sebagai dikunjungi
            visited[i] = 1;
            // Tambahkan kota ini ke rute saat ini pada posisi yang sesuai dengan level
            currRoute[level] = i;
            // Panggil fungsi tsp secara rekursif dengan level yang meningkat, menambahkan jarak kota ini
            tsp(level + 1, currDist + distMatrix[(int)currRoute[level - 1]][i], visited, currRoute);
            // Setelah kembali dari rekursi, tandai kota ini sebagai belum dikunjungi (backtracking)
            visited[i] = 0;
        }
    }
}

// Fungsi main
int main() {
    char filename[50];
    char startCity[50];
    printf("Enter list of cities file name: ");
    scanf("%s", filename);
    if (!readCities(filename)) {
        return 1;
    }
    printf("Enter starting point: ");
    scanf("%s", startCity);
    char cleanStartCity[50];
    strcpy(cleanStartCity, trimQuotes(startCity));
    // Mencari index kota awal
    for (int i = 0; i < numCities; i++) {
        if (strcasecmp(cities[i].name, cleanStartCity) == 0) {
            startIndex = i;
            break;
        }
    }
    if (startIndex == -1) {
        printf("Starting city not found in the list.\n");
        return 1;
    }
    // Membuat matriks jarak
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            distMatrix[i][j] = haversine(cities[i].latitude, cities[i].longitude, cities[j].latitude, cities[j].longitude);
        }
    }
    int visited[MAX_CITIES] = {0};
    double currRoute[MAX_CITIES + 1] = {0};
    currRoute[0] = startIndex;
    visited[startIndex] = 1;
    // Menghitung waktu eksekusi program
    clock_t start = clock();
    tsp(1, 0, visited, currRoute);
    clock_t end = clock();
    printBestRoute();
    double timeElapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time elapsed: %.10f s\n", timeElapsed);
    return 0;
}
