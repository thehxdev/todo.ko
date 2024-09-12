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
#define WRITE_BUFF_LEN  (TASK_NAME_LEN+3)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
    #define WITH_PROC_OPS
#endif /* WITH_PROC_OPS */

static ssize_t todo_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset);
static ssize_t todo_write(struct file *filp, const char __user *buffer, size_t length, loff_t *offset);

static struct tasklist tl = { .head = NULL, .tail = NULL, .count = 0, .tlen = 0 };
static struct proc_dir_entry *todo_proc_file;

#ifdef WITH_PROC_OPS
static const struct proc_ops todo_proc_fops = {
    .proc_read = todo_read,
    .proc_write = todo_write,
};
#else
static const struct file_operations todo_proc_fops = {
    .read = todo_read,
    .write = todo_write,
};
#endif /* WITH_PROC_OPS */

static ssize_t todo_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset) {
    struct task *t = tl.head;
    size_t lbuff_len = tl.tlen+tl.count+1, off = 0;
    ssize_t ret = lbuff_len;

    if (*offset >= lbuff_len || tl.count == 0)
        return 0;

    char *lbuff = kzalloc(lbuff_len, GFP_KERNEL);
    if (!lbuff) {
        pr_err("[ERROR] failed to allocate buffer to write tasks\n");
        return 0;
    }

    while (t) {
        sprintf(lbuff+off, "%s\n", t->name);
        off += t->len + 1;
        t = t->next;
    }

    if (copy_to_user(buffer, lbuff, lbuff_len)) {
        ret = -EFAULT;
        goto defer_ret;
    }
    *offset += lbuff_len;

defer_ret:
    kfree(lbuff);
    return ret;
}

static ssize_t todo_write(struct file *filp, const char __user *buffer, size_t length, loff_t *offset) {
    // A: Write a linux driver
    // D: Write a linux driver
    if (length < 4) {
        pr_err("[ERROR] input is too short\n");
        goto ret;
    }
    size_t klen = length;

    if (klen > WRITE_BUFF_LEN)
        klen = WRITE_BUFF_LEN;

    char kbuffer[WRITE_BUFF_LEN] = { 0 };
    if (copy_from_user(kbuffer, buffer, klen)) {
        pr_err("[ERROR] copy_from_user failed\n");
        return -EFAULT;
    }

    char cmd = kbuffer[0];
    char *task_name = &kbuffer[3];
    switch (cmd) {
        case 'A':
            add_handler(&tl, task_name);
            break;
        case 'D':
            delete_handler(&tl, task_name);
            break;
        default: {
            pr_err("[ERROR] unknown command %c\n", cmd);
            return -EFAULT;
        }
    }

ret:
    return length;
}

static int __init todo_init(void) {
    todo_proc_file = proc_create(PROCFS_NAME, 0666, NULL, &todo_proc_fops);
    if (!todo_proc_file) {
        pr_err("[ERROR] could not initialize /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }

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
