/**
 * @file lcgrand.cpp
 * @brief Linear Congruential Generator for Pseudorandom Number Generation
 * @author http://www.sju.edu/~sforman/courses/2000F_CSC_5835/ from Simlib.c
 * /

/* Constants used in the pseudorandom number generation algorithm */

#define MODULUS 2147483647 /**< Upper bound on the range of numbers that can be generated */
#define MULT1       24112 /**< Multiplier used in the generation of the next random number */
#define MULT2       26143 /**< Second multiplier used in the generation of the next random number */

/* Array of seeds for the first 100 numbers */
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
 * @brief Generates the next pseudorandom number
 *
 * This function generates the next pseudorandom number in the sequence using the Linear Congruential Generator algorithm.
 *
 * @param num Index into the seed array
 * @return The next pseudorandom number as a double in the range of 0 to 1
 */
double LCGrand(int num) {

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

    return (zi >> 7 | 1) / 16777216.0;
}





