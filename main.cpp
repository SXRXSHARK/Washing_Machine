  
#include "DigitalOut.h"
#include "PinNames.h"
#include "mbed.h"
#include <cstdio>

// Define output pins
BusOut leds_bus(PC_0, PC_1, PB_0, PA_4); // Create BusOut Object called led_bus and assign bus pins (LSB, ....., MSB)

// Define analogue inputs to read potentiometer voltage
AnalogIn potentiometer1(PA_5); // For washing machine settings
AnalogIn potentiometer2(PA_6); // For temperature settings

// Define start/stop button
DigitalIn startStopButton(PC_10);

// Define temperature LEDs (Blue, Green, Red)
DigitalOut blueLed(PB_10); // Change to PB_10
DigitalOut greenLed(PB_5); // Change to PB_5
DigitalOut redLed(PB_3);   // Change to PB_3

// Define buzzer pin
PwmOut buzzer(PA_15); // Buzzer pin

// Define light sensor pin
AnalogIn ldr(PA_1); // Light sensor pin

// Define DigitalIn pin for start button
DigitalIn startButton(PC_11); // Start button pin
DigitalIn emergencyStopButton(PD_2);

DigitalOut seg1(PA_12);
DigitalOut seg2(PA_11);
DigitalOut seg3(PB_12);
DigitalOut seg4(PB_11);
DigitalOut seg5(PB_2);
DigitalOut seg6(PB_1);
DigitalOut seg7(PB_15);
DigitalOut seg8(PB_14);



// Segment pins order: A, B, C, D, E, F, G
BusOut SegDis(PA_11, PA_12, PB_1, PB_15, PB_14, PB_12, PB_11);
DigitalOut decimal_point(PB_2);

// Hexadecimal values for displaying numbers 0 to 9 and letters A to F
int hexDis[] = {0x00, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

// Define wait time between sensor readings in milliseconds
const int WAIT_TIME_MS = 1000; // Example: 1 second

// Declare function prototypes
bool cycleStarted = false;
void init_leds();
void play_simple_melody(const int *melody, const int *tempo);
bool isDoorClosed()
{
    return ldr.read() < 0.10f; // Return true if light sensor reading is below 0.10
}

void displayDigit(int digit)
{
    // Display a single digit on the seven-segment display
    SegDis.write(hexDis[digit]);
}

// Declare variables
float pot_value1 = 0.0f;        // Create & initialise float variable to store potentiometer value for washing machine settings
float pot_value2 = 0.0f;        // Create & initialise float variable to store potentiometer value for temperature settings
int led_state = 0;              // Create & initialise int variable to control the output state of the bus
bool machineOn = false;          // Variable to track if the machine is on
bool melodyPlayed = false;       // Variable to track if the melody has been played
int prev_led_state = 0;          // Variable to track the previous LED state
const char *prev_setting = "";  // Variable to store the previous setting
float prev_temp_setting = 0.0f; // Variable to store the previous temperature setting

// Array to store settings corresponding to each LED transition
const char *settings[] = {
    "Cottons",    // Setting for LED 1
    "Synthetics", // Setting for LED 2
    "Delicates",  // Setting for LED 3
    "Quick Wash"  // Setting for LED 4
};

// Define note frequencies
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523

#define BUZZER_NOTE_C5 523
#define BUZZER_NOTE_G4 392
#define BUZZER_NOTE_E4 330
#define BUZZER_NOTE_A4 440

// Define the melody for the emergency stop buzzer tune
const int buzzer1_melody[] = {BUZZER_NOTE_A4, BUZZER_NOTE_E4, BUZZER_NOTE_G4, BUZZER_NOTE_C5
};

// Define the tempo for the emergency stop buzzer tune
const int buzzer1_tempo[] = {
    200, 200, 200, 200, 200, 200 // Duration of each sound and silence
};

// Function to play the buzzer tune for emergency stop

const int simple_melody[] = {
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};

const int simple_tempo[] = {
    500, 500, 500, 500, 500, 500, 500, 500
};

const int simple1_melody[] = {
    NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4
};

const int simple1_tempo[] = {
    500, 500, 500, 500, 500, 500, 500, 500
};

void playSimpleMelody(const int *melody, const int *tempo);

#define BUZZER_NOTE_C5 523
#define BUZZER_NOTE_G4 392
#define BUZZER_NOTE_E4 330
#define BUZZER_NOTE_A4 440

// Define the melody for the buzzer tune
const int buzzer_melody[] = {
    BUZZER_NOTE_C5, BUZZER_NOTE_G4, BUZZER_NOTE_E4, BUZZER_NOTE_A4
};

// Define the tempo for the buzzer tune
const int buzzer_tempo[] = {
    250, 250, 250, 250 // Adjust tempo as needed
};

#define BUZZER_NOTE_E5 659
#define BUZZER_NOTE_G5 784
#define BUZZER_NOTE_C6 1047
#define BUZZER_NOTE_D6 1175

// Define the melody for the buzzer tune

// Function to play the buzzer tune
void play_buzzer_tune(const int *melody, const int *tempo)
{
    for (int i = 0; i < sizeof(buzzer_melody) / sizeof(buzzer_melody[0]); i++)
    {
        int duration = tempo[i]; // Duration of the note
        int note = melody[i];    // Get the current note

        // Set buzzer frequency and pulse width to play the note
        buzzer.period_us(1000000 / note);
        buzzer.write(0.5); // Set duty cycle to 50% for sound
        buzzer.pulsewidth_us(buzzer.read_period_us() / 2);

        // Play the note for the calculated duration
        ThisThread::sleep_for(duration);
        // Silence the buzzer after playing the note
        buzzer.write(0);
        ThisThread::sleep_for(10ms); // Add a small delay between notes
    }
}
// Function to play the buzzer tune for emergency stop
void play_buzzer_tune1(const int *melody, const int *tempo)
{
    for (int i = 0; i < sizeof(buzzer1_melody) / sizeof(buzzer1_melody[0]); i++)
    {
        int duration = tempo[i]; // Duration of the note
        int note = melody[i];    // Get the current note

        // Set buzzer frequency and pulse width to play the note
        buzzer.period_us(1000000 / note);
        buzzer.write(0.5); // Set duty cycle to 50% for sound
        buzzer.pulsewidth_us(buzzer.read_period_us() / 2);

        // Play the note for the calculated duration
        ThisThread::sleep_for(duration);
        // Silence the buzzer after playing the note
        buzzer.write(0);
        ThisThread::sleep_for(10ms); // Add a small delay between notes
    }
}
// Function to stop the washing machine cycle
    void emergencyStop() {
    machineOn = false;       // Turn off the machine
    cycleStarted = false;    // Reset cycle status
    melodyPlayed = false;    // Reset melody played state
    buzzer.pulsewidth_us(0); // Silence the buzzer
    leds_bus = 0; // Turn off all LEDs
    buzzer = 0; // Stop the buzzer
    blueLed = 0;
    greenLed = 0;
    redLed = 0;
    SegDis = 0;
    play_buzzer_tune1(buzzer1_melody, buzzer1_tempo);
    buzzer.pulsewidth_us(0);
    printf("The emergency stop button has been activated.\n");
    printf("Please press the start/stop button to reset it\n");
   
}


// Function to start the washing machine cycle
void startCycle() {
    float ldrValue = ldr.read(); // Read LDR value

    // Check if the door is closed
    if (isDoorClosed()) {
        // Cycle can start
        cycleStarted = true;
        leds_bus = 0;
        blueLed = 0;
        greenLed = 0;
        redLed = 0;

        // Print current settings and inform that settings cannot be changed
        printf("Current setting: %s\n", settings[led_state - 1]);
        printf("Current Temperature: ");
        if (prev_temp_setting == 0) {
            printf("40°C\n");
        } else if (prev_temp_setting == 1) {
            printf("50°C\n");
        } else {
            printf("60°C\n");
        }
        printf("The settings cannot be changed.\n"); // Print message
        printf("Door is closed now. Cycle has started\n");
        printf("Wash Cycle in progress...\n");

        // Countdown timer for 9 seconds
        for (int seconds = 9; seconds >= 0; seconds--) {
            if (emergencyStopButton == 1) {
                emergencyStop(); // Call emergency stop function if button is pressed
                return;
            }
            blueLed = 1;
            greenLed = 1;
            redLed = 1;
            printf("Time remaining: %d seconds\n", seconds);

            // Display the countdown on the seven-segment display
            displayDigit(seconds);

            ThisThread::sleep_for(1s);
        }

        printf("Wash cycle is complete.\n");

        ThisThread::sleep_for(2s);

        printf("Draining in progress....\n");

        // Countdown timer for draining
        for (int seconds = 5; seconds >= 0; seconds--) {
            if (emergencyStopButton == 1) {
                emergencyStop(); // Call emergency stop function if button is pressed
                return;
            }
            blueLed = 0;
            greenLed = 1;
            redLed = 1;
            printf("Time remaining: %d seconds\n", seconds);

            // Display the countdown on the seven-segment display
            displayDigit(seconds);

            ThisThread::sleep_for(1s);
        }

        printf("Draining is complete. Please open the door.\n");
        play_buzzer_tune(buzzer_melody, buzzer_tempo);
        SegDis = 0;

        blueLed = 1;
        greenLed = 1;
        redLed = 0; // Turn off red LED to indicate cycle completion
    } else {
        // Door is not closed properly
        printf("Door is not closed properly. Please close the door to begin the cycle.\n");

        // Wait until the door is closed properly
        while (!isDoorClosed()) {
            ThisThread::sleep_for(100ms); // Adjust the time interval as needed
        }

        // Cycle can start
        cycleStarted = true;
        leds_bus = 0;
        blueLed = 0;
        greenLed = 0;
        redLed = 0;

        // Print current settings and inform that settings cannot be changed
        printf("Current setting: %s\n", settings[led_state - 1]);
        printf("Current Temperature: ");
        if (prev_temp_setting == 0) {
            printf("40°C\n");
        } else if (prev_temp_setting == 1) {
            printf("50°C\n");
        } else {
            printf("60°C\n");
        }
        printf("The settings cannot be changed.\n"); // Print message
        printf("Door is closed now. Cycle has started\n");
        printf("Wash Cycle in progress...\n");

        // Countdown timer for 9 seconds
        for (int seconds = 9; seconds >= 0; seconds--) {
            if (emergencyStopButton == 1) {
                emergencyStop(); // Call emergency stop function if button is pressed
                return;
            }
            blueLed = 1;
            greenLed = 1;
            redLed = 1;
            printf("Time remaining: %d seconds\n", seconds);

            // Display the countdown on the seven-segment display
            displayDigit(seconds);

            ThisThread::sleep_for(1s);
        }

        printf("Wash Cycle is complete.\n");

        ThisThread::sleep_for(2s);

        printf("Draining in progress....\n");

        // Countdown timer for draining
        for (int seconds = 5; seconds >= 0; seconds--) {
            if (emergencyStopButton == 1) {
                emergencyStop(); // Call emergency stop function if button is pressed
                return;
            }
            blueLed = 0;
            greenLed = 1;
            redLed = 1;
            printf("Time remaining: %d seconds\n", seconds);

            // Display the countdown on the seven-segment display
            displayDigit(seconds);

            ThisThread::sleep_for(1s);
        }

        printf("Draining is complete. Please open the door.\n");
        play_buzzer_tune(buzzer_melody, buzzer_tempo);
        // Clear the seven-segment display after the cycle is complete
        SegDis = 0;

        // Turn off blue, green, and red LEDs to indicate cycle completion
        blueLed = 1;
        greenLed = 1;
        redLed = 0;
    }
}

    
// Function to check and close the door  
void checkAndCloseDoor()
{
    // If the door is opened (light detected), wait for it to close
    if (!isDoorClosed())
    {
        printf("The door is opened, please close the door...\n");

        // Indicate to the user to close the door by lighting up LEDs
        blueLed = 1;
        greenLed = 0;
        redLed = 1;

        // Wait for the door to close
        while (!isDoorClosed())
        {
            ThisThread::sleep_for(100ms); // Wait for the door to close
        }

        // Inform the user that the door is closed
        printf("The door is closed.\n");
        leds_bus = 0; // Turn off all LEDs
        blueLed = 0;
        redLed = 0;
        greenLed = 0;
        printf("Restarting the cycle...\n");
        ThisThread::sleep_for(5s); // Delay to allow user to release the button
        machineOn = false;        // Turn off the machine
        cycleStarted = false;     // Reset cycle status
        melodyPlayed = false;     // Reset melody played state
        printf("Please select the wash cycle setting and temperature.\n");
    }

    // While the start button is not pressed, allow user to adjust settings
    while (startButton == 0)
    {
        // Check if the stop button is pressed
        if (startStopButton == 1)
        {
            leds_bus = 0; // Turn off all LEDs
            machineOn = false; // Update machine state
            melodyPlayed = false; // Reset melody played state
            buzzer = 0; // Stop the buzzer
            SegDis = 0; // Clear the seven-segment display
            return; // Exit the function
        }

        // Read potentiometer values
        pot_value1 = potentiometer1.read(); // Washing machine settings
        pot_value2 = potentiometer2.read(); // Temperature settings

        // Map potentiometer value to the range of LEDs (1 to 4) for washing machine settings
        // and round to the nearest integer
        led_state = static_cast<int>(pot_value1 * 4) + 1;

        // Turn on the LED corresponding to the current setting for washing machine settings
        leds_bus = (1 << (led_state - 1));

        // Check if the setting has changed for washing machine settings
        if (prev_led_state != led_state)
        {
            // Print the new setting if it has changed for washing machine settings
            printf("Wash cycle setting: %s\n", settings[led_state - 1]);
            prev_led_state = led_state; // Update the previous LED state
        }

        // Adjust temperature settings based on potentiometer value
        int temp_setting = static_cast<int>(pot_value2 * 3); // Map to 0, 1, 2
        if (temp_setting != prev_temp_setting)
        {
            // Print temperature setting only if it has changed
            printf("Temperature: ");
            if (temp_setting == 0)
            {
                blueLed = 1;
                greenLed = 0;
                redLed = 0;
                printf("40°C\n");
            }
            else if (temp_setting == 1)
            {
                blueLed = 0;
                greenLed = 1;
                redLed = 0;
                printf("50°C\n");
            }
            else
            {
                blueLed = 0;
                greenLed = 0;
                redLed = 1;
                printf("60°C\n");
            }
            prev_temp_setting = temp_setting;
        }

        ThisThread::sleep_for(100ms); // Add a small delay to prevent rapid loop execution
    }
}




int main() {
    init_leds(); // Initialise LEDs
    SegDis.write(0x00); // Initialize segments
    ThisThread::sleep_for(2s);
    emergencyStopButton.mode(PullUp); // Set emergency stop button as pull-up

    bool lightDetected = false;
    // Main loop
    while (1) {
        // Check if the start/stop button is pressed
        if (startStopButton == 1) {
            // Toggle the washing machine state
            if (!machineOn) {
                printf("Washing machine is turned on.\n");
                // Play a melody if not already played
                if (!melodyPlayed) {
                    play_simple_melody(simple_melody, simple_tempo);
                    melodyPlayed = true;
                    buzzer.pulsewidth_us(0);
                }
                machineOn = true; // Update machine state
                cycleStarted = false; // Reset cycle state when machine is started
                printf("Please select the wash cycle setting and temperature.\n");
            } else {
                printf("Washing machine is turned off.\n"); // Print message
                leds_bus = 0; // Turn off all LEDs
                machineOn = false; // Update machine state
                melodyPlayed = false;
                buzzer = 0; // Stop the buzzer
                blueLed = 0;
                greenLed = 0;
                redLed = 0;
                SegDis = 0;
                play_simple_melody(simple1_melody, simple1_tempo);
                buzzer.pulsewidth_us(0);
            }
            // Wait until the button is released
            while (startStopButton == 1);
        } 

        // If the machine is turned on
        if (machineOn) {
            // Read potentiometer values only if the cycle has not started
            if (!cycleStarted) { 
                // Read potentiometer values
                pot_value1 = potentiometer1.read(); // Washing machine settings
                pot_value2 = potentiometer2.read(); // Temperature settings

                // Map potentiometer value to the range of LEDs (1 to 4) for washing machine settings
                // and round to the nearest integer
                led_state = static_cast<int>(pot_value1 * 4) + 1;

                // Turn on the LED corresponding to the current setting for washing machine settings
                leds_bus = (1 << (led_state - 1));

                // Check if the setting has changed for washing machine settings
                if (prev_led_state != led_state) {
                    // Print the new setting if it has changed for washing machine settings
                    printf("Wash cycle setting: %s\n", settings[led_state - 1]);
                    prev_led_state = led_state; // Update the previous LED state
                }

                // Adjust temperature settings based on potentiometer value
                int temp_setting = static_cast<int>(pot_value2 * 3); // Map to 0, 1, 2
                if (temp_setting != prev_temp_setting) { 
                    // Print temperature setting only if it has changed
                    printf("Temperature: ");
                    if (temp_setting == 0) {
                        blueLed = 1;
                        greenLed = 0;
                        redLed = 0;
                        printf("40∞C\n");
                    } else if (temp_setting == 1) {
                        blueLed = 0;
                        greenLed = 1;
                        redLed = 0;
                        printf("50∞C\n");
                    } else {
                        blueLed = 0;
                        greenLed = 0;
                        redLed = 1;
                        printf("60∞C\n");
                    }
                    prev_temp_setting = temp_setting;
                }
            }
        }

        // Check if the start button is pressed
        if (startButton == 1 && machineOn) {
            startCycle(); // Start the washing machine cycle
            int seconds = 0;
            SegDis = 0;
            
        }

        // Check if the start/stop button is pressed before starting the cycle
         if (machineOn && startButton == 0 && cycleStarted)
        {
            ThisThread::sleep_for(3s);
            if (!isDoorClosed())
            {
                // Close the door automatically
                checkAndCloseDoor();
                SegDis = 0;
                machineOn = true;
                cycleStarted = true;
                lightDetected = false;
                
            }
        }
    }

        ThisThread::sleep_for(WAIT_TIME_MS);
    }



// Function to initialize LEDs
void init_leds()
{
    // Add your initialization code here
}

// Function to play a simple melody
void play_simple_melody(const int *melody, const int *tempo) {
    for (int i = 0; i < sizeof(simple_melody) / sizeof(simple_melody[0]); i++) {
        int duration = tempo[i]; // Duration of the note
        int note = melody[i];    // Get the current note

        // Set buzzer frequency and pulse width to play the note
        buzzer.period_us(1000000 / note);
        buzzer.pulsewidth_us(buzzer.read_period_us() / 2);
        
        // Play the note for the calculated duration
        ThisThread::sleep_for(duration);
    }
}
// Function to play a simple but different melody
void playSimpleMelody(const int *melody, const int *tempo) {
    for (int i = 0; i < sizeof(simple1_melody) / sizeof(simple1_melody[0]); i++) {
        int duration = tempo[i]; // Duration of the note
        int note = melody[i];    // Get the current note

        // Set buzzer frequency and pulse width to play the note
        buzzer.period_us(1000000 / note);
        buzzer.pulsewidth_us(buzzer.read_period_us() / 2);
        
        // Play the note for the calculated duration
        ThisThread::sleep_for(duration);
 }
}           