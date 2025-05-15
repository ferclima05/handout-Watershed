import cv2
import numpy as np

# --- Carrega a imagem ---
img = cv2.imread('moedas1.png')
if img is None:
    raise FileNotFoundError("Imagem 'moedas1.png' não encontrada.")
orig = img.copy()

# --- Pré-processamento ---
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
blur = cv2.GaussianBlur(gray, (5,5), 0)

# Limiarização binária inversa com Otsu
_, thresh = cv2.threshold(blur, 0, 255,
                          cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)

# Remove ruídos com abertura morfológica
kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3,3))
opening = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel, iterations=2)

# --- Determina background e foreground certos ---
sure_bg = cv2.dilate(opening, kernel, iterations=3)

dist = cv2.distanceTransform(opening, cv2.DIST_L2, 5)
_, sure_fg = cv2.threshold(dist, 0.5 * dist.max(), 255, cv2.THRESH_BINARY)
sure_fg = np.uint8(sure_fg)

unknown = cv2.subtract(sure_bg, sure_fg)

# --- Cria marcadores ---
_, markers = cv2.connectedComponents(sure_fg)
markers = markers + 1           # faz background = 1 em vez de 0
markers[unknown == 255] = 0     # região desconhecida = 0

# --- Aplica Watershed ---
markers = cv2.watershed(orig, markers)

# --- Prepara cores para cada região ---
# identifica todas as labels (>1 são as moedas)
labels = np.unique(markers)
obj_labels = [lab for lab in labels if lab > 1]

# gera cores HSV bem espaçadas, depois converte para BGR
hsv = np.zeros((len(obj_labels), 1, 3), dtype=np.uint8)
hsv[:, 0, 0] = np.linspace(0, 179, len(obj_labels), dtype=np.uint8)
hsv[:, 0, 1] = 255
hsv[:, 0, 2] = 255
cores_bgr = cv2.cvtColor(hsv, cv2.COLOR_HSV2BGR)
cores = {lab: tuple(int(c) for c in cores_bgr[i,0]) 
         for i, lab in enumerate(obj_labels)}

# --- Cria imagem de saída preenchida ---
seg_filled = np.zeros_like(orig)

# preenche cada região com a cor correspondente
for lab, color in cores.items():
    seg_filled[markers == lab] = color

# opcional: desenha as fronteiras em vermelho
#seg_filled[markers == -1] = [0, 0, 255]
#cv2.imwrite('watershed_SS.png', seg_filled)

# --- Exibe resultados ---
cv2.imshow("Original", img)
cv2.imshow("Watershed Preenchido", seg_filled)
cv2.waitKey(0)
cv2.destroyAllWindows()
