#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

/* Meta Information */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eythor");
MODULE_DESCRIPTION(
    "Encoder counter for Project 4 in Embedded Systems Programming");

#define GPIO_17 529 // according to /sys/kernel/debug/gpio
#define PROC_NAME "counter"

/** variable contains pin number o interrupt controller to which GPIO 17 is
 * mapped to */
unsigned int irq_number;

static long unsigned int counter;
static struct proc_dir_entry *entry;
static DEFINE_MUTEX(counter_mutex); // Mutex to protect the counter

/**
 * @brief Interrupt service routine is called, when interrupt is triggered
 */

static irqreturn_t gpio_irq_handler(int irq, void *dev_id) {
  mutex_lock(&counter_mutex);
  counter++;
  mutex_unlock(&counter_mutex);
  return IRQ_HANDLED;
}

static ssize_t counter_read(struct file *file, char __user *buf, size_t count,
                            loff_t *offset) {
  char buffer[20]; // Buffer to hold the counter value
  int len;

  mutex_lock(&counter_mutex);
  len = snprintf(buffer, sizeof(buffer), "%lu\n", counter);

  // Check if offset is at the end of the file
  if (*offset >= len) {
    mutex_unlock(&counter_mutex);
    return 0; // End of file
  }

  // Copy the counter value to user space
  if (copy_to_user(buf, buffer, len)) {
    mutex_unlock(&counter_mutex);
    return -EFAULT;
  }

  *offset += len; // Update the offset
  mutex_unlock(&counter_mutex);
  return len; // Return the number of bytes read
}

const struct proc_ops fops = {
    .proc_read = counter_read,
    // .proc_lseek = seq_lseek,
    // .proc_release = single_release,
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
  printk("qpio_irq: Loading module... ");

  /* Create proc entry */
  entry = proc_create(PROC_NAME, 0660, NULL, &fops);
  if (!entry) {
    return -ENOMEM; // Error creating /proc entry
  }
  printk("/proc/%s created\n", PROC_NAME);

  /* Setup the gpio */
  gpio_free(GPIO_17);
  if (gpio_request(GPIO_17, "rpi-gpio-17")) {
    printk("Error!\nCan not allocate GPIO 17\n");
    return -1;
  }

  /* Set GPIO 17 direction */
  if (gpio_direction_input(17)) {
    printk("Error!\nCan not set GPIO 17 to input!\n");
    gpio_free(GPIO_17);
    return -1;
  }

  /* Setup the interrupt */
  irq_number = gpio_to_irq(GPIO_17);

  if (request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING,
                  "my_gpio_irq", NULL) != 0) {
    printk("Error!\nCan not request interrupt nr.: %d\n", irq_number);
    gpio_free(GPIO_17);

    return -1;
  }

  printk("Done!\n");
  printk("GPIO 17 is mapped to IRQ Nr.: %d\n", irq_number);

  return 0;
}

/**

 * @brief This function is called, when the module is removed from the kernel

 */

static void __exit ModuleExit(void) {
  printk("gpio_irq: Unloading module... ");
  proc_remove(entry); // Remove /proc entry
  printk("/proc/%s removed\n", PROC_NAME);
  free_irq(irq_number, NULL);
  gpio_free(17);
}

module_init(ModuleInit);
module_exit(ModuleExit);
