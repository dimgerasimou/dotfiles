#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "toml.h"

#define STR_SIZE 512

static TomlArray* appendarraykey(TomlArray *arr, const char *key);
static TomlArray* createarray(TomlArray *parent);
static TomlArrayKey* createarraykey(const char *key);
static TomlKeyVal *createkeyval(const char *key, const char *val);
static void deletearray(TomlArray *arr);
static void deletekeyval(TomlKeyVal *item);
static void deletearray(TomlArray *arr);
static TomlArray* findparent(const char *key, TomlArray *current);
static int parsebool(TomlKeyVal *item, unsigned int *ret, char *errstr);
static int parsedouble(TomlKeyVal *item, double *ret, char *errstr);
static int parsestring(TomlKeyVal *item, char **ret, char *errstr);
static int parseuint(TomlKeyVal *item, unsigned int *ret, char *errstr);

static TomlArray*
appendarraykey(TomlArray *arr, const char *key)
{
	TomlArrayKey *keyarr;

	// Check if the key already exists
	for (size_t i = 0; i < arr->nkey; i++) {
		keyarr = arr->key[i];
		if (!strcmp(key, keyarr->key)) {
			TomlArray **newarr;
			TomlArray *newarrelem;

			keyarr->narr++;
			if (!(newarr = realloc(keyarr->arr, keyarr->narr * sizeof(TomlArray*))))
				return NULL;
			keyarr->arr = newarr;
			
			if (!(newarrelem = createarray(arr)))
				return NULL;
			keyarr->arr[keyarr->narr - 1] = newarrelem;

			return keyarr->arr[keyarr->narr - 1];
		}
	}

	// Key does not exist, add a new key
	TomlArrayKey **newkeys;

	arr->nkey++;
	if (!(newkeys = realloc(arr->key, arr->nkey * sizeof(TomlArrayKey*))))
		return NULL;
	arr->key = newkeys;

	// Create a new array key
	if (!(keyarr = createarraykey(key)))
		return NULL;
	arr->key[arr->nkey - 1] = keyarr;

	// Initialize the new key's array
	keyarr->narr = 1;
	if (!(keyarr->arr = malloc(sizeof(TomlArray*))))
		return NULL;
	
	if (!(keyarr->arr[0] = createarray(arr)))
		return NULL;

	return keyarr->arr[0];
}

static TomlArray*
createarray(TomlArray *parent)
{
	TomlArray *ret;

	if (!(ret = malloc(sizeof(TomlArray))))
		return NULL;
	
	ret->parentarr = parent;
	ret->nitem = 0;
	ret->nkey = 0;
	ret->item = NULL;
	ret->key = NULL;

	return ret;
}

static TomlArrayKey*
createarraykey(const char *key)
{
	TomlArrayKey *ret;

	if (!(ret = malloc(sizeof(TomlArrayKey))))
		return NULL;
	
	ret->key = strdup(key);
	ret->narr = 0;
	ret->arr = NULL;

	return ret;
}

static TomlKeyVal*
createkeyval(const char *key, const char *val)
{
	TomlKeyVal *ret = NULL;

	ret = malloc(sizeof(TomlKeyVal));
	if (!ret)
		return NULL;

	ret->key = strdup(key);
	ret->val = strdup(val);
	return ret;
}

static void
deletearray(TomlArray *arr)
{
	if (!arr)
		return;

	for (size_t i = 0; i < arr->nitem; i++)
		deletekeyval(arr->item[i]);
	for (size_t i = 0; i < arr->nkey; i++) {
		for (size_t j = 0; j < arr->key[i]->narr; j++) {
			deletearray(arr->key[i]->arr[j]);
		}
	}
	if (arr->key)
		(free(arr->key));
	free(arr);
	arr = NULL;
}

static void
deletekeyval(TomlKeyVal *item)
{
	if (!item)
		return;
	if (item->key)
		free(item->key);
	if (item->val)
		free(item->val);
	free(item);
	item = NULL;
}

static TomlArray*
findparent(const char *key, TomlArray *arr)
{
	TomlArray *ptr;

	if (!arr->parentarr)
		return arr;

	for (ptr = arr->parentarr; ptr != NULL; ptr = ptr->parentarr) {
		for (size_t i = 0; i < ptr->nkey; i++) {
			if (!strcmp(ptr->key[i]->key, key))
				return ptr;
		}
	}

	return arr;
}

static int
parsebool(TomlKeyVal *item, unsigned int *ret, char *errstr)
{
	if (!strcmp(item->val, "true") || !strcmp(item->val, "True")) {
		*ret = 1;
		return 0;
	}

	if (!strcmp(item->val, "false") || !strcmp(item->val, "False")) {
		*ret = 0;
		return 0;
	}

	char str[STR_SIZE];

	snprintf(str, sizeof(str) - 1, "Failed to parse bool - key:%s - val:%s", item->key, item->val);
	errstr = strdup(str);
	return 1;
}

static int
parsedouble(TomlKeyVal *item, double *ret, char *errstr)
{
	char *ptr;
	unsigned int sep = 0;

	for (ptr = (char*) item->val; *ptr != '\0'; ptr++) {
		if (*ptr <= '9' && *ptr >= '0')
			continue;
		if (*ptr == '.') {
			sep++;
			if (ptr != (char*) item->val && sep <= 1
			    && (*(ptr + 1) <= '9' && *(ptr + 1) >= '0')
			    && (*(ptr - 1) <= '9' && *(ptr - 1) >= '0'))
				continue;
		}
		if (*ptr == '-' && ptr == (char*) item->val)
			continue;

		char str[STR_SIZE];

		snprintf(str, sizeof(str) - 1, "Failed to parse double - key:%s - val:%s", item->key, item->val);
		errstr = strdup(str);
		return 1;
	}

	sscanf(item->val, "%lf", ret);
	return 0;
}

static int
parsestring(TomlKeyVal *item, char **ret, char *errstr)
{
	char *ptr;
	char *temp;

	temp = strdup(item->val);
	ptr = strchr(temp, '\0');

	if (*(--ptr) == '"') {
		*ptr = '\0';
		if (*temp == '"') {
			*ret = strdup(temp + 1);
			return 0;
		}
	}

	char str[STR_SIZE];

	snprintf(str, sizeof(str) - 1, "Failed to parse string - key:%s - val:%s", item->key, item->val);
	errstr = strdup(str);
	return 1;
}

static int
parseuint(TomlKeyVal *item, unsigned int *ret, char *errstr)
{
	char *ptr;

	for (ptr = (char*) item->val; *ptr != '\0'; ptr++) {
		if (*ptr > '9' || *ptr < '0') {
			char str[STR_SIZE];

			snprintf(str, sizeof(str) - 1, "Failed to parse uint - key:%s - val:%s", item->key, item->val);
			errstr = strdup(str);
			return 1;
		}
	}

	sscanf(item->val, "%u", ret);
	return 0;
}

void trimwhitespace(char *str) {
	char *end;
	char *original = str;

	while(isspace((unsigned char)*str)) str++;

	if(*str == 0) {
		*original = '\0';
		return;
	}

	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end)) end--;

	*(end+1) = '\0';

	if (str != original) {
		memmove(original, str, end - str + 2); // +2 to include the null terminator
	}
}

static int
addkeyval(const char *str, TomlArray *arr)
{
	char key[1024];
	char val[1024];
	char *ptr;

	if (!(ptr = strchr(str, '=')))
		return 1;

	strcpy(val, (ptr + 1));
	strncpy(key, str, ptr - str + 1);
	key[ptr - str] = '\0';

	arr->nitem++;
	arr->item = realloc(arr->item, arr->nitem * sizeof(TomlKeyVal));
	if (!arr->item)
		return 1;

	arr->item[arr->nitem - 1] = createkeyval(key, val);
	return 0;
}

TomlArray*
tomlgetconfig(FILE *fp)
{
	char buf[1024];
	char *ptr;
	TomlArray *baseptr;
	TomlArray *arr;

	arr = createarray(NULL);
	baseptr = arr;

	while (fgets(buf, sizeof(buf), fp)) {
		trimwhitespace(buf);

		if (buf[0] == '#' || buf[0] == '\0')
			continue;
		
		ptr = strchr(buf, '\0');


		if (*(buf) == '[' && *(buf + 1) == '[' && *(ptr - 1) == ']' && *(ptr - 2) == ']') {
			*(ptr - 2) = '\0';
			ptr = buf + 2;
			arr = findparent(ptr, arr);
			if (!(arr = appendarraykey(arr, ptr))) {
				deletearray(arr);
				return NULL;
			}
			continue;
		}
		if (addkeyval(buf, arr)) {
			deletearray(arr);
			return NULL;
		}
	}

	return baseptr;
}

int
main(void)
{
	FILE *fp;

	fp = fopen("/home/dimgerasimou/.config/xrandr-setup/xrandr-setup.toml", "r");
	if (!fp)
		return 69;

	TomlArray *main = tomlgetconfig(fp);
	TomlArray *screen;
	TomlArray *monitor;
	
	fclose(fp);
	for (size_t i = 0; i < main->key[0]->narr; i++) {
		screen = main->key[0]->arr[i];
		printf("%zu - %s nitem:%zu\n", i, main->key[0]->key, screen->nitem);
		for (size_t j = 0; j < screen->key[0]->narr; j++) {
			monitor = screen->key[0]->arr[j];
			printf("\t%zu - %s nitem:%zu\n", j, screen->key[0]->key, monitor->nitem);
		}
		
	}

	deletearray(main);
	return 0;
}