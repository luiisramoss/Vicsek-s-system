#ifndef _GLIBCXX_HAVE_QUICK_EXIT
#define _GLIBCXX_HAVE_QUICK_EXIT 1
#endif
#define _GLIBCXX_HAS_GTHREADS 1

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <omp.h>
#include <random>

using namespace std;

// Estructura para almacenar los momentos estadísticos de cada simulación
struct Stats {
    double va;
    double va2;
    double va4;
};

// Función de simulación con Ruido Vectorial / Intrínseco
Stats simular(int N, double eta, double L, bool guardar_posiciones, string nombre_archivo) {
    const double vi = 0.03; // Velocidad de las partículas
    const double r = 1.0;
    const double dt = 1.0;
    
    // AUMENTADOS para mejorar la estadística del Cumulante de Binder
    const int pasos_totales = 3000;  
    const int pasos_equilibrio = 1000;

    vector<double> x(N), y(N), theta(N);
    ofstream f_pos;
    if (guardar_posiciones) f_pos.open(nombre_archivo);

    // Inicialización aleatoria
    for (int i = 0; i < N; i++) {
        x[i] = (rand() / (double)RAND_MAX) * L;
        y[i] = (rand() / (double)RAND_MAX) * L;
        theta[i] = (rand() / (double)RAND_MAX) * 2.0 * M_PI;
    }
    int M = (int)(L / r);              // número de celdas por lado
    double cell_size = L / M;

    vector<vector<vector<int>>> grid(M, vector<vector<int>>(M));
    
    // Acumuladores estadísticos
    double suma_va = 0.0;
    double suma_va2 = 0.0;
    double suma_va4 = 0.0;
    int muestras = 0;

    for (int t = 0; t < pasos_totales; t++) {
        // --- 1. GUARDAR ESTADO ACTUAL ---
        if (guardar_posiciones) {
            for (int i = 0; i < N; i++) {
                f_pos << x[i] << "," << y[i] << "," << cos(theta[i]) << "," << sin(theta[i]) << "\n";
            }
            f_pos << "\n"; // Separador de frames
        }

        // --- 2. ACTUALIZAR POSICIONES ---
        for (int i = 0; i < N; i++) {
            x[i] += cos(theta[i]) * vi * dt;
            y[i] += sin(theta[i]) * vi * dt;

            // Condiciones de contorno periódicas
            if (x[i] >= L) x[i] -= L; else if (x[i] < 0) x[i] += L;
            if (y[i] >= L) y[i] -= L; else if (y[i] < 0) y[i] += L;
        }

        // --- 3. CALCULAR NUEVAS DIRECCIONES ---
        for (int i = 0; i < M; i++)
            for (int j = 0; j < M; j++)
                grid[i][j].clear();

        for (int i = 0; i < N; i++) {
            int cx = (int)(x[i] / cell_size);
            int cy = (int)(y[i] / cell_size);
            grid[cx][cy].push_back(i);
        }

        vector<double> n_theta(N);
        
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < N; i++) {
            double sS = 0, sC = 0;
            int n_vecinos = 0; 
            
            int cx = (int)(x[i] / cell_size);
            int cy = (int)(y[i] / cell_size);

            for (int dx_cell = -1; dx_cell <= 1; dx_cell++) {
                for (int dy_cell = -1; dy_cell <= 1; dy_cell++) {

                    int nx = (cx + dx_cell + M) % M;
                    int ny = (cy + dy_cell + M) % M;

                    for (int j : grid[nx][ny]) {
                        double dx = x[j] - x[i];
                        double dy = y[j] - y[i];

                        if (dx > L/2.0) dx -= L; else if (dx < -L/2.0) dx += L;
                        if (dy > L/2.0) dy -= L; else if (dy < -L/2.0) dy += L;

                        if ((dx*dx + dy*dy) <= r*r) {
                            sS += sin(theta[j]);
                            sC += cos(theta[j]);
                            n_vecinos++;
                        }
                    }
                }
            }
          
            // Ruido vectorial escalado por la densidad local (n_vecinos)
            double angulo_ruido = (rand() / (double)RAND_MAX) * 2.0 * M_PI;
            sC += n_vecinos * eta * cos(angulo_ruido); 
            sS += n_vecinos * eta * sin(angulo_ruido);
            
            n_theta[i] = atan2(sS, sC);
        }
        theta = n_theta;

        // --- 4. CÁLCULO DE MOMENTOS ESTADÍSTICOS (TERMOLIZACIÓN) ---
        if (t >= pasos_equilibrio) {
            double vx = 0, vy = 0;
            for (int i = 0; i < N; i++) { 
                vx += cos(theta[i]); 
                vy += sin(theta[i]); 
            }
            double va_actual = sqrt(vx*vx + vy*vy) / (double)N;
            
            suma_va += va_actual;
            suma_va2 += (va_actual * va_actual);
            suma_va4 += (va_actual * va_actual * va_actual * va_actual);
            muestras++;
        }
    }
    if (guardar_posiciones) f_pos.close();
    
    Stats res;
    if (muestras > 0) {
        res.va = suma_va / muestras;
        res.va2 = suma_va2 / muestras;
        res.va4 = suma_va4 / muestras;
    } else {
        res.va = 0; res.va2 = 0; res.va4 = 0;
    }
    return res;
}

int main() {
    srand(time(NULL));

    // --- PARTE 1: ANIMACIÓN ---
    int N_esp = 1000;
    double eta_esp = 2;
    double L_esp = 20; 
    
    cout << "Generando animacion para N=" << N_esp << "..." << endl;
    simular(N_esp, eta_esp, L_esp, true, "posicionesvec.txt");


    // --- PARTE 2: CUMULANTE DE BINDER Y VA VS RUIDO (ALTA RESOLUCIÓN) ---
    vector<int> lista_N = {40, 100, 400, 1000}; 
    double rho = 4.0;
    // Mantenemos 10 realizaciones para asegurar buena estadística
    int num_realizaciones = 10; 

    ofstream fout("resultados_binder_etavec.txt");
    fout << "# N eta va_promedio binder" << endl;

    cout << "\nCalculando parametros criticos y Cumulante de Binder (Alta Resolucion)..." << endl;
    for (int n_val : lista_N) {
        double L_calc = sqrt(n_val / rho);
        cout << " -> Procesando N = " << n_val << endl;

        // ¡EL CAMBIO ESTÁ AQUÍ! 
        // Vamos de 0.0 a 1.0 con pasos de 0.02 (50 puntos de resolución)
        for (double e = 0.0; e <= 1.0; e += 0.02) { 
            double va_acum = 0.0;
            double va2_acum = 0.0;
            double va4_acum = 0.0;

            for (int r = 0; r < num_realizaciones; r++) {
                Stats res = simular(n_val, e, L_calc, false, "");
                va_acum += res.va;
                va2_acum += res.va2;
                va4_acum += res.va4;
            }

            double va_final = va_acum / num_realizaciones;
            double va2_final = va2_acum / num_realizaciones;
            double va4_final = va4_acum / num_realizaciones;

            // Fórmula del Cumulante de Binder (G)
            double binder = 0.0;
            if (va2_final > 0) {
                binder = 1.0 - (va4_final) / (3.0 * va2_final * va2_final);
            }

            fout << n_val << " " << e << " " << va_final << " " << binder << endl;
        }
    }
    fout.close();

    // --- PARTE 3: VA VS DENSIDAD ---
    cout << "\nCalculando va vs Densidad (L=20, eta=0.5)..." << endl;
    double L_parte3 = 20.0;
    double eta_parte3 = 0.5;
    int realizaciones_densidad = 5;
    
    ofstream fout_b("resultados_va_rhovec.txt");
    fout_b << "# rho va_promedio N" << endl;

    vector<int> lista_N_densidad = {10,15, 20,25, 30,35, 40,50, 60,70, 80,90, 100, 110,120, 130,160, 200,240,280, 320, 400, 600, 800,1000,1100,1200,1300,1400,1500,1600}; 

    for (int n_val : lista_N_densidad) {
        double current_rho = n_val / (L_parte3 * L_parte3);
        double va_acumulado = 0.0;

        for (int r = 0; r < realizaciones_densidad; r++) {
            Stats res = simular(n_val, eta_parte3, L_parte3, false, "");
            va_acumulado += res.va;
        }

        double va_final = va_acumulado / (double)realizaciones_densidad;
        fout_b << current_rho << " " << va_final << " " << n_val << endl;
    }

    fout_b.close();

    cout << "Proceso completado con exito." << endl;
    return 0;
}