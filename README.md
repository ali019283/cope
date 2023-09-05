# cope
cope is a minimal package manager.

don't expect much out of it, it may lack some important features and it may have bugs.

# how to use
```
+-------------------------------------------------------------------+
| -> cope           [d|b|r|s] [pkg]...                              |
| -> d              install packages                                |
| -> b              d but force install dependency tree             |
| -> f              d but ignore install dependency tree            |
| -> r              remove packages                                 |
| -> s              r but ignore dependencies                       |
| -> l              list installed packages                         |
| -> u              update repo                                     |
+-------------------------------------------------------------------+
```

# installing / compiling
run 
```
# make
```
to compile

run 
```
# make install
```
to install

don't forget to run 
```
# cd /var/db
# git clone https://github.com/ali019283/rp
```

