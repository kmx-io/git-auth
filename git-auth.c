/*
 * git-auth  -  restrict git commands
 * Copyright 2012 Thomas de Grivel <billitch@gmail.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <err.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

#include "sympackage.h"
#include "rule.h"

#ifndef ENV_AUTH_ID
# define ENV_AUTH_ID "GIT_AUTH_ID"
#endif

#ifndef SHELL
# define SHELL "git-shell"
#endif

s_sympackage g_sympkg;

void log_args (const char *op, int argc, const char **argv)
{
  char msg[2048];
  size_t m = 0;
  int a;
  for (a = 0; a < argc && m < sizeof(msg); a++) {
    msg[m++] = ' ';
    m += strlcpy(msg + m, argv[a], sizeof(msg) - m);
  }
  syslog(LOG_INFO, "%s%s", op, msg);
}

void log_cmd (const char *op, s_symtable *cmd)
{
  char msg[2048];
  size_t m = 0;
  size_t a;
  assert(cmd);
  for (a = 0; a < cmd->count && m < sizeof(msg); a++) {
    msg[m++] = ' ';
    if (a == 0) {
      m += strlcpy(msg + m, ENV_AUTH_ID, sizeof(msg) - m);
      msg[m++] = '=';
    }
    m += strlcpy(msg + m, cmd->sym[a], sizeof(msg) - m);
  }
  syslog(LOG_INFO, "%s%s", op, msg);
}

t_sym read_symbol (const char **buf)
{
  const char *b = *buf;
  const char *start;
  while (isspace(*b))
    b++;
  start = b;
  while (*b && !isspace(*b))
    b++;
  *buf = b;
  return (start == b) ? NULL :
    sympackage_intern_n(&g_sympkg, start, b - start);
}

void rules_read (s_rules *rr, const char *path)
{
  syslog(LOG_INFO, "READ %s", path);
  FILE *fp = fopen(path, "r");
  char line[2048];
  while (fgets(line, sizeof(line) - 4, fp)) {
    const char *l = line;
    s_rule r;
    t_sym s;
    syslog(LOG_DEBUG, "LINE %s", line);
    rule_init(&r);
    while ((s = read_symbol(&l)) && (*s != '#')) {
      syslog(LOG_DEBUG, "SYMBOL %s", s);
      rule_add(&r, s);
    }
    if (r.count >= 2) {
      log_cmd("RULE", &r);
      rules_add(rr, &r);
    }
    else if (r.count == 1)
      syslog(LOG_WARNING, "invalid rule: %s", line);
  }
  if (ferror(fp)) {
    fclose(fp);
    err(3, "%s", path);
  }
  fclose(fp);
}

int rule_match (s_rule *r, s_symtable *cmd)
{
  assert(r);
  assert(cmd);
  if (r->count > cmd->count)
    return 0;
  size_t i = r->count;
  t_sym *rs = r->sym;
  t_sym *cs = cmd->sym;
  static t_sym sym_wild = 0;
  if (!sym_wild)
    sym_wild = sympackage_intern_static(&g_sympkg, "*");
  while (i--) {
    if (*rs != sym_wild && *rs != *cs)
      return 0;
    syslog(LOG_INFO, "%s %s", *rs, *cs);
    rs++;
    cs++;
  }
  return 1;
}

int auth (s_rules *rr, s_symtable *cmd)
{
  size_t i = rr->count;
  s_rule *r = rr->rule;
  while (i--) {
    if (rule_match(r, cmd))
      return 1;
    r++;
  }
  return 0;
}

void init_package ()
{
  sympackage_init(&g_sympkg);
  sympackage_intern_static(&g_sympkg, "*");
}

void usage (const char *argv0)
{
  fprintf(stderr, "Usage: %s=ID %s CMD ...\n", ENV_AUTH_ID, argv0);
  exit(5);
}

void cmd_init (s_symtable *cmd, t_sym id, int argc, const char *argv[])
{
  symtable_init(cmd);
  symtable_add(cmd, id);
  while (argc--)
    symtable_add(cmd, sympackage_intern(&g_sympkg, *argv++));
}

int main (int argc, char **argv)
{
  s_rules rr;
  openlog(argv[0], LOG_PID, LOG_AUTH);
  log_args("NEW", argc, (const char **)argv);
  if (argv[argc])
    err(1, "bad argument list");
  if (argc < 2)
    usage(argv[0]);
  init_package();
  rules_init(&rr);
  rules_read(&rr, "/etc/git-auth.conf");
  const char *env_auth_id = getenv(ENV_AUTH_ID);
  t_sym id = sympackage_intern(&g_sympkg, env_auth_id ? env_auth_id : "");
  s_symtable cmd;
  cmd_init(&cmd, id, argc - 1, (const char **) argv + 1);
  if (auth(&rr, &cmd)) {
    log_cmd("ALLOW", &cmd);
    closelog();
    cmd.sym[0] = SHELL;
    rules_free(&rr);
    sympackage_free(&g_sympkg);
    execvp(argv[1], argv + 1);
    err(2, "exec failed");
    return 2;
  }
  log_cmd("DENY", &cmd);
  closelog();
  return 1;
}
