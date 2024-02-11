/*
 * Copyright (C) 2023 Carsten Paeth <calle@calle.in-berlin.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#include <syslog.h>

#include "liquidctl-plugin.h"

/* ------------------------------------------------------------------------- */

#define PLUGIN_DEBUG 0

/* ------------------------------------------------------------------------- */

const gchar *sensors_applet_plugin_name(void);

/* ------------------------------------------------------------------------- */

#define LIQUIDCTL_CACHE_SECONDS 5

enum {
    LIQUIDCTL_NO_SENSOR_ERROR,
    LIQUIDCTL_SENSOR_NOT_FOUND_ERROR
};

gchar *plugin_name = "liquidctl";

/* ------------------------------------------------------------------------- */

static struct unitmap {
   gchar     *unit;
   SensorType type;
   IconType   icon;
} unitmap[] = {
  { .unit = "°C", .type = TEMP_SENSOR, .icon = CPU_ICON },
  { .unit = "rpm", .type = FAN_SENSOR, .icon = FAN_ICON },
};

static struct unitmap *liquid_get_unitmap(gchar *unit)
{
   size_t i;
   for (i = 0; i < sizeof(unitmap)/sizeof(unitmap[0]); i++) {
      if (strcmp(unitmap[i].unit, unit) == 0)
         return &unitmap[i];
   }
   return NULL;
}

/* ------------------------------------------------------------------------- */

struct liquid_sensor {
   gchar   *url;
   gchar   *chip;
   gchar   *sensorname;
   gdouble value;
   gchar   *unit;
};

static void liquid_sensor_free(gpointer ptr)
{
   struct liquid_sensor *p = (struct liquid_sensor *)ptr;
   free(p->url);
   free(p->chip);
   free(p->sensorname);
   free(p->unit);
   free(p);
}

static GList *status_cache = NULL;
static time_t cache_time = 0;

/* ------------------------------------------------------------------------- */

#if PLUGIN_DEBUG

#define LOGFILE_FN "/tmp/mate-sensors-applet-plugin-liquidctl.log"

static void mylog(const char *prefix, const char *buf)
{
   FILE *fp = fopen(LOGFILE_FN, "a");
   fprintf(fp, "%s: %s\n", prefix, buf);
   fclose(fp);
}

static void errmsg(char *format, ...)
{
   char buf[1024];
   va_list args;

   va_start(args, format);
   vsnprintf(buf, sizeof(buf), format, args);
   va_end(args);
   mylog("ERROR", buf);
}

static void debug(char *format, ...)
{
   char buf[1024];
   va_list args;

   va_start(args, format);
   vsnprintf(buf, sizeof(buf), format, args);
   va_end(args);
   mylog("ERROR", buf);
}

#else
static inline void errmsg(char *format, ...) { }
static inline void debug(char *format, ...) { }
#endif


/* ------------------------------------------------------------------------- */

static GList *load_liquid_status(void)
{
    /*
     * Corsair Hydro H100i Pro
     * ├── Liquid temperature     27.5  °C
     * ├── Fan 1 speed            1175  rpm
     * ├── Fan 2 speed            1170  rpm
     * ├── Pump mode             quiet  
     * └── Pump speed             1140  rpm
     */
    GList *list = NULL;
    FILE *fp;
    ssize_t len;
    char *line = NULL;
    size_t linemem = 0;
    gchar *chip = NULL;

    if (status_cache && cache_time + LIQUIDCTL_CACHE_SECONDS > time(0))
       return status_cache;

    fp = popen("liquidctl status", "r");
    if (fp == NULL) {
       errmsg("%s: popen('liquidctl status') failed", __FUNCTION__);
       g_list_free_full(list, liquid_sensor_free);
       return status_cache;
    }

    while ((len = getline(&line, &linemem, fp)) > 0) {
       char *s = line;
       char *sensorname;
       char *unit;
       char *tmp;
       gdouble value;


       if (line[len-1] == '\n')
          line[len-1] = 0;

       debug("%s: '%s'", __FUNCTION__, line);

       if (isascii(*s)) { // "Corsair Hydro H100i Pro"
          free(chip);
	  chip = g_strdup(line);
	  if (chip == NULL) {
	     errmsg("%s: g_strdup failed", __FUNCTION__);
	     break;
	  }
	  continue;
       }
       while (*s && !isspace(*s)) s++; // "├── Liquid temperature     27.5  °C"
       if (*s == 0) // skip line
          continue;
       while (*s && isspace(*s)) s++; // " Liquid temperature     27.5  °C"
       // "Liquid temperature     27.5  °C"
       sensorname = s;
       s = strstr(s, "  ");
       if (s == NULL) // skip line
          continue;
       *s++ = 0;
       // sensorname = "Liquid temperature";
       while (*s && isspace(*s)) s++; // "27.5  °C"
       tmp = s;
       value = strtod(s, &tmp);
       if (tmp == s) // skip line (it is a string value)
          continue;
       s = tmp; // "  °C"
       while (*s && isspace(*s)) s++; // "°C"
       unit = s;

       struct liquid_sensor *p = (struct liquid_sensor *)g_malloc0 (sizeof(struct liquid_sensor));
       if (p == NULL) {
	  errmsg("%s: alloc failed", __FUNCTION__);
          continue;
       }
       p->url = g_strdup_printf ("sensor://%s/%s", chip, sensorname);
       if (p->url == NULL) {
	  errmsg("%s: g_strdup_printf(url) failed", __FUNCTION__);
	  liquid_sensor_free(p);
	  continue;
       }
       for (s = p->url; *s; s++) {
          if (isspace(*s))
	     *s = '_';
       }
       p->chip = g_strdup(chip);
       if (p->chip == NULL) {
	  errmsg("%s: g_strdup(%s) failed", __FUNCTION__, chip);
	  liquid_sensor_free(p);
	  continue;
       }
       p->sensorname = g_strdup(sensorname);
       if (p->sensorname == NULL) {
	  errmsg("%s: g_strdup(%s) failed", __FUNCTION__, sensorname);
	  liquid_sensor_free(p);
	  continue;
       }
       p->value = value;
       p->unit = g_strdup(unit);
       if (p->unit == NULL) {
	  errmsg("%s: g_strdup(%s) failed", __FUNCTION__, unit);
	  liquid_sensor_free(p);
	  continue;
       }

       list = g_list_append(list, p);
    }
    free(line);

    int ret = pclose(fp);
    if (ret != 0) {
       errmsg("%s: pclose() failed (%d)", __FUNCTION__, ret);
    }
    g_list_free_full(status_cache, liquid_sensor_free);
    cache_time = time(0);
    status_cache = list;
    return list;
}

static GList *liquidctl_plugin_init(void)
{
   GList *sensors = NULL;
   GList *liquid_sensors_list;
   guint i;

   debug("%s: start", __FUNCTION__);

   liquid_sensors_list = load_liquid_status();
   if (liquid_sensors_list == NULL) {
      errmsg("%s: error no sensors found", __FUNCTION__);
      return sensors;
   }

   for (i = 0; i < g_list_length(liquid_sensors_list); i++) {
      struct liquid_sensor *p = (struct liquid_sensor *)g_list_nth_data(liquid_sensors_list, i);
      SensorType type;
      IconType icon;

      struct unitmap *unitmap = liquid_get_unitmap(p->unit);
      if (unitmap) {
         type = unitmap->type;
	 icon = unitmap->icon;
      } else {
         type = TEMP_SENSOR;
	 icon = GENERIC_ICON;
      }

      gchar *label = g_strdup_printf ("%s: %s", p->chip, p->sensorname);
      sensors_applet_plugin_add_sensor(&sensors,
                                       p->url,
				       label,
				       label,
				       type,
				       TRUE,
				       icon,
				       DEFAULT_GRAPH_COLOR);
      g_free(label);
   }
   debug("%s: end: %u sensors", __FUNCTION__, g_list_length(sensors));

   return sensors;
}

static gdouble liquidctl_plugin_get_sensor_value(const gchar *path,
                                                 const gchar *id,
                                                 SensorType type,
                                                 GError **error)
{
   GList *liquid_sensors_list;
   guint i;

   liquid_sensors_list = load_liquid_status();
   if (liquid_sensors_list == NULL) {
      // g_set_error (error, SENSORS_APPLET_PLUGIN_ERROR, LIQUIDCTL_NO_SENSOR_ERROR, "Error no sensors");
      return 0.0;
   }

   for (i = 0; i < g_list_length(liquid_sensors_list); i++) {
      struct liquid_sensor *p = (struct liquid_sensor *)g_list_nth_data(liquid_sensors_list, i);
      if (strcmp(p->url, path) == 0)
         return p->value;
   }
   // g_set_error (error, SENSORS_APPLET_PLUGIN_ERROR, LIQUIDCTL_SENSOR_NOT_FOUND_ERROR, "Error sensor not found");
   return 0.0;
}

/* -------- export --------------------------------------------------------- */

const gchar *sensors_applet_plugin_name(void)
{
    return plugin_name;
}

GList *sensors_applet_plugin_init(void)
{
    return liquidctl_plugin_init();
}

gdouble sensors_applet_plugin_get_sensor_value(const gchar *path,
                                               const gchar *id,
                                               SensorType type,
                                               GError **error)
{
    return liquidctl_plugin_get_sensor_value(path, id, type, error);
}

