#ifndef TOML_H
#define TOML_H

#include <stdio.h>

typedef struct TomlKeyVal;
typedef struct TomlArray;


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