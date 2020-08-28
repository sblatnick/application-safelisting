/*  
 * Application Safelisting Kernel Module
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>
#include <linux/unistd.h>

#define DRIVER_AUTHORS "Steven Blatnick, Nate Ewan"
#define DRIVER_DESC    "Application Safelisting Kernel Module"
#define LOG_PREFIX     "ASL: "

typedef void (*sys_call_ptr_t)(void);
static sys_call_ptr_t *sys_call_table;

/* UTILS */
static void l(const char *str)
{
  char *output = kmalloc(sizeof(*str) + sizeof(LOG_PREFIX), GFP_KERNEL);
  strcpy(output, LOG_PREFIX);
  strcat(output, str);
  printk(KERN_INFO "%s\n", output);
  kfree(output);
}

/* OVERRIDES */
asmlinkage int (*old_execve)(const char *pathname, char *const argv[], char *const envp[]);
asmlinkage int asl_execve(const char *pathname, char *const argv[], char *const envp[]) {
  l(pathname);
  return old_execve(pathname, argv, envp);
}

static int __init init_asl(void)
{
  l("init");
  sys_call_table = (sys_call_ptr_t *)kallsyms_lookup_name("sys_call_table");
  old_execve = sys_call_table[__NR_execve];

  write_cr0(read_cr0() & (~0x10000));

  sys_call_table[__NR_execve] = (sys_call_ptr_t)asl_execve;

  write_cr0(read_cr0() | 0x10000);

  return 0;
}

static void __exit cleanup_asl(void)
{
  l("removed");

  write_cr0(read_cr0() & (~0x10000));

  sys_call_table[__NR_execve] = (sys_call_ptr_t)old_execve;

  write_cr0(read_cr0() | 0x10000);
}

module_init(init_asl);
module_exit(cleanup_asl);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHORS);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("block");
