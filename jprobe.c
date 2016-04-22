#include <linux/module.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>

int my_callback(struct task_struct *p,const struct sched_attr *attr,bool user, bool pi){
printk(KERN_ALERT "__sched_setscheduler\n");
	jprobe_return();
return 0;
}

static struct jprobe my_jprobe = {
    .kp = {
        .symbol_name = "__sched_setscheduler",
    },
    .entry = (kprobe_opcode_t *) my_callback
};

static int __init
jprobe_init(void)
{
register_jprobe(&my_jprobe);
printk(KERN_ALERT "plant jprobe at %p, handler addr %p\n",my_jprobe.kp.addr, my_jprobe.entry); 

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
