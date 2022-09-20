#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <readline/readline.h>
#include <limits.h>
#include <signal.h>
#include <setjmp.h>

typedef void (*sighandler_t)(int);

static sigjmp_buf env;
static volatile sig_atomic_t jump_active = 0;

void signint_handler() {
    if (jump_active) {
        siglongjmp(env, 42);
    }
}

void sigstop_handler() {
    if (jump_active) {
        siglongjmp(env, 43);
    }
}

char **get_input(char *input) {
    char **command = malloc(8 * sizeof(char *));
    char *separator = " ";
    char *parsed;
    int index = 0;

    parsed = strtok(input, separator);
    while (parsed != NULL) {
        command[index++] = parsed;
        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}

// Set ourselves up for a nice little buffer overrun here
char cwd[PATH_MAX];
char prompt_buf[PATH_MAX];
static char *fell_prompt = "\x1b[38;5;42mfell %s>\x1b[0m ";

void set_prompt(char *cwd) {
    char *home = getenv("HOME");
    if (home) {
        int home_len = strlen(home);
        if (strncmp(home, cwd, home_len) == 0) {
            cwd[0] = '~';
            cwd[1] = '/';
            int c = 1;
            while (cwd[c++]){
                cwd[c] = cwd[c + home_len - 1];
            }
        }
    }
    sprintf(&prompt_buf[0], fell_prompt, cwd);
}

int main(void) {
    struct sigaction int_action;
    int_action.sa_handler = signint_handler;
    sigemptyset(&int_action.sa_mask);
    int_action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &int_action, NULL);

    signal(SIGINT, SIG_IGN);

    struct sigaction stop_action;
    stop_action.sa_handler = sigstop_handler;
    sigemptyset(&stop_action.sa_mask);
    stop_action.sa_flags = SA_RESTART;
    sigaction(SIGSTOP, &stop_action, NULL);

    signal(SIGSTOP, SIG_IGN);


    for (;;) {
        if (sigsetjmp(env, 1) == 42) {
            printf("\n");
            continue;
        }
        jump_active = 1;

        getcwd(&cwd[0], PATH_MAX);
        set_prompt(cwd);
        char *r = readline(prompt_buf);

        // We received a Ctrl+d
        if (r == NULL) {
            exit(0);
        }

        char **input = get_input(r);

        if (strcmp(input[0], "cd") == 0) {
            int result = chdir(input[1]);
            if (result < 0) {
                fprintf(stderr, "Error with directory: %s %i\n", input[1], result);
            }
            continue;
        }

        pid_t child = fork();

        if (child < 0) {
            perror("Could not fork");
            exit(1);
        }

        if (child == 0) {
            signal(SIGINT, SIG_DFL);
            signal(SIGSTOP, SIG_DFL);
            if (execvp(input[0], input) < 0) {
                fprintf(stderr, "Error: Could not execute %s\n", input[0]);
            }
        } else {
            int status;
            pid_t wait_pid;
            while (1) {
                wait_pid = waitpid(child, &status, WNOHANG | WUNTRACED);
                if (wait_pid != child) {
                    /* printf("Not the same: %i\n", status); */
                } else {
                    printf("Something happened: %i", status);
                    if (WIFSTOPPED(status)) {
                        printf("Was it stopped? %i\n", WIFSTOPPED(status));
                        break;
                    }
                    if (WIFEXITED(status)) {
                        printf("Exited %i returned %i\n", wait_pid, WIFEXITED(status));
                        break;
                    }
                    if (WIFSIGNALED(status)) {
                        printf("Signaled returned %i\n", WIFSIGNALED(status));
                        break;
                    }
                }
            }
        }

        free(r);
        free(input);
    }
}
