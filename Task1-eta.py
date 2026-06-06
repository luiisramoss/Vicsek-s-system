import matplotlib.pyplot as plt
import numpy as np
import os

# Nombre de tu archivo de datos
archivo = os.path.join(os.path.dirname(__file__), "resultados_va_etavec.txt")

# 1. Comprobamos que el archivo existe
if not os.path.exists(archivo):
    print(f"Error: No se encuentra el archivo '{archivo}'. Asegúrate de que está en la misma carpeta.")
    exit()

# 2. Cargamos todos los datos de golpe
# datos[:, 0] será N, datos[:, 1] será eta, y datos[:, 2] será v_a#
datos = np.loadtxt(archivo)

N_col = datos[:, 0]
eta_col = datos[:, 1]
va_col = datos[:, 2]

# 3. Creamos el lienzo de la gráfica
plt.figure(figsize=(8, 6))

# 4. Filtramos y dibujamos para cada población (N)
# N = 40
mask_40 = (N_col == 40)
plt.plot(eta_col[mask_40], va_col[mask_40], 's', color='black', label='N = 40', markersize=5, markerfacecolor='none')

# N = 100
mask_100 = (N_col == 100)
plt.plot(eta_col[mask_100], va_col[mask_100], '+', color='forestgreen', label='N = 100', markersize=6)

# N = 400
mask_400 = (N_col == 400)
plt.plot(eta_col[mask_400], va_col[mask_400], 'x', color='crimson', label='N = 400', markersize=6)

# N = 1000 (¡Aprovechamos que está en tu archivo!)
mask_1000 = (N_col == 1000)
plt.plot(eta_col[mask_1000], va_col[mask_1000], '^', color='royalblue', label='N = 1000', markersize=5)

# 5. Textos y estética
plt.xlabel(r'$\eta$', fontsize=12)
plt.ylabel(r'$v_a$', fontsize=12)
#plt.title('Transición de Primer Orden (Ruido Vectorial)', fontsize=14, fontweight='bold', pad=15)

plt.grid(True, linestyle=':', alpha=0.6)
plt.xlim(-0.1, 5.2)
plt.ylim(-0.02, 1.05) 

plt.legend(loc='upper right', frameon=True, shadow=True)

# 6. Mostrar la gráfica
plt.tight_layout()
plt.show()