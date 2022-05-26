// Black box functions
void ejectBlackBox();               // Ejects the black box if a majority of electronic controllers agree to do so. 
void deployParachute();             // Deploys the parachute if a majority of electronic controllers agree to do so.
void blackBoxAppendState();         // Appends the state of each microcontroller to the black box. The state is comprised of altitude, airspeed, heading, etc.
void recoveryStart();               // Activates recovery beacons if a majority of electronic controllers agree to do so. 

// Processing Functions
state pState{};                             // Create a global variable representing previous state
bool shouldEject()                          //  Computes whether the black box should be ejected. 
{
    state cState = getState();              // Gets current aircraft and black box sensor states, already accounting for possible errors
    bool E = false;                         // Start value at false for ejection vote
    
    E |= abs(pState.alt - cState.alt) > 20; // If the distance between the previous altitude and the current altitude is greater than 20 then eject
                                            // this represents a change of 40 feet/sec, which is well outside of operational conditions
    E |= abs(cState.pitch) > 30*(360/65535) // If the pitch is 30 degrees up or down, eject, the aircraft is outside of operational conditions
    
    E |= abs(cState.roll) > 50*(360/65535)  // If the roll angle is greater than 50 degrees, the aircraft is outside of operational conditions
    
    pState = getState();                    // Update the previous state
};

int main()                                              //  Entry point for program
{                                                       //
    while (true)                                        //  While the controller is running, do this until the black box is ejected.
    {
        if(pilotEjectSwitch())                          // If the pilot indicates a manual ejection, 
        { 
            ejectBlackBox();                                // Vote to eject the black box
            break;                                          // Exit the infinite loop
        }
        else if(shouldEject())                          // If automated systems detect a critical flight error where the aircraft is likely going down, 
        {
            ejectBlackBox();                                // Vote to eject the black box
            break;                                          // Exit the infinite loop
        }
        else                                            // If the above contitions were not true,
        {
            sleep_ms(10000);                                  // Put the controller into a sleep state for 10 seconds to reduce power consumption and give the pilots a chance to recover
        }
    }

    // Black box ejected! Determine when parachute should be deployed
    while (true)
    {
        if( (blackBoxgetAltitude() < 5000) && (blackBoxgetAltitude() > 50) ) // If the altitude is below 5000 feet,
        {
            deployParachute()                               // Vote to deploy the parachute
            blackBoxAppendState()                           // Records that the black box voted to deploy the parachute
            
        }
        elseif(blackBoxgetAltitude() <= 50)             // If the altitide is below 50 feet,
        {
            recoveryStart();                            // Vote to start the recovery functionality
            blackBoxAppendState()                       // Records that the black box voted to started the beacons
        }
        
        sleep_ms(10000);                                  // Put the controller into a sleep state for 10 seconds to save power and give the pilots a chance to recover
    }
    return 0;
}