# ups-examples

Repo obsahuje ukazkove priklady jednoduchych server/klient aplikaci v C, Java, ruby a golang. Priklady schvalne obsahuji drobne chyby, aby bylo treba kod pochopit a najit trivialni opravu.

## C

* `TCPa` - TCP protokol a `fork()` pro paralelni zpracovani pozadavku
* `TCPb` - Stejne jako TCPa, je demonstrace moznosti pouzit nad sockety klasicke 
* `TCPc` - TCP protokol a `pthreads` pro paralelni zpracovani pozadavku
* `TCPd` - TCP protokol a `select` pro pseudoparalelni zpracovani pozadavku
* `UDP` - Jednoduchy UDP klient a server
* `UNIX` - Klient a server - `SOCK_STREAM` pres unixove sockety

## Java

* `TCPa` - TCP jednoduchy klient/server v Jave
* `TCPb` - TCP klient a server vyuzivajici pro obsluhu pozadavku vlakna
* `UDP` - Jednoduchy UDP klient a server
* `ups_client_example` - ukazka mozne architektury implementace sitove komunikace

## ruby

* `server_calc.rb` - TCP kalkulacka pro cviceni - jednoduchy format zprav pro volani matematickych operace s jednoduchou validaci
* `server_echo.rb` - TCP echo server (vrati co prislo a ukonci spojeni)
* `server_reverse.rb` - TCP server - klient dostane nahodny retezec, jeho ukolem je jej vratit v obracenem poradi a server vyhodnodi stav

## golang

* Ukazka echo server a klienta, server prijme spojeni, pro kazde spusti go routine kde odpovi stejnym textem ktery prijal.
