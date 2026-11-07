#pragma once

#define TMA_MSG "too many arguments"
#define HELP_HELP_MSG "show help information"
#define EXTERN_HELP_MSG "external command or application"
#define EXIT_HELP_MSG "exit the shell"
#define PWD_HELP_MSG "print the current directory"
#define CD_HELP_MSG "change the current directory"
#define HISTORY_HELP_MSG "print the history of commands"
#define HISTORY_INVALID_MSG "command invalid"
#define HISTORY_NO_LAST_MSG "no command entered"
#define GETCWD_ERROR_MSG "unable to get current directory"
#define CHDIR_ERROR_MSG "unable to change directory"
#define READ_ERROR_MSG "unable to read command"
#define FORK_ERROR_MSG "unable to fork"
#define EXEC_ERROR_MSG "unable to execute command"
#define WAIT_ERROR_MSG "unable to wait for child"

/*
 * Formatting string used to combine a command and a message
 *
 * Example:
 * printf(FORMAT_MSG, "cd", CD_HELP_MSG);
 * 
 * Note: printf() is not signal safe so cannot be used in signal handlers.
 */
#define FORMAT_MSG "%s: %s\n"

/*
 * Formatting string used to combine a number and a command for `history`.
 *
 * Example:
 * printf(FORMAT_HISTORY, 5, "cd /");
 */
#define FORMAT_HISTORY "%d\t%s\n"
