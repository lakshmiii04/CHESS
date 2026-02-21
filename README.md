# ♟️ Chess — 2D Chess Game in C using SDL

A fully playable desktop Chess game built using **C and SDL3 graphics library**.
This project implements real chess mechanics, graphical rendering, and user interaction without using any game engine.



## 🎮 Features

* Complete chess board with graphical pieces
* Mouse-based movement
* Correct movement rules for all pieces
* Path blocking for rook, bishop and queen
* Check detection
* Pawn promotion (Q / R / B / N)
* Restart game (`R`)
* Draw match (`D`)
* Turn indicator in window title



## 🧠 Concepts Used

* 2D rendering using SDL3
* Event driven programming
* Game state management
* Collision / path checking logic
* Board representation using 2D arrays
* Keyboard and mouse input handling
* Texture loading & transparency


## 🖥️ Controls

| Action     | Key                        |
| ---------- | -------------------------- |
| Move piece | Click piece → Click square |
| Restart    | R                          |
| Draw       | D                          |
| Promotion  | Q / R / B / N              |



## 📂 Project Structure


SDL-Chess/
 ├─ chessgame.c
 ├─ assets/
 │   ├─ wp.bmp
 │   ├─ bp.bmp
 │   ├─ ...
 └─ README.md


## ⚙️ Build & Run

### Windows (MinGW)


gcc chessgame.c -o chessgame -lSDL3
./chessgame


Make sure `SDL3.dll` is in the same folder.




## 🚀 Future Improvements

* Checkmate & stalemate detection
* Castling & en passant
* Drag-and-drop movement
* AI opponent (Minimax)
* Move highlighting

