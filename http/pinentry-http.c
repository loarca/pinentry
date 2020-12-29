/* pinentry-tty.c - A minimalist dumb terminal mechanism for PIN entry
 * Copyright (C) 2014 Serge Voilokov
 * Copyright (C) 2015 Daniel Kahn Gillmor <dkg@fifthhorseman.net>
 * Copyright (C) 2015 g10 Code GmbH
 *
 * This file is part of PINENTRY.
 *
 * PINENTRY is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PINENTRY is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 * SPDX-License-Identifier: GPL-2.0+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#ifdef HAVE_UTIME_H
#include <utime.h>
#endif /*HAVE_UTIME_H*/
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <gpg-error.h>

#include "pinentry.h"
#include "memory.h"

int
http_cmd_handler (pinentry_t pinentry)
{
  int rc = 0;

  // http request
  FILE *fp;
  char *data = secmem_malloc(4096);

  /* Open the command for reading. */
  fp = popen("curl http://localhost:46727", "r");
  if (fp == NULL)
  {
    printf("Failed to run command\n");
    exit(1);
  }

  /* Read the output a line at a time - output it. */
  // while (fgets(data, sizeof(data), fp) != NULL)
  // {
  //   printf("%s", data);
  // }
  fgets(data, 4096, fp);

  /* close */
  pclose(fp);

  char *pos;
  if ((pos = strchr(data, '\n')) != NULL)
    *pos = '\0';

  // pinentry
  pinentry_setbuffer_use(pinentry, data, 0);
  pinentry->result = pinentry->pin_len;
  rc = 1;
  return rc;
}

pinentry_cmd_handler_t pinentry_cmd_handler = http_cmd_handler;

int
main (int argc, char *argv[])
{
  pinentry_init ("pinentry-http");

  /* Consumes all arguments.  */
  pinentry_parse_opts(argc, argv);

  if (pinentry_loop ())
    return 1;

  return 0;
}
