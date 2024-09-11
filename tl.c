#include <linux/slab.h>
#include <linux/string.h>
#include "tl.h"

static struct task *task_append(struct tasklist *tl, const char *name, size_t len) {
    struct task *t;

    t = kzalloc(sizeof(*t), GFP_KERNEL);
    if (!t) {
        pr_err("failed to create task with name '%s'\n", name);
        goto ret;
    }

    strncpy(t->name, name, TASK_NAME_LEN-1);
    t->len = len;

    if (!tl->head)
        tl->head = t;
    else
        tl->tail->next = t;

    t->prev = tl->tail;
    tl->tail = t;
    tl->tlen += len;
    tl->count++;

ret:
    return t;
}

static void task_delete(struct tasklist *tl, const char *name) {
    struct task *t = tl->head, *prev, *next;

    while (strcmp(t->name, name) != 0)
        t = t->next;

    if (t) {
        prev = t->prev;
        next = t->next;
        if (prev)
            prev->next = next;
        if (next)
            next->prev = prev;
        if (t == tl->head)
            tl->head = next;
        if (t == tl->tail)
            tl->tail = prev;
        tl->tlen -= t->len;
        tl->count--;
        kfree(t);
    }
}

void tasklist_print_ordered(struct tasklist *tl) {
    int i = 0;
    struct task *t = tl->head;
    while (t) {
        pr_info("[%d] %s\n", i, t->name);
        t = t->next;
        i++;
    }
}

int tasklist_destroy(struct tasklist *tl) {
    int i = 0;
    struct task *t, *next;
    t = tl->head;
    while (t) {
        next = t->next;
        kfree(t);
        t = next;
        i++;
    }
    return i;
}

int add_handler(struct tasklist *tl, const char *name) {
    return (!(task_append(tl, name, strlen(name)) != NULL));
}

int delete_handler(struct tasklist *tl, const char *name) {
    task_delete(tl, name);
    return 0;
}
