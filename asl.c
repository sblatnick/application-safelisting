/*  
 * Application Safelisting Kernel Module
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kallsyms.h>

static char *sym_name = "sys_call_table";

typedef asmlinkage long (*sys_call_ptr_t)(const struct pt_regs *);
static sys_call_ptr_t *sys_call_table;
typedef asmlinkage long (*custom_write) (const char __user *filename, int flags, umode_t mode);

custom_write old_write;

static asmlinkage long my_write(const char __user *filename, int flags, umode_t mode)
{
  printk(KERN_INFO "ASL: Attempted Write!");
  return old_write(filename, flags, mode);
}

static int __init init_asl(void)
{
  print(KERN_INFO "ASL: beginning overwite of sys_calL_table");

  // Retrieve system call table pointer
  sys_call_table = (sys_call_ptr_t *)kallsyms_lookup_name(sym_name);
  
  // Store old write syscall
  old_write = (custom_write)sys_call_table[__NR_write];

  // Temporarily disable write protection
  write_cr0(read_cr0() & (~0x10000));

  // Overwrite the syscall table entry
  sys_call_table[__NR_write] = (sys_call_ptr_t)my_write;

  // Re-enable write protection
  write_cr0(read_cr0() | 0x10000);

  print(KERN_INFO "ASL: end overwite of sys_calL_table");

  return 0;
}

static void __exit cleanup_asl(void)
{
  // Temporarily disable write protection
  write_cr0(read_cr0() & (~0x10000));

  // Overwrite the syscall table entry with old write
  sys_call_table[__NR_write] = (sys_call_ptr_t)old_write;

  // Re-enable write protection
  write_cr0(read_cr0() | 0x10000);
}

module_init(init_asl);
module_exit(cleanup_asl);
