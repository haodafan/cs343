#include "config.h"
#include <fstream>
#include <sstream>
#include <iostream>

void processConfigFile( const char * configFile, ConfigParms & cparms )
{
    std::cout << "PROCESSING CONFIG FILE" << std::endl;

    // First, set defaults
    cparms.stopCost = 1;
    cparms.numStudents = 2; 
    cparms.numStops = 5;
    cparms.maxNumStudents = 5;
    cparms.timerDelay = 2; 
    cparms.maxStudentDelay = 10;
    cparms.maxStudentTrips = 3; 
    cparms.groupoffDelay = 10; 
    cparms.conductorDelay = 5; 
    cparms.parentalDelay = 10; 
    cparms.numCouriers = 1;

    /*
    Note that there must be at least 2 stops, at least 1 student, and at least 1 courier in the system. 
    The stopCost and maxNumStudents must be greater than 0.
    */
    
    // New values read from config file 
    std::ifstream file(configFile);

    std::string line;
    while (std::getline(file, line))
    {
        // get rid of comments 
        bool comment = false;
        for (unsigned int i = 0; i < line.size(); i++)
        {
            if (line[i] == '#') comment = true;
            if (comment) line[i] = ' ';
        }

        // Fill cparms
        std::istringstream iss(line);
        std::string variable;
        unsigned int value; 
        iss >> variable >> value; 

        if (variable == "StopCost") cparms.stopCost = value;
        else if (variable == "NumStudents") cparms.numStudents = value;
        else if (variable == "NumStops") cparms.numStops = value;
        else if (variable == "MaxNumStudents") cparms.maxNumStudents = value;
        else if (variable == "TimerDelay") cparms.timerDelay = value;
        else if (variable == "MaxStudentDelay") cparms.maxStudentDelay = value;
        else if (variable == "MaxStudentTrips") cparms.maxStudentTrips = value;
        else if (variable == "GroupoffDelay") cparms.groupoffDelay = value;
        else if (variable == "ConductorDelay") cparms.conductorDelay = value;
        else if (variable == "ParentalDelay") cparms.parentalDelay = value;
        else if (variable == "NumCouriers") cparms.numCouriers = value;
    }

    // DEBUGGING
    std::cout << "StopCost " << cparms.stopCost << std::endl; 
    std::cout << "NumStudents " << cparms.numStudents << std::endl; 
    std::cout << "NumStops " << cparms.numStops << std::endl; 
    std::cout << "MaxNumStudents " << cparms.maxNumStudents << std::endl; 
    std::cout << "TimerDelay " << cparms.timerDelay << std::endl; 
    std::cout << "MaxStudentDelay " << cparms.maxStudentDelay << std::endl; 
    std::cout << "MaxStudentTrips " << cparms.maxStudentTrips << std::endl; 
    std::cout << "GroupoffDelay " << cparms.groupoffDelay << std::endl; 
    std::cout << "ConductorDelay " << cparms.conductorDelay << std::endl; 
    std::cout << "ParentalDelay " << cparms.parentalDelay << std::endl; 
    std::cout << "NumCouriers " << cparms.numCouriers << std::endl; 
    // END OF DEBUGGING
}