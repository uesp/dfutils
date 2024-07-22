main(int ac, char **av)
{
        int i;
        for(i=1; i < ac; i++)
                printf("%s %#x\n", av[i], hash(av[i]));
}
 
hash(char *ptr)
{
  long int val = 0;

  while(*ptr)
    val = (val<<1) + *ptr++;

  return val;
}
