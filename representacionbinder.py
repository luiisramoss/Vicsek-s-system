import os
import matplotlib.pyplot as plt
import pandas as pd

# --- 1. LECTURA DE DATOS ---
archivo = os.path.join(os.path.dirname(__file__), "resultados_binder_etavec.txt")

# Leemos el archivo ignorando la cabecera (comment='#')
df = pd.read_csv(archivo, sep=r'\s+', comment='#', names=['N', 'eta', 'va', 'binder'])

# --- 2. CONFIGURACIÓN DE LA GRÁFICA ---
plt.figure(figsize=(9, 6))

# Diccionarios de estilos para mantener consistencia visual
estilos = {40: 's-', 100: '+-', 400: 'x-', 1000: '^-'}
colores = {40: 'black', 100: 'green', 400: 'blue', 1000: 'red'}

# Obtenemos los valores únicos de N que hay en el archivo por si has simulado otros
tamanos_N = sorted(df['N'].unique())

# --- 3. DIBUJAR LAS CURVAS ---
for N in tamanos_N:
    # Filtramos los datos para el tamaño N actual
    datos_N = df[df['N'] == N]
    
    # Extraemos las columnas X (ruido) e Y (binder)
    x_eta = datos_N['eta'].values
    y_binder = datos_N['binder'].values
    
    # Asignamos estilo y color si están definidos, si no usamos un estándar
    fmt = estilos.get(N, 'o-')
    color = colores.get(N, 'purple')
    
    # Dibujamos la curva
    if N == 40:
        plt.plot(x_eta, y_binder, fmt, label=f'N={N}', markersize=6, 
                 markerfacecolor='none', markeredgecolor=color, color=color)
    else:
        plt.plot(x_eta, y_binder, fmt, label=f'N={N}', markersize=7, color=color)

# --- 4. FORMATO Y ESTÉTICA ---
# Línea horizontal en 0 (Crítico para ver si es de 1er o 2do orden)
plt.axhline(0, color='gray', linestyle='--', alpha=0.7, linewidth=1.5)

# Etiquetas
plt.xlabel('$\eta$', fontsize=13)
plt.ylabel('$G$', fontsize=13)
#plt.title('Cumulante de Binder vs Ruido Vectorial', fontsize=14, fontweight='bold', pad=15)

# Cuadrícula y Leyenda
plt.grid(True, linestyle=':', alpha=0.6)
plt.legend(loc='best', frameon=True, shadow=True, fontsize=11)

# Ajuste automático de márgenes
plt.tight_layout()

# Mostrar gráfica
plt.show()