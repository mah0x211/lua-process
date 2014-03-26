# lua-process

process module.

## functions

### getppid() / getpid()

get parent or calling process id.

**Returns**

1.  number: process id.


### getgid() / getegid()

get real or effective group id of calling process.

**Returns**

1.  number: group id.


### getuid() / geteuid()

get real or effective user id of calling process.

**Returns**

1.  number: user id.


### getcwd()

get working directory pathname.

**Returns**

1.  pathname: working directory pathname on success, or nil on failure.

2.  errno: dependent on a system.


### getrusage()

get information about resource utilization.

**Returns**

1.  usage: table of struct rusage on success, or nil on failure.

2.  errno: dependent on a system.


### chdir( path )

change current working directory.

**Parameters**

-   path: valid directory path.

**Returns**

1.  boolean: true on success, or false on failure.

2.  errno: EINVAL, or dependent on a system.


### fork()

create child process.

**Returns**

1.  number: 0 to the child process, and a child process id to the
    calling process on success, or -1 on failure.

2.  errno: dependent on a system.


### sleep( sec )

suspend execution of the calling process until specified seconds.

**Parameters**

-   sec: unsigned integer.

**Returns**

1.  number: greater than 0 on success, or -1 on failure.

2.  errno: EINVAL, or dependent on a system.


