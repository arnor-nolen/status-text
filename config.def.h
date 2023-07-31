#define MAX_STRING_SIZE 1024

const char *start_delimiter = "[";
const char *end_delimiter = "]";

task_t task_list[] = {
    /* Command, refresh interval(sec) */
    {"date '+%a %F %I:%M %p'", 5},
};
