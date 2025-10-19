# Questions - TME 3 : Threads

Instructions : copiez vos réponses dans ce fichier (sous la question correspondante). A la fin de la séance, commitez vos réponses.

## Question 1.

```
./TME3 ../WarAndPeace.txt freqstd && ./TME3 ../WarAndPeace.txt freqstdf && ./TME3 ../WarAndPeace.txt freq && ./TME3 ../WarAndPeace.txt partition && ./TME3 ../WarAndPeace.txt mt_mutex && ./TME3 ../WarAndPeace.txt mt_hmutex && ./TME3 ../WarAndPeace.txt mt_hashes && ./TME3 ../WarAndPeace.txt mt_hhashes && ./TME3 ../WarAndPeace.txt mt_hfine && ../check.sh *.freq 


traces pour les 3 modes, invocation a check qui ne rapporte pas d'erreur
```
Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 296 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 260 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 280 ms
All files are identical

## Question 2.

start vaut 0

end vaut file_size

Code des lambdas :

freqstdf :

[&](const std::string& word) {
    total_words++;
    um[word]++;
};

freq :

[&](const std::string& word) {
    total_words++;
    hm.incrementFrequency(word);
};


```
code
```

Accès identifiés : On accède par référence à toute les variables parce que c'est &. Ici, on accède notament à total_words et um/hm. On apporte des modification à ces 2 variables.

## Question 3.

Continuez de compléter ce fichier avec vos traces et réponses.

...

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 357 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 301 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 267 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 262 ms
All files are identical


## Question 4.

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 287 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 265 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 350 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 341 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_naive N=4), containing 3235342 bytes
terminate called without an active exception
Abandon

## Question 5.(Fait avec pc perso donc temps plus rapide)

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 192 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 167 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 170 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 164 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hnaive N=4), containing 3235342 bytes
terminate called without an active exception
Abandon (core dumped)


## Question 6.

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 329 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 271 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 265 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 259 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_atomic N=2), containing 3235342 bytes
Erreur de segmentation

On a toujours des problèmes.

## Question 7.

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 180 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 180 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 178 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 177 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hatomic N=4), containing 3235342 bytes
Total runtime (wall clock) : 96 ms

supprime les races sur les compteurs individuels,
mais ne protège pas les structures de données elles-mêmes (les buckets, la table, les pointeurs, les insertions).

## Question 8.

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 293 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 255 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 270 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 299 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_mutex N=4), containing 3235342 bytes
Total runtime (wall clock) : 198 ms
Files freq.freq and mt_atomic.freq differ
Files freq.freq and mt_naive.freq differ
Files freqstdf.freq and mt_atomic.freq differ
Files freqstdf.freq and mt_naive.freq differ
Files freqstd.freq and mt_atomic.freq differ
Files freqstd.freq and mt_naive.freq differ
Files mt_atomic.freq and mt_mutex.freq differ
Files mt_atomic.freq and mt_naive.freq differ
Files mt_atomic.freq and partition.freq differ
Files mt_mutex.freq and mt_naive.freq differ
Files mt_naive.freq and partition.freq differ

## Question 9.

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 194 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 170 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 182 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 169 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_mutex N=4), containing 3235342 bytes
Total runtime (wall clock) : 154 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hmutex N=4), containing 3235342 bytes
Total runtime (wall clock) : 151 ms

Avec 1 thread : 

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 188 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 160 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 177 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 170 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_mutex N=4), containing 3235342 bytes
Total runtime (wall clock) : 152 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hmutex N=1), containing 3235342 bytes
Total runtime (wall clock) : 174 ms

## Question 10.

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 161 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 173 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 173 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 191 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_mutex N=4), containing 3235342 bytes
Total runtime (wall clock) : 149 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hmutex N=4), containing 3235342 bytes
Total runtime (wall clock) : 183 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hashes N=4), containing 3235342 bytes
Total runtime (wall clock) : 99 ms
Files freq.freq and mt_hnaive.freq differ
Files freq.freq and mt_naive.freq differ
Files freqstdf.freq and mt_hnaive.freq differ
Files freqstdf.freq and mt_naive.freq differ
Files freqstd.freq and mt_hnaive.freq differ
Files freqstd.freq and mt_naive.freq differ
Files mt_hashes.freq and mt_hnaive.freq differ
Files mt_hashes.freq and mt_naive.freq differ
Files mt_hatomic.freq and mt_hnaive.freq differ
Files mt_hatomic.freq and mt_naive.freq differ
Files mt_hmutex.freq and mt_hnaive.freq differ
Files mt_hmutex.freq and mt_naive.freq differ
Files mt_hnaive.freq and mt_mutex.freq differ
Files mt_hnaive.freq and mt_naive.freq differ
Files mt_hnaive.freq and partition.freq differ
Files mt_mutex.freq and mt_naive.freq differ
Files mt_naive.freq and partition.freq differ

Elle evite les sychrnonisation psk on n'a plus de map partagé.

## Question 12.

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 191 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 157 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 169 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 166 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_mutex N=4), containing 3235342 bytes
Total runtime (wall clock) : 148 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hmutex N=4), containing 3235342 bytes
Total runtime (wall clock) : 167 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hashes N=4), containing 3235342 bytes
Total runtime (wall clock) : 97 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hhashes N=4), containing 3235342 bytes
Total runtime (wall clock) : 101 ms
Files freq.freq and mt_hnaive.freq differ
Files freq.freq and mt_naive.freq differ
Files freqstdf.freq and mt_hnaive.freq differ
Files freqstdf.freq and mt_naive.freq differ
Files freqstd.freq and mt_hnaive.freq differ
Files freqstd.freq and mt_naive.freq differ
Files mt_hashes.freq and mt_hnaive.freq differ
Files mt_hashes.freq and mt_naive.freq differ
Files mt_hatomic.freq and mt_hnaive.freq differ
Files mt_hatomic.freq and mt_naive.freq differ
Files mt_hhashes.freq and mt_hnaive.freq differ
Files mt_hhashes.freq and mt_naive.freq differ
Files mt_hmutex.freq and mt_hnaive.freq differ
Files mt_hmutex.freq and mt_naive.freq differ
Files mt_hnaive.freq and mt_mutex.freq differ
Files mt_hnaive.freq and mt_naive.freq differ
Files mt_hnaive.freq and partition.freq differ
Files mt_mutex.freq and mt_naive.freq differ
Files mt_naive.freq and partition.freq differ

## Question 11.

Preparing to parse ../WarAndPeace.txt (mode=freqstd N=4), containing 3235342 bytes
Total runtime (wall clock) : 220 ms
Preparing to parse ../WarAndPeace.txt (mode=freqstdf N=4), containing 3235342 bytes
Total runtime (wall clock) : 190 ms
Preparing to parse ../WarAndPeace.txt (mode=freq N=4), containing 3235342 bytes
Total runtime (wall clock) : 195 ms
Preparing to parse ../WarAndPeace.txt (mode=partition N=4), containing 3235342 bytes
Total runtime (wall clock) : 185 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_mutex N=4), containing 3235342 bytes
Total runtime (wall clock) : 169 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hmutex N=4), containing 3235342 bytes
Total runtime (wall clock) : 185 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hashes N=4), containing 3235342 bytes
Total runtime (wall clock) : 107 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hhashes N=4), containing 3235342 bytes
Total runtime (wall clock) : 126 ms
Preparing to parse ../WarAndPeace.txt (mode=mt_hfine N=4), containing 3235342 bytes
Total runtime (wall clock) : 123 ms
Files freq.freq and mt_hnaive.freq differ
Files freq.freq and mt_naive.freq differ
Files freqstdf.freq and mt_hnaive.freq differ
Files freqstdf.freq and mt_naive.freq differ
Files freqstd.freq and mt_hnaive.freq differ
Files freqstd.freq and mt_naive.freq differ
Files mt_hashes.freq and mt_hnaive.freq differ
Files mt_hashes.freq and mt_naive.freq differ
Files mt_hatomic.freq and mt_hnaive.freq differ
Files mt_hatomic.freq and mt_naive.freq differ
Files mt_hfine.freq and mt_hnaive.freq differ
Files mt_hfine.freq and mt_naive.freq differ
Files mt_hhashes.freq and mt_hnaive.freq differ
Files mt_hhashes.freq and mt_naive.freq differ
Files mt_hmutex.freq and mt_hnaive.freq differ
Files mt_hmutex.freq and mt_naive.freq differ
Files mt_hnaive.freq and mt_mutex.freq differ
Files mt_hnaive.freq and mt_naive.freq differ
Files mt_hnaive.freq and partition.freq differ
Files mt_mutex.freq and mt_naive.freq differ
Files mt_naive.freq and partition.freq differ

Nous sommes bcp plus rapide en mt_hfine qu'en mt_hmutex psk on ne bloque pas toute la table lors de la section critique.
Donc, il y a plus de concurrence. C'est pour cela que l'execution est plus rapide.