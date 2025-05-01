#include <stdio.h>
#include <stdlib.h>

#define MAX_WIDTH 1024
#define MAX_HEIGHT 1024

unsigned char imagem[MAX_WIDTH * MAX_HEIGHT];
unsigned char resultado[MAX_WIDTH * MAX_HEIGHT];

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <entrada.pgm> <saida.pgm> <limiar>\n", argv[0]);
        return 1;
    }

    FILE *fin = fopen(argv[1], "rb");
    if (!fin) {
        perror("Erro ao abrir imagem de entrada");
        return 1;
    }

    char tipo[3];
    int largura, altura, maxval;
    fscanf(fin, "%2s", tipo);
    if (tipo[0] != 'P' || tipo[1] != '5') {
        fprintf(stderr, "Formato PGM inválido.\n");
        return 1;
    }

    fscanf(fin, "%d %d %d", &largura, &altura, &maxval);
    fgetc(fin); // pular o \n

    fread(imagem, sizeof(unsigned char), largura * altura, fin);
    fclose(fin);

    int limiar = atoi(argv[3]);

    for (int i = 0; i < largura * altura; i++) {
        resultado[i] = (imagem[i] > limiar) ? 255 : 0;
    }

    FILE *fout = fopen(argv[2], "wb");
    if (!fout) {
        perror("Erro ao abrir imagem de saída");
        return 1;
    }

    fprintf(fout, "P5\n%d %d\n255\n", largura, altura);
    fwrite(resultado, sizeof(unsigned char), largura * altura, fout);
    fclose(fout);

    printf("Imagem segmentada salva em %s\n", argv[2]);
    return 0;
}
