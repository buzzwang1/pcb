call gzip -9 -k -f %*
call python gzipHack.py %* %*.gz
copy %*.gz %*.gz.bin