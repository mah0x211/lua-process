/*
 *  Copyright (C) 2014 Masatoshi Teruya
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <lauxlib.h>
#include <lualib.h>


#define lstate_num2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushnumber(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_fn2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushcfunction(L,v); \
    lua_rawset(L,-3); \
}while(0)


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
    
    lua_pushinteger( L, pid );
    if( pid != -1 ){
        return 1;
    }
    
    // got error
    lua_pushstring( L, strerror( errno ) );
    
    return 2;
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
        { "sleep", sleep_lua },
        { "nsleep", nsleep_lua },
        { "errno", errno_lua },
        { "strerror", strerror_lua },
        { "gettimeofday", gettimeofday_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = method;

    
    lua_newtable( L );
    // add methods
    do {
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    } while( ptr->name );
    
    return 1;
}
