/**
 * Task and Task list definitions
 */

#ifndef TODO_TL_H
#define TODO_TL_H

#define CMDS_COUNT      (8)
#define TASK_NAME_LEN   (32)

struct task {
    char name[TASK_NAME_LEN];
    size_t len;
    struct task *next;
    struct task *prev;
};

struct tasklist {
    struct task *head;
    struct task *tail;
    size_t count;
    size_t tlen;
};


int add_handler(struct tasklist *tl, const char *name);
int delete_handler(struct tasklist *tl, const char *name);
int tasklist_destroy(struct tasklist *tl);
void tasklist_print_ordered(struct tasklist *tl);

#endif /* TODO_TL_H */
