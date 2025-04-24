/** @file lc3asm.cpp
 * @brief LC-3 Assembler
 *
 * @author Student Name
 * @note   cwid: 123456
 * @date   Spring 2024
 * @note   ide:  VS Code Editor / IDE ; g++ 8.2.0 / GNU Make 4.2.1
 *
 * Command line invocation of completed LC-3 Assembler
 */
#define _POSIX_C_SOURCE 2
#include "assembler.h"
#include <stdlib.h>
#include <unistd.h>

void usage()
{
  printf("usage: lc3asm [-v -o OUTFILE] FILE\n");
  printf("Assemble LC-3 program given in FILE");
  printf("\n");
  printf("Arguments:\n");
  printf("  -v            verbose output, show results of symbol table and assembly passes\n");
  printf("  -o OUTFILE    produce output to OUTFILE, instead of FILE.lc3\n");
  exit(1);
}

int main(int argc, char** argv)
{
  // parse command line arguments
  char* infile;
  char* outfile = NULL;
  bool verbose = false;
  char c;

  // check command line arguments/flags
  while ((c = getopt(argc, argv, "o:v")) != -1)
  {
    switch (c)
    {
    case 'v':
      verbose = true;
      break;
    case 'o':
      outfile = optarg;
      break;
    default:
      usage();
    }
  }

  // get required input file name
  if (optind != (argc - 1))
  {
    usage();
  }
  else
  {
    infile = argv[optind];
  }

  printf("verbose: %d\n", verbose);
  printf("input  file <%s>\n", infile);
  printf("output file <%s>\n", outfile);

  // assemble the file
  lc3asm(infile, outfile, verbose);

  return 0;
}
