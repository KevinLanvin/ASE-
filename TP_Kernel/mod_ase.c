#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <asm/uaccess.h>


static struct proc_dir_entry *proc_entry;
static struct proc_dir_entry *proc_dir;

static const struct file_operations ase_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = seq_open,
    .read       = seq_read,
    .write      = seq_write,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static int __init
ase_proc_init(void)
{
    proc_entry  = proc_create("ase_cmd", 0666, NULL, &ase_proc_fops);
    proc_dir = proc_mkdir("ase",NULL);
/*
create_proc_entry("ase_cmd",0666,proc_entry);
create_proc_entry("ase",0666,proc_dir);
*/
    return 0;
}

static void __exit
ase_proc_exit(void)
{
    remove_proc_entry("ase_cmd", NULL);
    remove_proc_entry("ase", NULL);
}

module_init(ase_proc_init);
module_exit(ase_proc_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ase+ dir, cmd files");
