/**
 * @archivo erlangf.cpp
 * @fecha 14/06/2024
 * @brief Este archivo contiene la implementación de fórmulas Erlang B y C y una función factorial.
 */
 
#include <cmath>


/**
 * @brief Calcula el factorial de un número.
 *
 * Esta función calcula el factorial de un número dado. Utiliza un double para el resultado y un int como variable de iteración.
 * Puede usarse con unsigned long long para números más grandes y con unsigned int para la iteracion
 *
 * @param n El número para calcular el factorial.
 * @return El factorial del número de entrada.
 */
double factorial(int n) {
    double result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= 1;
    }
    return result;
}

/**
 * @brief Calcula la fórmula de Erlang B.
 *
 * Esta función calcula la fórmula Erlang B, que se utiliza para determinar la probabilidad de bloqueo,
 * esto significa, la probabilidad de que todas las líneas (servidores) estén OCUPADOS y una llamada (llegada) adicional será bloqueada.
 *
 * @param m La cantidad de servidores en el sistema.
 * @param tasaLlegadas La velocidad a la que llegan los clientes al sistema.
 * @param tasaServicio La velocidad a la que los servidores pueden atender a los clientes.
 * @return El resultado de la fórmula Erlang B.
 */
double ErlangB(int m, double tasaLlegadas, double tasaServicio) {

    double trafico = tasaLlegadas * (1 / tasaServicio);
    double lower = 0;
    double upper = std::pow(trafico, m) / factorial(m);

    for (int i = 0; i <= m; i++) {
        lower += (std::pow(trafico, i) / factorial(i));
    }

    return upper / lower;
}

/**
 * @brief Calcula la fórmula de Erlang C.
 *
 * Esta función calcula la fórmula de Erlang C, que se utiliza para determinar la probabilidad
 * que un cliente tiene que esperar en la cola.
 *
 * @param m La cantidad de servidores en el sistema.
 * @param tasaLlegadas La velocidad a la que llegan los clientes al sistema.
 * @param tasaServicio La velocidad a la que los servidores pueden atender a los clientes.
 * @return El resultado de la fórmula Erlang C.
 */
double ErlangC(int m, double tasaLlegadas, double tasaServicio){
    double trafico = tasaLlegadas * (1 / tasaServicio);
    double lower = 0;
    double upper = std::pow(trafico, m) / factorial(m);

    for (int i = 0; i <= m-1; i++) {
        double sideA = (std::pow(trafico, i) / factorial(i));
        lower += sideA;
    }

    double sideB = (std::pow(trafico, m) / (factorial(m) * (m - trafico)));
    lower += sideB;

    return upper / lower;
};
