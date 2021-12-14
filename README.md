# GIT-AUTH


## Synopsis

    GIT_AUTH_ID=ID git-auth COMMAND [ARGS ...]</td>


## Description

**git-auth** starts by reading rules from /etc/git/auth.conf, one rule
per line. Empty lines and lines starting with # are ignored. Each rule
is made of tokens separated by one or more spaces.

**git-auth** executes COMMAND and ARGS using execvp if the requested ID,
COMMAND and ARGS match any of the rules defined in /etc/git/auth.conf.

**git-auth** matches a rule by matching all the rule's tokens with ID,
COMMAND and ARGS in order. Comparison is case sensitive.

A wildcard token "*" matches any string.


## Environment

 * **GIT_AUTH_ID** : an arbitrary token to match with the rules.
This is usually set for each public key in /home/git/.ssh/authorized_keys
with
    environment="GIT_AUTH_ID=..." ssh-rsa ...


## Configuration

Access control rules for each git repository go into
`/etc/git/auth.conf`.

The syntax of this file is one rule per line.
Each rule is composed of symbols separated by spaces.
In order :
 - A **GIT_AUTH_ID** value that was set in
   `/home/git/.ssh/authorized_keys`.
 - Permissions, either `rw` or `r`.
 - A repository path, relative to `/home/git`,
   example: `cl-adams/adams.git`


## Files

 * /etc/git/auth.conf
 * /home/git/.ssh/authorized_keys


# See also

[kmxgit](https://git.kmx.io/kmx.io/kmxgit)
is a web interface on top of git-auth written in Elixir / Phoenix.


# Copying

git-auth - restrict git commands

Copyright 2012,2021 Thomas de Grivel <thoxdg@gmail.com>

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
