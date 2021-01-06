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

#ifndef RULE_H
#define RULE_H

/* one rule per user, mode, path */

typedef struct rule {
        const char *user;
#define RULE_MODE_R  1
#define RULE_MODE_W  2
#define RULE_MODE_RW 3
        int         mode;
        const char *path;
        const char *src_path;
        int         src_line;
} s_rule;

/* null terminated array of null terminated arrays of rules. */

#define RULES_MAX (1024 * 10)

int read_rules (s_rule rules[RULES_MAX], const char *path);

#endif
