# HARE
A Fast Prototype for Regulating Highly Automated Robot Ecologies written in C++. 

# Running Environment

The program was built sucessfully with the following environment:

  OS - Mac OSX 10.11
  
  Compiler- Apple LLVM version 8.0.0 (clang-800.0.38)
  
  Target- x86_64-apple-darwin15.6.0
  
  Thread model- posix
  

#Directions

To run the program, please follow the directions listed as below:
  1. Open the terminal, and clone the project by using the command "git clone https://github.com/wenshen/hare.git".
  2. Switch to the directory "../hare/".
  3. Create a data folder "mkdir data".
  4. Build the project by typing "make" in the terminal.
  5. Run the program by the following command: 
      "./JiaoTong 4nodes [numVehicles] [gameLength] [percentLearning] [tollLimitation?] [psuedoName]"
      - [numVehicles]: the number (greater than 0) of vehicles in the game, e.g., 300 denotes there are 300 vehicles. 
      - [gameLength]: the length of the game (in minutes), e.g., 15 denotes a 15-minute game.
      - [percentLearning]: the percentage of learning agents in the game, e.g., 80 denotes 80% of 
                           the vehicles are learning agents.
      - [tollLimitation?]: whether the toll is limited, with "yes" or "no"
      - [psuedoName]: the pseudo name of the human player.
      
Then the player may regulate the vehicles using tolls.
  
The data logs can be found in the "/hare/data" directory. Customerization of the game 
is available by changing the parameters in the file "4nodes.txt" under the directory of "/hare/game".

If you find this repo helfull, we would really appreciate it if you could cite our paper in your work:

@inproceedings{shen2017regulating, 

title={Regulating Highly Automated Robot Ecologies: Insights from Three User Studies},

author={Shen, Wen and Khemeiri, Alanoud Al and Almehrezi, Abdulla and Enezi, Wael Al and Rahwan, Iyad and Crandall, Jacob W.},

booktitle={Proceedings of the Fifth International Conference on Human-Agent Interaction (HAI 2017)}, year={2017},

organization={ACM} 

}
  
For inquiries, please send me an email via wen.shen [at] uci.edu.

      
