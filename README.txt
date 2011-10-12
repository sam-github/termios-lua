



** termios - get and set terminal attributes, line control, and baud rate

These functions take either file descriptors (numbers), or io objects from the
standard library as arguments.


-- fd = termios.fileno(io)

Return the file descriptor number for the specified io object.


-- io = termios.setblocking(io[, blocking])

blocking is true to set blocking, and false to set non-blocking (default is false)


-- io = termios.canonical(io, canonical, when)

Turns canonical mode on and off for a TTY.  Canonical defaults to true.

When is "now", "drain", or "flush". Default is "flush".


-- io = termios.cfsetspeed(io, speed, when)


-- io = termios.cfsetispeed(io, speed, when)


-- io = termios.cfsetospeed(io, speed, when)

Set speed for input and output, input only, or output only.

Speed is the baud rate, and must be one of those supported by termios, 0, 1200,
1400, 4800, 9600, 38400 are common.

When is "now", "drain", or "flush". Default is "flush".


-- io = termios.tcflush(io, direction)

See man page for tcflush()

Direction is either "in", "out", or "both". It defaults to "both".


-- io = termios.cfraw(io, when)

See man page for cfmakeraw()

When is "now", "drain", or "flush". Default is "flush".


-- fd = termios.open(path)

The path must exist, and is opened read-write.


-- termios.close(fd)

Close fd, which must be a number, not an io object.
