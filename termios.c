/*
Copyright (c) 2011 Wurldtech Security Technologies All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/

/*-
** termios - get and set terminal attributes, line control, and baud rate

These functions take either file descriptors (numbers), or io objects from the
standard library as arguments.
*/

/* TODO - use standard failure idiom: return nil, errmsg, errno */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"


#define REGID "wt.termios"

static int check_fileno(lua_State *L, int index)
{
    if (lua_isnumber(L, index)) {
        return luaL_checkint(L, index);
    } else {
        FILE** f = luaL_checkudata(L, index, LUA_FILEHANDLE);

        if (*f == NULL)
            luaL_error(L, "attempt to use a closed file");

        return fileno(*f);
    }
}

static int check_when(lua_State *L, int index)
{
    static const char* opts[] = { "now", "drain", "flush", NULL };
    static int opti[] = { TCSANOW, TCSADRAIN, TCSAFLUSH };

    return opti[ luaL_checkoption(L, index, "flush", opts) ];
}

static void check_tcgetattr(lua_State *L, int fd, struct termios *termios)
{
    if (tcgetattr(fd, termios) < 0) {
        luaL_error(L, "tcgetattr failed [%d] %s", errno, strerror(errno));
    }
}

static void check_tcsetattr(lua_State *L, int fd, int opt, struct termios *termios)
{
    if (tcsetattr(fd, opt, termios) < 0) {
        luaL_error(L, "tcsetattr failed [%d] %s", errno, strerror(errno));
    }
}

/*-
-- fd = termios.fileno(io)

Return the file descriptor number for the specified io object.
*/
static int ltermios_fileno(lua_State *L)
{
    int fd = check_fileno(L, 1);

    lua_pushnumber(L, fd);

    return 1;
}

/*-
-- io = termios.setblocking(io[, blocking])

blocking is true to set blocking, and false to set non-blocking (default is false)
*/
static int ltermios_setblocking(lua_State *L)
{
    int fd = check_fileno(L, 1);
    int block = lua_toboolean(L, 2);
    
    int flags = fcntl(fd, F_GETFL);
    if (flags<0) {
        return luaL_error(L, "fcntl failed [%d] %s", errno, strerror(errno));
    }
    
    /* to SET blocking, we CLEAR O_NONBLOCK */
    if (block) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    
    flags = fcntl(fd, F_SETFL, flags);
    if (flags<0) {
        return luaL_error(L, "fcntl failed [%d] %s", errno, strerror(errno));
    }
    
    lua_settop(L, 1);

    return 1;
}

/*-
-- io = termios.canonical(io, canonical, when)

Turns canonical mode on and off for a TTY.  Canonical defaults to true.

When is "now", "drain", or "flush". Default is "flush".
*/
/* TODO should canonical default to false? */
static int optboolean(lua_State* L, int narg, int d)
{
    if (lua_isnil(L, narg)) {
        return d;
    }
    return lua_toboolean(L, narg);
}
static int ltermios_canonical(lua_State *L)
{
    int fd = check_fileno(L, 1);
    int canonical = optboolean(L, 2, 1);
    int opt = check_when(L, 3);
    
    struct termios termios={0};
    check_tcgetattr(L, fd, &termios);

    if (canonical) {
        termios.c_lflag |= ICANON;
    }
    else {
        termios.c_lflag &= ~ICANON;
    }

    check_tcsetattr(L, fd, opt, &termios);

    lua_settop(L, 1);

    return 1;
}

/*-
-- io = termios.cfsetspeed(io, speed, when)
-- io = termios.cfsetispeed(io, speed, when)
-- io = termios.cfsetospeed(io, speed, when)

Set speed for input and output, input only, or output only.

Speed is the baud rate, and must be one of those supported by termios, 0, 1200,
1400, 4800, 9600, 38400 are common.

When is "now", "drain", or "flush". Default is "flush".
*/

typedef int cfspeedfn(struct termios *termios_p, speed_t speed);

static int setspeed(lua_State* L, cfspeedfn* speedfn)
{
    int fd = check_fileno(L, 1);
    int opt = check_when(L, 3);
    speed_t speed = 0;
    struct termios termios;

    switch(luaL_checkint(L, 2)) {
#ifdef B0
        case 0: speed = B0; break;
#endif
#ifdef B50
        case 50: speed = B50; break;
#endif
#ifdef B75
        case 75: speed = B75; break;
#endif
#ifdef B110
        case 110: speed = B110; break;
#endif
#ifdef B134
        case 134: speed = B134; break;
#endif
#ifdef B150
        case 150: speed = B150; break;
#endif
#ifdef B200
        case 200: speed = B200; break;
#endif
#ifdef B300
        case 300: speed = B300; break;
#endif
#ifdef B600
        case 600: speed = B600; break;
#endif
#ifdef B1200
        case 1200: speed = B1200; break;
#endif
#ifdef B1800
        case 1800: speed = B1800; break;
#endif
#ifdef B2400
        case 2400: speed = B2400; break;
#endif
#ifdef B4800
        case 4800: speed = B4800; break;
#endif
#ifdef B9600
        case 9600: speed = B9600; break;
#endif
#ifdef B19200
        case 19200: speed = B19200; break;
#endif
#ifdef B38400
        case 38400: speed = B38400; break;
#endif
#ifdef B57600
        case 57600: speed = B57600; break;
#endif
#ifdef B115200
        case 115200: speed = B115200; break;
#endif
#ifdef B230400
        case 230400: speed = B230400; break;
#endif
        default:
            return luaL_argerror(L, 2, "unsupported serial speed");
    }

    check_tcgetattr(L, fd, &termios);

    if (speedfn(&termios, speed) < 0) {
        return luaL_error(L, "setspeed failed [%d] %s", errno, strerror(errno));
    }

    check_tcsetattr(L, fd, opt, &termios);

    lua_settop(L, 1);

    return 1;
}

static int ltermios_cfsetspeed(lua_State* L)
{
    return setspeed(L, cfsetspeed);
}
static int ltermios_cfsetispeed(lua_State* L)
{
    return setspeed(L, cfsetispeed);
}
static int ltermios_cfsetospeed(lua_State* L)
{
    return setspeed(L, cfsetospeed);
}

/*-
-- io = termios.tcflush(io, direction)

See man page for tcflush()

Direction is either "in", "out", or "both". It defaults to "both".
*/
static int ltermios_tcflush(lua_State *L)
{
    int fd = check_fileno(L, 1);

    static const char* opts[] = { "in", "out", "both", NULL };
    static int opti[] = { TCIFLUSH, TCOFLUSH, TCIOFLUSH };

    int opt = opti[ luaL_checkoption(L, 2, "both", opts) ];

    if(tcflush(fd, opt) < 0) {
        return luaL_error(L, "tcflush failed [%d] %s", errno, strerror(errno));
    }

    lua_settop(L, 1);

    return 1;
}

/*-
-- io = termios.cfraw(io, when)

See man page for cfmakeraw()

When is "now", "drain", or "flush". Default is "flush".
*/
static int ltermios_cfraw(lua_State *L)
{
    int fd = check_fileno(L, 1);
    int opt = check_when(L, 2);
    struct termios termios;

    check_tcgetattr(L, fd, &termios);

    cfmakeraw(&termios);

    check_tcsetattr(L, fd, opt, &termios);

    lua_settop(L, 1);

    return 1;
}

/*-
-- fd = termios.open(path)

The path must exist, and is opened read-write.
*/
/* Could I depend on luaposix for this? */
static int ltermios_open(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);
    int fd = open(path, O_NOCTTY|O_RDWR);

    if (fd < 0) {
        return luaL_error(L, "open %s failed [%d] %s", path, errno, strerror(errno));
    }

    lua_pushinteger(L, fd);

    return 1;
}

/*-
-- termios.close(fd)

Close fd, which must be a number, not an io object.
*/
static int ltermios_close(lua_State *L)
{
    close(luaL_checkint(L, 1));

    return 0;
}

static const luaL_reg termios[] =
{
    {"fileno",            ltermios_fileno},
    {"setblocking",       ltermios_setblocking},
    {"nonblock",          ltermios_setblocking}, /* for backwards compatibility TODO remove */
    {"canonical",         ltermios_canonical}, /* TODO should be called setcanonical() */
    {"tcflush",           ltermios_tcflush},
    {"tcraw",             ltermios_cfraw}, /* for backwards compatibility TODO remove*/
    {"cfraw",             ltermios_cfraw},
    {"cfsetspeed",        ltermios_cfsetspeed},
    {"cfsetispeed",       ltermios_cfsetispeed},
    {"cfsetospeed",       ltermios_cfsetospeed},
    {"open",              ltermios_open},
    {"close",             ltermios_close},
    {NULL, NULL}
};

LUALIB_API int luaopen_termios (lua_State *L)
{
    luaL_register(L, "termios", termios);

    return 1;
}

