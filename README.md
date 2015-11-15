# lua-process

process module.

## Installation

```sh
luarocks install process --from=http://mah0x211.github.io/rocks/
```

## Constants

these constants defined at the `process.*`

**Use for `waitpid` API**

 - `WNOHANG`
 - `WUNTRACED`
 - `WCONTINUED`
 - `WNOWAIT`


## Environment

### Getting environment

- `env:table = getenv()`  
    get environment values.

**Returns**

1. `env`: env table.


## Process ID

### Getting process id

- `pid:number = getpid()`  
    get calling process id.
- `pid:number = getppid()`  
    get parent process id.

**Returns**

1. `pid`: process id.


## Group ID

### Getting group id

- `gid:number = getgid( [gname:string] )`  
    get real group id of a calling process or a specified group-name.
- `gid:number = getegid()`  
    get effective group id of calling process.

**Parameters**

- `gname`: group name.

**Returns**

1. `gid`: group id.


### Getting group name

- `gname:string = getgname( [gid:number] )`  
    get group name of a calling process or specified group id.

**Parameters**

- `gid`: group id.

**Returns**

- `gname`: group name.


### Setting group id

- `err:string = setgid( gid:number or gname:string )`  
    set real group id.
- `err:string = setegid( gid:number or gname:string )`  
    set effective group id.
- `err:string = setregid( rgid:number or gname:string, egid:number or gname:string )`  
    set real and effective group id.

**Parameters**

- `gid`: group id.
- `gname`: group name.
- `rgid`: real group id.
- `egid`: effective group id.

**Returns**

1. `err`: nil on succes, or error string on failure.


## User ID

### Getting user id

- `uid:number = getuid( [uname:string] )`
    get real user id of a calling process or a specified user-name.
- `uid:number = geteuid()`
    get effective user id of calling process.


**Parameters**

- `uname`: user name.

**Returns**

1. `uid`: user id.


### Getting user name

- `uname:string = getuname( [uid:number] )`  
    get user name of calling process or a specified user id.

**Parameters**

- `uid`: user id.

**Returns**

- `uname`: user name.


### Setting user id

- `err:string = setuid( uid:number or uname:string )`  
    set real user id.
- `err:string = seteuid( uid:number or uname:string )`  
    set effective user id.
- `err:string = setreuid( ruid:number or uname:string, euid:number or uname:string )`  
    set real and effective user id.

**Parameters**

- `uid`: user id.
- `uname`: user name.
- `ruid`: real user id.
- `euid`: effective user id.

**Returns**

1. `err`: nil on succes, or error string on failure.



## Session ID

### Getting session id

- `sid:number, err:string = getsid( pid:number )`  
    get session id of the specified process.

**Parameters**

- `pid`: process id.

**Returns**

1. `sid`: session id.
2. `err`: nil on succes, or error string on failure.


### Create new session id

- `sid:number, err:string = sesid()`  
    creates a new session.

**Returns**

1. `sid`: session id.
2. `err`: nil on succes, or error string on failure.


## Resource Utilization

### Getting resource info

- `usage:table, err:string = getrusage()`  
    get information about resource utilization.

**Returns**

1. `usage`: table of `struct rusage`.
2. `err`: nil on success, or error string on failure.


## Current Working Directory

### Getting current working directory

- `path:string, err:string = getcwd()`  
  get working directory pathname.

**Returns**

1. `path`: working directory pathname.
2. `err`:  nil on success, or error string on failure

### Seting current working directory

- `err:string = chdir( path:string )`  
    change current working directory.

**Parameters**

- `path`: valid directory path string.

**Returns**

1. `err`:  nil on success, or error string on failure.


## Child Process

### Create child process

- `pid:number, err:string =  fork()`  
    create child process.

**Returns**

1. `pid`: 0 to the child process, and a child process id to the
   calling process on success, or nil on failure.
2. `err`: nil on success, or error string on failure.


### Wait for process termination

- `status:table, err:string = waitpid( pid:number [, ...] )`  
    wait for process termination.  

please refer to `man 2 waitpid` for more details.

**Parameters**

- `pid`: process id.
- `...`: to use the following options;  
 - `WNOHANG`
 - `WUNTRACED`
 - `WCONTINUED`
 - `WNOWAIT`

**Returns**

1. `status`: wait status table.  
 - `nohang` = `true` if `waitpid` syscall returned 0.
 - `exit` = `exit_status:number` if `WIFEXITED` is true.
 - `termsig` = `signo:number` if `WIFSIGNALED` is true.
 - `stopsig` = `signo:number` if `WIFSIGNALED` is true.
 - `continue` = `true` if `WIFCONTINUED` is true
2. `err`: nil on success, or error string on failure.


### Execute specified file

- `child:process.child, err:string = exec( path:string [, args:table [, env:table [, cwd:string]]] )`  
    execute a file.

please refer to `man 2 execve` for more details.

**Parameters**

- `path`: filepath.
- `args`: argument array table.
- `env`: argument key-value pair table.

**Returns**

1. `child`: instantance of `process.child` module.
2. `err`: nil on success, or error string on failure.


## Suspend execution for an interval of time

- `rc:number = sleep( sec:number )`  
    suspend execution of the calling process until specified seconds.
- `rc:number = nsleep( nsec:number )`  
    suspend execution of the calling process until specified nanoseconds.


**Parameters**

- `sec`: unsigned integer number.
- `nsec`: unsigned integer.

**Returns**

1. `rc`: 0 on success, and >0 if an error occurs.


## Errors

### Getting current process/thread errno.

- `errno:number = errno()`  

**Returns**

1. `errno`: current process/thread errno.


### Getting message string corresponding to errno.

- `err:string = strerror( errno:number )`

**Parameters**

- `errno`: error number that defined in errno.h.  
  if passed argument is nil then to use global errno.

**Returns**

1. `err`: error string.


## Date and Time

- `sec:number, err:string = gettimeofday()`  
    get the time as well as a timezone.

**Returns**

1. `sec`: current time.
2. `err`: nil on success, or error string on failure.


## Descriptors

- `newfd:int, err:string = dup( oldfd:int )`
- `newfd:int, err:string = dup2( oldfd:int, newfd:int )`

duplicate an existing file descriptor.

**Returns**

1. `newfd`: new file descriptor on success, or nil on failure.
2. `err`: nil on success, or error string on failure.


## Instance of `process.child` module

`process.exec` API return this instance on success.

**Example**

```lua
local exec = require('process').exec;
local cmd = exec( 'echo', { 'hello world' } );
-- read from stdout
print( cmd:stdout() ); -- 'hello world\n'
```
### Getting process id

- `pid:number = child:pid()`  
    get process id.

**Returns**

1. `pid`: process id.


### Reading data from stdout/stderr

- `data:string, err:string, again:boolean = child:stdout()`  
    read the data from stdout of child process.
- `data:string, err:string, again:boolean = child:stderr()`  
    read the data from stderr of child process.

**Returns**

1. `data`: data as string.
2. `err`: nil on success, or error string on failure.
3. `again`: true if got a EAGAIN or EWOULDBLOCK.

### Write the data to stdin

- `len:number, err:string, again:boolean = child:stdin( data:string )`  
    write the data to stdin of child process.

**Parameters**

- `data`: string.

**Returns**

1. `len`: number of bytes written.
1. `err`: nil on success, or error string on failure.
2. `again`: true if got a EAGAIN or EWOULDBLOCK.


### Send signal to a child process

- `err:string = child:kill( [signo:number] )`  
    send signal to a child process.

**Parameters**

- `signo`: signal number. default `SIGTERM`.


**Returns**

1. `err`: nil on success, or error string on failure.

