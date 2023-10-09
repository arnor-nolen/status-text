#define MAX_STRING_SIZE 1024

const char *start_delimiter = "[";
const char *end_delimiter = "]";

task_t task_list[] = {
    /* Number of packages that can be updated. */
    {"pacman -Qu | wc -l | awk '{printf(\" %4d\\n\"), $1}'", 15},
    /* Current CPU load. */
    {"awk -v a=\"$(awk '/cpu /{print $2+$4,$2+$4+$5}' /proc/stat; sleep 1)\" "
     "'/cpu /{split(a,b,\" \"); {printf(\" %3.f%\\n\"), "
     "100*($2+$4-b[1])/($2+$4+$5-b[2])}}'  /proc/stat",
     15},
    /* Current memory load. */
    {"free -t | awk 'NR == 2 {printf(\"󰍛 %3.f%\\n\"), $3/$2*100}'", 15},
    /* Current volume. */
    {"(pulsemixer --get-mute; pulsemixer --get-volume) | awk "
     "'NR==1{printf(\"%s\"), $1 ? \"󰝟 \": \"󰕾 \" "
     "};NR==2{printf(\"%3d%\\n\"), "
     "$1}'",
     0},
    /* Current datetime. */
    {"date '+%a %F %I:%M %p'", 5},
    /* Keyboard layout. */
    {"cat /home/arnor/.cache/.dwl_kbd_layout", 0},
};
