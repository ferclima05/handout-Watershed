
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_WIDTH 1024
#define MAX_HEIGHT 1024
#define MAX_K 10
#define MAX_ITER 10

unsigned char imagem[MAX_WIDTH * MAX_HEIGHT];
unsigned char resultado[MAX_WIDTH * MAX_HEIGHT];
int largura, altura;

int centroids[MAX_K];
int counts[MAX_K];
int sums[MAX_K];
int k;

void inicializar_centroids() {
    for (int i = 0; i < k; i++) {
        centroids[i] = (255 / k) * i; // centróides inicializados igualmente espaçados
    }
}

int mais_proximo(int pixel) {
    int melhor = 0;
    int menor_dist = abs(pixel - centroids[0]);
    for (int i = 1; i < k; i++) {
        int dist = abs(pixel - centroids[i]);
        if (dist < menor_dist) {
            menor_dist = dist;
            melhor = i;
        }
    }
    return melhor;
}

void kmeans() {
    for (int iter = 0; iter < MAX_ITER; iter++) {
        for (int i = 0; i < k; i++) {
            counts[i] = 0;
            sums[i] = 0;
        }

        for (int i = 0; i < largura * altura; i++) {
            int grupo = mais_proximo(imagem[i]);
            resultado[i] = grupo;
            counts[grupo]++;
            sums[grupo] += imagem[i];
        }

        for (int i = 0; i < k; i++) {
            if (counts[i] > 0)
                centroids[i] = sums[i] / counts[i];
        }
    }

    // Mapear os grupos para valores de intensidade
    for (int i = 0; i < largura * altura; i++) {
        resultado[i] = 255 / (k - 1) * resultado[i];
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <moedas1.pgm> <saida.pgm> <k>\n", argv[0]);
        return 1;
    }

    FILE *fin = fopen(argv[1], "rb");
    if (!fin) {
        perror("Erro ao abrir imagem de entrada");
        return 1;
    }

    char tipo[3];
    int maxval;
    fscanf(fin, "%2s", tipo);
    if (tipo[0] != 'P' || tipo[1] != '5') {
        fprintf(stderr, "Formato PGM inválido.\n");
        return 1;
    }

    fscanf(fin, "%d %d %d", &largura, &altura, &maxval);
    fgetc(fin); // pular o \n

    fread(imagem, sizeof(unsigned char), largura * altura, fin);
    fclose(fin);

    k = atoi(argv[3]);
    if (k < 2 || k > MAX_K) {
        printf("Valor de k inválido. Use entre 2 e %d.\n", MAX_K);
        return 1;
    }

    inicializar_centroids();
    kmeans();

    FILE *fout = fopen(argv[2], "wb");
    fprintf(fout, "P5\n%d %d\n255\n", largura, altura);
    fwrite(resultado, sizeof(unsigned char), largura * altura, fout);
    fclose(fout);

    printf("Imagem segmentada salva em %s\n", argv[2]);
    return 0;
}
