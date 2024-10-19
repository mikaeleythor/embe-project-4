#include <chrono>
#include <cstdlib>
#include <iostream>
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CONT_DELAY_MS 5
#define REF_VEL 3000
#define MAX_VEL 3350
#define KP 0.59
#define MAX_DUTY_CYCLE 1000000
#define MIN_DUTY_CYCLE 0

#define PRINT_COUNT 20

static int print_counter = 0;
void conditional_print(double velocity, int pwm) {
  print_counter++;
  if (print_counter >= PRINT_COUNT) {
    print_counter = 0;
    auto timestamp = std::chrono::system_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(timestamp.time_since_epoch()).count();
    std::cout << time;
    printf(": v=%.2f\tpwm=%d\n", velocity, pwm);
  }
}

int main(int argc, char *argv[]) {
  int target_velocity;
  if (argc > 1) {
    target_velocity = atoi(argv[1]);
  } else {
    target_velocity = REF_VEL;
  }

  FILE *fp;
  char buffer[20];

  // Initialize pigpio
  if (gpioInitialise() < 0) {
    printf("pigpio initialization failed.\n");
    return 1;
  }

  printf("pigpio initialized successfully.\n");

  int gpio_pin = 12; // Use hardware PWM pin (GPIO 18 or 12, 13, 19)

  // Set hardware PWM frequency and duty cycle
  int frequency = 500000; // 5kHz
  int dutycycle = 500000; // Duty cycle in range 0-1000000 (50% duty cycle)

  // Start hardware PWM on the specified pin
  gpioHardwarePWM(gpio_pin, frequency, dutycycle);

  unsigned long int counter = 0, last_count = 0;
  long int diff;
  double velocity;
  while (true) {
    // Read the counter value
    fp = fopen("/proc/counter", "r");
    if (fp == NULL) {
      perror("Failed to open /proc/counter");
      return EXIT_FAILURE;
    }

    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
      last_count = counter;
      counter = std::strtoul(buffer, nullptr, 10);
      diff = counter - last_count;
      velocity = (double)diff / (unsigned long int)CONT_DELAY_MS;
      double pwm = KP * (target_velocity - velocity) / MAX_VEL * MAX_DUTY_CYCLE;
      pwm = (pwm > MAX_DUTY_CYCLE) ? MAX_DUTY_CYCLE : pwm;
      pwm = (pwm < MIN_DUTY_CYCLE) ? MIN_DUTY_CYCLE : pwm;
      gpioHardwarePWM(gpio_pin, frequency, pwm);
      conditional_print(velocity, pwm);
    }

    fclose(fp);

    // Run PWM for 10 seconds
    usleep(CONT_DELAY_MS * 1000); // Delay in microseconds (10 seconds)
  }

  // Stop the hardware PWM
  gpioHardwarePWM(gpio_pin, 0, 0);

  // Terminate pigpio
  gpioTerminate();

  return EXIT_SUCCESS;
}
