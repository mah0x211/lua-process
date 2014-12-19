/*
 *  Copyright (C) 2014 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a 
 *  copy of this software and associated documentation files (the "Software"), 
 *  to deal in the Software without restriction, including without limitation 
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.
 *
 *  tmpl/process_tmpl.c
 *  lua-process
 *
 *  Created by Masatoshi Teruya on 14/03/27.
 */

#include "lprocess.h"


extern char **environ;

// MARK: API
static int getpid_lua( lua_State *L )
{
    lua_pushinteger( L, getpid() );
    return 1;
}

static int getppid_lua( lua_State *L )
{
    lua_pushinteger( L, getppid() );
    return 1;
}


static int getgid_lua( lua_State *L )
{
    lua_pushinteger( L, getgid() );
    return 1;
}

static int getegid_lua( lua_State *L )
{
    lua_pushinteger( L, getegid() );
    return 1;
}


static int getuid_lua( lua_State *L )
{
    lua_pushinteger( L, getuid() );
    return 1;
}

static int geteuid_lua( lua_State *L )
{
    lua_pushinteger( L, geteuid() );
    return 1;
}


static int getcwd_lua( lua_State *L )
{
    char *cwd = getcwd( NULL, 0 );
    
    if( cwd ){
        lua_pushstring( L, cwd );
        free( cwd );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int getrusage_lua( lua_State *L )
{
    struct rusage usage;
    
    if( getrusage( RUSAGE_SELF, &usage ) == 0 ){
        lua_newtable( L );
        lstate_num2tbl( L, "maxrss", usage.ru_maxrss );
        lstate_num2tbl( L, "ixrss", usage.ru_ixrss );
        lstate_num2tbl( L, "idrss", usage.ru_idrss );
        lstate_num2tbl( L, "isrss", usage.ru_isrss );
        lstate_num2tbl( L, "minflt", usage.ru_minflt );
        lstate_num2tbl( L, "majflt", usage.ru_majflt );
        lstate_num2tbl( L, "nswap", usage.ru_nswap );
        lstate_num2tbl( L, "inblock", usage.ru_inblock );
        lstate_num2tbl( L, "oublock", usage.ru_oublock );
        lstate_num2tbl( L, "msgsnd", usage.ru_msgsnd );
        lstate_num2tbl( L, "msgrcv", usage.ru_msgrcv );
        lstate_num2tbl( L, "nsignals", usage.ru_nsignals );
        lstate_num2tbl( L, "nvcsw", usage.ru_nvcsw );
        lstate_num2tbl( L, "nivcsw", usage.ru_nivcsw );
        lua_pushstring( L, "utime" );
        lua_newtable( L );
        lstate_num2tbl( L, "sec", usage.ru_utime.tv_sec );
        lstate_num2tbl( L, "usec", usage.ru_utime.tv_usec );
        lua_rawset( L, -3 );
        lua_pushstring( L, "stime" );
        lua_newtable( L );
        lstate_num2tbl( L, "sec", usage.ru_stime.tv_sec );
        lstate_num2tbl( L, "usec", usage.ru_stime.tv_usec );
        lua_rawset( L, -3 );
        
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}

static int chdir_lua( lua_State *L )
{
    size_t len = 0;
    const char *dir = luaL_checklstring( L, 1, &len );
    
    if( chdir( dir ) == 0 ){
        return 0;
    }
    
    // got error
    lua_pushstring( L, strerror( errno ) );
    
    return 1;
}


static int fork_lua( lua_State *L )
{
    pid_t pid = fork();
    
    if( pid != -1 ){
        lua_pushinteger( L, pid );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int waitpid_lua( lua_State *L )
{
    const int argc = lua_gettop( L );
    pid_t pid = luaL_checkinteger( L, 1 );
    pid_t rpid = 0;
    int rc = 0;
    int opts = 0;
    
    // check opts
    if( argc > 1 )
    {
        int i = 2;
        
        for(; i <= argc; i++ ){
            opts |= luaL_optint( L, i, 0 );
        }
    }
    
    rpid = waitpid( pid, &rc, opts );
    // WNOHANG
    if( rpid == 0 ){
        lua_createtable( L, 0, 2 );
        lstate_num2tbl( L, "pid", pid );
        lstate_bool2tbl( L, "nohang", 1 );
        return 1;
    }
    else if( rpid != -1 )
    {
        lua_createtable( L, 0, 2 );
        lstate_num2tbl( L, "pid", rpid );
        // exit status
        if( WIFEXITED( rc ) ){
            lstate_num2tbl( L, "exit", WEXITSTATUS( rc ) );
        }
        // exit signal number
        else if( WIFSIGNALED( rc ) ){
            lstate_num2tbl( L, "termsig", WTERMSIG( rc ) );
        }
        // stop signal 
        else if( WIFSTOPPED( rc ) ){
            lstate_num2tbl( L, "stopsig", WSTOPSIG( rc ) );
        }
        // continue signal
        else if( WIFCONTINUED( rc ) ){
            lstate_bool2tbl( L, "continue", 1 );
        }
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
}


static int exec_lua( lua_State *L )
{
    int argc = lua_gettop( L );
    const char *cmd = luaL_checkstring( L, 1 );
    pid_t pid = 0;
    int rc = 0;
    array_t argv;
    array_t envs;
    iopipe_t iop;
    
    // init arg containers
    if( arr_init( &argv, ARG_MAX ) == -1 ||
        arr_push( &argv, (char*)cmd ) == -1 || 
        arr_init( &envs, 0 ) == -1 || 
        iop_init( &iop ) == -1 ){
        arr_dispose( &argv );
        arr_dispose( &envs );
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    // manipute arg length
    else if( argc > 3 ){
        argc = 3;
    }
    
    // check args
    switch( argc )
    {
        // envs
        case 3:
            rc = kvp2arr( L, 3, &envs, "%s=%s" );
            // got error
            if( rc != 0 )
            {
                arr_dispose( &argv );
                arr_dispose( &envs );
                iop_dispose( &iop );
                if( rc == EINVAL ){
                    lua_pushnil( L );
                    lua_pushliteral( L, "env must be pair table" );
                }
                else {
                    lua_pushnil( L );
                    lua_pushstring( L, strerror( errno ) );
                }
                return 2;
            }
            // push last NULL item
            else if( arr_push( &envs, NULL ) == -1 ){
                arr_dispose( &argv );
                arr_dispose( &envs );
                iop_dispose( &iop );
                lua_pushnil( L );
                lua_pushstring( L, strerror( errno ) );
                return 2;
            }
        // argv
        case 2:
            rc = ivp2arr( L, 2, &argv );
            // got error
            if( rc != 0 )
            {
                arr_dispose( &argv );
                arr_dispose( &envs );
                iop_dispose( &iop );
                if( rc == E2BIG ){
                    lua_pushnil( L );
                    lua_pushfstring( L, "argv must be less than %d", ARG_MAX );
                }
                else if( rc == EINVAL ){
                    lua_pushnil( L );
                    lua_pushliteral( L, "argv must be ipair table" );
                }
                else {
                    lua_pushnil( L );
                    lua_pushstring( L, strerror( errno ) );
                }
                return 2;
            }
            // push last NULL item
            else if( arr_push( &argv, NULL ) == -1 ){
                arr_dispose( &argv );
                arr_dispose( &envs );
                iop_dispose( &iop );
                lua_pushnil( L );
                lua_pushstring( L, strerror( errno ) );
                return 2;
            }
        break;
    }
    
    pid = fork();
    // child
    if( pid == 0 )
    {
        // set std-in-out-err
        if( iop_set( &iop ) == 0 )
        {
            void *envp = NULL;
            
            // set new environ
            if( argc == 3 ){
                envp = (void*)environ;
                environ = envs.elts;
            }
            execvp( cmd, argv.elts );
            // got error
            // recover old environ
            if( envp ){
                environ = (char**)envp;
            }
        }
        arr_dispose( &argv );
        arr_dispose( &envs );
        fputs( strerror( errno ), stderr );
        _exit(0);
    }
    
    arr_dispose( &argv );
    arr_dispose( &envs );
    // got error
    if( pid == -1 ){
        lua_pushnil( L );
        lua_pushstring( L, strerror( errno ) );
        return 2;
    }
    
    // parent
    // close read-stdin, write-stdout
    iop_unset( &iop );
    if( newpchild( L, pid, iop.fds[IOP_IN_WRITE], iop.fds[IOP_OUT_READ], 
                   iop.fds[IOP_ERR_READ] ) != 0 )
    {
        int err = errno;
        
        // kill process safety
        if( kill( pid, SIGTERM ) == 0 )
        {
            sleep(1);
            rc = 0;
            if( waitpid( pid, &rc, WNOHANG ) == 0 ){
                kill( pid, SIGKILL );
            }
        }
        lua_pushnil( L );
        lua_pushstring( L, strerror( err ) );
        return 2;
    }
    
    return 1;
}


static int sleep_lua( lua_State *L )
{
    lua_Integer sec = luaL_checkinteger( L, 1 );
    
    lua_pushinteger( L, sleep( sec ) );
    return 1;
}


static int nsleep_lua( lua_State *L )
{
    lua_Integer nsec = luaL_checkinteger( L, 1 );
    struct timespec req = {
        .tv_sec = nsec / UINT64_C(1000000000),
        .tv_nsec = nsec % UINT64_C(1000000000)
    };
    
    lua_pushinteger( L, nanosleep( &req, NULL ) );
    
    return 1;
}


static int errno_lua( lua_State *L )
{
    lua_pushinteger( L, errno );
    return 1;
}


static int strerror_lua( lua_State *L )
{
    int err = errno;
    
    if( !lua_isnoneornil( L, 1 ) ){
        err = luaL_checkint( L, 1 );
    }
    
    lua_pushstring( L, strerror( err ) );
    
    return 1;
}


static int gettimeofday_lua( lua_State *L )
{
    struct timeval tv;
    
    if( gettimeofday( &tv, NULL ) == 0 ){
        lua_pushnumber( L, (lua_Number)tv.tv_sec +
                         (lua_Number)tv.tv_usec/1000000 );
        return 1;
    }

    // got error
    lua_pushinteger( L, -1 );
    lua_pushstring( L, strerror( errno ) );

    return 2;
}


LUALIB_API int luaopen_process( lua_State *L )
{
    struct luaL_Reg method[] = {
        { "getpid", getpid_lua },
        { "getppid", getppid_lua },
        { "getgid", getgid_lua },
        { "getegid", getegid_lua },
        { "getuid", getuid_lua },
        { "geteuid", geteuid_lua },
        { "getcwd", getcwd_lua },
        { "getrusage", getrusage_lua },
        { "chdir", chdir_lua },
        { "fork", fork_lua },
        { "waitpid", waitpid_lua },
        { "exec", exec_lua },
        { "sleep", sleep_lua },
        { "nsleep", nsleep_lua },
        { "errno", errno_lua },
        { "strerror", strerror_lua },
        { "gettimeofday", gettimeofday_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = method;
    
    // define fd metatable
    luaopen_process_child( L );
    // create module table
    lua_newtable( L );
    // add methods
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    
    // set waitpid options
#define GEN_WAITPID_OPT_DECL
    // set errno
#define GEN_ERRNO_DECL

    return 1;
}
