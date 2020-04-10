#include <fstream>

void processConfigFile( const char * configFile, ConfigParms & cparms )
{
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
    while (std::getline(infile, line))
    {
        // get rid of comments 
        bool comment == false;
        for (int i = 0; i < line.size(); i++)
        {
            if (line[i] == "#") comment = true;
            if (comment) line[i] = ' ';
        }

        // Fill cparms
        std::istringstream iss(line)
        string variable;
        unsigned int value; 
        iss >> variable >> value; 

        switch (variable)
        {
            case "StopCost": cparms.stopCost = value; break;
            case "NumStudents": cparms.numStudents = value; break;
            case "NumStops": cparms.numStops = value; break;
            case "MaxNumStudents": cparms.maxNumStudents = value; break;
            case "TimerDelay": cparms.timerDelay = value; break;
            case "MaxStudentDelay": cparms.maxStudentDelay = value; break;
            case "MaxStudentTrips": cparms.maxStudentTrips = value; break;
            case "GroupoffDelay": cparms.groupoffDelay = value; break;
            case "ConductorDelay": cparms.conductorDelay = value; break;
            case "ParentalDelay": cparms.parentalDelay = value; break;
            case "NumCouriers": cparms.numCouriers = value; break;
        }
    }
}