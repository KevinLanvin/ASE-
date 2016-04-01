#include <linux/module.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>
#include <asm/uaccess.h>


int my_callback(const char *name, umode_t mode, struct proc_dir_entry *parent,const struct file_operations *proc_fops) {
printk(KERN_ALERT "proc_create\n");
pr_info("proc_create\n");

	jprobe_return();
return 0;
}

static struct jprobe my_jprobe = {
    .kp = {
        .symbol_name = "proc_create",
    },
    .entry = (kprobe_opcode_t *) my_callback

};

static int __init
jprobe_init(void)
{
int ret;
ret= register_jprobe(&my_jprobe);
	if (ret < 0) {
		printk(KERN_INFO "register_jprobe failed, returned %d\n", ret);
		return -1;
	}
	printk(KERN_INFO "Planted jprobe at %p, handler addr %p\n",
	       my_jprobe.kp.addr, my_jprobe.entry);
	return 0;
}

static void __exit
jprobe_exit(void)
{
	unregister_jprobe(&my_jprobe);
	printk(KERN_INFO "jprobe at %p unregistered\n", my_jprobe.kp.addr);
}


module_init(jprobe_init);
module_exit(jprobe_exit);

MODULE_LICENSE("GPL");
