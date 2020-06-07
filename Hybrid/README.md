
![](/logo.jpg)


# HIGH PERFORMANCE COMPUTING

<br/>

## HPC PROJECT

<br/>

### HYBRID (OMP + MPI) PROGRAM

<br/>

#### Group:
Daniel Vieira Cordeiro<br/>
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

This section is divided into 2 folders:

- **Static**
- **Dynamic**

Each of these folders contains two folders:

- **Execution Files**
- **Sample Output**

#### Execution Files

This folder contains the auxiliary files used to execute the code in the Moore Cluster.

There are 6 files:

- 2-2.sh
- executor.sh
- mandelbrot-OMP-1-1.c
- names.txt

#### Sample Output

This folder contains two sample output files that will be better described on the **Output** section.

---

## Instructions

**Disclaimer:** this process is fairly manual and extensive and is planned to be updated soon. But due to lack of time, this is what we have for now.

**Disclaimer 2:** for each generated .sh "run file" it is necessary to adjust the input parameters to the corresponding ones.

In order to execute all necessary executions we follow this steps:

On a directory on your local computer:

1 - Add the files in **Execution Files**

2 - Execute **creator_run.sh** 

```bash
$ ./creator_run.sh
```

to generate the correct the correctly named ".sh" run files to dispatch jobs to the execution queue.

3 - Adjust the input parameters on the newly generated files.

4 - Upload the files in **Execution Files** to the moore cluster.

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
W: 600, H: 400, Iterations: 1000000 Processes:2 Threads: 2

Elapsed time: 447.3859 seconds.

Elapsed time with printing: 448.1876 seconds.
```

and a **".ppm"** file, which contains the calculated *Mandelbrot set*.

The correct image should resemble the following:

![](/mandelbrot.png)

A sample output can be found in the folder **Sample Output**.