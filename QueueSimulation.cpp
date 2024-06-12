#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "lcgrand.h"

constexpr int QUEUE_LIMIT = 100;
constexpr int BUSY = 1;
constexpr int IDLE = 0;

/**
 * @file QueueSimulation.cpp
 * @brief This file contains the QueueSimulation class which simulates a single-server queueing system.
 */

/**
 * @class QueueSimulation
 * @brief The QueueSimulation class simulates a single-server queueing system.
 *
 * The QueueSimulation class uses a queue to simulate the arrival and departure of customers in a single-server system.
 * The class contains methods to initialize the simulation, handle the arrival and departure of customers, update time-average statistical accumulators, and generate reports.
 */
class QueueSimulation {
private:
    int nextEventType, numCustomersDelayed,
        numDelaysRequired, numEvents,
        numInQueue,serverStatus;

    float  areaNumInQueue, areaServerStatus,
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
    void Initialize(){
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
    void Timing(){
        float MinTimeNextEvent = 1.0e+29;
        nextEventType = 0;

        for (int i = 0; i < numEvents; ++i) {
            if(timeNextEvent[i] < MinTimeNextEvent){
                MinTimeNextEvent = timeNextEvent[i];
                nextEventType = i;
            }
        }

        if (nextEventType == 0){
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
    void Arrive(){
        float delay;

        timeNextEvent[1] = simulationTime + GetExponential(meanInterArrival);

        if (serverStatus == BUSY){
            ++numInQueue;

            if (numInQueue > QUEUE_LIMIT){
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
    void Depart(){
        float delay;

        if (numInQueue == 0){
            serverStatus = IDLE;
            timeNextEvent[2] = 1.0e+30;
        } else {
            --numInQueue;

            delay = simulationTime - timeArrival[1];
            totalOfDelays += delay;

            ++numCustomersDelayed;
            timeNextEvent[2] = simulationTime + GetExponential(meanService);

            for (int i = 1; i <= numInQueue ; ++i) {
                timeArrival[i] = timeArrival[i + 1];
            }
        }
    }

    /**
     * @brief Generates reports.
     *
     * This method calculates and prints the average delay in the queue, the average number of customers in the queue, the server utilization rate, and the simulation end time.
     */
    void Report(){
        outfileReports << "\n\n";
        outfileReports << "Average delay in queue: " << (totalOfDelays/static_cast<float>(numCustomersDelayed)) << " minutes. \n";
        outfileReports << "Average number of clients in queue: " << (areaNumInQueue/simulationTime) << " clients. \n";
        outfileReports << "Server utilization rate: " << (areaServerStatus/simulationTime) << " . \n";
        outfileReports << "Time simulation ended at: " << simulationTime << " minutes. \n";
    }

    /**
     * @brief Updates time-average statistical accumulators.
     *
     * This method calculates the time since the last event and updates the area under the number-in-queue function and the server-busy indicator function.
     */
    void UpdateTimeAvgStats(){
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
    float GetExponential(float mean){
        return -mean * std::log(LCGrand(1));
    }

public:

    /**
     * @brief The QueueSimulation constructor.
     *
     * This constructor initializes the timeArrival and timeNextEvent vectors.
     */
    QueueSimulation() : timeArrival(QUEUE_LIMIT + 1), timeNextEvent(3){
        /*infileParameters.open("params.txt");
        outfileReports.open("reports.txt");

        numEvents = 2;

        infileParameters >> meanInterArrival >> meanService >> numDelaysRequired;

        outfileReports << "Single-server queueing system (M/M/1 model): \n";
        outfileReports << "Mean inter-Arrival time: " << meanInterArrival << " minutes. \n";
        outfileReports << "Mean service time: " << meanService << " minutes. \n";
        outfileReports << "Number of customers: " << numDelaysRequired << " customers. \n";*/
    }

    /**
     * @brief Runs the simulation.
     *
     * This method initializes the simulation, then runs it until the specified number of customers have been delayed. It determines the next event, updates the time-average statistical accumulators, and handles the arrival or departure of customers based on the event type. Finally, it generates reports.
     */
    void run(){
        Initialize();

        while(numCustomersDelayed < numDelaysRequired){
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
        infileParameters.close();
        outfileReports.close();
    }

    void printFileContents() {
        std::string line;

        // Print the contents of the input file
        std::ifstream infile("params.txt");
        if (infile.is_open()) {
            std::cout << "Contents of the input file (params.txt):\n";
            while (getline(infile, line)) {
                std::cout << line << '\n';
            }
            infile.close();
        } else {
            std::cout << "Unable to open the input file.\n";
        }

        // Print the contents of the output file
        std::ifstream outfile("reports.txt");
        if (outfile.is_open()) {
            std::cout << "\nContents of the output file (reports.txt):\n";
            while (getline(outfile, line)) {
                std::cout << line << '\n';
            }
            outfile.close();
        } else {
            std::cout << "Unable to open the output file.\n";
        }
    }
};

int main() {
    try {
        QueueSimulation simulation;
        simulation.printFileContents();
        //simulation.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}