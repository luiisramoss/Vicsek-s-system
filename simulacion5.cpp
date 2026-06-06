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

// Función de simulación que sirve tanto para datos como para animación
double simular(int N, double eta, double L, bool guardar_posiciones, string nombre_archivo) {
    const double vi = 0.03; // Velocidad de las partículas
    const double r = 1.0;
    const double dt = 1.0;
    const int pasos_totales = 20000;
    const int pasos_equilibrio = 400;

    vector<double> x(N), y(N), theta(N);
    ofstream f_pos;
    if (guardar_posiciones) f_pos.open(nombre_archivo);

    for (int i = 0; i < N; i++) {
        x[i] = (rand() / (double)RAND_MAX) * L;
        y[i] = (rand() / (double)RAND_MAX) * L;
        theta[i] = (rand() / (double)RAND_MAX) * 2.0 * M_PI;
    }
    int M = (int)(L / r);              // número de celdas por lado
    double cell_size = L / M;

    // grid[i][j] contiene índices de partículas en esa celda
    vector<vector<vector<int>>> grid(M, vector<vector<int>>(M));
    double suma_va = 0.0;
    int muestras = 0;

    for (int t = 0; t < pasos_totales; t++) {
        // --- 1. GUARDAR ESTADO ACTUAL ---
        if (guardar_posiciones && (t % 100 == 0)) {
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
        // --- GRID: limpiar ---
        for (int i = 0; i < M; i++)
            for (int j = 0; j < M; j++)
                grid[i][j].clear();

        // --- GRID: asignar partículas ---
        for (int i = 0; i < N; i++) {
            int cx = (int)(x[i] / cell_size);
            int cy = (int)(y[i] / cell_size);
            grid[cx][cy].push_back(i);
        }

        vector<double> n_theta(N);
        
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < N; i++) {
            double sS = 0, sC = 0;
            int cx = (int)(x[i] / cell_size);
            int cy = (int)(y[i] / cell_size);

            // recorrer celdas vecinas (incluida la propia)
            for (int dx_cell = -1; dx_cell <= 1; dx_cell++) {
                for (int dy_cell = -1; dy_cell <= 1; dy_cell++) {

                    int nx = (cx + dx_cell + M) % M;
                    int ny = (cy + dy_cell + M) % M;

                    for (int j : grid[nx][ny]) {

                        double dx = x[j] - x[i];
                        double dy = y[j] - y[i];

                        // condiciones periódicas
                        if (dx > L/2.0) dx -= L; else if (dx < -L/2.0) dx += L;
                        if (dy > L/2.0) dy -= L; else if (dy < -L/2.0) dy += L;

                        if ((dx*dx + dy*dy) <= r*r) {
                            sS += sin(theta[j]);
                            sC += cos(theta[j]);
                        }
                    }
                }
            }
          
            double ruido = ((rand() / (double)RAND_MAX) - 0.5) * eta;
            n_theta[i] = atan2(sS, sC) + ruido;
        }
        theta = n_theta;

        // --- 4. CÁLCULO DE V_A ---
        if (t >= pasos_equilibrio) {
            double vx = 0, vy = 0;
            for (int i = 0; i < N; i++) { vx += cos(theta[i]); vy += sin(theta[i]); }
            suma_va += sqrt(vx*vx + vy*vy) / (double)N;
            muestras++;
        }
    }
    if (guardar_posiciones) f_pos.close();
    return (muestras > 0) ? (suma_va / muestras) : 0;
}

int main() {
    srand(time(NULL));

    // --- PARTE 1: CASO CONCRETO PARA ANIMACIÓN ---
    int N_esp = 20000;
    double eta_esp = 0.5;
    double L_esp = 200.0; 
    
    cout << "Generando animacion para N=" << N_esp << ", L=" << L_esp << ", eta=" << eta_esp << "..." << endl;
    simular(N_esp, eta_esp, L_esp, true, "posiciones.txt");


    // --- PARTE 2: ESTUDIO ESTADÍSTICO SUAVIZADO ---
    // SE ELIMINARON N=4000 Y N=10000 PARA AGILIZAR LA EJECUCIÓN
    vector<int> lista_N = {40, 100, 400,1000}; 
    double rho = 4.0;
    int num_realizaciones = 3; 

    ofstream fout("resultados_va_eta.txt");

    for (int n_val : lista_N) {
        double L_calc = sqrt(n_val / rho);
        cout << "Calculando N=" << n_val << endl;

        // Alta resolución (pasos de 0.25)
        for (double e = 0.0; e <= 5.0; e += 0.25) { 
            double va_acumulado_realizaciones = 0.0;

            for (int r = 0; r < num_realizaciones; r++) {
                va_acumulado_realizaciones += simular(n_val, e, L_calc, false, "");
            }

            double va_final = va_acumulado_realizaciones / num_realizaciones;
            fout << n_val << " " << e << " " << va_final << endl;
        }
        fout << endl;
    }
    fout.close();


    // --- PARTE 3: va vs DENSIDAD ---
    cout << "Parte 3: Calculando va vs Densidad (L=20, eta=0.5)..." << endl;
    double L_parte3 = 20.0;
    double eta_parte3 = 2.0;
    int realizaciones = 5;
    ofstream fout_b("resultados_va_rho.txt");
    fout_b << "# rho va_promedio N" << endl;

    // Solo mantenemos la lista original de densidades para graficar el comportamiento b
    vector<int> lista_N_densidad = {20, 40, 80, 160, 320, 640, 1280, 2560, 3800}; 

    for (int n_val : lista_N_densidad) {
        double current_rho = n_val / (L_parte3 * L_parte3);
        double va_acumulado = 0.0;

        for (int r = 0; r < realizaciones; r++) {
            va_acumulado += simular(n_val, eta_parte3, L_parte3, false, "");
        }

        double va_final = va_acumulado / (double)realizaciones;
        fout_b << current_rho << " " << va_final << " " << n_val << endl;
        cout << "  Calculado para rho = " << current_rho << " (N=" << n_val << ")" << endl;
    }

    fout_b.close();

    cout << "Proceso completado." << endl;
    return 0;
}