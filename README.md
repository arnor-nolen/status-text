# status-text
Asynchronous executor of shell commands. Intended to be used to set statusbar text.

Program outputs to `stdout`. Every line it outputs should be used to set the statusbar. You can achieve this by piping the output to another program that will set the status.

# Configuration

Copy an example configuration file and modify it accordingly.

```sh
cp config.def.h config.h
```

# Installing

```sh
make
sudo make install
```

# Forcing refresh
You can send a signal using Linux signaling mechanism to force a command reexecution. This is especially useful when you set refresh interval to 0 and would like to manually trigger the update.
```sh
# Use 34 + command array index as a signal number.
# This will reexecute first command and update the status text information.
kill -34 $(pidof status-text)
```

# Known limitations

* Commands should always have same output length (i.e. always output 3 symbols).
