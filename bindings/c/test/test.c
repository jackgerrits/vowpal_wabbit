#include <stdio.h>

#include "vw/experimental/example.h"
#include "vw/experimental/io.h"

int main()
{
  vw_feature_space* fs;
  allocate_feature_space(&fs);
  feature_space_push_feature(fs, 10, 5.f);
  uint64_t* indices;
  float* vals;
  int len;
  feature_space_get_features(fs, &indices, &vals, &len);
  for (int i = 0; i < len; i++)
  {
    printf("%ld %f\n", indices[i], vals[i]);
  }
  deallocate_feature_space(fs);
  return 0;
}