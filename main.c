/**
 * Simple todo app as Linux kernel module
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "tl.h"

#define RETURN_OK   (0)
#define PROCFS_NAME "todo"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
    #define WITH_PROC_OPS
#endif /* WITH_PROC_OPS */

static ssize_t todo_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset);

typedef int(*cmd_handler)(struct tasklist *, const char *);
static const cmd_handler handlers[CMDS_COUNT] = {
    ['A' % CMDS_COUNT] = add_handler,
    ['D' % CMDS_COUNT] = delete_handler,
};
static struct tasklist tl = { .head = NULL, .tail = NULL, .count = 0, .tlen = 0 };

static struct proc_dir_entry *todo_proc_file;

#ifdef WITH_PROC_OPS
static const struct proc_ops todo_proc_fops = {
    .proc_read = todo_read,
};
#else
static const struct file_operations todo_proc_fops = {
    .read = todo_read,
};
#endif /* WITH_PROC_OPS */

static ssize_t todo_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
    struct task *t = tl.head;
    size_t lbuff_len = tl.tlen+tl.count+1, off = 0;

    if (*offset >= tl.tlen || tl.count == 0)
        return 0;

    char *lbuff = kzalloc(lbuff_len, GFP_KERNEL);
    if (!lbuff) {
        pr_err("[ERROR] failed to allocate buffer to write tasks\n");
        return 0;
    }

    while (t) {
        sprintf(lbuff+off, "%s\n", t->name);
        off += t->len + 2;
        t = t->next;
    }

    if (copy_to_user(buffer, lbuff, lbuff_len))
        return -EFAULT;

    kfree(lbuff);
    *offset += lbuff_len;
    return lbuff_len;
}

static int __init todo_init(void) {
    todo_proc_file = proc_create(PROCFS_NAME, 0666, NULL, &todo_proc_fops);
    if (!todo_proc_file) {
        proc_remove(todo_proc_file);
        pr_err("[ERROR] could not initialize /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }

    add_handler(&tl, "Write a linux driver");
    add_handler(&tl, "Write a blog post");

    pr_info("[INFO] todo app initialized\n");
    return RETURN_OK;
}

static void __exit todo_exit(void) {
    proc_remove(todo_proc_file);
    int removed = tasklist_destroy(&tl);
    pr_info("destroyed task list and removed %d tasks\n", removed);
    pr_info("todo app uninitialized\n");
}

module_init(todo_init);
module_exit(todo_exit);

MODULE_AUTHOR("Hossein Khosravi");
MODULE_DESCRIPTION("Simple todo app as Linux kernel module");
MODULE_LICENSE("GPL");
