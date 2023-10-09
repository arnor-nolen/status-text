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

# Usage example

Example config: 

```c
#define MAX_STRING_SIZE 1024

const char *start_delimiter = "[";
const char *end_delimiter = "]";

task_t task_list[] = {
    /* Number of packages that can be updated. */
    {"pacman -Qu | wc -l | awk '{printf(\"P %4d\\n\"), $1}'", 15},
    /* Current CPU load. */
    {"awk -v a=\"$(awk '/cpu /{print $2+$4,$2+$4+$5}' /proc/stat; sleep 1)\" "
     "'/cpu /{split(a,b,\" \"); {printf(\"C %3.f%\\n\"), "
     "100*($2+$4-b[1])/($2+$4+$5-b[2])}}'  /proc/stat",
     15},
    /* Current memory load. */
    {"free -t | awk 'NR == 2 {printf(\"M %3.f%\\n\"), $3/$2*100}'", 15},
    /* Current volume. */
    {"(pulsemixer --get-mute; pulsemixer --get-volume) | awk "
     "'NR==1{printf(\"%s\"), $1 ? \"X \": \"V \" "
     "};NR==2{printf(\"%3d%\\n\"), "
     "$1}'",
     0},
    /* Current datetime. */
    {"date '+%a %F %I:%M %p'", 5},
    /* Keyboard layout. */
    {"cat /home/arnor/.cache/.dwl_kbd_layout", 0},
};
```

In the example, time updates periodically, every 5 seconds.
In the example, volume never updates by itself. It only updates when I execute one of these commands from a separate shell:

```sh
# Change volume through pulsemixer and send signal to status-text to update the value. Signal 37 is used because volume has an index of 3.
pulsemixer --change-volume -5 && kill -37 $(pidof status-text)
pulsemixer --change-volume +5 && kill -37 $(pidof status-text)
```

Example output (notice how volume and time update):

```sh
[P    0]
[P    0][C   1%]
[P    0][C   1%][M  10%]
[P    0][C   1%][M  10%][V  75%]
[P    0][C   1%][M  10%][V  75%][Mon 2023-10-09 10:38 PM]
[P    0][C   1%][M  10%][V  75%][Mon 2023-10-09 10:38 PM][en]
[P    0][C   1%][M  10%][V  75%][Mon 2023-10-09 10:38 PM][en]
[P    0][C   1%][M  10%][V  80%][Mon 2023-10-09 10:38 PM][en]
[P    0][C   1%][M  10%][V  80%][Mon 2023-10-09 10:38 PM][en]
[P    0][C   1%][M  10%][V  80%][Mon 2023-10-09 10:38 PM][en]
[P    0][C   1%][M  10%][V  80%][Mon 2023-10-09 10:38 PM][en]
[P    0][C   1%][M  10%][V  80%][Mon 2023-10-09 10:38 PM][en]
[P    0][C   1%][M  10%][V  80%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  80%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  75%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  55%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  45%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  40%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  40%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  40%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  40%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  35%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  30%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  25%][Mon 2023-10-09 10:39 PM][en]
[P    0][C   1%][M  10%][V  25%][Mon 2023-10-09 10:39 PM][en]
```
