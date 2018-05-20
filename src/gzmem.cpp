#include <Rcpp.h>
using namespace Rcpp;

#include <zlib.h>
#include <Rdefines.h>
#include <Rinternals.h>
#include "R_ext/Memory.h"
#include "R_ext/Utils.h"

static voidpf R_zlib_alloc(voidpf ptr, uInt items, uInt size) {
  return(R_alloc(items, size));
}

static void R_zlib_free(voidpf ptr, voidpf addr) {}

//' Compress a raw vector
//'
//' @param r_content Raw vector of data to compress.
//' @param format A character scalar indicating the type of compression to apply.
//' This must be one of "gzip", "zlib" or "raw".
//' @return Raw vector of compressed data.
//' @export
//' @examples
//' raw <- charToRaw("The quick brown fox jumps over the lazy dog.")
//' compressed <- mem_compress(raw, format = "gzip")
//' decompressed <- mem_inflate(compressed, format = "gzip", 1000)
// [[Rcpp::export]]
SEXP mem_compress(SEXP r_content, String format) {

  int status, windowBits, numProtects = 0, level = 1, method = Z_DEFLATED, memLevel = 9, strategy = 0;
  uLongf destLen = 0;
  z_stream strm;

  if (format == "gzip"){
    windowBits = 15+16;
  } else if (format == "zlib"){
    windowBits = 15;
  } else if (format == "raw"){
    windowBits = -15;
  } else {
    Rcpp::stop("Invalid format argument");
  }


  strm.zalloc = NULL;
  strm.zfree = NULL;
  strm.opaque = NULL;
  strm.total_out = 0;
  strm.next_in = RAW(r_content);
  strm.avail_in = GET_LENGTH(r_content);

  SEXP r_result = Rf_allocVector(RAWSXP, strm.avail_in * 1.01 + 12);

  status = deflateInit2(&strm, level, method, windowBits, memLevel, strategy);
  if(status != Z_OK) return(r_content);

  destLen = GET_LENGTH(r_result);

  do {
    strm.next_out = RAW(r_result) + strm.total_out;
    strm.avail_out = destLen - strm.total_out;

    status = deflate(&strm, Z_FINISH);
    if (status == Z_STREAM_END)
      break;
    else if (status == Z_OK) {
      SET_LENGTH(r_result, 2*destLen);
      PROTECT(r_result); numProtects++;
      destLen *= 2;
    } else if (status == Z_MEM_ERROR) {
      return(r_content);
    }
  } while(1);

  SET_LENGTH(r_result, strm.total_out);

  deflateEnd(&strm);

  if (numProtects) UNPROTECT(numProtects);

  return(r_result);

}

//' Inflate a raw vector that was compressed
//'
//' @param r_source raw vector of compressed data.
//' @param format A character scalar indicating the type of compression that was applied.
//' This must be one of "gzip", "zlib" or "raw".
//' @param r_guess_size your best guess as to the size of uncompressed data. Not ideal,
//'        and this won't be necessary in future releases. Reember, this is a direct
//'        port from the defunct \code{Rcompression} package.\cr
//'        \cr
//'        Aim high as you'll only get back the actual number of bytes
//'        in the uncompressed data.
//' @return Raw vector of expanded data.
//' @export
//' @examples
//' raw <- charToRaw("The quick brown fox jumps over the lazy dog.")
//' compressed <- mem_compress(raw, format = "gzip")
//' decompressed <- mem_inflate(compressed, format = "gzip", 1000)
// [[Rcpp::export]]
SEXP mem_inflate(SEXP r_source, String format, SEXP r_guess_size) {

  z_stream stream;
  int err, len, guess_size = REAL(r_guess_size)[0], windowBits;
  unsigned char *ans = (unsigned char *)R_alloc(guess_size, sizeof(unsigned char));
  SEXP r_ans;

  if (format == "gzip"){
    windowBits = MAX_WBITS+16;
  } else if (format == "zlib"){
    windowBits = MAX_WBITS;
  } else if (format == "raw"){
    windowBits = -MAX_WBITS;
  } else {
    Rcpp::stop("Invalid format argument");
  }

  stream.next_in = RAW(r_source);
  stream.avail_in = GET_LENGTH(r_source);
  stream.next_out = (unsigned char *)ans;
  stream.avail_out = guess_size;

  stream.zalloc = R_zlib_alloc;
  stream.zfree = R_zlib_free;
  stream.opaque = NULL;

  err = inflateInit2(&stream, windowBits);
  if(err != Z_OK) {
    Rcpp::stop("cannot establish the uncompress/inflate stream on this data");
  }

  err = inflate(&stream, Z_FINISH);

  if (err < 0) {
    inflateEnd(&stream);
    Rcpp::stop("Failed to uncompress the raw data");
  }

  len = stream.total_out;

  inflateEnd(&stream);

  r_ans = Rf_allocVector(RAWSXP, stream.total_out);
  memcpy(RAW(r_ans), ans, stream.total_out);

  return(r_ans);

}


