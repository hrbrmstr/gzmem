
`gzmem` : Better gz in memory compression/decompression

A partial resurrection of `Rcompression` for [this](http://stackoverflow.com/q/39707388/1457051).

The following functions are implemented:

-   `mem_compress`: Compress a raw vector using gz compression
-   `mem_inflate`: Inflate a raw vector that was gz compressed

### TODO

-   \[ \] Rewrite in Rcpp
-   \[ \] Make inflation dynamic vs relying on guessed bufffer size

### Installation

``` r
devtools::install_git("https://gitlab.com/hrbrmstr/gzmem.git")
```

### Usage

``` r
library(gzmem)
library(magrittr)

# current verison
packageVersion("gzmem")
```

    ## [1] '0.1.0'

``` r
text <- "Hey, the room is getting smaller. No, it's not. *He's* getting *bigger*!"

text %>% 
  charToRaw() %>% 
  mem_compress(format = "gzip")
```

    ##  [1] 1f 8b 08 00 00 00 00 00 04 03 3d ca b1 09 c0 30 0c 04 c0 55 3e 95 41 18 cf e1 2a 3b 24 f0 28 02 db 02 4b 4d b6 4f
    ## [39] 97 ab af f3 ad c8 87 d8 ee 13 16 50 66 da 52 c4 bc c6 e0 6e 38 bd c2 b2 04 96 67 83 74 96 90 7f c9 6d aa dc 72 7c
    ## [77] ac 89 c5 99 48 00 00 00

``` r
text %>% 
  charToRaw() %>% 
  mem_compress(format = "gzip") %>% 
  mem_inflate(format = "gzip", 1024) %>% 
  rawToChar()
```

    ## [1] "Hey, the room is getting smaller. No, it's not. *He's* getting *bigger*!"

### Test Results

``` r
library(gzmem)
library(testthat)

date()
```

    ## [1] "Fri May 18 09:34:26 2018"

``` r
test_dir("tests/")
```

    ## ✔ | OK F W S | Context
    ## ══ testthat results  ═══════════════════════════════════════════════════════════════════════════════════════════════════════════
    ## OK: 3 SKIPPED: 0 FAILED: 0
    ## 
    ## ══ Results ═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
    ## Duration: 0.2 s
    ## 
    ## OK:       0
    ## Failed:   0
    ## Warnings: 0
    ## Skipped:  0
    ## 
    ## You are a coding rockstar!
