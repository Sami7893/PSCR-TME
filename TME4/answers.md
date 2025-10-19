# TME4 Answers

Tracer vos expériences et conclusions dans ce fichier.

Le contenu est indicatif, c'est simplement la copie rabotée d'une IA, utilisée pour tester une version de l'énoncé.
On a coupé ses réponses complètes (et souvent imprécises voire carrément fausses, deadlocks etc... en Oct 2025 les LLM ont encore beaucoup de mal sur ces questions, qui demandent du factuel et des mesures, et ont de fortes tendances à inventer).
Cependant on a laissé des indications en particulier des invocations de l'outil possibles, à adapter à votre code.

## Question 1: Baseline sequential

### Measurements (Release mode)

**Resize + pipe mode:**
```
./build/TME4 -m resize -i input_images -o output_images

Thread 140560655638336 (main): 5711 ms CPU
Total runtime (wall clock): 6270 ms
Memory usage: Resident: 74.8 MB, Peak: 149 MB
Total CPU time across all threads: 5711 ms




./build/TME4 -m pipe -i input_images -o output_images

Thread 140255128712896 (treatImage): 5072 ms CPU
Thread 140255168909120 (main): 16 ms CPU
Total runtime (wall clock): 5497 ms
Memory usage: Resident: 59.5 MB, Peak: 141 MB
Total CPU time across all threads: 5088 ms

le temps "wall clock" est du aux I/O. Le pic de RAM observé est du aux chargement des images.

```



## Question 2: Steps identification

I/O-bound: 
- QImage original = pr::loadImage(file); 
- pr::saveImage(resized, outputFile);

CPU-bound: 
- pr::findImageFiles(opts.inputFolder, [&](const std::filesystem::path& file));
- QImage resized = pr::resizeImage(original);

parallelisable a priori ? oui on peut paralléliser le traitement de l'image. 

## Question 3: BoundedBlockingQueue analysis

Oui elle peut supporter plusieurs productuers et consommateurs. 
La situation ou un appel peut etre bloqué, si un thread est entrain de push une image, un autre thread ne peut pas pop ou push une image.

La lambda dans cv.wait dans la fonction push permet de vérifier si la taille de la queue_ est < à max_size_ si c'est faux, alors le thread va sleep à nouveau sinon il reprend le verrou et continu son exécution.

Cette lambda est meilleur car elle evite les erreur de boucle infini au niveau d'un while et aussi d'eviter les "spurious wakeup"


## Question 4: Pipe mode study

FILE_POISON est une constante définie dans task.h. Le thread va récuperer cette valeur et va break donc sortir de son while(true) ce qui va lui permettre d'arreter son traitement d'image. 

Order/invert :
Ordre imposé sinon deadlock dans les 3 cas.


## Question 5: Multi-thread pipe_mt

Implement pipe_mt mode with multiple worker threads.

For termination, ... poison pills...

Measurements:
- N=1: 
```
./build/TME4 -m pipe_mt -n 1 -i input_images -o output_images
...
Thread 139947803670208 (treatImage): 5802 ms CPU
Thread 139947843333952 (main): 16 ms CPU
Total runtime (wall clock): 6238 ms
Memory usage: Resident: 59.7 MB, Peak: 141 MB
Total CPU time across all threads: 5818 ms
```
- N=2: 
```
./build/TME4 -m pipe_mt -n 2 -i input_images -o output_images
...
Thread 140071833429696 (treatImage): 2609 ms CPU
Thread 140071841822400 (treatImage): 2714 ms CPU
Thread 140071881948992 (main): 18 ms CPU
Total runtime (wall clock): 2946 ms
Memory usage: Resident: 87.5 MB, Peak: 238 MB
Total CPU time across all threads: 5341 ms

```
- N=4: 
```
./build/TME4 -m pipe_mt -n 4 -i input_images -o output_images
...
Thread 140100795098816 (treatImage): 1279 ms CPU
Thread 140100786706112 (treatImage): 1324 ms CPU
Thread 140100803491520 (treatImage): 1406 ms CPU
Thread 140100778313408 (treatImage): 1536 ms CPU
Thread 140100843171648 (main): 17 ms CPU
Total runtime (wall clock): 1692 ms
Memory usage: Resident: 128 MB, Peak: 368 MB
Total CPU time across all threads: 5562 ms
```
- N=8: 
```
./build/TME4 -m pipe_mt -n 8 -i input_images -o output_images
...
Thread 140309061158592 (treatImage): 673 ms CPU
Thread 140309035980480 (treatImage): 666 ms CPU
Thread 140309069551296 (treatImage): 670 ms CPU
Thread 140309027587776 (treatImage): 642 ms CPU
Thread 140309052765888 (treatImage): 720 ms CPU
Thread 140309077944000 (treatImage): 755 ms CPU
Thread 140309044373184 (treatImage): 757 ms CPU
Thread 140309086336704 (treatImage): 844 ms CPU
Thread 140309135806272 (main): 17 ms CPU
Total runtime (wall clock): 989 ms
Memory usage: Resident: 245 MB, Peak: 593 MB
Total CPU time across all threads: 5744 ms
```

Best: 16 threads

## Question 6: TaskData struct

```cpp
struct TaskData {
    std::filesystem::path file;
    QImage image;
    bool poison{false};  
};
```

Fields: QImage "image" for the image data, std::filesystem::path "filename"

Use stockage direct for QImage, because 


TASK_POISON: 
const TaskData TASK_POISON("",QImage(),true);

## Question 7: ImageTaskQueue typing

pointers vs values

Choose BoundedBlockingQueue<TaskData> as consequence

car elle évite toute gestion manuelle de mémoire (new / delete) et exploite la sémantique copy-on-write de QImage, qui rend la copie très peu coûteuse.
Elle est donc plus sûre, simple et efficace pour ce cas.

## Question 8: Pipeline functions

Implement reader, resizer, saver in Tasks.cpp.

mt_pipeline mode: Creates threads for each stage, with configurable numbers.

Termination: Main pushes the appropriate number of poisons after joining the previous stage.

Measurements: 
```
./build/TME4 -m mt_pipeline -i input_images -o output_images
...
Thread 134267996075584 (saver): 285 ms CPU
Thread 134268004468288 (resizer): 76 ms CPU
Thread 134268012860992 (reader): 3443 ms CPU
Thread 134268061949312 (main): 17 ms CPU
Total runtime (wall clock): 3463 ms
Memory usage: Resident: 116 MB, Peak: 508 MB
Total CPU time across all threads: 3821 ms
```


## Question 9: Configurable parallelism

Added nbread, nbresize, nbwrite options.


Timings:
- 1/1/1 (default): 
```
./build/TME4 -m mt_pipeline_2 -i input_images -o output_images
...


Image resizer starting with input folder 'input_images', output folder 'output_images', mode 'mt_pipeline_2', nthreads 4', nbread 1', nbresize 1', nbwrite 1
Thread 132631331202624 (saver): 274 ms CPU
Thread 132631339595328 (resizer): 79 ms CPU
Thread 132631347988032 (reader): 3553 ms CPU
Thread 132631400467840 (main): 17 ms CPU
Total runtime (wall clock): 3574 ms
Memory usage: Resident: 79.6 MB, Peak: 499 MB
Total CPU time across all threads: 3923 ms
```

- 1/4/1: 
```
./build/TME4 -m mt_pipeline_2 --nbread 1 --nbresize 4 --nbwrite 1 -i input_images -o output_images
```

Image resizer starting with input folder 'input_images', output folder 'output_images', mode 'mt_pipeline_2', nthreads 4', nbread 1', nbresize 4', nbwrite 1
Thread 126769307567680 (resizer): 13 ms CPU
Thread 126769282389568 (resizer): 27 ms CPU
Thread 126769299174976 (resizer): 12 ms CPU
Thread 126769273996864 (saver): 274 ms CPU
Thread 126769290782272 (resizer): 33 ms CPU
Thread 126769315960384 (reader): 3341 ms CPU
Thread 126769369898368 (main): 17 ms CPU
Total runtime (wall clock): 3360 ms
Memory usage: Resident: 162 MB, Peak: 582 MB
Total CPU time across all threads: 3717 ms

- 4/1/1: 
```
./build/TME4 -m mt_pipeline_2 --nbread 4 --nbresize 1 --nbwrite 1 -i input_images -o output_images
```

Thread 129893387699776 (saver): 376 ms CPU
Thread 129893396092480 (resizer): 85 ms CPU
Thread 129893404485184 (reader): 1107 ms CPU
Thread 129893421270592 (reader): 1139 ms CPU
Thread 129893429663296 (reader): 1154 ms CPU
Thread 129893412877888 (reader): 1281 ms CPU
Thread 129893473123712 (main): 21 ms CPU
Total runtime (wall clock): 1349 ms
Memory usage: Resident: 142 MB, Peak: 578 MB
Total CPU time across all threads: 5163 ms

- 1/1/4: 
```
./build/TME4 -m mt_pipeline_2 --nbread 1 --nbresize 1 --nbwrite 4  -i input_images -o output_images

Thread 125334171346496 (saver): 64 ms CPU
Thread 125334146168384 (saver): 89 ms CPU
Thread 125334162953792 (saver): 89 ms CPU
Thread 125334154561088 (saver): 25 ms CPU
Thread 125334179739200 (resizer): 86 ms CPU
Thread 125334188131904 (reader): 3522 ms CPU
Thread 125334223719808 (main): 19 ms CPU
Total runtime (wall clock): 3541 ms
Memory usage: Resident: 117 MB, Peak: 508 MB
Total CPU time across all threads: 3894 ms
```

- 4/4/1: 
```
./build/TME4 -m mt_pipeline_2 --nbread 4 --nbresize 4 --nbwrite 1  -i input_images -o output_images

Thread 124283304179264 (resizer): 29 ms CPU
Thread 124283329357376 (resizer): 15 ms CPU
Thread 124283312571968 (resizer): 26 ms CPU
Thread 124283295786560 (saver): 353 ms CPU
Thread 124283320964672 (resizer): 12 ms CPU
Thread 124283354535488 (reader): 1096 ms CPU
Thread 124283337750080 (reader): 1148 ms CPU
Thread 124283346142784 (reader): 1160 ms CPU
Thread 124283362928192 (reader): 1220 ms CPU
Thread 124283416407424 (main): 21 ms CPU
Total runtime (wall clock): 1279 ms
Memory usage: Resident: 171 MB, Peak: 621 MB
Total CPU time across all threads: 5080 ms
```

- 1/4/4: 
```
./build/TME4 -m mt_pipeline_2 --nbread 1 --nbresize 4 --nbwrite 4  -i input_images -o output_images

Thread 133315496691264 (resizer): 15 ms CPU
Thread 133315446335040 (saver): 59 ms CPU
Thread 133315463120448 (saver): 93 ms CPU
Thread 133315479905856 (resizer): 31 ms CPU
Thread 133315488298560 (resizer): 12 ms CPU
Thread 133315454727744 (saver): 73 ms CPU
Thread 133315471513152 (saver): 46 ms CPU
Thread 133315505083968 (resizer): 22 ms CPU
Thread 133315513476672 (reader): 3411 ms CPU
Thread 133315566972288 (main): 19 ms CPU
Total runtime (wall clock): 3432 ms
Memory usage: Resident: 161 MB, Peak: 580 MB
Total CPU time across all threads: 3781 ms
```

- 4/1/4: 
```
./build/TME4 -m mt_pipeline_2 --nbread 4 --nbresize 1 --nbwrite 4  -i input_images -o output_images

Thread 128760952477248 (saver): 142 ms CPU
Thread 128760944084544 (saver): 66 ms CPU
Thread 128760969262656 (saver): 52 ms CPU
Thread 128760960869952 (saver): 119 ms CPU
Thread 128760977655360 (resizer): 90 ms CPU
Thread 128761011226176 (reader): 1093 ms CPU
Thread 128760986048064 (reader): 1124 ms CPU
Thread 128760994440768 (reader): 1142 ms CPU
Thread 128761002833472 (reader): 1249 ms CPU
Thread 128761060318592 (main): 19 ms CPU
Total runtime (wall clock): 1303 ms
Memory usage: Resident: 177 MB, Peak: 593 MB
Total CPU time across all threads: 5096 ms
```

- 4/4/4: 
```
./build/TME4 -m mt_pipeline_2 --nbread 4 --nbresize 4 --nbwrite 4  -i input_images -o output_images

Thread 128612932507200 (saver): 81 ms CPU
Thread 128612924114496 (saver): 72 ms CPU
Thread 128612915721792 (saver): 116 ms CPU
Thread 128612974470720 (resizer): 35 ms CPU
Thread 128612966078016 (resizer): 12 ms CPU
Thread 128612940899904 (saver): 120 ms CPU
Thread 128612957685312 (resizer): 22 ms CPU
Thread 128612949292608 (resizer): 18 ms CPU
Thread 128612999648832 (reader): 1109 ms CPU
Thread 128613008041536 (reader): 1092 ms CPU
Thread 128612991256128 (reader): 1160 ms CPU
Thread 128612982863424 (reader): 1238 ms CPU
Thread 128613057998208 (main): 24 ms CPU
Total runtime (wall clock): 1290 ms
Memory usage: Resident: 179 MB, Peak: 641 MB
Total CPU time across all threads: 5099 ms
```

Best config: 4/4/1
interprétation

## Question 10: Queue sizes impact


With size 1: 
```
./build/TME4 -m pipe_mt -n 2 --queue-size 1 -i input_images -o output_images
...

Image resizer starting with input folder 'input_images', output folder 'output_images', mode 'pipe_mt', nthreads 2', nbread 1', nbresize 1', nbwrite 1', queue-size 1
Thread 136111006737984 (treatImage): 2074 ms CPU
Thread 136111015130688 (treatImage): 2200 ms CPU
Thread 136111056952704 (main): 2 ms CPU
Total runtime (wall clock): 2331 ms
Memory usage: Resident: 93.4 MB, Peak: 228 MB
Total CPU time across all threads: 4276 ms
```

With size 100: 
```
./build/TME4 -m pipe_mt -n 2 --queue-size 100 -i input_images -o output_images
...

Image resizer starting with input folder 'input_images', output folder 'output_images', mode 'pipe_mt', nthreads 2', nbread 1', nbresize 1', nbwrite 1', queue-size 100
Thread 135007555614272 (treatImage): 2002 ms CPU
Thread 135007564006976 (treatImage): 2114 ms CPU
Thread 135007614193024 (main): 1 ms CPU
Total runtime (wall clock): 2229 ms
Memory usage: Resident: 80.3 MB, Peak: 228 MB
Total CPU time across all threads: 4117 ms
```
- 8/8/8: 
```
./build/TME4 -m mt_pipeline_2 --nbread 8 --nbresize 8 --nbwrite 8  -i input_images -o output_images

Image resizer starting with input folder 'input_images', output folder 'output_images', mode 'mt_pipeline_2', nthreads 4', nbread 8', nbresize 8', nbwrite 8', queue-size 10
Thread 138090783872576 (resizer): 9 ms CPU
Thread 138090817443392 (resizer): 7 ms CPU
Thread 138090775479872 (resizer): 17 ms CPU
Thread 138090800657984 (resizer): 19 ms CPU
Thread 138090792265280 (resizer): 7 ms CPU
Thread 138090750301760 (saver): 54 ms CPU
Thread 138090699945536 (saver): 46 ms CPU
Thread 138090741909056 (saver): 61 ms CPU
Thread 138090691552832 (saver): 19 ms CPU
Thread 138090708338240 (saver): 18 ms CPU
Thread 138090716730944 (saver): 50 ms CPU
Thread 138090733516352 (saver): 45 ms CPU
Thread 138090725123648 (saver): 72 ms CPU
Thread 138090809050688 (resizer): 3 ms CPU
Thread 138090758694464 (resizer): 14 ms CPU
Thread 138090767087168 (resizer): 16 ms CPU
Thread 138090825836096 (reader): 602 ms CPU
Thread 138090834228800 (reader): 583 ms CPU
Thread 138090842621504 (reader): 650 ms CPU
Thread 138090859406912 (reader): 595 ms CPU
Thread 138090851014208 (reader): 664 ms CPU
Thread 138090867799616 (reader): 790 ms CPU
Thread 138090884585024 (reader): 598 ms CPU
Thread 138090876192320 (reader): 695 ms CPU
Thread 138090918432128 (main): 28 ms CPU
Total runtime (wall clock): 1152 ms
Memory usage: Resident: 305 MB, Peak: 767 MB
Total CPU time across all threads: 5662 ms
```

- 1/32/1: 
```
./build/TME4 -m mt_pipeline_2 --nbread 1 --nbresize 32 --nbwrite 1  -i input_images -o output_images

Image resizer starting with input folder 'input_images', output folder 'output_images', mode 'mt_pipeline_2', nthreads 4', nbread 1', nbresize 32', nbwrite 1', queue-size 10
Thread 137814367561280 (resizer): 4 ms CPU
Thread 137814350775872 (resizer): 3 ms CPU
Thread 137814401132096 (resizer): 0 ms CPU
Thread 137814426310208 (resizer): 3 ms CPU
Thread 137814292026944 (resizer): 0 ms CPU
Thread 137814417917504 (resizer): 0 ms CPU
Thread 137814451488320 (resizer): 0 ms CPU
Thread 137814384346688 (resizer): 0 ms CPU
Thread 137814443095616 (resizer): 7 ms CPU
Thread 137814535415360 (resizer): 3 ms CPU
Thread 137814283634240 (resizer): 0 ms CPU
Thread 137814300419648 (resizer): 6 ms CPU
Thread 137814476666432 (resizer): 0 ms CPU
Thread 137814468273728 (resizer): 0 ms CPU
Thread 137814527022656 (resizer): 5 ms CPU
Thread 137814485059136 (resizer): 0 ms CPU
Thread 137814501844544 (resizer): 3 ms CPU
Thread 137814375953984 (resizer): 3 ms CPU
Thread 137814459881024 (resizer): 1 ms CPU
Thread 137814325597760 (resizer): 0 ms CPU
Thread 137814275241536 (resizer): 16 ms CPU
Thread 137814434702912 (resizer): 0 ms CPU
Thread 137814342383168 (resizer): 8 ms CPU
Thread 137814493451840 (resizer): 0 ms CPU
Thread 137814518629952 (resizer): 6 ms CPU
Thread 137814392739392 (resizer): 0 ms CPU
Thread 137814333990464 (resizer): 0 ms CPU
Thread 137814317205056 (resizer): 0 ms CPU
Thread 137814510237248 (resizer): 3 ms CPU
Thread 137814359168576 (resizer): 2 ms CPU
Thread 137814308812352 (resizer): 9 ms CPU
Thread 137814266848832 (saver): 283 ms CPU
Thread 137814409524800 (resizer): 6 ms CPU
Thread 137814543808064 (reader): 3614 ms CPU
Thread 137814586129792 (main): 26 ms CPU
Total runtime (wall clock): 3653 ms

- 32/32/32: 
```
./build/TME4 -m mt_pipeline_2 --nbread 32 --nbresize 32 --nbwrite 32  -i input_images -o output_images

Image resizer starting with input folder 'input_images', output folder 'output_images', mode 'mt_pipeline_2', nthreads 4', nbread 32', nbresize 32', nbwrite 32', queue-size 10
Thread 137203968161344 (reader): 40 ms CPU
Thread 137204043695680 (reader): 130 ms CPU
Thread 137203976554048 (reader): 48 ms CPU
Thread 137204001732160 (reader): 42 ms CPU
Thread 137204010124864 (reader): 50 ms CPU
Thread 137204110837312 (reader): 50 ms CPU
Thread 137204077266496 (reader): 56 ms CPU
Thread 137203984946752 (reader): 115 ms CPU
Thread 137203892627008 (reader): 232 ms CPU
Thread 137204035302976 (reader): 205 ms CPU
Thread 137203934590528 (reader): 61 ms CPU
Thread 137203959768640 (reader): 200 ms CPU
Thread 137203993339456 (reader): 111 ms CPU
Thread 137204052088384 (reader): 74 ms CPU
Thread 137204119230016 (reader): 127 ms CPU
Thread 137204094051904 (reader): 140 ms CPU
Thread 137204127622720 (reader): 94 ms CPU
Thread 137203884234304 (reader): 222 ms CPU
Thread 137203909412416 (reader): 131 ms CPU
Thread 137204102444608 (reader): 136 ms CPU
Thread 137204068873792 (reader): 115 ms CPU
Thread 137203942983232 (reader): 234 ms CPU
Thread 137203833878080 (resizer): 3 ms CPU
Thread 137203859056192 (resizer): 0 ms CPU
Thread 137203674416704 (resizer): 3 ms CPU
Thread 137203632453184 (resizer): 3 ms CPU
Thread 137203707987520 (resizer): 0 ms CPU
Thread 137203825485376 (resizer): 1 ms CPU
Thread 137203716380224 (resizer): 0 ms CPU
Thread 137203783521856 (resizer): 2 ms CPU
Thread 137203666024000 (resizer): 0 ms CPU
Thread 137203867448896 (resizer): 2 ms CPU
Thread 137203657631296 (resizer): 8 ms CPU
Thread 137203766736448 (resizer): 0 ms CPU
Thread 137203741558336 (resizer): 0 ms CPU
Thread 137203791914560 (resizer): 2 ms CPU
Thread 137203800307264 (resizer): 0 ms CPU
Thread 137203640845888 (resizer): 12 ms CPU
Thread 137203733165632 (resizer): 0 ms CPU
Thread 137203749951040 (resizer): 0 ms CPU
Thread 137204144408128 (reader): 215 ms CPU
Thread 137203850663488 (resizer): 12 ms CPU
Thread 137203624060480 (resizer): 0 ms CPU
Thread 137203758343744 (resizer): 0 ms CPU
Thread 137203724772928 (resizer): 0 ms CPU
Thread 137203875841600 (resizer): 4 ms CPU
Thread 137203691202112 (resizer): 2 ms CPU
Thread 137203447813696 (saver): 4 ms CPU
Thread 137203649238592 (resizer): 10 ms CPU
Thread 137203817092672 (resizer): 0 ms CPU
Thread 137203775129152 (resizer): 0 ms CPU
Thread 137203842270784 (resizer): 0 ms CPU
Thread 137203699594816 (resizer): 4 ms CPU
Thread 137203808699968 (resizer): 1 ms CPU
Thread 137203498169920 (saver): 11 ms CPU
Thread 137203582096960 (saver): 4 ms CPU
Thread 137203472991808 (saver): 11 ms CPU
Thread 137203431028288 (saver): 14 ms CPU
Thread 137203380672064 (saver): 0 ms CPU
Thread 137203531740736 (saver): 23 ms CPU
Thread 137203372279360 (saver): 0 ms CPU
Thread 137203573704256 (saver): 25 ms CPU
Thread 137203397457472 (saver): 35 ms CPU
Thread 137203540133440 (saver): 14 ms CPU
Thread 137203506562624 (saver): 0 ms CPU
Thread 137203422635584 (saver): 0 ms CPU
Thread 137203514955328 (saver): 0 ms CPU
Thread 137203464599104 (saver): 11 ms CPU
Thread 137203598882368 (saver): 7 ms CPU
Thread 137203481384512 (saver): 27 ms CPU
Thread 137203548526144 (saver): 4 ms CPU
Thread 137203414242880 (saver): 0 ms CPU
Thread 137203389064768 (saver): 0 ms CPU
Thread 137203556918848 (saver): 0 ms CPU
Thread 137203590489664 (saver): 0 ms CPU
Thread 137203347101248 (saver): 0 ms CPU
Thread 137203489777216 (saver): 5 ms CPU
Thread 137203456206400 (saver): 24 ms CPU
Thread 137203405850176 (saver): 0 ms CPU
Thread 137203607275072 (saver): 32 ms CPU
Thread 137203523348032 (saver): 0 ms CPU
Thread 137203439420992 (saver): 24 ms CPU
Thread 137203355493952 (saver): 15 ms CPU
Thread 137203363886656 (saver): 32 ms CPU
Thread 137203565311552 (saver): 20 ms CPU
Thread 137204136015424 (reader): 320 ms CPU
Thread 137203682809408 (resizer): 6 ms CPU
Thread 137203926197824 (reader): 332 ms CPU
Thread 137203615667776 (resizer): 8 ms CPU
Thread 137203901019712 (reader): 235 ms CPU
Thread 137204085659200 (reader): 237 ms CPU
Thread 137203917805120 (reader): 369 ms CPU
Thread 137204060481088 (reader): 280 ms CPU
Thread 137204026910272 (reader): 170 ms CPU
Thread 137203951375936 (reader): 198 ms CPU
Thread 137204018517568 (reader): 251 ms CPU
Thread 137204186942848 (main): 35 ms CPU
Total runtime (wall clock): 1271 ms
Memory usage: Resident: 295 MB, Peak: 996 MB
Total CPU time across all threads: 5680 ms

Best 8/8/8


## Question 11: BoundedBlockingQueueBytes

Oui, parallélisable passage de 5000ms à 1200ms en runtime.

## Question 12: Why important

Always allow push if current_bytes == 0, ...

Fairness: ...

## Bonus