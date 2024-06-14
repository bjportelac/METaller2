#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>

#include "lcgrand.h"
#include "erlangf.h"

constexpr int QUEUE_LIMIT = 1000;
constexpr int BUSY = 1;
constexpr int IDLE = 0;
constexpr int SEED_RAND_VAL = 50;

constexpr char PARAMS_ABS_PATH[] = R"(Your\absolute\path\here)";
constexpr char REPORT_ABS_PATH[] = R"(Your\absolute\path\here)";


/**
 * @file MM1Simulation.cpp
 * @brief This file contains the MM1Simulation class which simulates a single-server queueing system.
 *
 * @copyright A. M. Law, Simulation modeling and analysis. New York, Ny: Mcgraw-Hill Education, 2015.
 *
 * This Class implements the M/M/1 Single server system presented by  Law , which used ANSI-standard version of the language C, as defined by Kernighan and Ritchie (1988).
 * This version migrates the C code implementation to a C++ implementation which improves readability and debugging.
 *
 * @date June 2024.
 */

/**
 * @class MM1Simulation
 * @brief The MM1Simulation class simulates a single-server queueing system.
 *
 * The MM1Simulation class uses a queue to simulate the arrival and departure of customers in a single-server system.
 * The class contains methods to initialize the simulation, handle the arrival and departure of customers, update time-average statistical accumulators, and generate reports.
 */
class MM1Simulation {
private:
    int nextEventType, numCustomersDelayed,
            numDelaysRequired, numEvents,
            numInQueue, serverStatus;

    float areaNumInQueue, areaServerStatus,
            meanInterArrival, meanService,
            simulationTime, timeLastEvent, totalOfDelays;


    std::vector<float> timeArrival;
    std::vector<float> timeNextEvent;

    std::ifstream infileParameters;
    std::ofstream outfileReports;

    /**
     * @brief Initializes the simulation.
     *
     * This method sets the initial simulation time, server status, number in queue, and time of the last event. It also initializes the statistical counters and the event list.
     */
    void Initialize() {
        simulationTime = 0.0;

        serverStatus = IDLE;
        numInQueue = 0;
        timeLastEvent = 0.0;

        numCustomersDelayed = 0;
        totalOfDelays = 0.0;
        areaNumInQueue = 0.0;
        areaServerStatus = 0.0;

        timeNextEvent[1] = simulationTime + GetExponential(meanInterArrival);
        timeNextEvent[2] = 1.0e+30;
    }

    /**
     * @brief Determines the next event type and updates the simulation time.
     *
     * This method finds the smallest time in the timeNextEvent array and updates the simulation time to this value. If the event list is empty, it prints an error message and terminates the program.
     */
    void Timing() {
        float MinTimeNextEvent = 1.0e+29;
        nextEventType = 0;

        for (int i = 1; i <= numEvents; ++i) {
            if (timeNextEvent[i] < MinTimeNextEvent) {
                MinTimeNextEvent = timeNextEvent[i];
                nextEventType = i;
            }
        }

        if (nextEventType == 0) {
            outfileReports << "\nEvent list empty at time: " << simulationTime;
            exit(1);
        }

        simulationTime = MinTimeNextEvent;
    }

    /**
     * @brief Handles the arrival of a customer.
     *
     * This method schedules the next arrival event and checks if the server is busy. If the server is busy, it increases the number of customers in the queue. If the server is idle, it schedules the next departure event.
     */
    void Arrive() {
        float delay;

        timeNextEvent[1] = simulationTime + GetExponential(meanInterArrival);

        if (serverStatus == BUSY) {
            ++numInQueue;

            if (numInQueue > QUEUE_LIMIT) {
                outfileReports << "Error: Overflow of the rimeArrival vector at: \n";
                outfileReports << "Time: " << simulationTime << " \n";
                exit(2);
            }

            timeArrival[numInQueue] = simulationTime;
        } else {
            delay = 0.0;
            totalOfDelays += delay;

            ++numCustomersDelayed;
            serverStatus = BUSY;

            timeNextEvent[2] = simulationTime + GetExponential(meanService);
        }
    }

    /**
     * @brief Handles the departure of a customer.
     *
     * This method checks if the queue is empty. If the queue is empty, it sets the server status to idle. If the queue is not empty, it decreases the number of customers in the queue, calculates the delay, and schedules the next departure event.
     */
    void Depart() {
        float delay;

        if (numInQueue == 0) {
            serverStatus = IDLE;
            timeNextEvent[2] = 1.0e+30;
        } else {
            --numInQueue;

            delay = simulationTime - timeArrival[1];
            totalOfDelays += delay;

            ++numCustomersDelayed;
            timeNextEvent[2] = simulationTime + GetExponential(meanService);

            for (int i = 1; i <= numInQueue; ++i) {
                timeArrival[i] = timeArrival[i + 1];
            }
        }
    }

    /**
     * @brief Generates reports.
     *
     * This method calculates and prints the average delay in the queue, the average number of customers in the queue, the server utilization rate, and the simulation end time.
     */
    void Report() {
        outfileReports << "\n\n";
        outfileReports << "=============================================\n";
        outfileReports << "|| Simulation Results\n";
        outfileReports << "=============================================\n";
        outfileReports << "|| Average delay in queue: " << std::setw(10) << (totalOfDelays / static_cast<float>(numCustomersDelayed)) << " minutes.\n";
        outfileReports << "|| Average number of clients in queue: " << std::setw(10) << (areaNumInQueue / simulationTime) << " clients.\n";
        outfileReports << "|| Server utilization rate: " << std::setw(10) << (areaServerStatus / simulationTime) << " .\n";
        outfileReports << "|| Time simulation ended at: " << std::setw(10) << simulationTime << " minutes.\n";
        outfileReports << "=============================================\n";
        outfileReports << "|| Erlang formulas Values: \n";
        outfileReports << "=============================================\n";
        outfileReports << "|| Erlang B: " << std::setw(10) << ErlangB(1,meanInterArrival,meanService) << " \n";
        outfileReports << "|| Erlang C: " << std::setw(10) << ErlangC(1,meanInterArrival,meanService) << " \n";
        outfileReports << "=============================================\n";
    }

    /**
     * @brief Updates time-average statistical accumulators.
     *
     * This method calculates the time since the last event and updates the area under the number-in-queue function and the server-busy indicator function.
     */
    void UpdateTimeAvgStats() {
        float timeSinceLastEvent = simulationTime - timeLastEvent;
        timeLastEvent = simulationTime;

        areaNumInQueue += static_cast<float>(numInQueue) * timeSinceLastEvent;
        areaServerStatus += static_cast<float>(serverStatus) * timeSinceLastEvent;
    }

    /**
     * @brief Returns an exponential random variable.
     *
     * This method returns an exponential random variable using the provided mean.
     *
     * @param mean The mean value for the exponential distribution.
     * @return An exponential random variable.
     */
    static float GetExponential(float mean) {
        return -mean * std::log(LCGrand(SEED_RAND_VAL));
    }

    /**
     * @brief Closes open resources
     *
     */
    void Cleanup() {
        if (infileParameters.is_open()) {
            infileParameters.close();
        }
        if (outfileReports.is_open()) {
            outfileReports.close();
        }
    }

public:

    /**
     * @brief The MM1Simulation constructor.
     *
     * This constructor initializes the timeArrival and timeNextEvent vectors.
     */
    MM1Simulation() : timeArrival(QUEUE_LIMIT + 1), timeNextEvent(3) {
        infileParameters.open(PARAMS_ABS_PATH);
        outfileReports.open(REPORT_ABS_PATH);

        numEvents = 2;

        infileParameters >> meanInterArrival >> meanService >> numDelaysRequired;

        outfileReports << "=============================================\n";
        outfileReports << "|| Single-server queueing system (M/M/1 model)\n";
        outfileReports << "=============================================\n";
        outfileReports << "|| Mean inter-Arrival time: " << std::setw(10) << meanInterArrival << " minutes.\n";
        outfileReports << "|| Mean service time:      " << std::setw(10) << meanService << " minutes.\n";
        outfileReports << "|| Number of customers:    " << std::setw(10) << numDelaysRequired << " customers.\n";
        outfileReports << "|| Seed value for reproduction: " << std::setw(10) << SEED_RAND_VAL << "\n";
        outfileReports << "=============================================\n";
    }

    /**
     * @brief Runs the simulation.
     *
     * This method initializes the simulation, then runs it until the specified number of customers have been delayed. It determines the next event, updates the time-average statistical accumulators, and handles the arrival or departure of customers based on the event type. Finally, it generates reports.
     */
    void run() {
        Initialize();

        while (numCustomersDelayed < numDelaysRequired) {
            Timing();
            UpdateTimeAvgStats();

            switch (nextEventType) {
                case 1:
                    Arrive();
                    break;
                case 2:
                    Depart();
                    break;
            }
        }

        Report();
        Cleanup();
    }

};

int main() {
    try {
        MM1Simulation simulation;
        simulation.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 3;
    }
    return 0;
}