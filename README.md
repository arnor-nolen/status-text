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

# Output example

Example config: 

```c
#define MAX_STRING_SIZE 1024

const char *start_delimiter = "[";
const char *end_delimiter = "]";

task_t task_list[] = {
    {"pacman -Qu | wc -l | awk '{printf(\" %4d\\n\"), $1}'", 15},
    {"awk -v a=\"$(awk '/cpu /{print $2+$4,$2+$4+$5}' /proc/stat; sleep 1)\" "
     "'/cpu /{split(a,b,\" \"); {printf(\" %3.f%\\n\"), "
     "100*($2+$4-b[1])/($2+$4+$5-b[2])}}'  /proc/stat",
     15},
    {"free -t | awk 'NR == 2 {printf(\"󰍛 %3.f%\\n\"), $3/$2*100}'", 15},
    {"(pulsemixer --get-mute; pulsemixer --get-volume) | awk "
     "'NR==1{printf(\"%s\"), $1 ? \"󰝟 \": \"󰕾 \" "
     "};NR==2{printf(\"%3d%\\n\"), "
     "$1}'",
     0},
    {"date '+%a %F %I:%M %p'", 5},
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

Example output (notice how volume updates):

```sh
[    0]
[    0][   1%]
[    0][   1%][󰍛  10%]
[    0][   1%][󰍛  10%][󰕾  75%]
[    0][   1%][󰍛  10%][󰕾  75%][Mon 2023-10-09 10:38 PM]
[    0][   1%][󰍛  10%][󰕾  75%][Mon 2023-10-09 10:38 PM][en]
[    0][   1%][󰍛  10%][󰕾  75%][Mon 2023-10-09 10:38 PM][en]
[    0][   1%][󰍛  10%][󰕾  80%][Mon 2023-10-09 10:38 PM][en]
[    0][   1%][󰍛  10%][󰕾  80%][Mon 2023-10-09 10:38 PM][en]
[    0][   1%][󰍛  10%][󰕾  80%][Mon 2023-10-09 10:38 PM][en]
[    0][   1%][󰍛  10%][󰕾  80%][Mon 2023-10-09 10:38 PM][en]
[    0][   1%][󰍛  10%][󰕾  80%][Mon 2023-10-09 10:38 PM][en]
[    0][   1%][󰍛  10%][󰕾  80%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  80%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  75%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  55%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  45%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  40%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  40%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  40%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  40%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  35%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  30%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  25%][Mon 2023-10-09 10:39 PM][en]
[    0][   1%][󰍛  10%][󰕾  25%][Mon 2023-10-09 10:39 PM][en]
```
