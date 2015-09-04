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

#include "lprocess.h"


static int stdin_lua( lua_State *L )
{
    pchild_t *chd = luaL_checkudata( L, 1, PROCESS_CHILD_MT );
    size_t len = 0;
    const char *str = luaL_checklstring( L, 2, &len );
    size_t remain = len;
    char *ptr = (char*)str;
    ssize_t bytes = 0;
    
    do
    {
        // got error
        if( ( bytes = write( chd->fds[0], ptr, remain ) ) == -1 )
        {
            lua_pushinteger( L, remain );
            lua_pushstring( L, strerror( errno ) );
            // check non-blocking mode
            if( errno == EAGAIN || errno == EWOULDBLOCK ){
                lua_pushboolean( L, 1 );
                return 3;
            }
            return 2;
        }
        // end-of-file or succeeded
        else if( ( bytes == 0 && remain == 0 ) || ( remain -= bytes ) == 0 ){
            break;
        }
        ptr += bytes;
    
    } while(1);
    
    lua_pushinteger( L, len );
    return 1;
}


static inline int read_lua( lua_State *L, int type )
{
    pchild_t *chd = luaL_checkudata( L, 1, PROCESS_CHILD_MT );
    char buf[LUAL_BUFFERSIZE] = {0};
    ssize_t bytes = read( chd->fds[type], &buf, LUAL_BUFFERSIZE );
    
    if( bytes > 0 ){
        lua_pushlstring( L, buf, bytes );
        return 1;
    }
    // end-of-file
    else if( bytes == 0 ){
        lua_pushnil( L );
        return 1;
    }
    
    // got error
    lua_pushnil( L );
    lua_pushstring( L, strerror( errno ) );
    // check non-blocking mode
    if( errno == EAGAIN || errno == EWOULDBLOCK ){
        lua_pushboolean( L, 1 );
        return 3;
    }
    
    return 2;
}

static int stderr_lua( lua_State *L )
{
    return read_lua( L, 2 );
}

static int stdout_lua( lua_State *L )
{
    return read_lua( L, 1 );
}


static int kill_lua( lua_State *L )
{
    pchild_t *chd = luaL_checkudata( L, 1, PROCESS_CHILD_MT );
    int signo = (int)luaL_optinteger( L, 2, SIGTERM );
    int rc = kill( chd->pid, signo );
    
    if( rc == 0 ){
        return 0;
    }
    
    // got error
    lua_pushstring( L, strerror( errno ) );
    
    return 1;
}


static int pid_lua( lua_State *L )
{
    pchild_t *chd = luaL_checkudata( L, 1, PROCESS_CHILD_MT );
    
    lua_pushinteger( L, chd->pid );
    
    return 1;
}


static int gc_lua( lua_State *L )
{
    pchild_t *chd = lua_touserdata( L, 1 );
    int i = 0;
    
    for(; i < 3; i++ )
    {
        if( chd->fds[i] ){
            close( chd->fds[i] );
            chd->fds[i] = 0;
        }
    }
    
    return 0;
}


static int tostring_lua( lua_State *L )
{
    lua_pushfstring( L, PROCESS_CHILD_MT ": %p", lua_touserdata( L, 1 ) );
    return 1;
}


LUALIB_API int luaopen_process_child( lua_State *L )
{
    struct luaL_Reg mmethod[] = {
        { "__gc", gc_lua },
        { "__tostring", tostring_lua },
        { NULL, NULL }
    };
    struct luaL_Reg method[] = {
        { "pid", pid_lua },
        { "kill", kill_lua },
        { "stdin", stdin_lua },
        { "stdout", stdout_lua },
        { "stderr", stderr_lua },
        { NULL, NULL }
    };
    struct luaL_Reg *ptr = mmethod;
    
    // create metatable
    luaL_newmetatable( L, PROCESS_CHILD_MT );
    // metamethods
    while( ptr->name ){
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    }
    // methods
    lua_pushstring( L, "__index" );
    lua_newtable( L );
    ptr = method;
    while( ptr->name ){
        lstate_fn2tbl( L, ptr->name, ptr->func );
        ptr++;
    }
    lua_rawset( L, -3 );
    lua_pop( L, 1 );
    
    return 0;
}


