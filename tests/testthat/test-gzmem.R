context("basic functionality")
test_that("gzip round trips", {

  text <- "The quick brown fox jumps over the lazy dog."

  mangled <- rawToChar(mem_inflate(mem_compress(charToRaw(text), format = "gzip"),  format = "gzip", 1000))

  expect_equal(text, mangled)

})
test_that("zlib round trips", {

  text <- "The quick brown fox jumps over the lazy dog."

  mangled <- rawToChar(mem_inflate(mem_compress(charToRaw(text), format = "zlib"), format = "zlib", 1000))

  expect_equal(text, mangled)

})
test_that("raw round trips", {

  text <- "The quick brown fox jumps over the lazy dog."

  mangled <- rawToChar(mem_inflate(mem_compress(charToRaw(text), format = "raw"), format = "raw", 1000))

  expect_equal(text, mangled)

})
