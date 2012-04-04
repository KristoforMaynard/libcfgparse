/*
 * configparse.c
 *      this is version 2.0 of configParse
 *
 *  Created on: Jul 12, 2009
 *      Author: maynard
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "configparse.h"

extern int verb;

/* these functions should not be used outside this */
/* source file, they are called by functions       */
/* that are exposed in configparse.h               */
int configParseReadFile(configList_t *list, const char *fname);
void configParseWriteFile(configList_t *list, const char *fname);

configNodeInt_t* configParseFindInt(configList_t *list, char *skey);
configNodeFloat_t* configParseFindFloat(configList_t *list, char *skey);
configNodeDouble_t* configParseFindDouble(configList_t *list, char *skey);
configNodeString_t* configParseFindString(configList_t *list, char *skey);


/****************************************************************/

configList_t* configParseInitList(char *name){
	configList_t *list;
	list = malloc(sizeof(configList_t));
	list->first_int=NULL; list->last_int=NULL;
	list->first_float=NULL; list->last_float=NULL;
	list->first_double=NULL; list->last_double=NULL;
	list->first_string=NULL; list->last_string=NULL;
	list->first_comment=NULL; list->last_comment=NULL;
	list->total_entries = 0;
	list->name = name;
	return list;
}

configList_t* configParseFreeList(configList_t *list){
	configNodeInt_t *tmp_inode;
	configNodeFloat_t *tmp_fnode;
	configNodeDouble_t *tmp_dnode;
	configNodeString_t *tmp_snode;
	configNodeComment_t *tmp_cnode;

	while(list->first_int != NULL){
		tmp_inode = list->first_int->next;
		free(list->first_int);
		list->first_int = tmp_inode;
	}
	list->last_int = list->first_int;

	while(list->first_float != NULL){
		tmp_fnode = list->first_float->next;
		free(list->first_float);
		list->first_float = tmp_fnode;
	}
	list->last_float = list->first_float;

	while(list->first_double != NULL){
		tmp_dnode = list->first_double->next;
		free(list->first_double);
		list->first_double = tmp_dnode;
	}
	list->last_double = list->first_double;

	while(list->first_string != NULL){
		tmp_snode = list->first_string->next;
		if(!list->first_string->is_default && *list->first_string->value != NULL){
			free(*list->first_string->value);
			*list->first_string->value = NULL;
		}
		free(list->first_string);
		list->first_string = tmp_snode;
	}
	list->last_string = list->first_string;

	while(list->first_comment != NULL){
		tmp_cnode = list->first_comment->next;
		free(list->first_comment);
		list->first_comment = tmp_cnode;
	}
	list->last_comment = list->first_comment;

	free(list);
	list = NULL;
	return list;
}

/* resets values to their defaults                        */
/* reset_locks is true/false on whether you want          */
/* all values reset(even locked ones) and all locks       */
/* reset to CP_FALSE 								      */
/* else, locked values will remain unchanged by the reset */
void configParseReset(configList_t *list, int reset_locks){
	configNodeInt_t *cur_int;
	configNodeFloat_t *cur_float;
	configNodeDouble_t *cur_double;
	configNodeString_t *cur_string;

	cur_int = list->first_int;
	while(cur_int != NULL){
		if(reset_locks || !cur_int->lock){
			*cur_int->value = cur_int->def_val;
			if(reset_locks)
				cur_int->lock = CP_FALSE;
		}
		cur_int = cur_int->next;
	}

	cur_float = list->first_float;
	while(cur_float != NULL){
		if(reset_locks || !cur_float->lock){
			*cur_float->value = cur_float->def_val;
			if(reset_locks)
				cur_float->lock = CP_FALSE;
		}
		cur_float = cur_float->next;
	}

	cur_double = list->first_double;
	while(cur_double != NULL){
		if(reset_locks || !cur_double->lock){
			*cur_double->value = cur_double->def_val;
			if(reset_locks)
				cur_double->lock = CP_FALSE;
		}
		cur_double = cur_double->next;
	}

	cur_string = list->first_string;
	while(cur_string != NULL){
		if(reset_locks || !cur_string->lock){
			if(!cur_string->is_default){
				if(*cur_string->value != NULL){
					free(*cur_string->value);
					*cur_string->value = NULL;
				}
				*cur_string->value = cur_string->def_val;
				cur_string->is_default = 1;
			}
			if(reset_locks)
				cur_string->lock = CP_FALSE;
		}
		cur_string = cur_string->next;
	}
}

/* Do the file processing once all variables have been added to list */
/* returns 0 if config file was read                                 */
/* returns 1 if config file was written and defaults are in use      */
int configParseExecute(configList_t *list, const char *fname){
	if(configParseReadFile(list, fname) == 0){
		return 0;
	} else{
		configParseWriteFile(list, fname);
		return 1;
	}
}

/* returns 0 on success        */
/* returns 1 on file not exist */
int configParseReadFile(configList_t *list, const char *fname){
	FILE *file;

	char line[CONF_LINE_LEN];
	char key[CONF_LINE_LEN];
	char *nocommentline;
	char *comment_char;
	char *value_str;
	int value_length;
	int found;
	int line_num;

	int read_int;
	configNodeFloat_t *cur_float;
	configNodeDouble_t *cur_double;
	char *tmp_string;

	line_num = 0;
	cur_float = NULL;
	cur_double = NULL;
	tmp_string = NULL;

	if((file = fopen(fname, "r"))){
		while(fgets(line, CONF_LINE_LEN, file)){
			line_num += 1;
			found = -1;

			/*printf("line is:%s", line);*/

			/* this block kills comments, preceding whitespace, and the whole line */
			/* if it's empty after the comments are gone                           */
			comment_char = strchr(line, COMMENT_CHAR);
			if(comment_char != NULL)
				comment_char[0] = '\0';
			nocommentline = line;
			while(nocommentline[0] == ' ' || nocommentline[0] == '\t')
				nocommentline += 1;
			if(strncmp(line, "", CONF_LINE_LEN) == 0){
				continue;
			}

			sscanf(line, "%s = %*s\n", key);
			value_str = strchr(line, '=') + 1;
			/* there is a problem if there is no = in this line */
			if(value_str == NULL){
				fprintf(stderr, "Config Parse Error: No "
						"value on line %d of file %s.\n", line_num, fname);
				continue;
			}
			/* strip white space from beginning of value_str */
			while(value_str[0] == ' ' || value_str[0] == '\t')
				value_str += 1;
			value_length = strlen(value_str) + 1;

			/* find the key in list of integers */
			sscanf(value_str, "%d", &read_int);
			found = configParseSetInt(list, key, read_int, CP_FALSE);

			/* find the key in list of floats */
			if(found == -1){
				cur_float = configParseFindFloat(list, key);
				if(cur_float != NULL){
					if(!cur_float->lock){
						if(cur_float->sci_note){
							sscanf(value_str, "%e", cur_float->value);
						} else{
							sscanf(value_str, "%f", cur_float->value);
						}
					}
					found = 1;
				}
			}

			/* find the key in list of doubles */
			if(found == -1){
				cur_double = configParseFindDouble(list, key);
				if(cur_double != NULL){
					if(!cur_double->lock){
						if(cur_double->sci_note){
							sscanf(value_str, "%le", cur_double->value);
						} else{
							sscanf(value_str, "%lf", cur_double->value);
						}
					}
					found = 1;
				}
			}

			/* find the key in list of strings */
			if(found == -1){
				/* kill a newline if it's in there */
				if(value_str[strlen(value_str) - 1] == '\n'){
					value_str[strlen(value_str) - 1] = '\0';
				}
				tmp_string = calloc((strlen(value_str) + 1) , sizeof(char));
				strncpy(tmp_string, value_str, strlen(value_str));
				found = configParseSetString(list, key, tmp_string, CP_FALSE);
				if(found != 0){
					free(tmp_string);
				}
			}

			if(found == -1){
				fprintf(stderr, "Config Parse Error: key not found "
						"in file %s, line %d\n", fname, line_num);
			}
		}
		fclose(file);
		return 0;
	}
	return 1;
}

void configParseWriteFile(configList_t *list, const char *fname){
	FILE *file;
	int cur_entry;
	configNodeInt_t *cur_int;
	configNodeFloat_t *cur_float;
	configNodeDouble_t *cur_double;
	configNodeString_t *cur_string;
	configNodeComment_t *cur_comment;

	file = fopen(fname, "w");
	cur_entry = 0;
	cur_int = list->first_int;
	cur_float = list->first_float;
	cur_double = list->first_double;
	cur_string = list->first_string;
	cur_comment = list->first_comment;

	while(cur_entry < list->total_entries){
		if(cur_int && cur_int->num == cur_entry){
			fprintf(file, "%s = %d", cur_int->key, *(cur_int->value));
			if(strncmp(cur_int->comment, "", strlen(cur_int->comment)) == 0)
				fprintf(file, "\n");
			else
				fprintf(file, " %s %s\n", COMMENT_STR, cur_int->comment);
			cur_int = cur_int->next;
		}else if(cur_float && cur_float->num == cur_entry){
			if(cur_float->sci_note){
				fprintf(file, "%s = %e", cur_float->key, *(cur_float->value));
			} else{
				fprintf(file, "%s = %f", cur_float->key, *(cur_float->value));
			}
			if(strncmp(cur_float->comment, "", strlen(cur_float->comment)) == 0)
				fprintf(file, "\n");
			else
				fprintf(file, " %s %s\n", COMMENT_STR, cur_float->comment);
			cur_float = cur_float->next;
		}else if(cur_double && cur_double->num == cur_entry){
			if(cur_double->sci_note){
				fprintf(file, "%s = %e", cur_double->key, *(cur_double->value));
			} else{
				fprintf(file, "%s = %f", cur_double->key, *(cur_double->value));
			}
			if(strncmp(cur_double->comment, "", strlen(cur_double->comment)) == 0)
				fprintf(file, "\n");
			else
				fprintf(file, " %s %s\n", COMMENT_STR, cur_double->comment);
			cur_double = cur_double->next;
		}else if(cur_string && cur_string->num == cur_entry){
			fprintf(file, "%s = %s", cur_string->key, *(cur_string->value));
			if(strncmp(cur_string->comment, "", 1) == 0)
				fprintf(file, "\n");
			else
				fprintf(file, " %s %s\n", COMMENT_STR, cur_string->comment);
			cur_string = cur_string->next;
		}else if(cur_comment && cur_comment->num == cur_entry){
			fprintf(file, "%s %s\n", COMMENT_STR, cur_comment->value);
			cur_comment = cur_comment->next;
		}else{
			fprintf(stderr, "this is odd...\n");
		}
		cur_entry++;
	}

	fclose(file);
	return;
}

/* Print all the config variables to the screen with thier associated */
/* help blurbs, if any are supplied                                   */
void configParsePrintHelp(configList_t *list){
	int cur_entry;
	configNodeInt_t *cur_int;
	configNodeFloat_t *cur_float;
	configNodeDouble_t *cur_double;
	configNodeString_t *cur_string;
	configNodeComment_t *cur_comment;

	cur_entry = 0;
	cur_int = list->first_int;
	cur_float = list->first_float;
	cur_double = list->first_double;
	cur_string = list->first_string;
	cur_comment = list->first_comment;

	fprintf(stdout, "Config file help for: %s\n", list->name );
	fprintf(stdout, "\tKey\t\t   Info\n");
	fprintf(stdout, "\t---\t\t   ----\n");
	if(list->total_entries == 0)
		fprintf(stdout, "\tNONE\t\t   NONE\n");

	while(cur_entry < list->total_entries){
		if(cur_int && cur_int->num == cur_entry){
			fprintf(stdout, "\t%s\t\t  %s\n", cur_int->key, cur_int->help);
			cur_int = cur_int->next;
		}else if(cur_float && cur_float->num == cur_entry){
			fprintf(stdout, "\t%s\t\t  %s\n", cur_float->key, cur_float->help);
			cur_float = cur_float->next;
		}else if(cur_double && cur_double->num == cur_entry){
			fprintf(stdout, "\t%s\t\t  %s\n", cur_double->key, cur_double->help);
			cur_double = cur_double->next;
		}else if(cur_string && cur_string->num == cur_entry){
			fprintf(stdout, "\t%s\t\t  %s\n", cur_string->key, cur_string->help);
			cur_string = cur_string->next;
		}else if(cur_comment && cur_comment->num == cur_entry){
			fprintf(stdout, "\t%s %s\n", COMMENT_STR, cur_comment->value);
			cur_comment = cur_comment->next;
		}else{
			fprintf(stderr, "Internal Config Parse Error #1.\n");
		}
		cur_entry++;
	}
	fprintf(stdout, "\n");
	return;
}

/* search list of ints and return node that matches key */
/* return NULL on not found                             */
configNodeInt_t* configParseFindInt(configList_t *list, char *skey){
	configNodeInt_t *cur_node;
	cur_node = list->first_int;
	while(cur_node != NULL){
		if(strncmp(skey, cur_node->key, CONF_LINE_LEN) == 0){
			return cur_node;
		}
		cur_node = cur_node->next;
	}
	return NULL;
}

/* search list of floats and return node that matches key */
/* return NULL on not found                             */
configNodeFloat_t* configParseFindFloat(configList_t *list, char *skey){
	configNodeFloat_t *cur_node;
	cur_node = list->first_float;
	while(cur_node != NULL){
		if(strncmp(skey, cur_node->key, CONF_LINE_LEN) == 0){
			return cur_node;
		}
		cur_node = cur_node->next;
	}
	return NULL;
}

/* search list of doubles and return node that matches key */
/* return NULL on not found                             */
configNodeDouble_t* configParseFindDouble(configList_t *list, char *skey){
	configNodeDouble_t *cur_node;
	cur_node = list->first_double;
	while(cur_node != NULL){
		if(strncmp(skey, cur_node->key, CONF_LINE_LEN) == 0){
			return cur_node;
		}
		cur_node = cur_node->next;
	}
	return NULL;
}

/* search list of strings and return node that matches key */
/* return NULL on not found                             */
configNodeString_t* configParseFindString(configList_t *list, char *skey){
	configNodeString_t *cur_node;
	cur_node = list->first_string;
	while(cur_node != NULL){
		if(strncmp(skey, cur_node->key, CONF_LINE_LEN) == 0){
			return cur_node;
		}
		cur_node = cur_node->next;
	}
	return NULL;
}

/* add an integer to a given configList              */
/* The order of values in a file are given by        */
/* the order that they are added to the list.        */
/* Also, the config node points to the destination   */
/* always, this is how the destination value changes */
void configParseAddInt(configList_t *list, char *key,
							 int *destination, int default_val){
	/*return*/
	configParseAddIntWithHelp(list, key, destination,
								      default_val, "", "");
}
void configParseAddIntWithHelp(configList_t *list, char *key,
							 int *destination, int default_val,
							 char *comment, char *help){
	configNodeInt_t *newnode;
	newnode = malloc(sizeof(configNodeInt_t));
	newnode->key = key;
	newnode->num = list->total_entries;
	newnode->value = destination;
	newnode->next = NULL;
	newnode->def_val = default_val;
	newnode->comment = comment;
	newnode->help = help;
	newnode->lock = CP_FALSE;
	*(newnode->value) = newnode->def_val;

	if(list->last_int != NULL){
		list->last_int->next = newnode;
		list->last_int = list->last_int->next;
	} else{
		list->first_int = newnode;
		list->last_int = list->first_int;
	}
	list->total_entries++;
	return;
}

void configParseAddFloat(configList_t *list, char *key, float *destination,
							     float default_val, int use_sci_note){
	/*return*/
	configParseAddFloatWithHelp(list, key, destination, default_val,
										use_sci_note, "", "");
}
void configParseAddFloatWithHelp(configList_t *list, char *key, float *destination,
							     float default_val, int use_sci_note,
								 char *comment, char *help){
	configNodeFloat_t *newnode;
	newnode = malloc(sizeof(configNodeFloat_t));
	newnode->key = key;
	newnode->num = list->total_entries;
	newnode->value = destination;
	newnode->next = NULL;
	newnode->def_val = default_val;
	newnode->comment = comment;
	newnode->help = help;
	newnode->lock = CP_FALSE;
	*(newnode->value) = newnode->def_val;
	newnode->sci_note = use_sci_note;

	if(list->last_float != NULL){
		list->last_float->next = newnode;
		list->last_float = list->last_float->next;
	} else{
		list->first_float = newnode;
		list->last_float = list->first_float;
	}
	list->total_entries++;
	return;
}

void configParseAddDouble(configList_t *list, char *key, double *destination,
							     double default_val, int use_sci_note){
	/*return*/
	configParseAddDoubleWithHelp(list, key, destination, default_val,
							             use_sci_note, "", "");
}
void configParseAddDoubleWithHelp(configList_t *list, char *key, double *destination,
							     double default_val, int use_sci_note,
								 char *comment, char *help){
	configNodeDouble_t *newnode;
	newnode = malloc(sizeof(configNodeDouble_t));
	newnode->key = key;
	newnode->num = list->total_entries;
	newnode->value = destination;
	newnode->next = NULL;
	newnode->def_val = default_val;
	newnode->comment = comment;
	newnode->help = help;
	newnode->lock = CP_FALSE;
	*(newnode->value) = newnode->def_val;
	newnode->sci_note = use_sci_note;

	if(list->last_double != NULL){
		list->last_double->next = newnode;
		list->last_double = list->last_double->next;
	} else{
		list->first_double = newnode;
		list->last_double = list->first_double;
	}
	list->total_entries++;
	return;
}

/*NOTE: the destination of a string should NEVER be changed directly */
/*      it should only change as a result of reading a config file   */
/*      or a call to configParseReset(), else segfaults and memory   */
/*      leaks will happen in a most annoying fashion                 */
void configParseAddString(configList_t *list, char *key,
							 char **destination, char *default_val){
	/*return*/
	configParseAddStringWithHelp(list, key, destination,
								         default_val, "", "");
}
void configParseAddStringWithHelp(configList_t *list, char *key,
							 char **destination, char *default_val,
							 char *comment, char *help){
	configNodeString_t *newnode;
	newnode = malloc(sizeof(configNodeString_t));
	newnode->key = key;
	newnode->num = list->total_entries;
	newnode->value = destination;
	newnode->next = NULL;
	newnode->def_val = default_val;
	newnode->comment = comment;
	newnode->help = help;
	newnode->lock = CP_FALSE;
	*(newnode->value) = newnode->def_val;
	newnode->is_default = 1;

	if(list->last_string != NULL){
		list->last_string->next = newnode;
		list->last_string = list->last_string->next;
	} else{
		list->first_string = newnode;
		list->last_string = list->first_string;
	}
	list->total_entries++;
	return;
}

void configParseAddComment(configList_t *list, const char *text){
	configNodeComment_t *newnode;
	newnode = calloc(1, sizeof(configNodeComment_t));
	newnode->value = text;
	newnode->num = list->total_entries;

	if(list->last_comment != NULL){
		list->last_comment->next = newnode;
		list->last_comment = list->last_comment->next;
	} else{
		list->first_comment = newnode;
		list->last_comment = list->first_comment;
	}
	list->total_entries++;
	return;
}

/***************************************************************************/
int configParseSetInt(configList_t *list, char *skey, int value, int force){
	configNodeInt_t* node = configParseFindInt(list, skey);
	if(node != NULL){
		if(force || (!node->lock)){
			*(node->value) = value;
			return 0;
		}else{
			return 1;
		}
	}else{
		return -1;
	}
}
int configParseLockInt(configList_t *list, char *skey){
	configNodeInt_t* node = configParseFindInt(list, skey);
	if(node != NULL){
		node->lock = CP_TRUE;
		return 0;
	}else
		return -1; /*key not found*/
}
int configParseUnockInt(configList_t *list, char *skey){
	configNodeInt_t* node = configParseFindInt(list, skey);
	if(node != NULL){
		node->lock = CP_FALSE;
		return 0;
	}else
		return -1; /*key not found*/
}
int configParseLockIntValue(configList_t *list, char *skey, int value){
	int retval = configParseSetInt(list, skey, value, CP_TRUE); /*force overwrite of previous lock*/
	if(retval >= 0)
		retval = configParseLockInt(list, skey);
	return retval;
}

/*******************************************************************************/
int configParseSetFloat(configList_t *list, char *skey, float value, int force){
	configNodeFloat_t* node = configParseFindFloat(list, skey);
	if(node != NULL){
		if(force || (!node->lock)){
			*(node->value) = value;
			return 0;
		}else{
			return 1;
		}
	}else{
		return -1;
	}
}
int configParseLockFloat(configList_t *list, char *skey){
	configNodeFloat_t* node = configParseFindFloat(list, skey);
	if(node != NULL){
		node->lock = CP_TRUE;
		return 0;
	}else
		return -1; /*key not found*/
}
int configParseUnockFloat(configList_t *list, char *skey){
	configNodeFloat_t* node = configParseFindFloat(list, skey);
	if(node != NULL){
		node->lock = CP_FALSE;
		return 0;
	}else
		return -1; /*key not found*/
}

int configParseLockFloatValue(configList_t *list, char *skey, float value){
	int retval = configParseSetFloat(list, skey, value, CP_TRUE); /*force overwrite of previous lock*/
	if(retval >= 0)
		retval = configParseLockFloat(list, skey);
	return retval;
}

/*********************************************************************************/
int configParseSetDouble(configList_t *list, char *skey, double value, int force){
	configNodeDouble_t* node = configParseFindDouble(list, skey);
	if(node != NULL){
		if(force || (!node->lock)){
			*(node->value) = value;
			return 0;
		}else{
			return 1;
		}
	}else{
		return -1;
	}
}
int configParseLockDouble(configList_t *list, char *skey){
	configNodeDouble_t* node = configParseFindDouble(list, skey);
	if(node != NULL){
		node->lock = CP_TRUE;
		return 0;
	}else
		return -1; /*key not found*/
}
int configParseUnockDouble(configList_t *list, char *skey){
	configNodeDouble_t* node = configParseFindDouble(list, skey);
	if(node != NULL){
		node->lock = CP_FALSE;
		return 0;
	}else
		return -1; /*key not found*/
}
int configParseLockDoubleValue(configList_t *list, char *skey, double value){
	int retval = configParseSetDouble(list, skey, value, CP_TRUE); /*force overwrite of previous lock*/
	if(retval >= 0)
		retval = configParseLockDouble(list, skey);
	return retval;
}

/********************************************************************************/
/* set string assumes a value that has already been allocated */
int configParseSetString(configList_t *list, char *skey, char *value, int force){
	configNodeString_t* node = configParseFindString(list, skey);
	if(node != NULL){
		if(force || (!node->lock)){
			if((!node->is_default) && (*node->value != NULL)){
				free(*(node->value));
			}
			*(node->value) = value;
			node->is_default = CP_FALSE;
			return 0;
		}else{
			return 1;
		}
	}else{
		return -1;
	}
}
int configParseLockString(configList_t *list, char *skey){
	configNodeString_t* node = configParseFindString(list, skey);
	if(node != NULL){
		node->lock = CP_TRUE;
		return 0;
	}else
		return -1; /*key not found*/
}
int configParseUnockString(configList_t *list, char *skey){
	configNodeString_t* node = configParseFindString(list, skey);
	if(node != NULL){
		node->lock = CP_FALSE;
		return 0;
	}else
		return -1; /*key not found*/
}
int configParseLockStringValue(configList_t *list, char *skey, char* value){
	int retval = configParseSetString(list, skey, value, CP_TRUE); /*force overwrite of previous lock*/
	if(retval >= 0)
		retval = configParseLockString(list, skey);
	return retval;
}

/*
 * EOF
 */
