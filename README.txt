



** termios - get and set terminal attributes, line control, get and set baud rate

Arguments are conventional:

- io: either an io object from the standard io library or a file descriptor number

- fd: a file descriptor number

Return on failure is nil, followed by the error message from strerror(), followed by
error number, see errno.h.

Notes and caveats:

- Haven't tested portability, might need to test _BSD_SOURCE to see if cfsetspeed() or cfmakeraw() exist.

- Error strings could be annotated with info about the system call and args that failed.

- Could make better effort to document the functions, but your system's man pages really are the final reference.

- Doesn't support access to all the bits and pieces of struct termios... wrapping that thing as
  userdata would be quite the task.


-- fd = termios.fileno(io)

Return the file descriptor number for the specified io object.

Returns io on success, nil, errmsg, errno on failure.


-- io = termios.setblocking(io[, blocking])

Blocking is true to set blocking, and false to set non-blocking (default is false)

Returns io on success, nil, errmsg, errno on failure.


-- io = termios.setcanonical(io, canonical, when)

Turns canonical mode on and off for a TTY.  Canonical defaults to true.

When is "now", "drain", or "flush". Default is "flush".

Returns io on success, nil, errmsg, errno on failure.


-- speeds = { 0, 50, ..., [0] = true, [50] = true, ... }

speeds contains a list of supported speeds, iterable with ipairs(),
as well as setting each supported speed in the table to true.


-- io = termios.cfsetspeed(io, speed, when)


-- io = termios.cfsetispeed(io, speed, when)


-- io = termios.cfsetospeed(io, speed, when)

Set speed for input and output, input only, or output only.

Speed is the baud rate, and must be one of those supported by termios, 0, 1200,
1400, 4800, 9600, 38400 are common.

For output, a speed of zero disconnects the line.

For input, a speed of zero means set the input speed to the output speed.

When is "now", "drain", or "flush". Default is "flush".

Returns io on success, or nil, errmsg, errno on failure.


-- speed = termios.cfgetispeed(io)


-- speed = termios.cfgetospeed(io)

Get speed for input or output.

Speed is the baud rate.

Returns speed on success, or nil, errmsg, errno on failure.

If errmsg is "unsupported", the errno will be followed by the underlying
speed_t value returned by the C library, which may be useful for debugging.


-- io = termios.tcflush(io, direction)

See man page for tcflush()

Direction is either "in", "out", or "both". It defaults to "both".

Returns io on success, nil, errmsg, errno on failure.


-- io = termios.tcdrain(io).

See man page for tcdrain()

Returns io on success, nil, errmsg, errno on failure.


-- io = termios.tcsendbreak(io, duration).

See man page for tcsendbreak().

Duration is optional, and defaults to zero. If non-zero, its meaning is
apparently implementation-defined, it might even be ignored.

Returns io on success, nil, errmsg, errno on failure.


-- io = termios.cfraw(io, when)

See man page for cfmakeraw()

When is "now", "drain", or "flush". Default is "flush".

Returns io on success, nil, errmsg, errno on failure.


-- fd = termios.open(path)

The path must exist, and is opened read-write.

Returns fd on success, nil, errmsg, errno on failure.


-- termios.close(fd)

Close an fd, which must be a number, not an io object.

Returns nothing on success, nil, errmsg, errno on failure.
