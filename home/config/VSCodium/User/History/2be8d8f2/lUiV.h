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
	TomlArray *parentarr;

	TomlKeyVal **item;
	TomlArrayKey **arr;

	size_t nitem;
	size_t narr;
};

TomlArray* tomlgetconfig(FILE *fp);


#endif /* TOML_H */