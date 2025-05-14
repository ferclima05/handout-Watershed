from PIL import Image

# Caminho da imagem de entrada (formato .pgm)
pgm_path = "saida_thresh.pgm"

# Caminho da imagem de saída (formato .png)
png_path = "moedas_formato.png"

# Abre a imagem PGM e converte automaticamente
img = Image.open(pgm_path)

# Salva como PNG
img.save(png_path)

print(f"Imagem convertida e salva como {png_path}")
