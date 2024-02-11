
#include <stdio.h>

#include "liquidctl-plugin.h"

typedef struct sensor {
    const gchar *path;
    const gchar *id;
    const gchar *label;
    SensorType type;
    IconType icon;
} TestSensorsAppletSensorInfo;

void sensors_applet_plugin_add_sensor(GList **sensors,
                                      const gchar *path,
                                      const gchar *id,
                                      const gchar *label,
                                      SensorType type,
                                      gboolean enable,
                                      IconType icon,
                                      const gchar *graph_color)
{
   TestSensorsAppletSensorInfo *info;

   info = g_new0 (TestSensorsAppletSensorInfo, 1);

   info->path = g_strdup(path);
   info->id = g_strdup(id);
   info->label = g_strdup(label);
   info->type = type;
   info->icon = icon;

   *sensors = g_list_append(*sensors, info);

   printf("url %s\n", path);
   printf("chip %s\n", id);
   printf("sensorname %s\n", label);
   printf("type %d\n", type);
   printf("icon %d\n", icon);
   printf("\n");
}

int main(int ac, char *av[])
{
   GList *x = sensors_applet_plugin_init();
   return 0;
}
