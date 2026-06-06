import matplotlib.pyplot as plt
import numpy as np
import os

# 1. Definimos la ruta del archivo de forma segura y lo leemos
archivo = os.path.join(os.path.dirname(__file__), "resultados_va_rhovec.txt")

if not os.path.exists(archivo):
    print(f"Error: No se encuentra el archivo '{archivo}'. Asegúrate de que está en la misma carpeta que el script.")
    exit()

# np.loadtxt ignora automáticamente la cabecera '# rho va_promedio N'
datos = np.loadtxt(archivo)

# 2. Extraemos las columnas
# La columna 0 es rho (X), la columna 1 es va_promedio (Y), la columna 2 es N (no la dibujamos aquí)
rho = datos[:, 0]
va_promedio = datos[:, 1]

# 3. Creamos la figura
plt.figure(figsize=(8, 6))

# 4. Dibujamos la gráfica (Densidad frente a Parámetro de Orden)
plt.plot(rho, va_promedio, 'o', color='purple', label=r'Datos simulados', 
         linewidth=2, markersize=7, markerfacecolor='white', markeredgewidth=1.5)

# 5. Textos y etiquetas
plt.xlabel(r' $\rho$', fontsize=13)
plt.ylabel(r'$v_a$', fontsize=13)
#plt.title('Transición de fase inducida por la densidad (Ruido Vectorial)', fontsize=14, fontweight='bold', pad=15)

# 6. Estética y rejilla
plt.grid(True, linestyle=':', alpha=0.7)
plt.xlim(0, 5)
plt.ylim(0.15, 1) # Ajustado al rango de tus datos para enmarcar bien la curva

plt.legend(loc='lower right', frameon=True, shadow=True, fontsize=11)

# 7. Mostrar la gráfica
plt.tight_layout()
plt.show()