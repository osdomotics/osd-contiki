/**
 * \file
 *      Resource for crontab entry
 * \author
 *      Ralf Schlatterbeck <rsc@runtux.com>
 *
 * \brief get/put crontab entry
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "time_resource.h"
#include "jsonparse.h"
#include "er-coap.h"
#include "generic_resource.h"
#include "cron.h"

static size_t get_index_from_uri (const char *uri)
{
  const char *s;
  char *endptr;
  size_t idx;
  if (uri == NULL) {
    return MAX_CRON_ENTRIES;
  }
  if (NULL == (s = strrchr (uri, '/'))) {
    return MAX_CRON_ENTRIES;
  }
  idx = strtoul (s+1, &endptr, 10);
  if (s == endptr || *endptr != '\0') {
    return MAX_CRON_ENTRIES;
  }
  return idx;
}

int crontab_from_string
  (const char *name, const char *uri, const char *query, const char *s)
{
  const char *err;
  int res;
  size_t idx = get_index_from_uri (uri);
  if (idx >= MAX_CRON_ENTRIES) {
    return -1;
  }
  res = parse_crontab_line (s, get_cron_entry (idx), &err);
  if (res < 0) {
    printf ("Error parsing: %s\n", err);
    return -1;
  }
  return 0;
}

size_t
crontab_to_string
  ( const char *name
  , const char *uri
  , const char *query
  , char *buf
  , size_t bsize
  )
{
  /* FIXME: For now we only return "valid" or "invalid" until someone
   * comes up with a clever algorithm to reconstruct a crontab string
   * from the cron_entry struct.
   */
  size_t idx = get_index_from_uri (uri);
  struct cron_entry *e;
  if (idx >= MAX_CRON_ENTRIES) {
    return MAX_GET_STRING_LENGTH;
  }
  e = get_cron_entry (idx);
  if (e->flags & VALID) {
    return snprintf (buf, bsize, "valid");
  }
  return snprintf (buf, bsize, "invalid");
}

GENERIC_RESOURCE
  ( crontab
  , crontab-entry
  , s
  , 1
  , crontab_from_string
  , crontab_to_string
  );

/* Allocate all cron entries and the necessary resources */
void activate_cron_resources (void)
{
  size_t n;
  for (n=0; n<MAX_CRON_ENTRIES; n++) {
    resource_t *res;
    char *buf;
    size_t len;
    struct cron_entry *e;
    char name [15];
    /* Need to copy the resource because resource->url holds the path
     * under which we activate it using rest_activate_resource
     */
    if (NULL == (res = malloc (sizeof (*res)))) {
        printf ("Error malloc\n");
        break;
    }
    memcpy (res, &res_crontab, sizeof (*res));
    e = allocate_cron_entry ();
    assert (!(e->flags & VALID));
    len = snprintf (name, sizeof (name), "crontab/%u", n);
    name [sizeof (name) -1] = '\0';
    assert (len < 15);
    if (NULL == (buf = malloc (len + 1))) {
        printf ("Error malloc\n");
        break;
    }
    strcpy (buf, name);
    rest_activate_resource (res, buf);
  }
}

/*
 * VI settings, see coding style
 * ex:ts=8:et:sw=2
 */

