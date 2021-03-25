# Introduction-to-programmingII_Mini-Project-3
Design and implement an A.I. that can play the boardgame Othello.

### Tree Search
##### minimax
![image](https://user-images.githubusercontent.com/56674338/112515995-c48a2080-8dd1-11eb-941e-46042c4b40b2.png)
##### alpha-beta pruning
![image](https://user-images.githubusercontent.com/56674338/112516087-e08dc200-8dd1-11eb-9298-415e3feddcbd.png)

### State Value Function Design
stability：不會被移動的棋子（從角落連續的邊）

weight：每個位置分配一個值（角落最高，危險區是負值）

mobility（高到低）：下一步可走的數量

difference（遊戲結束時才看）：我方棋子數與敵方棋子數差
