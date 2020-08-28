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
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/export.h>

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
typedef asmlinkage int (*ptr_execve)(const char *pathname, const char __user *const __user *argv, const char __user *const __user *envp);
ptr_execve old_execve;
asmlinkage int asl_execve(const char *pathname, const char __user *const __user *argv, const char __user *const __user *envp) {
  l(pathname);
  //return old_execve(pathname, argv, envp);
  return do_execve(getname(pathname), argv, envp);
}

static int __init init_asl(void)
{
  l("init");
  sys_call_table = (sys_call_ptr_t *)kallsyms_lookup_name("sys_call_table");
  l("1 store old");
  old_execve = (ptr_execve)sys_call_table[__NR_execve];
  l("2 allow write");
  write_cr0(read_cr0() & (~0x10000));
  l("3 store new");
  sys_call_table[__NR_execve] = (void *)asl_execve;
  l("4 disallow write");
  write_cr0(read_cr0() | 0x10000);
  l("5 return");
  return 0;
}

static void __exit cleanup_asl(void)
{
  l("removed");

  write_cr0(read_cr0() & (~0x10000));

  sys_call_table[__NR_execve] = (void *)old_execve;

  write_cr0(read_cr0() | 0x10000);
}

module_init(init_asl);
module_exit(cleanup_asl);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHORS);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("block");
