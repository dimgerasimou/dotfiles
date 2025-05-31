#ifndef TOML_H
#define TOML_H

#include <stdio.h>

typedef struct TomlKeyVal TomlKeyVal;
typedef struct TomlArray TomlArray;


struct TomlKeyVal {
	const char *key;
	const char *val;
};

struct TomlArray {
	const char *key;

	TomlKeyVal **item;
	TomlArray **arr;

	size_t nitem;
	size_t narr;
};


#endif /* TOML_H */