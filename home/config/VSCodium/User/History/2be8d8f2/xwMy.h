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
	char *key;

	TomlKeyVal **item;
	TomlArray ***arr;
	TomlArray *parentarr;

	size_t nitem;
	size_t narr;
	size_t *nnarr;
};

TomlArray* tomlgetconfig(FILE *fp);


#endif /* TOML_H */