/* minimal code example showing how to call the zfp (de)compressor */

#include "zfp.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* compress or decompress array */
static int compress(double *array, size_t nx, double tolerance,
                    zfp_bool decompress) {
  int status = 0;    /* return value: 0 = success */
  zfp_type type;     /* array scalar type */
  zfp_field *field;  /* array meta data */
  zfp_stream *zfp;   /* compressed stream */
  void *buffer;      /* storage for compressed stream */
  size_t bufsize;    /* byte size of compressed buffer */
  bitstream *stream; /* bit stream to write to or read from */
  size_t zfpsize;    /* byte size of compressed stream */

  /* allocate meta data for the 1D array a[nx] */
  type = zfp_type_double;
  field = zfp_field_1d(array, type, nx);

  /* allocate meta data for a compressed stream */
  zfp = zfp_stream_open(NULL);

  /* set compression mode and parameters via one of four functions */
  /*  zfp_stream_set_reversible(zfp); */
  /*  zfp_stream_set_rate(zfp, rate, type, zfp_field_dimensionality(field),
   * zfp_false); */
  /*  zfp_stream_set_precision(zfp, precision); */
  zfp_stream_set_accuracy(zfp, tolerance);

  /* allocate buffer for compressed data */
  bufsize = zfp_stream_maximum_size(zfp, field);
  buffer = malloc(bufsize);

  /* associate bit stream with allocated buffer */
  stream = stream_open(buffer, bufsize);
  zfp_stream_set_bit_stream(zfp, stream);
  zfp_stream_rewind(zfp);

  /* compress or decompress entire array */
  if (decompress) {
    /* read compressed stream and decompress and output array */
    FILE *fp = fopen("doubles.dat.zfp", "rb");
    zfpsize = fread(buffer, 1, bufsize, fp);
    fclose(fp);
    if (!zfp_decompress(zfp, field)) {
      fprintf(stderr, "decompression failed\n");
      status = EXIT_FAILURE;
    } else {
      FILE *fp = fopen("decompressed_doubles.dat", "wb");
      fwrite(array, sizeof(double), zfp_field_size(field, NULL), fp);
      fclose(fp);
    }
  } else {
    /* compress array and output compressed stream */
    zfpsize = zfp_compress(zfp, field);
    if (!zfpsize) {
      fprintf(stderr, "compression failed\n");
      status = EXIT_FAILURE;
    } else {
      FILE *fp = fopen("doubles.dat.zfp", "wb");
      fwrite(buffer, 1, zfpsize, fp);
      fclose(fp);
    }
  }

  /* clean up */
  zfp_field_free(field);
  zfp_stream_close(zfp);
  stream_close(stream);
  free(buffer);
  free(array);

  return status;
}

int main(int argc, char *argv[]) {
  /* use -d to decompress rather than compress data */
  zfp_bool decompress = (argc == 2 && !strcmp(argv[1], "-d"));

  /* allocate array of doubles */
  size_t nx = 1562;
  double *array = malloc(nx * sizeof(double));

  /* read doubles from file */
  FILE *fp = fopen("../doubles.dat", "rb");
  if (!fp) {
    perror("Error opening file");
    exit(1);
  }

  size_t doubles_read = fread(array, sizeof(double), nx, fp);
  if (doubles_read != nx) {
    if (feof(fp))
      printf("Reached end of file, read %zu elements\n", doubles_read);
    else
      perror("Error reading file");
  }

  fclose(fp);

  /* compress or decompress array */
  return compress(array, nx, 1e-1, decompress);
}
