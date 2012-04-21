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

void stracat (char *buf, size_t bufsiz, const char **str_array, size_t sasiz)
{
  size_t b = 0;
  size_t a;
  assert(buf);
  assert(bufsiz);
  assert(str_array);
  assert(sasiz);
  for (a = 0; a < sasiz && b < bufsiz; a++) {
    if (a)
      buf[b++] = ' ';
    b += strlcpy(buf + b, str_array[a], bufsiz - b);
  }
}

void log_args (const char *op, int argc, const char **argv)
{
  char msg[2048];
  stracat(msg, sizeof(msg), argv, argc);
  syslog(LOG_INFO, "%s %s", op, msg);
}

void log_rule (const char *op, s_symtable *cmd)
{
  char msg[2048];
  size_t m = 0;
  assert(cmd);
  m += strlcpy(msg + m, ENV_AUTH_ID, sizeof(msg) - m);
  msg[m++] = '=';
  stracat(msg + m, sizeof(msg) - m, cmd->sym, cmd->count);
  syslog(LOG_INFO, "%s %s", op, msg);
}

void log_cmd (const char *op, s_symtable *cmd)
{
  char msg[2048];
  assert(cmd);
  stracat(msg, sizeof(msg), cmd->sym, cmd->count);
  syslog(LOG_INFO, "%s %s", op, msg);
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

void rule_read (s_rule *r, const char *buf)
{
  const char *b = buf;
  t_sym s;
  while ((s = read_symbol(&b)) && (*s != '#')) {
    syslog(LOG_DEBUG, "SYMBOL %s", s);
    rule_add(r, s);
  }
}

void rules_read (s_rules *rr, const char *path)
{
  syslog(LOG_DEBUG, "READ %s", path);
  FILE *fp = fopen(path, "r");
  char line[2048];
  while (fgets(line, sizeof(line) - 4, fp)) {
    s_rule r;
    syslog(LOG_DEBUG, "LINE %s", line);
    rule_init(&r);
    rule_read(&r, line);
    if (r.count >= 2) {
      log_rule("RULE", &r);
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
    syslog(LOG_DEBUG, "%s %s", *rs, *cs);
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
  fprintf(stderr, "Usage: %s=ID %s -c COMMAND\n", ENV_AUTH_ID, argv0);
  exit(5);
}

void cmd_init (s_symtable *cmd, t_sym id, const char *arg)
{
  rule_init(cmd);
  rule_add(cmd, id);
  rule_read(cmd, arg);
}

void cleanup ()
{
  closelog();
  sympackage_free(&g_sympkg);
}

void exec_cmd (const s_symtable *cmd)
{
  s_symtable xc;
  assert(cmd);
  symtable_init(&xc);
  symtable_add(&xc, SHELL);
  symtable_add(&xc, "-c");
  symtable_add(&xc, cmd->sym[1]);
  log_cmd("EXEC", &xc);
  cleanup();
  execvp(xc.sym[0], (char **)xc.sym);
  err(2, "exec failed");
}

int main (int argc, char **argv)
{
  s_rules rr;
  openlog(argv[0], LOG_PID, LOG_AUTH);
  log_args("NEW", argc, (const char **)argv);
  if (argv[argc])
    err(1, "bad argument list");
  if (argc != 3)
    usage(argv[0]);
  if (strcmp(argv[1], "-c"))
    usage(argv[0]);
  init_package();
  const char *env_auth_id = getenv(ENV_AUTH_ID);
  t_sym id = sympackage_intern(&g_sympkg, env_auth_id ? env_auth_id : "");
  s_symtable cmd;
  cmd_init(&cmd, id, argv[2]);
  rules_init(&rr);
  rules_read(&rr, "/etc/git-auth.conf");
  int auth_ok = auth(&rr, &cmd);
  rules_free(&rr);
  log_rule(auth_ok ? "ALLOW" : "DENY", &cmd);
  if (auth_ok) {
    exec_cmd(&cmd);
    // never reached
  }
  log_rule("DENY", &cmd);
  cleanup();
  return 1;
}
