#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* See resource_table definition in:

https://git.ti.com/cgit/pru-software-support-package/pru-software-support-package/tree/labs/Getting_Started_Labs/c_code/solution/am572x/resource_table_empty.h 
https://git.ti.com/cgit/pru-software-support-package/pru-software-support-package/tree/include/rsc_types.h

*/

struct my_resource_table {
  uint32_t ver;
  uint32_t num;
  uint32_t reserved[2];
  uint32_t offset[1];
};

int main(int argc, char** argv) {
  if(argc < 2) {
    printf("Must include output file name.\n");
    exit(1);
  }

  char* filename = argv[1];
  FILE *f; 

  f = fopen(filename, "wb");

  struct my_resource_table table =  { 1, 0, 0, 0 };

  fwrite(&table, sizeof(struct my_resource_table), 1, f);
}
