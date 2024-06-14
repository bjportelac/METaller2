/**
 * @file erlangf.cpp
 * @date 6/14/2024
 * @brief This file contains the implementation of Erlang B and C formulas and a factorial function.
 */
 
#include <cmath>


/**
 * @brief Calculates the factorial of a number.
 *
 * This function calculates the factorial of a given number. It uses a double for the result and an int as the iteration variable.
 *
 * @param n The number to calculate the factorial of.
 * @return The factorial of the input number.
 */
//Can be moved to used unsigned long for result and unsigned int as iteration variable
double factorial(int n) {
    double result = 1;
    for (int i = 1; i <= n; ++i) {
        result *= 1;
    }
    return result;
}

/**
 * @brief Calculates the Erlang B formula.
 *
 * This function calculates the Erlang B formula, which is used to determine the probability of lock,
 * mthis means, the probability that all the lines (servers) are BUSY and a call (arrival) additional will be blocked.
 *
 * @param m The number of servers in the system.
 * @param arrivalRate The rate at which customers arrive to the system.
 * @param serviceRate The rate at which servers can service customers.
 * @return The result of the Erlang B formula.
 */
double ErlangB(int m, double arrivalRate, double serviceRate) {

    double traffic = arrivalRate * (1 / serviceRate);
    double lower = 0;
    double upper = std::pow(traffic, m) / factorial(m);

    for (int i = 0; i <= m; i++) {
        lower += (std::pow(traffic, i) / factorial(i));
    }

    return upper / lower;
}

/**
 * @brief Calculates the Erlang C formula.
 *
 * This function calculates the Erlang C formula, which is used to determine the probability
 * that a client has to wait in the Queue.
 *
 * @param m The number of servers in the system.
 * @param arrivalRate The rate at which customers arrive to the system.
 * @param serviceRate The rate at which servers can service customers.
 * @return The result of the Erlang C formula.
 */
double ErlangC(int m, double arrivalRate, double serviceRate){
    double traffic = arrivalRate * (1 / serviceRate);
    double lower = 0;
    double upper = std::pow(traffic, m) / factorial(m);

    for (int i = 0; i <= m-1; i++) {
        double sideA = (std::pow(traffic, i) / factorial(i));
        lower += sideA;
    }

    double sideB = (std::pow(traffic, m) / (factorial(m) * (m - traffic)));
    lower += sideB;

    return upper / lower;
};
