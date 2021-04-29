:: -9 => best compression
:: -k => keep (don't delete) input files
:: -f => force overwrite of output file and compress links
call gzip -9 -k -f %*
call python gzipHack.py %* %*.gz %*.gz.bin
