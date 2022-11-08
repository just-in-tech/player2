#ifndef JSONGEN_H_INCLUDED

#define JSONGEN_H_INCLUDED

int json_start(char *json);
int json_write_int64(char *json,char const* key,long long number);
int json_write_string(char *json,char const* key,char * contents);
int json_write_int(char *json, char const* key, int number);
int json_finish(char *json);

#endif // JSON-GEN_H_INCLUDED
