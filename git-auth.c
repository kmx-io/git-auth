/*
 * git-auth  -  restrict git commands
 * Copyright 2012,2021 Thomas de Grivel <thoxdg@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include "rule.h"

#ifndef GIT_AUTH_ID_ENV
#define GIT_AUTH_ID_ENV "GIT_AUTH_ID"
#endif

#ifndef GIT_SHELL
#define GIT_SHELL "git-shell"
#endif

static void stracat (char *buf, size_t bufsz, int argc, const char **argv)
{
        size_t b = 0;
        int a;
        assert(buf);
        assert(bufsz);
        assert(argc);
        assert(argv);
        for (a = 0; a < argc; a++) {
                const char *arg;
                assert(b < bufsz - strlen(argv[a]));
                if (a)
                        buf[b++] = ' ';
                arg = argv[a];
                while (*arg)
                        buf[b++] = *(arg++);
        }
        buf[b] = 0;
}

static void stracat_quoted (char *buf, size_t bufsz, int argc, const char **argv)
{
        size_t b = 0;
        int a;
        assert(buf);
        assert(bufsz);
        assert(argc);
        assert(argv);
        for (a = 0; a < argc; a++) {
                const char *arg;
                assert(b < bufsz - 3 - strlen(argv[a]));
                if (a)
                        buf[b++] = ' ';
                buf[b++] = '"';
                arg = argv[a];
                while (*arg) {
                        char c = *(arg++);
                        if (c == '"')
                                buf[b++] = '\\';
                        buf[b++] = c;
                }
                buf[b++] = '"';
        }
        buf[b] = 0;
}

static void log_args (const char *op, int argc, const char **argv)
{
        char msg[2048];
        stracat(msg, sizeof(msg), argc, argv);
        syslog(LOG_INFO, "%s %s", op, msg);
}

static void log_rule (const char *op, s_rule *rule)
{
        char msg[2048];
        size_t m = 0;
        const char *mode;
        m += strlcpy(msg + m, GIT_AUTH_ID_ENV, sizeof(msg) - m);
        msg[m++] = '=';
        m += strlcpy(msg + m, rule->user, sizeof(msg) - m);
        msg[m++] = ' ';
        assert(1 <= rule->mode && rule->mode <= 3);
        switch (rule->mode) {
        case 1: mode = "r"; break;
        case 2: mode = "w"; break;
        case 3: mode = "rw"; break;
        }
        m += strlcpy(msg + m, mode, sizeof(msg) - m);
        msg[m++] = ' ';
        m += strlcpy(msg + m, rule->path, sizeof(msg) - m);
        syslog(LOG_INFO, "%s %s", op, msg);
}

/*
static void log_rules (s_rule rules[RULES_MAX])
{
        int r = 0;
        while (rules[r].user) {
                char buf[16];
                snprintf(buf, 15, "%d", r);
                log_rule(buf, &rules[r]);
                r++;
        }
}
*/

static void log_cmd (const char *op, int argc, const char **argv)
{
        char msg[2048];
        stracat(msg, sizeof(msg), argc, argv);
        syslog(LOG_INFO, "%s %s", op, msg);
}

/*
int unquote (char *buf, size_t bufsz, const char *str)
{
        size_t len = strlen(str);
        assert(len < bufsz);
        if (str[0] == '\'' && len > 1 && str[len - 1] == '\'') {
                strlcpy(buf, str + 1, bufsz);
                buf[len - 2] = 0;
                return 1;
        }
        strlcpy(buf, str, bufsz);
        return 0;
}

static int rule_match_path (const char *rule_path, const char *arg_path)
{
        char rp[1024];
        char ap[1024];
        unquote(rp, sizeof(rp), rule_path);
        unquote(ap, sizeof(ap), arg_path);
        return strcmp(rp, ap) == 0;
}
*/

static int rule_match (s_rule *rule, int argc, const char **argv)
{
        assert(rule);
        assert(argc);
        assert(argv);
        if (argc != 3)
                return 0;
        if (strcmp(rule->user, argv[0]))
                return 0;
        if (!strcmp(argv[1], "git-upload-pack") && !(rule->mode & 1))
                return 0;
        else if (!strcmp(argv[1], "git-receive-pack") && !(rule->mode & 2))
                return 0;
        if (strcmp(rule->path, argv[2]))
                return 0;
        return 1;
}

static int auth (s_rule rules[RULES_MAX], int argc, const char **argv)
{
        int r = 0;
        while (rules[r].user) {
                /* log_rule("MATCH", &rules[r]); */
                if (rule_match(&rules[r], argc, argv)) {
                        log_rule("ALLOW", &rules[r]);
                        return 1;
                }
                /* else
                   log_rule("DENY", &rules[r]); */
                r++;
        }
        return 0;
}

static void cleanup (void)
{
        closelog();
}

static void exec_cmd (int argc, const char **argv)
{
        char buf[2048];
        const char *cmd_argv[4];
        assert(argc);
        assert(argv);
        cmd_argv[0] = GIT_SHELL;
        cmd_argv[1] = "-c";
        stracat(buf, sizeof(buf), argc - 1, argv + 1);
        cmd_argv[2] = buf;
        cmd_argv[3] = NULL;
        log_cmd("EXEC", 3, cmd_argv);
        cleanup();
        execvp(cmd_argv[0], (char *const *) cmd_argv);
        err(1, "%s", cmd_argv[0]);
}

static void usage (const char *argv0)
{
        fprintf(stderr, "Usage: %s=ID %s -c GIT_COMMAND PATH\n",
                GIT_AUTH_ID_ENV, argv0);
        exit(1);
}

int main (int argc, char **argv)
{
        char buf[1024];
        char *bs;
        s_rule rules[RULES_MAX];
        const char *git_auth_id;
        int auth_ok;
        const char *cmd_argv[3];
        if (argc != 3) {
                char buf[1024];
                fprintf(stderr, "git-auth: wrong number of arguments: %d.\n", argc);
                stracat_quoted(buf, sizeof(buf), argc, (const char **) argv);
                fprintf(stderr, "%s\n", buf);
                usage(argv[0]);
        }
        if (strcmp(argv[1], "-c")) {
                fprintf(stderr, "expected -c as first argument.\n");
                usage(argv[0]);
        }
        git_auth_id = getenv(GIT_AUTH_ID_ENV);
        if (!git_auth_id) {
                fprintf(stderr, "missing %s.\n", GIT_AUTH_ID_ENV);
                usage(argv[0]);
        }
        cmd_argv[0] = git_auth_id;
        strlcpy(buf, argv[2], sizeof(buf));
        cmd_argv[1] = buf;
        bs = strchr(buf, ' ');
        *bs++ = 0;
        cmd_argv[2] = bs;
        openlog(argv[0], LOG_PID, LOG_AUTH);
        log_args("NEW", argc, (const char **) argv);
        read_rules(rules, "/etc/git/auth.conf");
        /* log_rules(rules); */
        auth_ok = auth(rules, 3, cmd_argv);
        log_cmd(auth_ok ? "ALLOW" : "DENY", 3, cmd_argv);
        if (auth_ok) {
                exec_cmd(3, cmd_argv);
                /* never reached */
        }
        cleanup();
        return 1;
}
