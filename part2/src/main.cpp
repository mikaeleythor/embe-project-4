#include <stdio.h>
#include <pigpio.h>

int main(int argc, char *argv[])
{
    // Initialize pigpio
    if (gpioInitialise() < 0)
    {
        printf("pigpio initialization failed.\n");
        return 1;
    }

    printf("pigpio initialized successfully.\n");

    int gpio_pin = 12; // Use hardware PWM pin (GPIO 18 or 12, 13, 19)

    // Set hardware PWM frequency and duty cycle
    int frequency = 5000;  // 5kHz
    int dutycycle = 500000; // Duty cycle in range 0-1000000 (50% duty cycle)

    // Start hardware PWM on the specified pin
    gpioHardwarePWM(gpio_pin, frequency, dutycycle);

    // Run PWM for 10 seconds
    gpioDelay(10000000); // Delay in microseconds (10 seconds)

    // Stop the hardware PWM
    gpioHardwarePWM(gpio_pin, 0, 0);

    // Terminate pigpio
    gpioTerminate();

    return 0;
}

