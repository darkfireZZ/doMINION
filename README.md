<h1>
  <img src="assets/logo.png" alt="Dominion Minion Logo" width="200" style="vertical-align: middle;">
  doMINION
</h1>

This project was created as part of the course "Software Engineering" at ETH
Zürich.
It is a digital version of the card game Dominion, which is a deck-building
game created by Donald X. Vaccarino and published by Rio Grande Games.

The project was originally developed on an internal GitLab repository of ETH
Zürich.
This is a clone of the repository, which I created to make the project public.

## Installation

### Ubuntu 24.04

First, install all dependencies of the project:
```
apt-get update && apt-get install \
    cmake \
    libwxgtk3.2-dev
```

If you also want the development dependencies:
```
apt-get update && apt-get install \
    clang \
    clang-tidy \
    cppcheck
```

Then you can build the project with
```
mkdir -p build
cd build
cmake ..
make
```


### Ubuntu 22.04

We provide a script that does all the work for you.
The script will install all dependencies and build the project.
Just run the following command:
```
./scripts/ubuntu_setup.sh
```

### Other Platforms

The project should also work on other UNIX-like systems.
However, we only provide instructions for Ubuntu 22.04 and 24.04.
We have also tested the project on
 - macOS
 - Arch Linux
 - Manjaro Linux

## Running the Project

After building the project (for example by using the compile.sh script in the "scripts" folder), 2 executables are created:
 - `client_exe`
 - `server_exe`

To get a list of all available options, run either of the executables with the
`--help` flag.

### Running Locally

If you want to test the project locally, you can run the server and client on
the same machine or on different machines in the same network.

First, start the server:
```bash
./server_exe
```

Then, start anywhere from 2 to 4 clients
```bash
./client_exe
```

### Running on `se.nicolabruhin.com`

If you like, you can also play a round of Dominion on our server.
To do so, start the client with the following command:
```bash
./client_exe
```

You will be prompted to enter the server address.
Just enter `se.nicolabruhin.com` and you are good to go.

## Rules of the Game
See [Dominion Strategy Wiki on Gameplay](https://wiki.dominionstrategy.com/index.php/Gameplay) 
for a very detailed description of the rules. 
Some of which are entirely unnecessary for playing this game as they are only 
featured in extensions of the base game that weren't implemented in this project.

As a very basic structure the game will be played as follows:

1. After all players have joined the game master (person who created the lobby)
may press the card selection button. There they will be faced with a screen where
they can choose 10 so called kingdom cards to play with. For beginners we recommend 
using our random selection feature which will save you the hassle of counting.
2. After the kingdom cards have been selected the game will start. 
The game master will also be the first player. From now on players will take turns of the following structure until either 3 piles are emptied or there are no province cards left.
    1. Action Phase: The player may play action cards from their hand. These cards will have a variety of effects that can be read on the card itself.
    The player starts with one available action (one card to play) but can increase this number by playing cards that give additional actions. If a player has no more actions or action cards left, or presses the end actions the game will automatically switch to: 
    2. Buy phase: The player can now buy cards from the board. The amount of money they have available is displayed on the left hand side of the screen as well as the number of individual buys they have left. As soon as no buys are left or the player presses the end turn button (this can also happen during action phase) the next player's turn will begin.

### Some visual cues
- Cards that can be played during the action phase will be highlighted in green.
- Cards that can be bought or otherwise gained from the board through an action cards will be colored in where the others will be greyed out.
- Be careful both the above happen on one click and can't be undone.
- Sometimes you'll have to select some cards from your hand. During these phases the background of the cards will go red and you can select them by clicking once, deselect by clicking once again and confirm your selection by pressing the confirm button. Check how many cards you have to select by reading the card carefully or checking with which choices the confirm button activates
- You can have a closer look at any card by right-clicking it. Close the pop-up before continuing.


## Developing

In order to develop on the project, it is best to use the provided `Dockerfile` to create a development environment.

### Building the Docker Image

To build the Docker image, run the following command in the root of the project:
```bash
cd docker
docker compose run --build --rm dev
```

This will build the image if necessary, start a container with the project mounted at `/dominion`
and drop you into a shell in the container.
The image provides all the necessary tools to build and test the project.

### Building and testing the Project

Generate the build files:
```bash
mkdir -p build
cd build
cmake ..
```

In the build directory you can now do the following:

Build the project:
```bash
cmake --build .
```

Run the tests:
```bash
cmake --build . --target test
```

Format the code (`clang-format`):
```bash
cmake --build . --target format
```

Run the linter (`cppcheck`):
```bash
cmake --build . --target check
```

### Before Pushing

Before pushing your changes, make sure to:
 - Run the tests
 - Format the code
 - Run the linter

## Documentation

Documentation is provided using Doxygen.

### How to Generate the Documentation

To create the documentation, run the following command from the root directory:

```bash
(cd doxygen && doxygen Doxyfile)
```

### Open the Documentation

After generating, open the documentation:

- On macOS:
    ```bash
    open doxygen/html/index.html
    ```
- On Linux:
    ```bash
    xdg-open doxygen/html/index.html
    ```

### Installing Doxygen

If Doxygen is not installed on your system, install it using the following commands:

- **Ubuntu/Debian**:
    ```bash
    sudo apt install doxygen
    ```
- **macOS**:
    ```bash
    brew install doxygen
    ```

### Debugging

If you encounter issues, here are some common fixes:

#### `warning: Graphviz is not installed or found.`
Graphviz is required for generating diagrams. Install it using:

- **Ubuntu/Debian**:
    ```bash
    sudo apt install graphviz
    ```
- **macOS**:
    ```bash
    brew install graphviz
    ```

#### `warning: pdflatex not found, unable to generate LaTeX output`
LaTeX is required for PDF documentation generation. Install it using:

- **Ubuntu/Debian**:
    ```bash
    sudo apt install texlive-latex-base texlive-latex-extra texlive-fonts-recommended
    ```
- **macOS**:
    ```bash
    brew install --cask mactex
    ```

## Accreditations
This project was made in the context of the course "Software Engineering" at ETH Zurich.
All images were either taken from [Dominion Strategy Wiki](https://wiki.dominionstrategy.com/index.php?title=Category:Card_images&fileuntil=EmbargoOld.jpg#mw-category-media)
under the Creative Commons Attribution license
or created by ourselves using image generating AI.

