#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

// Función de simulación que sirve tanto para datos como para animación
double simular(int N, double eta, double L, double theta_v, bool guardar_posiciones, string nombre_archivo) {
    const double vi = 0.03; 
    const double r = 1.0;
    const double dt = 1.0;
    const int pasos_totales = 3000;
    const int pasos_equilibrio = 2000;

    vector<double> x(N), y(N), theta(N);
    ofstream f_pos;
    if (guardar_posiciones) f_pos.open(nombre_archivo);

    for (int i = 0; i < N; i++) {
        x[i] = (rand() / (double)RAND_MAX) * L;
        y[i] = (rand() / (double)RAND_MAX) * L;
        theta[i] = (rand() / (double)RAND_MAX) * 2.0 * M_PI;
    }

    double suma_va = 0.0;
    int muestras = 0;

    for (int t = 0; t < pasos_totales; t++) {
        // --- 1. GUARDAR ESTADO ACTUAL ---
        if (guardar_posiciones) {
            for (int i = 0; i < N; i++) {
                // x, y, vx, vy
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

        // --- 3. CALCULAR NUEVAS DIRECCIONES (Cono de visión) ---
        vector<double> n_theta(N);
        for (int i = 0; i < N; i++) {
            double sS = 0, sC = 0;
            for (int j = 0; j < N; j++) {
                double dx = x[j] - x[i];
                double dy = y[j] - y[i];

                // Distancia mínima (imagen espejo)
                if (dx > L/2.0) dx -= L; else if (dx < -L/2.0) dx += L;
                if (dy > L/2.0) dy -= L; else if (dy < -L/2.0) dy += L;

                double d2= dx*dx + dy*dy;

                if (d2 <= r*r) { // Usar r*r es más rápido que
                    if (i==j){
                    sS += sin(theta[i]);
                sC += cos(theta[i]);
            } else {
                // Ángulo del vector que va de i a j
                double angulo_ij = atan2(dy, dx);
                
                // Diferencia angular entre mi dirección actual y la posición de j
                double dif = angulo_ij - theta[i];
                
                // Normalizar la diferencia al rango [-PI, PI]
                while (dif <= -M_PI) dif += 2.0 * M_PI;
                while (dif > M_PI) dif -= 2.0 * M_PI;

                // Solo sumamos si está dentro del cono theta_v / 2
                if (abs(dif) <= theta_v / 2.0) {
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
    // Aquí eliges libremente tus valores "especiales"
    int N_esp = 1000;
    double eta_esp = 0.1;
    double L_esp = 25.0; // L elegida manualmente
    double cono_vision = (2.0 * M_PI) / 3.0; // 120 grados
    
    cout << "Generando animacion para N=" << N_esp << ", L=" << L_esp << ", eta=" << eta_esp << "..." << endl;
    simular(N_esp, eta_esp, L_esp, cono_vision, true, "posicionesANGULO.txt");


    // --- PARTE 2: ESTUDIO DE v_a vs ANGULO DE VISIÓN ---
    cout << "\nCalculando va en funcion del angulo de vision..." << endl;
    
    // Parámetros fijos para este experimento
    int N_estudio = 400;       // Usamos 400 para que no tarde demasiado
    double L_estudio = 14;   // Densidad media para asegurar interacciones
    double eta_estudio = 1.0;  // Ruido bajo para que el orden sea posible
    int realizaciones = 5;     // Promedio de varias simulaciones para suavizar la gráfica

    ofstream fout("va_vs_angulo.txt");
    fout << "# angulo_rad va_promedio" << endl;

    // Barrido de ángulos desde 0 hasta 2*PI en pequeños incrementos
    for (double angulo = 0.0; angulo <= 2.0 * M_PI; angulo += 0.2) {
        double va_acumulado = 0.0;
        
        for (int r = 0; r < realizaciones; r++) {
            // Llamamos a simular sin guardar posiciones (false)
            va_acumulado += simular(N_estudio, eta_estudio, L_estudio, angulo, false, "");
        }
        
        double va_final = va_acumulado / realizaciones;
        fout << angulo << " " << va_final << endl;
        
        // Imprimimos en pantalla para ver el progreso (convertido a grados)
        cout << "  Angulo: " << angulo * 180.0 / M_PI << " grados -> va = " << va_final << endl;
    }
    fout.close();
    cout << "Archivo 'va_vs_angulo.txt' generado con exito." << endl;

return 0;
}