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
* `<img_size>` is a tag for the size of the images,
* `<objf>` is the distance function (1: Euclidean, 2: Squared Euclidean, 3: 1-norm, 4: infinity-norm),
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

Compilation and sample instance solution tests have been run also on a machine running Windows operating system.


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