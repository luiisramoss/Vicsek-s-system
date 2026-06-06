import matplotlib.pyplot as plt
import numpy as np

# Configuración de estilo profesional para sistemas complejos
plt.style.use('seaborn-v0_8-muted') 

def graficar_vicsek_alta_resolucion():
    # --- DATOS MANUALES DEL MODELO DE VICSEK (Resolución de 0.25) ---
    
    # Eje X: Valores de ruido comunes (Segunda columna)
    eta = [
        0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2, 2.25, 
        2.5, 2.75, 3, 3.25, 3.5, 3.75, 4, 4.25, 4.5, 4.75, 5
    ]
    
    # Eje Y: Valores del parámetro de orden va (Tercera columna)
    va_40 = [
        1, 0.997224, 0.98894, 0.975366, 0.956565, 0.931588, 0.904164, 0.87097, 
        0.831808, 0.789177, 0.743932, 0.695332, 0.647657, 0.593184, 0.532197, 
        0.474743, 0.415327, 0.347893, 0.28894, 0.227241, 0.192641
    ]
    
    va_100 = [
        1, 0.997044, 0.987987, 0.972258, 0.952646, 0.925803, 0.893904, 0.857428, 
        0.814029, 0.769704, 0.719712, 0.665805, 0.606191, 0.552085, 0.468871, 
        0.408478, 0.351385, 0.242047, 0.196269, 0.151042, 0.12158
    ]
    
    va_400 = [
        1, 0.996746, 0.986136, 0.967806, 0.946958, 0.914846, 0.874906, 0.834587, 
        0.783597, 0.73647, 0.675237, 0.611001, 0.538684, 0.474212, 0.383473, 
        0.326324, 0.208226, 0.142913, 0.103145, 0.0794174, 0.0619028
    ]

    # --- DISEÑO DE LA GRÁFICA ---
    plt.figure(figsize=(9, 6))
    
    # Dibujamos las curvas con sus respectivos marcadores
    plt.plot(eta, va_40, 's-',  label='N = 40',  markersize=5)  # Cuadrados
    plt.plot(eta, va_100, 'p-', label='N = 100', markersize=5)  # Pentágonos
    plt.plot(eta, va_400, 'x-', label='N = 400', markersize=6)  # Cruces X
    
    # Formato de etiquetas estilo publicación científica
    plt.xlabel(r'Ruido ($\eta$)', fontsize=13)
    plt.ylabel(r'Parámetro de Orden ($v_a$)', fontsize=13)
    plt.title('Transición de Fase en el Modelo de Vicsek (Alta Resolución)', fontsize=14, fontweight='bold', pad=15)
    
    # Ajustes de los límites y rejilla
    plt.xlim(-0.1, 5.1)
    plt.ylim(-0.05, 1.05)
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend(frameon=True, shadow=True, loc='upper right')
    
    plt.tight_layout()
    plt.show()

# Ejecución del bloque principal
if __name__ == "__main__":
    graficar_vicsek_alta_resolucion()