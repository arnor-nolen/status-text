#define _GNU_SOURCE /* For pipe2(). */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define LEN(x) (sizeof(x) / sizeof(x[0]))

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

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

char status_string[MAX_STRING_SIZE] = "";
char temp_string[MAX_STRING_SIZE] = "";

sigset_t signal_set;

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

void timer_callback(int signum) {
    /* Block all signals from interrupting execution. */
    sigprocmask(SIG_BLOCK, &signal_set, NULL);

    size_t elem_id = signum - SIGRTMIN;

    task_t *task = &task_list[elem_id];
    task_t *prev_task = &task_list[elem_id - 1];

    if (elem_id == 0U) {
        task->output.position = 0;
    } else {
        task->output.position =
            prev_task->output.position + prev_task->output.size;
    }

    task->subprocess = subprocess(task->command);

    /* Save everything until end of string. */
    strncpy(temp_string,
            status_string + task->output.position + task->output.size,
            MAX_STRING_SIZE - task->output.position - task->output.size);

    /* Copy start delimiter. */
    strncpy(status_string + task->output.position, start_delimiter,
            strlen(start_delimiter));

    size_t str_size = strlen(start_delimiter);

    /* Read command output from the pipe. */
    str_size += read(task->subprocess.readfd,
                     status_string + task->output.position + str_size,
                     MAX_STRING_SIZE - task->output.position - str_size) -
                1;

    /* Copy end delimiter. */
    strncpy(status_string + task->output.position + str_size, end_delimiter,
            strlen(end_delimiter));

    str_size += strlen(end_delimiter);

    /* Restore. */
    strncpy(status_string + task->output.position + str_size, temp_string,
            MAX_STRING_SIZE - task->output.position -
                MAX(task->output.size, str_size));

    /* Update positions of all commands following this command. */
    for (size_t i = elem_id + 1; i < LEN(task_list); ++i) {
        task_list[i].output.position += str_size - task->output.size;
    }

    task->output.size = str_size;

    close(task->subprocess.readfd);

    int stat = 0;
    waitpid(task->subprocess.pid, &stat, 0);

    printf("%s\n", status_string);

    /* Unblock the signals. */
    sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
}

int main(int argc, char *argv[]) {
    (void)(argc);
    (void)(argv);

    /* Initialize the signal set. */
    sigemptyset(&signal_set);
    for (size_t i = 0U; i != LEN(task_list); ++i) {
        sigaddset(&signal_set, SIGRTMIN + i);
    }

    /* Turn off stdout buffering. */
    if (setvbuf(stdout, NULL, _IONBF, 0)) {
        _exit(1);
    }

    for (size_t i = 0U; i != LEN(task_list); ++i) {
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

        timer_callback(SIGRTMIN + i);

        (void)signal(SIGRTMIN + i, &timer_callback);
    }

    while (1) {
        pause();
    }

    return 0;
}
