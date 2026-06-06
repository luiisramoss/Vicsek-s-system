#ifndef _GLIBCXX_HAVE_QUICK_EXIT
#define _GLIBCXX_HAVE_QUICK_EXIT 1
#endif
#define _GLIBCXX_HAS_GTHREADS 1

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <random>

using namespace std;

// Función optimizada SOLO para generar la animación (Ruido Vectorial + Cono de Visión)
void generar_animacion(int N, double eta, double L, double theta_v, string nombre_archivo) {
    const double vi = 0.03; // Velocidad de las partículas
    const double r = 1.0;   // Radio de interacción
    const double dt = 1.0;  // Paso de tiempo
    const int pasos_totales = 4000; // Suficiente tiempo para ver cómo se forman grupos
    
    vector<double> x(N), y(N), theta(N);
    ofstream f_pos(nombre_archivo);

    // 1. Inicialización aleatoria (Gas caótico)
    for (int i = 0; i < N; i++) {
        x[i] = (rand() / (double)RAND_MAX) * L;
        y[i] = (rand() / (double)RAND_MAX) * L;
        theta[i] = (rand() / (double)RAND_MAX) * 2.0 * M_PI;
    }

    // Bucle temporal
    for (int t = 0; t < pasos_totales; t++) {
        
        // --- GUARDAR ESTADO ACTUAL ---
        // Truco: Guardamos 1 de cada 5 frames para que el TXT no sea gigantesco y Python vaya fluido
        if (t % 5 == 0) { 
            for (int i = 0; i < N; i++) {
                f_pos << x[i] << "," << y[i] << "," << cos(theta[i]) << "," << sin(theta[i]) << "\n";
            }
            f_pos << "\n"; // Separador de frames para Python
        }

        // --- ACTUALIZAR POSICIONES ---
        for (int i = 0; i < N; i++) {
            x[i] += cos(theta[i]) * vi * dt;
            y[i] += sin(theta[i]) * vi * dt;

            // Condiciones de contorno periódicas (Toroide)
            if (x[i] >= L) x[i] -= L; else if (x[i] < 0) x[i] += L;
            if (y[i] >= L) y[i] -= L; else if (y[i] < 0) y[i] += L;
        }

        // --- CALCULAR NUEVAS DIRECCIONES ---
        vector<double> n_theta(N);
        for (int i = 0; i < N; i++) {
            double sS = 0, sC = 0;
            
            for (int j = 0; j < N; j++) {
                double dx = x[j] - x[i];
                double dy = y[j] - y[i];

                // Medir distancia mínima a través de los bordes
                if (dx > L/2.0) dx -= L; else if (dx < -L/2.0) dx += L;
                if (dy > L/2.0) dy -= L; else if (dy < -L/2.0) dy += L;

                // Si está dentro del radio de interacción...
                if ((dx*dx + dy*dy) <= r*r) {
                    if (i == j) {
                        // Yo siempre me veo a mí mismo
                        sS += sin(theta[i]);
                        sC += cos(theta[i]);
                    } else {
                        // Calcular si el vecino j está dentro de mi cono de visión
                        double angulo_ij = atan2(dy, dx);
                        double dif = angulo_ij - theta[i];
                        
                        // Normalizar diferencia angular a [-PI, PI]
                        while (dif <= -M_PI) dif += 2.0 * M_PI;
                        while (dif > M_PI) dif -= 2.0 * M_PI;

                        if (abs(dif) <= theta_v / 2.0) {
                            sS += sin(theta[j]);
                            sC += cos(theta[j]);
                        }
                    }
                }
            }
          
            // --- RUIDO VECTORIAL ---
            double angulo_ruido = (rand() / (double)RAND_MAX) * 2.0 * M_PI;
            sC += eta * cos(angulo_ruido); // Añadimos el vector de ruido
            sS += eta * sin(angulo_ruido);
            
            // Extraer el ángulo resultante final
            n_theta[i] = atan2(sS, sC);   
        }
        
        // Actualizar todos los ángulos a la vez
        theta = n_theta;
    }
    
    f_pos.close();
}

int main() {
    srand(time(NULL));

    // ==========================================
    // PANEL DE CONTROL DE LA ANIMACIÓN
    // ==========================================
    int N_esp = 1000;           // Número de pájaros
    double L_esp = 25;        // Tamaño de la caja (Densidad)
    double eta_esp = 0.5;       // Ruido Vectorial
    
    // Ángulo de visión (2.0 * M_PI = 360 grados / M_PI = 180 grados)
    double cono_vision = 2.0 * M_PI; 
    // ==========================================
    
    cout << "Generando animacion (N=" << N_esp << ", L=" << L_esp << ", eta=" << eta_esp << ")..." << endl;
    
    generar_animacion(N_esp, eta_esp, L_esp, cono_vision, "posicionesvec.txt");
    
    cout << "Proceso completado. Archivo 'posiciones.txt' listo para Python." << endl;
    return 0;
}