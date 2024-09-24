## Introduction
* This repository provides implementations of our algorithms for the top-k weighted stabbing query processing problem.
* The details of our algorithms can be found [here](https://arxiv.org/abs/2405.05601).

## Requirement
* Linux OS (Ubuntu).
   * The others have not been tested.
* `g++ 11.3.0` (or higher version).

## How to use
* Parameter configuration can be done via txt files in `parameter` directory.
* Dataset should be stored in `dataset` directory.
	* We assign a unique dataset ID for each dataset. You can freely assign it.
	* In `input_data()` of `utils.hpp`, you can freely write codes for reading your dataset.
	* The default file format is `left,right`.

## Citation
If you use our implementation, please cite the following paper.
``` 
@inproceedings{amagata2024efficient,  
    title={Efficient Algorithms for Top-k Stabbing Queries on Weighted Interval Data},  
    author={Amagata, Daichi and Yamada, Junya and Ji, Yuchen and Takahiro, Hara},  
    booktitle={DEXA},  
    pages={146--152},  
    year={2024}  
}
``` 

## License
Copyright (c) 2023 Daichi Amagata  
This software is released under the [MIT license](https://github.com/amgt-d1/top-k-weighted-stabbing/blob/main/license.txt).
