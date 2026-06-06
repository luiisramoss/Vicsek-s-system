import os
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy.optimize import least_squares

# Punto crítico unificado
eta_c = 3.28

# 1. Clase de ajuste lineal (Intacta)
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

# --- 2. LECTURA DE DATOS DESDE EL ARCHIVO ---
# Construimos la ruta absoluta basada en la ubicación de este script
archivo = os.path.join(os.path.dirname(__file__), "resultados_va_eta.txt")

# Cargamos el txt indicando que las columnas están separadas por espacios (\s+)
# y les asignamos los nombres 'N', 'eta', y 'va'
df = pd.read_csv(archivo, sep=r'\s+', header=None, names=['N', 'eta', 'va'])

# Extraemos el array de ruido (eta). Como es común para todos, basta con coger el de N=40
x_arr = df[df['N'] == 40]['eta'].values

# Extraemos los arrays de va para cada tamaño de población
y = df[df['N'] == 40]['va'].values
y1 = df[df['N'] == 100]['va'].values
y2 = df[df['N'] == 400]['va'].values
y3 = df[df['N'] == 1000]['va'].values

# --- 3. FILTRADO Y AJUSTE MATEMÁTICO UNIFICADO ---
# Filtramos quitando el cero absoluto y ruidos que saturen en va=1
# (Añadimos la protección > 0 para los arrays y evitar el error del logaritmo)
mask = (x_arr > 0) & (x_arr < eta_c) & (y > 0) & (y1 > 0) & (y2 > 0) & (y3 > 0)

x_rescalado = (eta_c - x_arr[mask]) / eta_c
x_ln = np.log(x_rescalado)

# Evaluamos los logaritmos de las cuatro poblaciones
y_ln_40 = np.log(y[mask])
y_ln_100 = np.log(y1[mask])
y_ln_400 = np.log(y2[mask])
y_ln_1000 = np.log(y3[mask])

# Concatenamos los datos de los cuatro tamaños en un solo set de datos global
X_global = np.concatenate([x_ln, x_ln, x_ln, x_ln])
Y_global = np.concatenate([y_ln_40, y_ln_100, y_ln_400, y_ln_1000])

# Ajustamos una ÚNICA pendiente y una ÚNICA intersección para todo el sistema
m_global, n_global, m_err_global, n_err_global = lsq(X_global, Y_global).fit()

# Imprimimos el resultado en la consola
print("-" * 40)
print(f"RESULTADO DEL AJUSTE GLOBAL:")
print(f"Exponente crítico (beta): {m_global:.4f} ± {m_err_global:.4f}")
print("-" * 40)

# --- 4. GRÁFICA EN ESCALA LOG-LOG (FORMATO PAPER) ---
plt.figure(figsize=(8, 6))

# Dibujamos las cuatro nubes de puntos
plt.loglog(x_rescalado, y[mask], 's', label='N=40', markersize=6, markerfacecolor='none', markeredgecolor='black')
plt.loglog(x_rescalado, y1[mask], '+g', label='N=100', markersize=7)
plt.loglog(x_rescalado, y2[mask], 'xk', label='N=400', markersize=6)
plt.loglog(x_rescalado, y3[mask], '^', label='N=1000', markersize=6)

# Dibujamos una única recta global
plt.loglog(x_rescalado, np.exp(n_global) * (x_rescalado ** m_global), 'black', 
         linewidth=1, alpha=0.8, label=f'Ajuste ($\\beta$={m_global:.2f})')

# Ajustes de formato
plt.xlim(0.1, 1.1)
plt.ylim(0, 1.1)

plt.xlabel(r'$(\eta_c(L) - \eta) / \eta_c(L)$', fontsize=13)
plt.ylabel(r'$v_a$', fontsize=13)
plt.title('Escalamiento del parámetro de orden cerca de la criticidad', fontsize=14, fontweight='bold', pad=15)

plt.grid(True, linestyle=':', alpha=0.5, which="both")
plt.legend(loc='lower right', frameon=True, shadow=True)

plt.tight_layout()
plt.show()