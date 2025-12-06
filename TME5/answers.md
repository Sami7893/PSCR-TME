# TME5 Correction: Parallelization of a Ray Tracer

## Baseline Sequential

### Question 1
Extracted TME5.zip from Moodle, added to repo, committed and pushed.

Configured project with CMake as previous TMEs. No dependencies, all handmade.

### Question 2

Ran `build/TME5` and generated `spheres.bmp`.

Platform: 
Intel N200 1.0-3.4Ghz all 4 Core, Win11, SSD Nvme
Temps/baseline choisi :
En not release:
Ray tracer starting with output 'spheres.bmp', resolution 3840x2000, spheres 250, mode sequential
Total time 139522ms.
En release:
Ray tracer starting with output 'spheres.bmp', resolution 3840x2000, spheres 250, mode sequential
Total time 7317ms.
## With Manual Threads

### Question 3
Implemented `void renderThreadPerPixel(const Scene& scene, Image& img)` in Renderer.

mesures
Ray tracer starting with output 'spheres.bmp', resolution 50x50, spheres 250, mode ThreadPerPixel
Total time 274ms

Ray tracer starting with output 'spheres.bmp', resolution 75x75, spheres 250, mode ThreadPerPixel
Total time 625ms.

Ray tracer starting with output 'spheres.bmp', resolution 100x100, spheres 250, mode ThreadPerPixel
Total time 1061ms.

### Question 4
Implemented `void renderThreadPerRow(const Scene& scene, Image& img)` in Renderer.

mesures

Ray tracer starting with output 'spheres.bmp', resolution 100x100, spheres 250, mode ThreadPerRow
Total time 10ms.

Ray tracer starting with output 'spheres.bmp', resolution 720x480, spheres 250, mode ThreadPerRow
Total time 170ms.

Ray tracer starting with output 'spheres.bmp', resolution 1920x1080, spheres 250, mode ThreadPerRow
Total time 716ms.

Ray tracer starting with output 'spheres.bmp', resolution 3840x2160, spheres 250, mode ThreadPerRow
Total time 2761ms.

### Question 5
Implemented `void renderThreadManual(const Scene& scene, Image& img, int nbthread)` in Renderer.

mesures

Ray tracer starting with output 'spheres.bmp', resolution 100x100, spheres 250, mode ThreadManual, threads 8
Total time 4ms.

Ray tracer starting with output 'spheres.bmp', resolution 720x480, spheres 250, mode ThreadManual, threads 8
Total time 127ms.

Ray tracer starting with output 'spheres.bmp', resolution 1920x1080, spheres 250, mode ThreadManual, threads 8
Total time 580ms

Ray tracer starting with output 'spheres.bmp', resolution 3840x2160, spheres 250, mode ThreadManual, threads 8
Total time 2440ms.

Ray tracer starting with output 'spheres.bmp', resolution 3840x2160, spheres 250, mode ThreadManual, threads 16
Total time 2268ms.

Ray tracer starting with output 'spheres.bmp', resolution 7680x4320, spheres 250, mode ThreadManual, threads 8
Total time 16100ms

## With Thread Pool

### Question 6
Queue class: blocking by default, can switch to non-blocking.

### Question 7
Pool class: constructor with queue size, start, stop.
Job abstract class with virtual run().

### Question 8
PixelJob: derives from Job, captures ?TODO?

renderPoolPixel: 

Mode "-m PoolPixel" with -n.

mesures

### Question 9
LineJob: derives from Job, captures TODO

renderPoolRow: 

Mode "-m PoolRow -n nbthread".

mesures

### Question 10
Best:

## Bonus

### Question 11

pool supportant soumission de lambda.