
![](/logo.jpg)


# HIGH PERFORMANCE COMPUTING

<br/>

## HPC PROJECT

<br/>

### OPENMP PROGRAM

<br/>

#### Group:
Daniel Vieira Cordeiro
Rafael Câmara Pereira 

<br/>

#### Coordination:
Lerida Monso, Josep Lluis

<br/>

##### Lleida
##### Academic year 2019-2020

<br/>

---

## Contents

The folder **Execution Files** contains the auxiliary files used to execute the code in the Moore Cluster.

There are 6 files:

- 1-1.sh
- creator_c.sh
- creator_run.sh
- executor.sh
- mandelbrot-OMP-1-1.c
- names.txt

---

## Instructions

**Disclaimer:** this process is fairly manual and extensive and is planned to be updated soon. But due to lack of time, this is what we have for now.

**Disclaimer 2:** repeat this process in different folders altering the number of threads on line 25.

In order to execute all necessary executions we follow this steps:

1 - Upload the files in **Execution Files** to the moore cluster.

2 - Execute  **creator_c.sh** 

```bash
$ ./creator_c.sh
```

to generate the correct the correctly named ".c" files to be executed.

3 - Alter the values os the generated ".c" files to de desired ones.

4 - Execute **creator_run.sh** 

```bash
$ ./creator_run.sh
```

to generate the correct the correctly named ".sh" run files to dispatch jobs to the execution queue.

5 - Execute **executor.sh**

```bash
$ ./executor.sh
```

to automatically compile the code and submit it to the Moore job execution queue.

<br/>

---

## Output

The output of each execution will consist of a **".e"** file with the execution time. for instance:

```
Elapsed time: 2.3377884179 seconds
```

and a **".ppm"** file, which contains the calculated *Mandelbrot set*.

The correct image should resemble the following:

![](/mandelbrot.png)

A sample output can be found in the folder **Sample Output**.