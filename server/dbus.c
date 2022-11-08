#include "dbus.h"
#include <systemd/sd-bus.h>
#include <stdbool.h>

int bus_read_property(const char *name, sd_bus_message *property,void *return_value) {
        char type;
        const char *contents;
        int r=0;



        r = sd_bus_message_peek_type(property, &type, &contents);
        if (r < 0)
                return r;

        switch (type) {

        case SD_BUS_TYPE_STRING: {
                const char *s;

                r = sd_bus_message_read_basic(property, type, &s);
                if (r < 0)
                        return r;
                //return_value=&s;
                //strcpy(s, &return_value);
                return r;
        }

        case SD_BUS_TYPE_BOOLEAN: {
                bool b;

                r = sd_bus_message_read_basic(property, type, &b);
                if (r < 0)
                        return r;
                return_value=&b;
                return r;
        }

        case SD_BUS_TYPE_INT64: {
                static int64_t i64;

                r = sd_bus_message_read_basic(property, type, &i64);
                if (r < 0)
                    return r;

            printf("%ld len\n", i64);
            return_value=&i64;
            return r;
        }

        case SD_BUS_TYPE_INT32: {
                int32_t i;

                r = sd_bus_message_read_basic(property, type, &i);
                if (r < 0)
                        return r;

                printf("%s=%i\n", name, (int) i);
                return_value=&i;
                printf("%d\n",return_value);
                return r;
        }

        case SD_BUS_TYPE_OBJECT_PATH:{
            char *p;

            r = sd_bus_message_read_basic(property, type, &p);
            if (r < 0)
                return r;

            printf("%s=%s\n", name, p);
            return_value=&p;

            return r;


        }

        case SD_BUS_TYPE_DOUBLE: {
                double d;

                r = sd_bus_message_read_basic(property, type, &d);
                if (r < 0)
                        return r;

                printf("%s=%g\n", name, d);
                return_value=&d;
                return r;
        }

        case SD_BUS_TYPE_ARRAY:
                if (strcmp(contents, "s")==0) {
                        bool first = true;
                        const char *str;

                        r = sd_bus_message_enter_container(property, SD_BUS_TYPE_ARRAY, contents);
                        if (r < 0)
                                return r;

                        while((r = sd_bus_message_read_basic(property, SD_BUS_TYPE_STRING, &str)) > 0) {
                                if (first)
                                        printf("%s=", name);

                                printf("%s%s", first ? "" : " ", str);

                                first = false;
                        }
                        if (r < 0)
                                return r;

                        if (first )
                                printf("%s=", name);
                        if (!first )
                                puts("");

                        r = sd_bus_message_exit_container(property);
                        if (r < 0)
                                return r;

                        return 1;

                } else {
                        printf("array unreadable");
                        return 0;
                }

                break;
        }

        return 0;
}
