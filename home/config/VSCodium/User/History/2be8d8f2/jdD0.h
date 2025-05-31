#ifndef TOML_H
#define TOML_H

#include <stdio.h>

typedef struct TomlKeyVal TomlKeyVal;
typedef struct TomlArray TomlArray;
typedef struct TomlArrayKey TomlArrayKey;

struct TomlKeyVal {
	char *key;
	char *val;
};

struct TomlArray {
	TomlArray *parentarr;

	TomlKeyVal **item;
	TomlArrayKey **key;

	size_t nitem;
	size_t nkey;
};

struct TomlArrayKey {
	char *key;

	TomlArray **arr;
	size_t narr;
};

TomlArrayKey* tomlgetarraykey(TomlArray *arr, const char *key);
TomlArray* tomlgetconfig(FILE *fp);
char* tomlgetstring(TomlArray *arr, const char *key);

#endif /* TOML_H */