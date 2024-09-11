# todo.ko
Todo app as Linux kernel module! (It's not fully functional yet)

> [!WARNING]
>  Because this is my first Linux kernel module, don't load the kernel module in your main operating system. Compile and load the module inside a Linux VM.

## Build
Build the module with GNU make:
```bash
make
```

## Use
At this point the module is not very interactive. You can get added todos but add/remove functionalities are not implemented yet.

### Load the module
```bash
sudo insmod todo.ko
```

### get all todos
```bash
cat /proc/todo
```

### remove and unload the module
```bash
sudo rmmod todo
```
