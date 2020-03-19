char * copyString(char * s)
{
  int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = (char*) malloc(n);
  if (t==NULL)
    printf("Out of memory");
  else strcpy(t,s);
  return t;
}
