Readme
======

This library parses command line arguments and config files using a 
python-like interface implemented in C. There isn't much documentation, but if
you look at the sample below, you'll see it's a fairly straight forward
interface.

Here is an example how to use it:

<pre><code>int main(int argc, char **argv){
  cfgparse_group_t cmd_group1 = {0};
  cfgparse_group_t cmd_group2 = {0};
  cfgparse_obj_t cmdobj = {0};
  cfgparse_obj_t cmdfileobj = {0};
  cfgparse_objlist_t cmdobjlist = {0};

  int a, ret, verb;
  float b;
  double c;
  char *d;

  /* create a group for general unix-y interface */
  cfgparseGroupCreate(&cmd_group1, "General");
  cfgparseAddFlag(&cmd_group1, CFGP_MASK_HELP, 'h', "help", "help");
  cfgparseAddFlag(&cmd_group1, CFGP_MASK_VERSION, 'V', "version", "version");
  cfgparseAddInt(&cmd_group1, CFGP_MODE_INCREMENT, 'v', NULL, &verb, 0, 
                 "increase verbosity");
  cfgparseAddInt(&cmd_group1, CFGP_MODE_DECREMENT, 'q', NULL, &verb, 0, 
                 "decrease verbosity");

  /* create a group to test data types. to suppress the empty space preceeding
   * this group in the help, use NULL instead of "" for the label */
  cfgparseGroupCreate(&cmd_group2, "");
  /* -a or --fA, requires argument, put value in variable a, default is -1 */
  cfgparseAddInt(&cmd_group2, CFGP_MODE_STORE, 'a', "fA", &a, -1, 
                 "a is an int");
  cfgparseAddFloat(&cmd_group2, CFGP_MODE_STORE, 'b', "fB", &b, -2.0, 
                   "b is a float");
  cfgparseAddDouble(&cmd_group2, CFGP_MODE_STORE, 'c', "fC", &c, -3.0,
                    "c is a double");
  cfgparseAddString(&cmd_group2, CFGP_MODE_STORE, 'd', "fD", &d, "-4",
                    "d is a string, yay strings!");

  /* create an object for reading the command line args using the two groups
   * given. this function can take as many arguments as you need, like printf,
   * just make sure you change the number preceeding the groups to match */
  cfgparseObjCreate(&cmdobj, NULL, &argc, &argv, 2, &cmd_group2, &cmd_group1);

  /* create an object to read file intest.txt using group 2 again 
   * remember, only long opts are used */
  cfgparseObjCreate(&cmdfileobj, "intest.txt", NULL, NULL, 1, &cmd_group2);
  
  /* cmdobj takes presidence since it is read last */
  cfgparseObjListCreate(&cmdobjlist, "./test_parse", "0.01", 
                        2, &cmdfileobj, &cmdobj);

  /* parse everything and exit gracefully if help or version was given */
  ret = cfgparseParse(&cmdobjlist);
  if(ret != 0){
    cfgparseObjListDestroy(&cmdobjlist);
    return (ret > 0) ? 0: -1;
  }

  cfgparseObjListDestroy(&cmdobjlist);
  return 0;
</code></pre>

License:
-------
This library is free as in beer :)

~Kris Maynard

