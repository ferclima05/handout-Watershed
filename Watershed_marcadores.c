#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define WIDTH 100
#define HEIGHT 100
#define MAX_QUEUE 10000

typedef struct {
    int x, y;
    int priority;  // intensidade do pixel
} Pixel;

int load_pgm(const char *filename, int image[HEIGHT][WIDTH]) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;

    char format[3];
    int w, h, maxval;

    fscanf(f, "%2s\n", format);
    if (format[0] != 'P' || format[1] != '2') return 0;

    fscanf(f, "%d %d\n%d\n", &w, &h, &maxval);
    if (w != WIDTH || h != HEIGHT) return 0;

    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            fscanf(f, "%d", &image[y][x]);

    fclose(f);
    return 1;
}

void save_pgm(const char *filename, int image[HEIGHT][WIDTH]) {
    FILE *f = fopen(filename, "w");
    fprintf(f, "P2\n%d %d\n255\n", WIDTH, HEIGHT);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            fprintf(f, "%d ", image[y][x]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}

void define_markers(int image[HEIGHT][WIDTH], int markers[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (image[y][x] < 50)  // fundo escuro
                markers[y][x] = 1;
            else if (image[y][x] > 200)  // objeto claro
                markers[y][x] = 2;
            else
                markers[y][x] = 0;
        }
    }
}

void save_labels_as_image(const char *filename, int label[HEIGHT][WIDTH]) {
    int output[HEIGHT][WIDTH];
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            output[y][x] = (label[y][x] % 256);  // para exibir como tons de cinza

    save_pgm(filename, output);
}

int image[HEIGHT][WIDTH];
int markers[HEIGHT][WIDTH];
int label[HEIGHT][WIDTH];

// Fila de prioridade simples
Pixel queue[MAX_QUEUE];
int queue_size = 0;

void push(int x, int y, int priority) {
    if (queue_size >= MAX_QUEUE) return;
    Pixel p = {x, y, priority};
    int i = queue_size++;
    while (i > 0 && queue[(i - 1) / 2].priority > p.priority) {
        queue[i] = queue[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    queue[i] = p;
}

Pixel pop() {
    Pixel top = queue[0];
    Pixel last = queue[--queue_size];
    int i = 0;
    while (2 * i + 1 < queue_size) {
        int child = 2 * i + 1;
        if (child + 1 < queue_size && queue[child + 1].priority < queue[child].priority)
            child++;
        if (last.priority <= queue[child].priority) break;
        queue[i] = queue[child];
        i = child;
    }
    queue[i] = last;
    return top;
}

int in_bounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

void watershed() {
    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    // Inicializar rótulos com -1 (não visitado)
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            label[y][x] = -1;

    // Inicializar a fila com os marcadores
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (markers[y][x] > 0) {
                label[y][x] = markers[y][x];
                push(x, y, image[y][x]);
            }
        }
    }

    while (queue_size > 0) {
        Pixel p = pop();
        for (int i = 0; i < 4; i++) {
            int nx = p.x + dx[i];
            int ny = p.y + dy[i];
            if (in_bounds(nx, ny) && label[ny][nx] == -1) {
                label[ny][nx] = label[p.y][p.x];
                push(nx, ny, image[ny][nx]);
            }
        }
    }
}

int main() {
    if (!load_pgm("img/entrada.pgm", image)) {
        printf("Erro ao carregar imagem\n");
        return 1;
    }

    define_markers(image, markers);

    watershed();

    save_labels_as_image("img/resultado.pgm", label);

    printf("Watershed concluído. Resultado salvo em 'resultado.pgm'\n");

    return 0;
}