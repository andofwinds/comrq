void cwait(unsigned int secs) {
  unsigned int reton = time(0) + secs;
  while (time(0) < reton)
    ;
}

size_t find_index(int array[], size_t size, int value)
{
  size_t index = 0;

  while (index < size && array[index] != value) index++;

  return (index == size ? -1 : index);
}
