/*
 * git-auth  -  restrict git commands
 * Copyright 2021 Thomas de Grivel <thoxdg@gmail.com>
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
#include "rule.h"

#define BUFSZ 1024

void skip_whitespace (char **b)
{
        while (**b && strchr(" \f\r\t\v", **b))
                (*b)++;
}

int rule_parse_user (s_rule *rule, char **b)
{
        char *s;
        char *t;
        size_t len;
        char *user;
        s = strchr(*b, ' ');
        t = strchr(*b, '\t');
        if (s && t && t < s)
                s = t;
        if (!s) {
                fprintf(stderr, "%s:%d: invalid rule user.\n",
                        rule->src_path, rule->src_line);
                return 1;
        }
        *s = 0;
        len = s - *b + 1;
        user = malloc(len);
        strlcpy(user, *b, len);
        *b = s + 1;
        rule->user = user;
        return 0;
}

int rule_parse_mode (s_rule *rule, char **b)
{
        char *s;
        char *t;
        skip_whitespace(b);
        s = strchr(*b, ' ');
        t = strchr(*b, '\t');
        if (s && t && t < s)
                s = t;
        if (!s) {
                fprintf(stderr, "%s:%d: invalid rule mode.\n",
                        rule->src_path, rule->src_line);
                return 1;
        }
        *s = 0;
        if (!strcmp(*b, "r")) {
                rule->mode = 1;
                *b += 2;
                return 0;
        }
        if (!strcmp(*b, "w")) {
                rule->mode = 2;
                *b += 2;
                return 0;
        }
        if (!strcmp(*b, "rw")) {
                rule->mode = 3;
                *b += 3;
                return 0;
        }
        if (!strcmp(*b, "*")) {
                rule->mode = 3;
                *b += 2;
                return 0;
        }
        fprintf(stderr, "%s:%d: invalid rule mode.\n",
                rule->src_path, rule->src_line);
        return 1;
}

int rule_parse_path (s_rule *rule, char **b)
{
        char *c;
        size_t len;
        char *path;
        skip_whitespace(b);
        if (!*b || !strcmp(*b, "'") || !strcmp(*b, "''")) {
                fprintf(stderr, "%s:%d: invalid rule path.\n",
                        rule->src_path, rule->src_line);
                return 1;
        }
        c = *b + strlen(*b) - 1;
        while (strchr(" \f\r\t\v", *c))
                c--;
        c++;
        *c = 0;
        len = c - *b + 1;
        path = malloc(len);
        strlcpy(path, *b, len);
        rule->path = path;
        return 0;
}

int rule_parse (s_rule *rule, char *buf, const char *path, int line)
{
        char *b = buf;
        skip_whitespace(&b);
        if (*b == 0 || *b == '#')
                return 1;
        rule->src_path = path;
        rule->src_line = line;
        if (rule_parse_user(rule, &b))
                return 2;
        if (rule_parse_mode(rule, &b))
                return 2;
        if (rule_parse_path(rule, &b))
                return 2;
        return 0;
}


int read_rules (s_rule rules[RULES_MAX], const char *path)
{
        FILE *fp;
        int line;
        int r;
        char buf[BUFSZ];
        char *nl;
        int error = 0;
        if (!(fp = fopen(path, "r")))
                err(1, "%s", path);
        line = 0;
        r = 0;
        while (!feof(fp)) {
                int i;
                line++;
                if (!fgets(buf, BUFSZ, fp)) {
                        if (ferror(fp))
                                err(1, "%s", path);
                        return r;
                }
                if (!(nl = strchr(buf, '\n'))) {
                        fprintf(stderr, "%s:%d: line too long.\n", path, line);
                        exit(1);
                }
                *nl = 0;
                if (r >= RULES_MAX - 1) {
                        fprintf(stderr, "%s: maximum number of rules "
                                "exceeded.\n", path);
                        exit(1);
                }
                i = rule_parse(&rules[r], buf, path, line);
                if (i == 0)
                        r++;
                else if (i == 2)
                        error = 1; 
        }
        bzero(&rules[r], sizeof(s_rule));
        if (error)
                exit(1);
        return r;
}
