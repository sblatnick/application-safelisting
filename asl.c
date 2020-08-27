/*  
 * Application Safelisting Kernel Module
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/slab.h>

#define DRIVER_AUTHORS "Steven Blatnick, Nate Ewan"
#define DRIVER_DESC    "Application Safelisting Kernel Module"
#define LOG_PREFIX     "ASL: "

static void l(char *str)
{
  char *output = kmalloc(sizeof(*str) + sizeof(LOG_PREFIX), GFP_KERNEL);
  strcpy(output, LOG_PREFIX);
  strcat(output, str);
  printk(KERN_INFO "%s\n", output);
  kfree(output);
}

static int __init init_asl(void)
{
  l("init");
  return 0;
}

static void __exit cleanup_asl(void)
{
  l("removed");
}

module_init(init_asl);
module_exit(cleanup_asl);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHORS);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("block");