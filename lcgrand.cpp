/**
 * @archivo lcgrand.cpp
 * @brief Generador lineal congruencial para generación de números pseudoaleatorios
 * @autor http://www.sju.edu/~sforman/courses/2000F_CSC_5835/ de Simlib.c
 * /

/* Constantes utilizadas en el algoritmo de generación de números pseudoaleatorios */

#define MODULUS 2147483647 /**< Límite superior en el rango de números que se pueden generar */
#define MULT1 24112 /**< Multiplicador utilizado en la generación del siguiente número aleatorio */
#define MULT2 26143 /**< Segundo multiplicador utilizado en la generación del siguiente número aleatorio */

/* Matriz de semillas para los primeros 100 números */
long zrng[] = {
        1,
        1973272912, 281629770, 20006270, 1280689831, 2096730329, 1933576050,
        913566091, 246780520, 1363774876, 604901985, 1511192140, 1259851944,
        824064364, 150493284, 242708531, 75253171, 1964472944, 1202299975,
        233217322, 1911216000, 726370533, 403498145, 993232223, 1103205531,
        762430696, 1922803170, 1385516923, 76271663, 413682397, 726466604,
        336157058, 1432650381, 1120463904, 595778810, 877722890, 1046574445,
        68911991, 2088367019, 748545416, 622401386, 2122378830, 640690903,
        1774806513, 2132545692, 2079249579, 78130110, 852776735, 1187867272,
        1351423507, 1645973084, 1997049139, 922510944, 2045512870, 898585771,
        243649545, 1004818771, 773686062, 403188473, 372279877, 1901633463,
        498067494, 2087759558, 493157915, 597104727, 1530940798, 1814496276,
        536444882, 1663153658, 855503735, 67784357, 1432404475, 619691088,
        119025595, 880802310, 176192644, 1116780070, 277854671, 1366580350,
        1142483975, 2026948561, 1053920743, 786262391, 1792203830, 1494667770,
        1923011392, 1433700034, 1244184613, 1147297105, 539712780, 1545929719,
        190641742, 1645390429, 264907697, 620389253, 1502074852, 927711160,
        364849192, 2049576050, 638580085, 547070247
};

/**
 * @brief Genera el siguiente número pseudoaleatorio
 *
 * Esta función genera el siguiente número pseudoaleatorio en la secuencia usando el algoritmo Generador Lineal Congruencial.
 *
 * @param num Índice en la matriz semilla
 * @return El siguiente número pseudoaleatorio como float en el rango de 0 a 1
 */
float LCGrand(int num) {

    long zi, lowprd, hi31;

    zi = zrng[num];
    lowprd = (zi & 65535) * MULT1;
    hi31 = (zi >> 16) * MULT1 + (lowprd >> 16);
    zi = ((lowprd & 65535) - MODULUS) + ((hi31 & 32767) << 16) + (hi31 >> 15);

    if (zi < 0) { zi += MODULUS; }

    lowprd = (zi & 65535) * MULT2;
    hi31 = (zi >> 16) * MULT2 + (lowprd >> 16);
    zi = ((lowprd & 65535) - MODULUS) + ((hi31 & 32767) << 16) + (hi31 >> 15);

    if (zi < 0) { zi += MODULUS; }

    zrng[num] = zi;

    return static_cast<float>((zi >> 7 | 1) / 16777216.0);
}