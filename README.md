# Matryoshka Iterated Inside Out Algorithm

This archive is distributed under the [CC BY-NC-SA 4.0 License](LICENSE).

This is a repository of the software and data
that were used in the research **An improved variant of the Iterated Inside Out
algorithm for solving the optimal transport
DOTmark instances** by R. Bargetto, F. Della Croce, and R. Scatamacchia.
The mentioned research extends to image processing the research [Iterated Inside Out: a new exact algorithm for the transportation problem](https://doi.org/10.1287/ijoc.2024.0642), and the related repository [IJOC repository](https://github.com/INFORMSJoC/2024.0642).
An ongoing development repository of the algorithm and all its variants is available at [Iterated Inside Out](https://github.com/robertobarg/IteratedInsideOut).


## Cite
If you use the material of this repo, please cite it.


## Description
This software is based on the implementation of Iterated Inside Out (IIO), a new exact algorithm for solving the transportation problem.
It implements the variant of IIO that efficiently solves image processing instances and the Matryoshka algorithm incorporating that variant.


## Content of the repository
This repository includes

* the C++ source code of the algorithm, directory [src](src/),
* the complete results of the experiments, directory [results](results/),
* the program configuration files to replicate the paper experiments, directory [cfgs](cfgs/),
* an SQL script for creating a relational database of the optimization results, directory [sql](sql/), 
* a set of useful scripts you may need, directory [scr](scr/), and
* a [makefile](Makefile) to compile the C++ code and generate the executable file.


## Problem instances
The DOTmark dataset is available at this [link](https://www.stochastik.math.uni-goettingen.de/index.php?id=215/).
A TP instance in the format of our software is to be generated with the script [generate an image processing TP](scr/geniptp.sh).
The Linux command line to run for generating an instance is
```
./scr/geniptp.sh <class_tag> <img_size> <objf> <data_pic_1> <data_pic2>
```
where

* `<class_tag>` is a tag for the instance (e.g., Cauchy, GRFrough, etc.),
* `<img_size>` is a tag for the size of the images (e.g., 32, 64, 128, etc.),
* `<objf>` is an integer identifying the distance function (1: Euclidean, 2: Squared Euclidean, 3: 1-norm, 4: infinity-norm),
* `<data_pic_1>` and `<data_pic_2>` are the data files of the two images.

Each image data file is a CSV file containing the image pixel intensities by rows.


## Instructions to run the program
If you compile the source code with the makefile we provide, you will find the new executable file in the subdirectory [bin](bin/).
In the Linux command line, use the following command to run the program.
```
./bin/iio instancefile.txt cfgs/iio.cfg

```
The command above runs the program silently (no message is sent to standard output) and all the messages are written to a file with extension `.log` created in the program execution directory.
If you want the program to write messages to standard output, add a third argument in the command line.
As an example
```
./bin/iio instancefile.txt cfgs/iio.cfg stdo

```

The program writes the optimization results to a text file with extension `.optres` created in the execution directory.
The file contains a single line of space-separated values.
Comments in the SQL file [sql/result.sql](sql/result.sql) describe the space-separated values as they are written into the `.optres` file by the program.

**Notice** that, unlike the other implemented algorithms, the run times corresponding to the fields `runtime` and `doublev9` in the [SQL table](sql/result.sql) written by the Matryoshka algorithm in the `.optres` file include only the IIO time, not the initial solution time.
To obtain the total run time, sum the values of `doublev9` and `doublev1`.

The parameter `MYEPS` in [src/util.h](./src/util.h) allows fine-tuning of numerical precision when handling instances of different sizes.
Its default value is `1e-9`, which is suitable for most instances. For very large-scale problems (e.g., with 512<sup>4</sup> or more variables), it is recommended to reduce this value to `1e-12` to improve numerical stability.

Compilation and sample instance solution tests have been run also on a machine running Windows operating system.


## Instructions to Replicate the Paper Experiments
To replicate the paper experiments, follow the steps below.

* **Download the DOTmark dataset** from this [link](https://www.stochastik.math.uni-goettingen.de/index.php?id=215/).
* **Extract the dataset**: unzip the file into the repository's root directory. You may rename the directory (e.g., `DOTmark`).
* **Compile the main program**: run `make -j8` in the root directory.
* **Generate the instances**
    * For instances with up to 256⁴ variables, run this [script256](scr/exprinstances.sh), e.g.:

```bash
./scr/exprinstances.sh DOTmark/Data
```

* For the sample of instances each with 512⁴ variables, run this other [script512](scr/exprinstances512.sh), e.g.:

```bash
./scr/exprinstances512.sh DOTmark/Data
```

* **Locate the generated instances**
    * 256⁴ instances: directory matching pattern `allinstances.*` (e.g., `allinstances.wKAKN8Hc`);
    * 512⁴ instances: directory matching pattern `allinstances512.*` (e.g., `allinstances512.cvnrk5qf`).
* **Instance file names**: all instances follow the pattern `OF*_*.txt`.
* **Size of the instance set**: keep in mind that the whole instance set you generate will occupy ~6GB on your disk.

### Solving Instances
**For 256⁴ instances**, use the already compiled binary and run the commands below (the directory name follows the example).
```bash
(cd allinstances.wKAKN8Hc/ && \
    find . -type f -name "OF*_*.txt" -exec ../bin/iio {} ../cfgs/matryiio4dtmrk.cfg && \
    find . -type f -name "OF*_*.txt" -exec ../bin/iio {} ../cfgs/iio4dtmrk.cfg)
```

**For 512⁴ instances** (requires recompilation):

1. Update `MYEPS` value in [src/util.h](src/util.h) from `1.0e-9` to `1.0e-12`.
2. Recompile: `make clean && make -j8`.
3. Run the solver on the 512⁴ directory of instances.

**Algorithm configurations**

* MATR-IIO4D: Use `cfgs/matryiio4dtmrk.cfg`
* IIO4D: Use `cfgs/iio4dtmrk.cfg`

### Generating Results

To create formatted result tables (requires [PostgreSQL](https://www.postgresql.org/)), run the following commands:
```bash
./scr/getresults.sh <dbname> <sql> <dir1>
./scr/getresults.sh <dbname> <sql> <dir2>
```
Where:

* `<dbname>` is your PostgreSQL database name;
* `<sql>` is the path to the [sql](sql/) directory;
* `<dir1>`, `<dir2>` are the instance directories (e.g., `allinstances.wKAKN8Hc`, `allinstances512.cvnrk5qf`).

## Ongoing development and support
This code is being developed on an ongoing basis.
You may want to check out the code main developer's [GitHub site](https://github.com/robertobarg).

For support in using this software, submit an
[issue](https://github.com/robertobarg/IteratedInsideOut/issues/new).

To be in touch, [send us an email](mailto:roberto.bargetto@polito.it;federico.dellacroce@polito.it;rosario.scatamacchia@polito.it?cc=roberto.bargetto@gmail.com&subject=IIO%20Repo%20-%20Question).


## License
Repository license file [LICENSE](LICENSE).

Shield: [![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg