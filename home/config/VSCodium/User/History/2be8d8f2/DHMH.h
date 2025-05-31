#ifndef TOML_H
#define TOML_H

#include <stdio.h>

typedef struct {
	const char *key;
	const char *val;
} TomlKeyVal;

typedef struct{
	const char *key;
} TomlArray;


#endif /* TOML_H */