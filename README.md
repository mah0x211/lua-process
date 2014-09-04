# lua-process

process module.

## Installation

```sh
luarocks install --from=http://mah0x211.github.io/rocks/ process
```

or 

```sh
git clone https://github.com/mah0x211/lua-process.git
cd lua-process
luarocks make
```

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

1.  errno: dependent on a system.


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

1.  number: 0 on success, and >0 if an error occurs.


### nsleep( nsec )

suspend execution of the calling process until specified nanoseconds.

**Parameters**

-   nsec: unsigned integer.

**Returns**

1.  number: 0 on success, or -1 on failure.


### errno()

returns a errno.

**Returns**

1.  errno: current process/thread errno.


### strerror( errno )

returns a message string corresponding to errno.

**Parameters**

-   errno: error number that defined in errno.h. 
    if passed argument is nil then to use global errno.

**Returns**

1.  string: error message string.


### gettimeofday()

get the time as well as a timezone

**Returns**

1   sec: >0 on success, or -1 on failure.
