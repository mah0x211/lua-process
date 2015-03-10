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
 *  src/lprocess.h
 *  lua-process
 *
 *  Created by Masatoshi Teruya on 14/12/19.
 *
 */

#ifndef ___LPROCESS___
#define ___LPROCESS___

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#if defined(__linux__)
#include <linux/limits.h>
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


// MARK: helper macros
#define lstate_num2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushnumber(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_bool2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushboolean(L,v); \
    lua_rawset(L,-3); \
}while(0)


#define lstate_fn2tbl(L,k,v) do{ \
    lua_pushstring(L,k); \
    lua_pushcfunction(L,v); \
    lua_rawset(L,-3); \
}while(0)


// MARK: fd metatable
#define PROCESS_CHILD_MT    "process.child"

typedef struct {
    pid_t pid;
    // 0: stdin, 1: stdout, 2: stderr
    int fds[3];
} pchild_t;


LUALIB_API int luaopen_process_child( lua_State *L );


// allocate procfd as userdata
static inline int newpchild( lua_State *L, pid_t pid, int ifd, int ofd, 
                             int efd )
{
    pchild_t *chd = lua_newuserdata( L, sizeof( pchild_t ) );
    
    if( !chd ){
        return -1;
    }
    *chd = (pchild_t){
        .pid = pid,
        .fds = { ifd, ofd, efd }
    };
    luaL_getmetatable( L, PROCESS_CHILD_MT );
    lua_setmetatable( L, -2 );
    
    return 0;
}


// MARK: array
typedef struct {
    char **elts;
    size_t len;
    size_t max;
} array_t;

static inline int arr_init( array_t *arr, size_t max )
{
    arr->elts = realloc( NULL, 0 );
    if( arr->elts ){
        return 0;
    }
    arr->len = 0;
    arr->max = max;
    
    return -1;
}


static inline void arr_dispose( array_t *arr )
{
    if( arr->elts ){
        free( (void*)arr->elts );
    }
}


static inline int arr_push( array_t *arr, char *data )
{
    size_t len = arr->len + 1;
    void *ptr = NULL;
    
    if( arr->max && len > arr->max ){
        errno = E2BIG;
        return -1;
    }
    else if( ( ptr = realloc( (void*)arr->elts, len * sizeof( char* ) ) ) ){
        arr->elts = (char**)ptr;
        arr->elts[arr->len++] = data;
        return 0;
    }
    
    return -1;
}


// key-value pair
static inline int kvp2arr( lua_State *L, int idx, array_t *arr, const char *fmt )
{
    char *ptr = NULL;
    
    luaL_checktype( L, idx, LUA_TTABLE );
    lua_pushnil( L );
    while( lua_next( L, idx ) != 0 )
    {
        // check key type
        if( lua_type( L, -2 ) != LUA_TSTRING ){
            lua_pop( L, 2 );
            return EINVAL;
        }
        // check val type
        switch( lua_type( L, -1 ) ){
            case LUA_TSTRING:
            case LUA_TNUMBER:
            case LUA_TBOOLEAN:
            break;
            default:
                lua_pop( L, 2 );
                return EINVAL;
        }
        
        // set item
        // format key-value pair
        ptr = (char*)lua_pushfstring( 
            L, fmt, lua_tostring( L, -2 ), lua_tostring( L, -1 )
        );
        if( !ptr || arr_push( arr, ptr ) == -1 ){
            lua_pop( L, 2 );
            return errno;
        }
        lua_pop( L, 2 );
    }
    lua_pop( L, 1 );
    
    return 0;
}


// index-value pair
static inline int ivp2arr( lua_State *L, int idx, array_t *arr )
{
    luaL_checktype( L, idx, LUA_TTABLE );
    lua_pushnil( L );
    while( lua_next( L, idx ) != 0 )
    {
        // check key type
        if( lua_type( L, -2 ) != LUA_TNUMBER ){
            lua_pop( L, 2 );
            return EINVAL;
        }
        // check val type
        switch( lua_type( L, -1 ) ){
            case LUA_TSTRING:
            case LUA_TNUMBER:
            case LUA_TBOOLEAN:
            break;
            default:
                lua_pop( L, 2 );
                return EINVAL;
        }
        // set item
        if( arr_push( arr, (char*)lua_tostring( L, -1 ) ) == -1 ){
            lua_pop( L, 2 );
            return errno;
        }
        lua_pop( L, 1 );
    }
    lua_pop( L, 1 );
    
    return 0;
}


// MARK: pipe
typedef enum {
    IOP_IN_READ = 0,
    IOP_IN_WRITE,
    IOP_OUT_READ,
    IOP_OUT_WRITE,
    IOP_ERR_READ,
    IOP_ERR_WRITE
} iop_type_e;


typedef struct {
    int fds[6];
} iopipe_t;


static inline void iop_dispose( iopipe_t *io )
{
    int i = 0;
    
    for(; i < 6; i++ )
    {
        if( io->fds[i] ){
            close( io->fds[i] );
            io->fds[i] = 0;
        }
    }
}


static inline int iop_init( iopipe_t *iop )
{
    memset( (void*)iop, 0, sizeof( iop ) );
    // create pipes
    if( pipe( iop->fds ) == 0 )
    {
        if( pipe( iop->fds + 2 ) == 0 )
        {
            if( pipe( iop->fds + 4 ) == 0 )
            {
                int i = 0;
                
                for(; i < 6; i++ ){
                    fcntl( iop->fds[i], F_SETFD, FD_CLOEXEC );
                }
                
                return 0;
            }
        }
    }
    iop_dispose( iop );
    
    return -1;
}


static inline int iop_set( iopipe_t *iop )
{
    // close stdin-write, stdout-read, stderr-read
    close( iop->fds[IOP_IN_WRITE] );
    close( iop->fds[IOP_OUT_READ] );
    close( iop->fds[IOP_ERR_READ] );
    // set stdin-read, stdout-write, stderr-write
    return -( dup2( iop->fds[IOP_IN_READ], STDIN_FILENO ) == -1 || 
              dup2( iop->fds[IOP_OUT_WRITE], STDOUT_FILENO ) == -1 ||
              dup2( iop->fds[IOP_ERR_WRITE], STDERR_FILENO ) == -1 );
}


static inline void iop_unset( iopipe_t *iop )
{
    // close stdin-read, stdout-write, stderr-write
    close( iop->fds[IOP_IN_READ] );
    close( iop->fds[IOP_OUT_WRITE] );
    close( iop->fds[IOP_ERR_WRITE] );
}


#endif
