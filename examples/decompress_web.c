#include "zfp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <emscripten.h>

/* Hacky way to expose malloc, for now */
EMSCRIPTEN_KEEPALIVE
void *exported_malloc(size_t size) { return malloc(size); }

EMSCRIPTEN_KEEPALIVE
/* Decompress array */
double *decompress(char *compressed, size_t compressed_size, size_t nx) {
  zfp_type type;     /* array scalar type */
  zfp_field *field;  /* array meta data */
  zfp_stream *zfp;   /* compressed stream */
  void *buffer;      /* storage for compressed stream */
  size_t bufsize;    /* byte size of compressed buffer */
  bitstream *stream; /* bit stream to write to or read from */
  size_t zfpsize;    /* byte size of compressed stream */
  double tolerance = 1e-1;

  /* Allocate buffer for decompressed doubles */
  double *decompressed = calloc(nx, sizeof(double));

  /* allocate meta data for the 1D array a[nx] */
  type = zfp_type_double;
  field = zfp_field_1d(decompressed, type, nx);

  /* allocate meta data for a compressed stream */
  zfp = zfp_stream_open(NULL);

  /* set compression mode and parameters via one of four functions */
  /*  zfp_stream_set_reversible(zfp); */
  /*  zfp_stream_set_rate(zfp, rate, type, zfp_field_dimensionality(field),
   * zfp_false); */
  /*  zfp_stream_set_precision(zfp, precision); */
  zfp_stream_set_accuracy(zfp, tolerance);

  // /* allocate buffer for compressed data */
  // bufsize = zfp_stream_maximum_size(zfp, field);
  // buffer = malloc(bufsize);

  /* associate bit stream with allocated buffer */
  stream = stream_open((void *)compressed, compressed_size);
  zfp_stream_set_bit_stream(zfp, stream);
  zfp_stream_rewind(zfp);

  /* decompress entire array */
  if (!zfp_decompress(zfp, field)) {
    printf("decompression failed\n");
    return NULL;
  }

  /* clean up */
  zfp_field_free(field);
  zfp_stream_close(zfp);
  stream_close(stream);
  free(buffer);
  free(decompressed);

  return decompressed;
}
