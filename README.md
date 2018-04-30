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

### env = getenv()

get environment variables.

**Returns**

- `env:table`: environment variables.


## Process ID

### pid = getpid()

get calling process id.

**Returns**

- `pid:number`: process id.


### pid = getppid()

get parent process id.

**Returns**

- `pid:number`: parent process id.


## Group ID

### gid = getgid( [gname] )

get real group id of a calling process or a specified group-name.

**Parameters**

- `gname:string`: group name.

**Returns**

- `gid:number`: group id.


### gid = getegid()

get effective group id of calling process.


**Returns**

- `gid:number`: group id.


### gname = getgname( [gid] )

get group name of a calling process or specified group id.

**Parameters**

- `gid:number`: group id.

**Returns**

- `gname:string`: group name.


### err = setgid( gid or gname )

set real group id.

**Parameters**

- `gid:number`: group id.
- `gname:string`: group name.

**Returns**

- `err:string`: nil on succes, or error string on failure.


### err = setegid( gid or gname )

set effective group id.

**Parameters**

- `gid:number`: group id.
- `gname:string`: group name.

**Returns**

- `err:string`: nil on succes, or error string on failure.


### err = setregid( rgid or gname, egid or gname )

set real and effective group id.

**Parameters**

- `rgid:number`: real group id.
- `egid:number`: effective group id.
- `gname:string`: group name.

**Returns**

- `err:string`: nil on succes, or error string on failure.


## User ID

### uid = getuid( [uname] )

get real user id of a calling process or a specified user-name.

**Parameters**

- `uname:string`: user name.

**Returns**

- `uid:number`: user id.


### uid = geteuid()

get effective user id of calling process.

**Returns**

- `uid:number`: user id.


### uname = getuname( [uid] )

get user name of calling process or a specified user id.

**Parameters**

- `uid:number`: user id.

**Returns**

- `uname:string`: user name.


### err = setuid( uid or uname )

set real user id.

**Parameters**

- `uid:number`: user id.
- `uname:string`: user name.

**Returns**

- `err:string`: nil on succes, or error string on failure.


### err = seteuid( uid or uname )

set effective user id.

**Parameters**

- `uid:number`: user id.
- `uname:string`: user name.

**Returns**

- `err:string`: nil on succes, or error string on failure.


### err = setreuid( ruid or uname, euid or uname )

set real and effective user id.

**Parameters**

- `ruid:number`: real user id.
- `euid:number`: effective user id.
- `uname:string`: user name.

**Returns**

- `err:string`: nil on succes, or error string on failure.


## Session ID

### sid, err = getsid( pid )

get session id of the specified process.

**Parameters**

- `pid:number`: process id.

**Returns**

- `sid:number`: session id.
- `err:string`: nil on succes, or error string on failure.


### sid, err = setsid()

creates a new session.

**Returns**

- `sid:number`: session id.
- `err:string`: nil on succes, or error string on failure.


## Resource Utilization

### usage, err = getrusage()

get information about resource utilization.

**Returns**

- `usage:table`: table of `struct rusage`.
- `err:string`: nil on success, or error string on failure.


## Current Working Directory

### path, err = getcwd()

get working directory pathname.

**Returns**

- `path:string`: working directory pathname.
- `err:string`:  nil on success, or error string on failure


### err = chdir( path )

change current working directory.

**Parameters**

- `path:string`: valid directory path string.

**Returns**

- `err:string`:  nil on success, or error string on failure.


## Child Process

### pid, err, again =  fork()

create child process.

**Returns**

- `pid:number`: 0 to the child process, and a child process id to the calling process on success, or nil on failure.
- `err:string`: nil on success, or error string on failure.
- `again:boolean`: true if got EAGAIN.


### status, err = waitpid( pid [, ...] )

wait for process termination.  
please refer to `man 2 waitpid` for more details.

**Parameters**

- `pid:number`: process id (default: `-1`).
- `...`: to use the following options;  
    - `WNOHANG`
    - `WUNTRACED`
    - `WCONTINUED`
    - `WNOWAIT`

**Returns**

- `status:table`: status table if succeeded.
    - `pid` = `pid:number`.
    - `exit` = `exit_status:number` if `WIFEXITED` is true.
    - `termsig` = `signo:number` if `WIFSIGNALED` is true.
    - `stopsig` = `signo:number` if `WIFSIGNALED` is true.
    - `continue` = `true` if `WIFCONTINUED` is true
    - `nochild` = `true` if `errno` is `ECHILD`.
- `err:string`: nil on success, or error string on failure.


### child, err = exec( path [, args [, env [, cwd [, nonblock]]]] )

execute a specified file.

please refer to `man 2 execve` for more details.

**Parameters**

- `path:string`: filepath.
- `args:table`: argument array table.
- `env:table`: argument key-value pair table.
- `cwd:string`: custom working directory.
- `nonblock:boolean`: if set to true, `child:stdin`, `child:stdout` and `child:stderr` are in non-blocking mode.
 
**Returns**

- `child:process.child`: instantance of [`process.child`](#instance-of-processchild-module) module.
- `err:string`: nil on success, or error string on failure.


## Suspend execution for an interval of time

### rc = sleep( sec )

suspend execution of the calling process until specified seconds.

**Parameters**

- `sec:number`: unsigned integer number.

**Returns**

- `rc:number`: 0 on success, and >0 if an error occurs.


### rc = nsleep( nsec )

suspend execution of the calling process until specified nanoseconds.

**Parameters**

- `nsec:number`: unsigned integer.

**Returns**

- `rc:number`: 0 on success, and >0 if an error occurs.


## Errors

### errno = errno()

getting current process/thread errno.


**Returns**

- `errno:number`: current process/thread errno.


### err = strerror( [errno:number] )

getting message string corresponding to errno.

**Parameters**

- `errno:number`: error number that defined in `errno.h`.  
  if passed argument is nil then to use global errno.

**Returns**

- `err:string`: error string.


## Date and Time

### sec, err = gettimeofday()

get the time as well as a timezone.

**Returns**

- `sec:number`: current time.
- `err:string`: nil on success, or error string on failure.


## Descriptors

### newfd, err = dup( oldfd )

create a copy of the file descriptor oldfd.  
please refer to `man 2 dup` for more details.

**Parameters**

- `oldfd:number`: file descriptor.

**Returns**

- `newfd:number`: new file descriptor.
- `err:string`: nil on success, or error string on failure.


### newfd, err = dup2( oldfd, newfd )

create a copy of the file descriptor oldfd.  
please refer to `man 2 dup2` for more details.

**Parameters**

- `oldfd:number`: file descriptor.
- `newfd:number`: file descriptor.

**Returns**

- `newfd:number`: new file descriptor.
- `err:string`: nil on success, or error string on failure.


### ok, err = close( fd )

close a existing file descriptor.

**Parameters**

- `fd:number`: file descriptor.

**Returns**

- `ok:boolean`: true on success, or false on failure.
- `err:string`: error string on failure.



## Instance of `process.child` module

`process.exec` API return this instance on success.

**Example**

```lua
local exec = require('process').exec;
local cmd = exec( 'echo', { 'hello world' } );
-- read from stdout
print( cmd:stdout() ); -- 'hello world\n'
```


### pid = child:pid()

get process id.

**Returns**

- `pid:number`: process id.


### fdin, fdout, fderr = child:fds()

get file descriptors of stdin, stdout and stderr.


**Returns**

- `fdin:number`: stdin file descriptor.
- `fdout:number`: stdout file descriptor.
- `fderr:number`: stderr file descriptor.


### data, err, again = child:stdout()

read the data from stdout of child process.

**Returns**

- `data:string`: data as string.
- `err:string`: nil on success, or error string on failure.
- `again:boolean`: true if got a `EAGAIN` or `EWOULDBLOCK`.


### data, err, again = child:stderr()

read the data from stderr of child process.

**Returns**

- `data:string`: data as string.
- `err:string`: nil on success, or error string on failure.
- `again:boolean`: true if got a `EAGAIN` or `EWOULDBLOCK`.


### len, err, again = child:stdin( data )

write the data to stdin of child process.

**Parameters**

- `data:string`: data string.

**Returns**

- `len:number`: number of bytes written.
- `err:string`: nil on success, or error string on failure.
- `again:boolean`: true if got a `EAGAIN` or `EWOULDBLOCK`.


### err = child:kill( [signo] )

send signal to a child process.

**Parameters**

- `signo:number`: signal number. default `SIGTERM`.

**Returns**

- `err:string`: nil on success, or error string on failure.

