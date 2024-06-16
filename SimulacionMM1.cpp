#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>

#include "lcgrand.h"
#include "erlangf.h"
#include "CustomerData.h"

constexpr int LIMITE_COLA = 2000;
constexpr int OCUPADO = 1;
constexpr int LIBRE = 0;
constexpr int VAL_ALE_SEMILLA = 1;

constexpr char PARAMS_ABS_PATH[] = R"(path/absoluto/aqui)";
constexpr char REPORT_ABS_PATH[] = R"(path/absoluto/aqui)";


/**
 * @archivo SimulacionMM1.cpp
 * @brief Este archivo contiene la clase SimulacionMM1 que simula un sistema de colas de un solo servidor.
 *
 * @copyright A. M. Law, Simulation modeling and analysis. New York, Ny: Mcgraw-Hill Education, 2015.
 * @coautor Jorge Eduardo Ortiz Triviño - Version en español del codigo original.
 *
 * Esta Clase implementa el sistema de servidor único M/M/1 presentado por Law, que utilizaba la versión estándar ANSI del lenguaje C, según lo definido por Kernighan y Ritchie (1988).
 * Esta versión migra la implementación del código C a una implementación de C++ que mejora la legibilidad y la depuración.
 *
 * @fecha junio de 2024.
*/

/**
 * @clase MM1Simulación
 * @brief La clase SimulacionMM1 simula un sistema de cola de un solo servidor.
 *
 * La clase SimulacionMM1 utiliza una cola para simular la llegada y salida de clientes en un sistema de servidor único.
 * La clase contiene métodos para inicializar la simulación, manejar la llegada y salida de clientes, actualizar acumuladores estadísticos de tiempo promedio y generar informes.
 */
class SimulacionMM1 {
private:
    int sigTipoEvento, numClientesEsperando,
            numEsperasRequerido, numEventos,
            numEnCola, estadoServidor, numCliente;

    float areaNumEnCola, areaEstadoServidor,
            mediaEntreLlegadas, mediaServicio,
            tiempoSimulacion, tiempoUltimoEvento, totalDeEsperas,
            tiempoUltimaLlegada;

    std::vector<float> tiempoLlegada;
    std::vector<float> tiempoProximoEvento;
    std::vector<CustomerData> datosClientes;

    std::ifstream parametros;
    std::ofstream resultados;

    /**
     * @brief Inicializa la simulación.
     *
     * Este método establece la hora de simulación inicial, el estado del servidor, el número en la cola y la hora del último evento. También inicializa los contadores estadísticos y la lista de eventos.
     */
    void inicializar() {
        tiempoSimulacion = 0.0;

        estadoServidor = LIBRE;
        numEnCola = 0;
        tiempoUltimoEvento = 0.0;

        numClientesEsperando = 0;
        totalDeEsperas = 0.0;
        areaNumEnCola = 0.0;
        areaEstadoServidor = 0.0;
        tiempoUltimaLlegada = 0.0;
        numCliente = 0;

        tiempoProximoEvento[1] = tiempoSimulacion + exponencial(mediaEntreLlegadas);
        tiempoProximoEvento[2] = 1.0e+30;
    }

    /**
     * @brief Determina el siguiente tipo de evento y actualiza el tiempo de simulación.
     *
     * Este método encuentra el tiempo más pequeño en la matriz tiempoProximoEvento y actualiza el tiempo de simulación a este valor. Si la lista de eventos está vacía, imprime un mensaje de error y finaliza el programa.
     */
    void temporizador() {
        float minTiempoSigEvento = 1.0e+29;
        sigTipoEvento = 0;

        for (int i = 1; i <= numEventos; ++i) {
            if (tiempoProximoEvento[i] < minTiempoSigEvento) {
                minTiempoSigEvento = tiempoProximoEvento[i];
                sigTipoEvento = i;
            }
        }

        if (sigTipoEvento == 0) {
            resultados << "\nLista de Eventos vacia en el instante: " << tiempoSimulacion;
            exit(1);
        }

        tiempoSimulacion = minTiempoSigEvento;
    }

    /**
     * @brief Gestiona la llegada de un cliente.
     *
     * Este método programa el próximo evento de llegada y verifica si el servidor está ocupado. Si el servidor está ocupado, aumenta la cantidad de clientes en la cola. Si el servidor está inactivo, programa el próximo evento de salida.
     */
    void llegada() {
        float demora, tiempoEntreLlegadas;

        tiempoProximoEvento[1] = tiempoSimulacion + exponencial(mediaEntreLlegadas);

        tiempoEntreLlegadas = tiempoSimulacion - tiempoUltimaLlegada;
        tiempoUltimaLlegada = tiempoSimulacion;

        CustomerData cliente{};
        cliente.timeDifference = tiempoEntreLlegadas;
        datosClientes.push_back(cliente);

        if (estadoServidor == OCUPADO) {
            ++numEnCola;

            if (numEnCola > LIMITE_COLA) {
                resultados << "Error: Sobrecarga en el vector tiempoLlegada: \n";
                resultados << "Tiempo: " << tiempoSimulacion << " \n";
                exit(2);
            }
            tiempoLlegada[numEnCola] = tiempoSimulacion;
        } else {
            demora = 0.0;
            totalDeEsperas += demora;

            ++numClientesEsperando;
            estadoServidor = OCUPADO;

            tiempoProximoEvento[2] = tiempoSimulacion + exponencial(mediaServicio);
        }
    }

    /**
     * @brief Gestiona la salida de un cliente.
     *
     * Este método comprueba si la cola está vacía. Si la cola está vacía, establece el estado del servidor en inactivo. Si la cola no está vacía, disminuye la cantidad de clientes en la cola, calcula el retraso y programa el próximo evento de salida.
     */
    void salida() {
        float demora;

        if (numEnCola == 0) {
            estadoServidor = LIBRE;
            tiempoProximoEvento[2] = 1.0e+30;
        } else {
            --numEnCola;

            demora = tiempoSimulacion - tiempoLlegada[1];
            totalDeEsperas += demora;

            datosClientes[numCliente].attentionTime = demora;
            datosClientes[numCliente].id = numCliente + 1;
            numCliente++;

            ++numClientesEsperando;
            tiempoProximoEvento[2] = tiempoSimulacion + exponencial(mediaServicio);
            for (int i = 1; i <= numEnCola; ++i) {
                tiempoLlegada[i] = tiempoLlegada[i + 1];
            }
        }
    }

    /**
     * @brief Genera informes.
     *
     * Este método calcula e imprime el retraso promedio en la cola, el número promedio de clientes en la cola, la tasa de utilización del servidor y la hora de finalización de la simulación.
     */
    void reporte() {
        resultados << "\n\n";
        resultados << "=============================================\n";
        resultados << "|| Resultado de la Simulacion\n";
        resultados << "=============================================\n";
        resultados << "|| Demora promedio en la cola:             " << std::setw(10) << (totalDeEsperas / static_cast<float>(numClientesEsperando)) << " minutos.\n";
        resultados << "|| Numero de Clientes promedio en la cola: " << std::setw(10) << (areaNumEnCola / tiempoSimulacion) << " clientes.\n";
        resultados << "|| Tasa de uso del servidor:                " << std::setw(10) << (areaEstadoServidor / tiempoSimulacion) << " .\n";
        resultados << "|| Simulacion terminada a los:             " << std::setw(10) << tiempoSimulacion << " minutos.\n";
        resultados << "=============================================\n";
        resultados << "|| Valores de las formulas de Erlang: \n";
        resultados << "=============================================\n";
        resultados << "|| Erlang B:                               " << std::setw(10) << ErlangB(1, mediaEntreLlegadas, mediaServicio) << " \n";
        resultados << "|| Erlang C:                               " << std::setw(10) << ErlangC(1, mediaEntreLlegadas, mediaServicio) << " \n";
        resultados << "=============================================\n";

        resultados << "=============================================\n";
        resultados << "|| Data de los clientes\n";
        resultados << "=============================================\n";
        resultados << "ID , Tiempo de llegada entre el cliente y su antecesor , Tiempo de atencion del cliente\n";
        for (const auto &customer: datosClientes) {
            if (customer.id == 0) {
                continue;
            }
            resultados << customer.id << " , " << customer.timeDifference << " , " << customer.attentionTime << "\n";
        }
        resultados << "=============================================\n";
    }

    /**
     * @brief Actualiza acumuladores estadísticos de tiempo promedio.
     *
     * Este método calcula el tiempo desde el último evento y actualiza el área bajo la función de número en cola y la función de indicador de servidor ocupado.
     */
    void actEstadisticasTiempoProm() {
        float timeSinceLastEvent = tiempoSimulacion - tiempoUltimoEvento;
        tiempoUltimoEvento = tiempoSimulacion;

        areaNumEnCola += static_cast<float>(numEnCola) * timeSinceLastEvent;
        areaEstadoServidor += static_cast<float>(estadoServidor) * timeSinceLastEvent;
    }

    /**
     * @brief Devuelve una variable aleatoria exponencial.
     *
     * Este método devuelve una variable aleatoria exponencial utilizando la media proporcionada.
     *
     * @param media El valor medio para la distribución exponencial.
     * @return Una variable aleatoria exponencial.
     */
    static float exponencial(float media) {
        return -media * std::log(LCGrand(VAL_ALE_SEMILLA));
    }

    /**
     * @brief Cierra recursos abiertos
     */
    void limpieza() {
        if (parametros.is_open()) {
            parametros.close();
        }
        if (resultados.is_open()) {
            resultados.close();
        }
    }

public:

    /**
     * @brief El constructor de SimulaciónMM1.
     *
     * Este constructor inicializa los vectores tiempoLlegada y tiempoProximoEvento.
     */
    SimulacionMM1() : tiempoLlegada(LIMITE_COLA + 1), tiempoProximoEvento(3) {
        parametros.open(PARAMS_ABS_PATH);
        resultados.open(REPORT_ABS_PATH);

        numEventos = 2;

        parametros >> mediaEntreLlegadas >> mediaServicio >> numEsperasRequerido;

        resultados << "=============================================\n";
        resultados << "|| Modelo de Colas Servidor Unico (Modelo M/M/1)\n";
        resultados << "=============================================\n";
        resultados << "|| Media entre llegadas:   " << std::setw(10) << mediaEntreLlegadas << " minutos.\n";
        resultados << "|| Media del servicio:     " << std::setw(10) << mediaServicio << " minutos.\n";
        resultados << "|| Numero de clientes:     " << std::setw(10) << numEsperasRequerido << " clientes.\n";
        resultados << "|| Valor de la semilla:    " << std::setw(10) << VAL_ALE_SEMILLA << "\n";
        resultados << "=============================================\n";
    }

    /**
     * @brief Ejecuta la simulación.
     *
     * Este método inicializa la simulación y luego la ejecuta hasta que se haya retrasado el número especificado de clientes. Determina el próximo evento, actualiza los acumuladores estadísticos de tiempo promedio y maneja la llegada o salida de clientes según el tipo de evento. Finalmente, genera informes.
     */
    void ejecutar() {
        inicializar();

        while (numClientesEsperando < numEsperasRequerido) {
            temporizador();
            actEstadisticasTiempoProm();

            switch (sigTipoEvento) {
                case 1:
                    llegada();
                    break;
                case 2:
                    salida();
                    break;
            }
        }

        reporte();
        limpieza();
    }

};

int main() {
    try {
        SimulacionMM1 simulacionMM1;
        simulacionMM1.ejecutar();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 3;
    }
    return 0;
}