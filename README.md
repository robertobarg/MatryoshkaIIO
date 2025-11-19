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


## Instruction to replicate the paper experiments
If you want to replicate the paper experiments, you have to procede as follows.

* Download the DOTmark dataset that is available at this [link](https://www.stochastik.math.uni-goettingen.de/index.php?id=215/).
* Extract the `.zip` file of the dataset into the root directory of the repository, you can eventually raname the dataset directory; e.g.,`DOTmark`.
* Compile the main program using the provided Makefile, e.g., run the command `make -j8` in the root directory of the repository.
* Generate all the instances with up 256<sup>4</sup> variables using this [script256](scr/exprinstances.sh) passing the first single argument the path to directory `Data` in the parent dataset directory; e.g., run the command  `./scr/exprinstances.sh DOTmark/Data`  in the root directory of the repository.
* All the generated instances are collected in a directory matching pattern `allinstances.\*` (for example, `allinstances.wKAKN8Hc`).
* Note that the name of every generated instance matches the pattern `OF*_*.txt`, and every instance file is a TXT file.
* In the same way, you can generate the sample of instances with 512<sup>4</sup> variables using another script, this [script512](scr/exprinstances512.sh).
* To solve the instance generated with [script256](scr/exprinstances.sh), you can use directly the compiled executable file `bin/iio`.
* To solve the instance generated with [script512](scr/exprinstances512.sh), you need to recompile the source code after having updated the macro `MYEPS` definition in `src/util.h` from `1.0e-9` to `1.0e-12`, then recompile.
* You can solve by means of the algorithms MATR-IIO4D and IIO4D all the generated instances using the algorithm configuration files `cfgs/matryiio4dtmrk.cfg` and `cfgs/iio4dtmrk.cfg` respectively.
* Once you have solved all or part of the instances, you can generate gracefully formated tables of results using this [script](scr/getresults.sh); note that for using that script you need to have installed [postgreSQL](https://www.postgresql.org/).

Let's see below a detailed example of commands that assumes the current directory to be the root directory of the repository.

To generate the set of instances with up 256<sup>4</sup> variables and sample of instances with 512<sup>5</sup> variables, run in the command line
````
./scr/exprinstances.sh DOTmark/Data
./scr/exprinstances512.sh DOTmark/Data
````
Let `allinstances.wKAKN8Hc` be, as an example, the created directory collecting all the instances generated with the first command.
To solve all these instance with both MATR-IIO4D and IIO4D, run in the command line
````
(cd allinstances.wKAKN8Hc/ && \
    find . -type f -name "OF*_*.txt" -exec ../bin/iio {} ../cfgs/matryiio4dtmrk.cfg && \
    find . -type f -name "OF*_*.txt" -exec ../bin/iio {} ../cfgs/iio4dtmrk.cfg)
````
The command above solve the instances one at a time.
If you a large computation power, you may prefer to articulate an advance command using, perhaps, `GNU parallel`.

After having updated the macro `MYEPS` definition in `src/util.h` from `1.0e-9` to `1.0e-12`, and recompiled the C++ code, you can solve the set instances with 512<sup>4</sup> variables in the directory with a name that looks like `allinstances512.cvnrk5qf`.

To collect all the optimization results and obtain tables like those reported in the directory [results](results/), run in command line
````
./scr/getresults.sh <dbname> <sql> <dir1>
./scr/getresults.sh <dbname> <sql> <dir2>
````
where
* `<dbname>` is the name of the postgreSQL database you have created;
* `<sql>` is the path to the directory [sql](sql/), and
* `<dir1>` and `<dir2>` are, in our example `allinstances.wKAKN8Hc` and `allinstances512.cvnrk5qf` respectively.


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