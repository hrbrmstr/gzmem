context("basic functionality")
test_that("we can do something", {

  text <- "The quick brown fox jumps over the lazy dog."

  mangled <- rawToChar(mem_inflate(mem_compress(charToRaw(text)), 1000))

  expect_equal(text, mangled)

})
