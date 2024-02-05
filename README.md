# mux_demux
Just run make, two programs are going to get created one that multiplexes 
and one that demultiplexes 

run ```make```

## requirements
1. make
2. python3
3. gcc

## execution
When make runs it creates a new data.h that includes an array of characters of 640 in length.
Each byte is represented in decimal and also each decimals has a representation in utf so that we
can display on the demux the characters. 

## Convert hex to utf
https://onlinetools.com/utf8/convert-hexadecimal-to-utf8

## Data Mocking
For generating new data run ./gen.py, it will generate a new data.h

## Infos
- Demuliplexer asks for next data not specific on size, data gets red byte per byte
- Multiplexer is creating the multiplexed data using ration on the sources, if we want to chage the pace then we alter the ration accordingly 

e.g. ratio is 1/1, one packet from source_0 and one packet from source_1
```
  source source_0 = {
    ... 
    .ratio = 1.0};

  source source_1 = {
    ...
    .ratio = 1.0};
```


e.g. ratio is 10/1, ten (10) packets from source_0 and 1 packet from source_1
```
  source source_0 = {
    ...
    .ratio = 10.0};

  source source_1 = {
    ...
    .ratio = 1.0};
```
- Packet size from multiplexer is 64 bytes on every `fetch` request happening through socket 
