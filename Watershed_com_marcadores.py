import cv2
import numpy as np
import string

# --- Configuração ---
# Carrega a imagem (coloque 'moedas1.png' no mesmo diretório deste script)
imagem = cv2.imread('moedas1.png')
if imagem is None:
    raise FileNotFoundError("Imagem 'moedas1.png' não encontrada. Verifique o caminho.")
img_copia = imagem.copy()

# Define marcadores usando letras a–z
letras = list(string.ascii_lowercase)  # ['a','b',…,'z']
NUM_MARCADORES = len(letras)

# Gera cores bem distintas usando espaço uniforme no círculo HSV
# Cria um array HSV (NUM_MARCADORES x 1 x 3), converte para BGR
hsv = np.zeros((NUM_MARCADORES, 1, 3), dtype=np.uint8)
hsv[:, 0, 0] = np.linspace(0, 179, NUM_MARCADORES, dtype=np.uint8)  # matiz de 0 a 179
hsv[:, 0, 1] = 255  # saturação máxima
hsv[:, 0, 2] = 255  # valor máximo
cores_bgr = cv2.cvtColor(hsv, cv2.COLOR_HSV2BGR)
# Converte para lista de tuplas
cores = [tuple(int(c) for c in color[0]) for color in cores_bgr]

# Matriz de marcadores e estado
marcadores = np.zeros(imagem.shape[:2], dtype=np.int32)
id_atual = 1       # índice de 1 a NUM_MARCADORES
desenhando = False # flag para saber se o mouse está pressionado

# --- Callback de mouse para desenhar ---

def desenhar(event, x, y, flags, param):
    global desenhando, id_atual
    if event == cv2.EVENT_LBUTTONDOWN:
        desenhando = True
        cv2.circle(img_copia, (x, y), 5, cores[id_atual-1], -1)
        cv2.circle(marcadores, (x, y), 5, id_atual, -1)
    elif event == cv2.EVENT_MOUSEMOVE and desenhando:
        cv2.circle(img_copia, (x, y), 5, cores[id_atual-1], -1)
        cv2.circle(marcadores, (x, y), 5, id_atual, -1)
    elif event == cv2.EVENT_LBUTTONUP:
        desenhando = False

cv2.namedWindow('Imagem')
cv2.setMouseCallback('Imagem', desenhar)

# Exibe instruções
print("Seletores de marcadores:")
for idx, letra in enumerate(letras, start=1):
    print(f"  {letra} → marcador {idx}")
print("Teclas: letras a–z para mudar marcador | '1': reset | '2': watershed | '3' ou ESC: sair")

# --- Loop principal ---
while True:
    cv2.imshow('Imagem', img_copia)
    tecla = cv2.waitKey(1) & 0xFF

    # ESC ou 'q' sai
    if tecla == 27 or tecla == ord('3'):
        break

    # 'r' reseta desenho e marcadores
    elif tecla == ord('1'):
        img_copia = imagem.copy()
        marcadores[:] = 0

    # 'w' aplica watershed e preenche regiões
    elif tecla == ord('2'):
        marc_ws = marcadores.copy()
        cv2.watershed(imagem, marc_ws)

        # prepara imagem de saída
        img_regions = np.zeros_like(imagem)

        # preenche interior de cada marcador com sua cor
        for mk in range(1, NUM_MARCADORES + 1):
            img_regions[marc_ws == mk] = cores[mk-1]

        # opcional: destacar as fronteiras em vermelho
        #img_regions[marc_ws == -1] = [0, 0, 255]
        cv2.imwrite('Resultado_Watershed.png', img_regions)

        cv2.imshow('Regiões Segmentadas', img_regions)

    # letras a–z para selecionar marcador ativo
    elif tecla in [ord(c) for c in letras]:
        idx = letras.index(chr(tecla)) + 1
        if 1 <= idx <= NUM_MARCADORES:
            id_atual = idx

# Fecha janelas
cv2.destroyAllWindows()