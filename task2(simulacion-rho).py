import os
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy.optimize import least_squares

# --- 1. CLASE DE AJUSTE LINEAL ---
class lsq:
    def __init__(self, x, y):
        self.A = np.vstack([x, np.ones(len(x))]).T
        self.b = y
        self.x0 = np.zeros(self.A.shape[1])

    def __residuals_func(self, x):
        return np.dot(self.A, x) - self.b

    def fit(self):
        self.res = least_squares(self.__residuals_func, self.x0)
        mse = sum((self.res.x[0]*self.A[:,0] + self.res.x[1]-self.b)**2)/ (len(self.b)-len(self.res.x))
        cov = np.linalg.inv(self.res.jac.T.dot(self.res.jac)) * mse
        variance = np.sqrt(np.diagonal(cov))
        return self.res.x[0], self.res.x[1], variance[0], variance[1]

# --- 2. LECTURA DE DATOS CON RUTAS DINÁMICAS ---
# Construimos la ruta absoluta basada en la ubicación de este script
archivo = os.path.join(os.path.dirname(__file__), "resultados_va_rho.txt")

# Cargamos el txt ignorando líneas de comentarios y asignando nombres.
# (Asegúrate de que los nombres de las columnas coincidan con el formato real de tu archivo)
df = pd.read_csv(archivo, sep=r'\s+', comment='#', names=['rho', 'va', 'N'])

# Extraemos los arrays
x = df['rho'].values
y = df['va'].values

# Densidad crítica estimada
rhoc = 0.72

# --- 3. TRANSFORMACIÓN MATEMÁTICA ---
mask = (x > rhoc)

x_rescalado = (x[mask] - rhoc) / rhoc
y_filtrado = y[mask]

x_ln = np.log(x_rescalado)
y_ln = np.log(y_filtrado)

# Ajuste lineal
m, n, m_var, n_var = lsq(x_ln, y_ln).fit()
r = np.corrcoef(x_ln, y_ln)[0, 1]

print("-" * 45)
print("--- ANÁLISIS DE TRANSICIÓN ---")
print(f"Archivo procesado: {os.path.basename(archivo)}")
print(f"Exponente crítico (slope): {m:.4f} ± {m_var:.4f}")
print(f"Coef. Pearson (R): {r:.5f}")
print("-" * 45)

# --- 4. GRÁFICA LOG-LOG ---
plt.figure(figsize=(7, 6))

plt.loglog(x_rescalado, y_filtrado, 's', 
           markersize=7, markerfacecolor='none', markeredgecolor='black', markeredgewidth=1.2)

plt.loglog(x_rescalado, np.exp(n) * (x_rescalado ** m), 'k-', linewidth=1.5, 
           label=f'Ajuste (slope = {m:.2f})')

# Textos
plt.xlabel(r'$(\rho - \rho_c(L)) / \rho_c(L)$', fontsize=14)
plt.ylabel(r'$v_a$', fontsize=14, rotation=0, labelpad=20)
#plt.title('Escalamiento del Parámetro de Orden', fontsize=13, fontweight='bold', pad=15)

# Coordenadas precisas
plt.xticks([1, 3, 10, 30], ['1', '3', '10', '30'])
plt.yticks([0.3, 0.4, 0.5, 0.6, 0.8, 1.0], ['0.3', '0.4', '0.5', '0.6', '0.8', '1.0'])

# Estética
plt.tick_params(axis='both', direction='in', length=6, width=1)
plt.tick_params(axis='both', which='minor', direction='in', length=3)

plt.legend(loc='lower right', frameon=False, fontsize=12)
plt.tight_layout()
plt.show()