#define _GNU_SOURCE /* For pipe2(). */

#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_STRING_SIZE 1024

typedef struct {
    size_t position;
    size_t size;
} output_t;

typedef struct {
    pid_t pid;
    int readfd;
} subprocess_t;

typedef struct {
    timer_t timer;
    struct itimerspec timerspec;
    struct sigevent sigevent;
} event_t;

typedef struct {
    const char *command;
    int period;
    subprocess_t subprocess;
    output_t output;
    event_t event;
} task_t;

#include "config.h"

subprocess_t subprocess(const char *command) {
    int fds[2];
    pipe2(fds, O_CLOEXEC);
    subprocess_t process = {fork(), fds[0]};

    if (process.pid == 0) {
        close(fds[0]);
        dup2(fds[1], STDOUT_FILENO);
        close(fds[1]);

        char *argument_list[] = {"sh", "-c", (char *)command, NULL};
        execvp(argument_list[0], argument_list);
    }

    close(fds[1]);

    return process;
}

void timer_callback(int signum) { printf("Signal %d caught.\n", signum); }

int main(int argc, char *argv[]) {
    (void)(argc);
    (void)(argv);

    char status_string[MAX_STRING_SIZE] = "";

    for (size_t i = 0U; i != sizeof(task_list) / sizeof(task_list[0]); ++i) {
        if (i == 0U) {
            task_list[i].output.position = 0;
        } else {
            task_list[i].output.position = task_list[i - 1].output.position +
                                           task_list[i - 1].output.size - 1;
        }

        task_list[i].subprocess = subprocess(task_list[i].command);

        ssize_t bytes =
            snprintf(status_string + task_list[i].output.position,
                     MAX_STRING_SIZE - task_list[i].output.position, "[");

        bytes += read(task_list[i].subprocess.readfd,
                      status_string + task_list[i].output.position + bytes,
                      MAX_STRING_SIZE - task_list[i].output.position - bytes);

        bytes += snprintf(
            status_string + task_list[i].output.position + bytes - 1,
            MAX_STRING_SIZE - task_list[i].output.position - bytes + 1, "]");

        task_list[i].output.size = bytes;

        close(task_list[i].subprocess.readfd);

        task_list[i].event.timerspec.it_value.tv_sec = task_list[i].period;
        task_list[i].event.timerspec.it_interval.tv_sec = task_list[i].period;

        task_list[i].event.sigevent.sigev_notify = SIGEV_SIGNAL;
        task_list[i].event.sigevent.sigev_signo = SIGRTMIN + i;
        task_list[i].event.sigevent.sigev_value.sival_ptr =
            &task_list[i].event.timerspec;

        if (timer_create(CLOCK_REALTIME, &task_list[i].event.sigevent,
                         &task_list[i].event.timer) == 0) {
            timer_settime(task_list[i].event.timer, 0,
                          &task_list[i].event.timerspec, NULL);
        }

        (void)signal(SIGRTMIN + i, &timer_callback);
    }

    printf("%s\n", status_string);

    for (size_t i = 0U; i != sizeof(task_list) / sizeof(task_list[0]); ++i) {
        int status = 0;
        pid_t pid = waitpid(task_list[i].subprocess.pid, &status, 0);
    }

    while (1) {
        pause();
    }

    return 0;
}
