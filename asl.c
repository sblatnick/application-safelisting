/*  
 * Application Safelisting Kernel Module
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init init_asl(void)
{
  printk(KERN_INFO "ASL: Hello world 1.\n");
  return 0;
}

static void __exit cleanup_asl(void)
{
  printk(KERN_INFO "ASL: Goodbye world 1.\n");
}

module_init(init_asl);
module_exit(cleanup_asl);