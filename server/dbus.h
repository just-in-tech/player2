#ifndef DBUS_H_INCLUDED
#define DBUS_H_INCLUDED

#include <systemd/sd-bus.h>

int bus_read_property(const char *name, sd_bus_message *property,void *return_value);

#endif // DBUS_H_INCLUDED
