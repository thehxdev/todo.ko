# todo.ko
Todo app as Linux kernel module!

> [!WARNING]
>  Since this is my first Linux kernel module, don't load the kernel module in your main operating system. Compile and load the module inside a Linux VM.

## Build
Build the module with GNU make:
```bash
make
```

## Use
Basic operations and instructions:

### Load the module
```bash
sudo insmod todo.ko
```

### Get all todos
```bash
cat /proc/todo
```

### Add a new task
```bash
echo -n 'A: Read a new book' > /proc/todo
```

### Delete a task
```bash
echo -n 'D: Read a new book' > /proc/todo
```

### remove and unload the module
```bash
sudo rmmod todo
```

## Command syntax
When you write to `/proc/todo`, the module parses your input. The first character MUST be an uppercase english ascii character.
At this point only `A` for add and `D` for delete are supported. The general syntax:
```txt
command := CMD_CHAR: TASK
```
A command contains a single uppercase character which specifies the operation, a colon right after command character and the rest is the task itself.
