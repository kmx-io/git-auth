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

#include <string.h>
#include <stdlib.h>
#include <err.h>

#include "symbol.h"

char * string_clone (const char *string)
{
  if (string) {
    size_t len = strlen(string);
    char *clone = malloc(len + 1);
    if (!clone)
      err(10, "string_clone malloc");
    strlcpy(clone, string, len + 1);
    return clone;
  }
  return NULL;
}

char * string_clone_n (const char *string, size_t len)
{
  if (string) {
    char *clone = malloc(len + 1);
    if (!clone)
      err(10, "string_clone_n malloc");
    strlcpy(clone, string, len + 1);
    return clone;
  }
  return NULL;
}
