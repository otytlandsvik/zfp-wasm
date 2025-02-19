// hardcoded double vector length for now
let originalLength = 1562;

// malloc and memcpy compressed array into wasm memory
let compressed_p = Module._exported_malloc(arrayFromServer.length);
Module.HEAP8.set(arrayFromServer, compressed_p);

// malloc for uncompressed doubles
// let new_data_p = Module._exported_malloc(originalLength * 8);

// Run decompression
let new_data_p = Module._decompress(
  compressed_p,
  arrayFromServer.length,
  // new_data_p,
  originalLength,
);

// Retrieve new data
let new_data = Module.HEAPF64.subarray(
  new_data_p / 8,
  new_data_p / 8 + originalLength,
);
