import matplotlib.pyplot as plt
import numpy as np
import os

# 1. Cargar los datos
archivo = os.path.join(os.path.dirname(__file__), "va_vs_angulo.txt")
if not os.path.exists(archivo):
    print(f"Error: No se encontró el archivo {archivo}")
    exit()

datos = np.loadtxt(archivo)
theta_rad = datos[:, 0]
va = datos[:, 1]

# 2. Normalizar el ángulo (Dividir entre 2*PI para que vaya de 0 a 1)
theta_norm = theta_rad / (2.0 * np.pi)

# 3. Dibujar la gráfica
plt.figure(figsize=(8, 6))

# Dibujamos con línea y marcadores usando el ángulo normalizado
plt.plot(theta_norm, va, '-o', color='royalblue', linewidth=2, 
         markersize=8, markerfacecolor='white', markeredgewidth=1.5, label='Datos simulados')

# Ajustes estéticos
plt.xlabel(r'Ángulo de Visión Normalizado ($\theta_v / 2\pi$)', fontsize=13)
plt.ylabel(r'Parámetro de Orden $v_a$', fontsize=13)
plt.title('Efecto del campo visual en el orden macroscópico', fontsize=14, fontweight='bold', pad=15)

# Límites y tics (forzamos que el eje X vaya de 0 a 1 de 0.25 en 0.25)
plt.xlim(0, 1.0)
plt.ylim(0, 1.05)
plt.xticks([0, 0.25, 0.5, 0.75, 1.0], ['0', '0.25', '0.50', '0.75', '1.0'])

plt.grid(True, linestyle=':', alpha=0.7)
plt.legend(loc='lower right', frameon=True)

plt.tight_layout()
plt.show()