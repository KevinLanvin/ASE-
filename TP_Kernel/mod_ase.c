#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <asm/cputime.h>

static struct proc_dir_entry *proc_entry;
static struct proc_dir_entry *proc_dir;

#define BUFFER_LEN 6
static char buffer[BUFFER_LEN];
static int  flag = 0;


static int 
show_exec_time(struct seq_file *m, void* v){


return 0;
}



static int 
ase_proc_show(struct seq_file *m, void *v)
{
    if (flag){
    printk(KERN_INFO "ASE_CMD: coucou");
	}
    return 0;
}


static int 
file_proc_open(struct inode *inode, struct file *file)
{
int data = PDE_DATA(inode);
if(data==0){
return 0;
}

rcu_read_lock();
struct pid *pid = find_vpid(data);
struct task_struct *task = pid_task(pid,PIDTYPE_PID);
printk(KERN_INFO "%d : %ld\n",data,cputime_to_usecs(task->stime));
rcu_read_unlock();
//seq_printf(&file, "%d : %ld\n",data,cputime_to_usecs(task->utime));
return 1;

    //return single_open(file, show_exec_time, (void*)data );
}


static int 
ase_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, ase_proc_show, NULL );
}

static const struct file_operations fichierrandom = {
    .owner      = THIS_MODULE,
    .open       = file_proc_open,
    .read       = seq_read,
    .write      = seq_write,
    .llseek     = seq_lseek,
    .release    = single_release,
};


static int write_ase_cmd(struct file *filp, const char __user *buff,
                   size_t len, loff_t *data)
{
long res;
    if (len > (BUFFER_LEN - 1)) {
        printk(KERN_INFO "ASE_CMD: error, input too long");
        return -EINVAL;
    }
    else if (copy_from_user(buffer, buff, len)) {
        printk(KERN_INFO "ASE_CMD: copy from user");
        return -2;
    }
    buffer[len-1] = '\0';
    kstrtol(buffer, 0, &res);
    flag = res;
proc_create_data(buffer, 0644, proc_dir, &fichierrandom,(void *)flag);
    printk(KERN_INFO "ASE_CMD: Creation fichier -> Ok\n");
return len;
}




static const struct file_operations ase_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = ase_proc_open,
    .read       = seq_read,
    .write      = write_ase_cmd,
    .llseek     = seq_lseek,
    .release    = single_release,
};




static int __init
ase_proc_init(void)
{
    proc_entry  = proc_create("ase_cmd", 0666, NULL, &ase_proc_fops);
    proc_dir = proc_mkdir("ase",NULL);
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
