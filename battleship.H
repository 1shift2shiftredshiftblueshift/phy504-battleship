#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <random>
///
/// Global variables, including initializing one random number generator for the whole program and defining the distribution
/// that the computer will use to determine if it's guessing randomly or on a ship. 
///
std::random_device rd;
std::mt19937 gen(rd()); 
std::uniform_real_distribution<double> attack_dist(0,1); 
int GRID_SIZE = 10;


/// Forward declarations
class Ship;
class Grid;
std::ostream& operator<< (std::ostream& os, Grid& grid);
void print_hidden(Grid& g);
std::vector<int> make_safe_coords(int size, int x, int y, char direction);
int validate_ship(Grid& grid, int ship_size, int x, int y, char direction);
Ship read_in_ship(std::string ship_name, int ship_size, Grid &grid);
Grid make_computer_grid(std::vector<std::string> names, std::vector<int> lengths);
void attack_of_the_computer(Grid& player_grid);
void attack(Grid& grid, int attack_coord, bool show_ships);
int coord_to_grid(int x,int y);
std::vector<int> grid_to_coord (int c);

///
/// The Ship class represents a ship that lives on a grid. 
///
class Ship {
    private:
        int size; //Length of ship in number of squares
        std::string name; //Ship name
        std::vector<int> safe_coords; //Grid coordinates occupied by this ship that have not been hit
        std::vector<int> hit_coords; //Grid coordinates occupied by this ship that have been hit 
    public:
        ///
        /// Constructor.
        ///
        Ship(std::string _name, int _size, int _x, int _y, char direction) { //constructor 
            name = _name;
            size = _size; 
            hit_coords = std::vector<int>{};
            safe_coords = make_safe_coords(_size,_x,_y,direction);
        }
    
        /// 
        /// Returns the integer length of the ship in squares. 
        /// 
        int get_size() { return size;}
        /// 
        /// Returns the string name of the ship. 
        /// 
        std::string get_name() {return name;}
        /// 
        /// Returns the vector<int> list of coordinates inhabited by the ship that have not yet been attacked. These are 
        /// flattened grid indices. 
        /// 
        std::vector<int> get_safe_coords() {return safe_coords;}
        /// 
        /// Returns the vector<int> list of coordinates inhabited by the ship that have been attacked. These are flattened
        /// grid indices. 
        /// 
        std::vector<int> get_hit_coords() {return hit_coords;}
        
        ///
        /// Hit a square of the ship by removing the attacked coordinate from safe_coords and appending it to hit_coords. 
        /// Takes an integer coordinate to attack, in flattened grid index form. 
        ///
        void hit(int coord) { 
            hit_coords.push_back(coord); 
            safe_coords.erase(std::remove(safe_coords.begin(), safe_coords.end(), coord), safe_coords.end());
        }

};

///
/// The Grid class represent's a player's (or the computer's) board, inhabited by ships. 
///
class Grid { 
    private:
        std::vector<int> open_coords; //Grid indices of coordinates not occupied by ships
        std::vector<int> ship_coords; //Grid indices of coordinates occupied by ships 
        std::vector<Ship> ships; //Vector of ships within the grid
        std::vector<int> hit_coords; //Grid indices of coordinates that have been attacked. A coordinate belongs to exactly  
                                     //one of open_coords, ship_coords, or hit_coords. 
        std::vector<int> hit_ship_coords; //Grid indices that belonged to a ship but have been hit. Kind of a silly kludge.
    public:

        ///
        /// Constructor. 
        ///
        Grid() {
            ship_coords=std::vector<int>{};
            for (int i=0; i<GRID_SIZE*GRID_SIZE;i++) {
                open_coords.push_back(i); //Populates open_coords with every grid_index. This will be updated in each phase.
            }
            hit_coords = {};
            ships = {};
            ship_coords = {};
        }

        ///
        /// Returns the vector<int> of coords that neither are occupied by a ship nor have been attacked.
        ///
        std::vector<int> get_open_coords() {return open_coords;}
        ///
        /// Returns the vector<int> of coords that are occupied by a ship but have not been attacked.
        ///
        std::vector<int> get_ship_coords() {return ship_coords;}
        ///
        /// Returns the vector<int> of coords that have been attacked.
        ///
        std::vector<int> get_hit_coords() {return hit_coords;}

        ///
        /// Returns the vector<Ship> of ships inhabiting this grid.
        ///
        std::vector<Ship> get_ships() {return ships;}

        ///
        /// Returns the vector<int> of grid indices that belong to ships and have been hit.
        ///
        std::vector<int> get_hit_ship_coords() {return hit_ship_coords;}

        ///
        /// Adds a ship to the grid. Takes a Ship object to add, and removes the coordinates it inhabits from 
        /// open_coords and adds then to ship_coords. Also adds the Ship object to ships.
        ///
        void add_ship(Ship& ship) {
            for (auto c : ship.get_safe_coords()) {
                ship_coords.push_back(c); 
                open_coords.erase(std::remove(open_coords.begin(), open_coords.end(), c), open_coords.end());
            }
            ships.push_back(ship);
        }

        ///
        /// Attacks a location on the grid. Takes an integer grid index to attack, and removes this index from open_coords 
        /// and ship_coords and adds it to hit_coords. Then, calls Ship.hit(). 
        ///
        void hit(int coord) {
            hit_coords.push_back(coord);
            open_coords.erase(std::remove(open_coords.begin(), open_coords.end(), coord), open_coords.end());
            ship_coords.erase(std::remove(ship_coords.begin(), ship_coords.end(), coord), ship_coords.end());
            for (Ship s : ships) {
                std::vector<int> safe_coords = s.get_safe_coords();
                if (std::find(safe_coords.cbegin(),safe_coords.cend(),coord) != safe_coords.cend()) {
                    s.hit(coord);
                    hit_ship_coords.push_back(coord);
                }
            }
        }

        ///
        /// Forward declaration of the overloaded insertion operator. 
        ///
        friend std::ostream& operator<< (std::ostream& os, const Grid& grid);
};

///
/// Overload of the insertion operator for the Grid class. Prints an underscore for an open coordinate, an x for a ship, and
/// an o for a coordinate that has been attacked. Also prints an index above and to the left of the grid. 
///
std::ostream& operator<< (std::ostream& os, Grid& grid) {
    std::vector<std::vector<char>> physical_grid{};
    for (int r=0;r<GRID_SIZE;r++) {
        physical_grid.push_back(std::vector<char>{});
        for (int c=0;c<GRID_SIZE;c++) {
            physical_grid[r].push_back('_');
        }

    }
    std::vector<int> ship_coords = grid.get_ship_coords();
    for (int s: ship_coords) {
        physical_grid[s/GRID_SIZE][s%GRID_SIZE] = 'x';
    }
    for (int s: grid.get_hit_coords()) {
        physical_grid[s/GRID_SIZE][s%GRID_SIZE] = 'o';
    }

    os << "  0 1 2 3 4 5 6 7 8 9" << std::endl;
    for (int i=0; i<GRID_SIZE;i++) {
        os << i << " ";
        for (int j=0; j<GRID_SIZE;j++) {
            os << physical_grid[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}

///
/// Prints out a grid that shows only the attacked locations and whether they hit or not, not the ship locations. Takes the 
/// grid object to be printed out. 
///
void print_hidden(Grid& g) {
    const auto ship_coords = g.get_ship_coords();
    const auto hit_coords = g.get_hit_coords();
    const auto ships = g.get_ships();
    const auto hit_ship_coords = g.get_hit_ship_coords();
    std::vector<std::vector<char>> physical_grid{};
    
    for (int r=0;r<GRID_SIZE;r++) {
        physical_grid.push_back(std::vector<char>{});
        for (int c=0;c<GRID_SIZE;c++) {
            physical_grid[r].push_back('_');
        }
    }

    for (int c : hit_coords) {
        physical_grid[c/GRID_SIZE][c%GRID_SIZE] = 'o';
    }
    
    for (int c : hit_ship_coords) {
        physical_grid[c/GRID_SIZE][c%GRID_SIZE] = 'x';
    }
    
    std::cout << "  0 1 2 3 4 5 6 7 8 9" << std::endl;
    for (int i=0; i<GRID_SIZE;i++) {
        std::cout << i << " ";
        for (int j=0; j<GRID_SIZE;j++) {
            std::cout << physical_grid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}


/// 
/// Make a list of coordinates inhabited by a ship. Takes the ship's integer size, the x and y coordinates of the edge space,
/// and the direction (u, d, r, or l) that the rest of the ship will go. Returns a vector<int> of grid indices inhabited by 
/// the ship. Used in validating ship placement and populating a Grid object's ship_coords. 
/// 
std::vector<int> make_safe_coords(int size, int x, int y, char direction) { 
    std::vector<int> safe_coords{};
    if (direction == 'u') { //If the ship is oriented up from x and y
        for (int i=0; i<size; i++) { //populate the ship coordinates
            safe_coords.push_back((y - i)*GRID_SIZE + x);
        }
    } else if (direction == 'd') { //Down
        for (int i=0; i<size; i++) {
            safe_coords.push_back((y+i) *GRID_SIZE + x);
        }
    } else if (direction == 'l') { //Left 
        for (int i=0; i<size; i++) {
            safe_coords.push_back(y*GRID_SIZE + x - i);
        }
    } else { //Assume direction is right otherwise 
        for (int i=0; i<size; i++) {
            safe_coords.push_back(y*GRID_SIZE + x + i);
        }
    }
    return safe_coords;
}

/// 
/// Checks a potential ship to make sure it doesn't overlap an existing ship or go out of bounds. Takes the Grid object that
/// the ship will inhabit, the int length of the ship, the int x and y location of the corner of the ship, and the char
/// direction that the ship will go. Returns 0 if the placement is valid, 1 if the ship overlaps another ship, and -1 if the
/// ship would go out of bounds. 
/// 
int validate_ship(Grid& grid, int ship_size, int x, int y, char direction) { 
    std::vector<int> other_ship_coords = grid.get_ship_coords(); 
    std::vector<int> coords_for_temp_ship = make_safe_coords(ship_size,x,y,direction);
    if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) { //Check if the edge of the ship is out of bounds
        return -1;
    }
    for (auto c : coords_for_temp_ship) { //Check if the ship would overlap any other ships
        if(std::find(other_ship_coords.cbegin(), other_ship_coords.cend(), c) != std::cend(other_ship_coords)) {
            return 1;
        }
        if ( c < 0 || c >= GRID_SIZE*GRID_SIZE || (c%GRID_SIZE != x & c/GRID_SIZE != y)) { //Check if the ship would go out
                                                                                           //of bounds
            return -1;
        }
    }
    return 0; //Otherwise, the ship placement is valid! 
}

///
/// Handles user input of a ship. Takes the string name of the ship, the int length of the ship, and the grid object the
/// ship will inhabit, and returns a Ship object. This is where protection against out-of-bounds and overlapping ships 
/// happens, and where the loop to ask the user to try again after a faulty placement occurs. 
///
Ship read_in_ship(std::string ship_name, int ship_size, Grid &grid) {
    auto other_ship_coords = grid.get_ship_coords(); //List of coordinates already occupied by ships in this grid
    int x=-1;
    int y=-1;
    int xt; //temporary x: for reading in 
    int yt; //temporary y: for reading in
    char direction;
    do {
        std::cout << "Enter the x coordinate for your " << ship_name << " (" << ship_size << " squares): ";
        std::cin >> xt;
        std::cout << std::endl << "Enter the y coordinate for your " << ship_name << " (" << ship_size << " squares): ";
        std::cin >> yt;
        std::cout << std::endl << "Enter the direction for your " << ship_name << " (" << ship_size << " squares): ";
        std::cin >> direction;
        std::vector<int> coords_for_temp_ship = make_safe_coords(ship_size,xt,yt,direction);
        int val = validate_ship(grid,ship_size,xt,yt,direction);
        if (val == -1) {
            std::cout << "Your ship will go out of the board. Try again. "<<std::endl;
        } else if (val == 1) {
            std::cout << "Your ship overlaps another ship. Try again" << std::endl;
        } else if (val == 0) {
            x = xt;
            y = yt;
        } else {
            std::cout << "Congrats! You've found an error message that should never be printed! Try again I suppose?" <<std::endl;
        }
    } while (x < 0 || y < 0);
    Ship rtn = Ship(ship_name, ship_size, x, y, direction);
    grid.add_ship(rtn);
    return rtn;
}

/// 
/// Generates & returns a Grid object for the computer grid. Takes a vector<string> of ship names and a vector<int> of ship
/// lengths, and randomly places each ship on the grid (with protection against overlap & out of bounds). Returns the Grid 
/// object. 
/// 
Grid make_computer_grid(std::vector<std::string> names, std::vector<int> lengths) {
    Grid g = Grid();
    std::random_device rd;
    std::vector<char> directions{'u','d','r','l'}; //for choosing a random direction and calling validate_ship
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<int> coord_dist(0, GRID_SIZE-1); //for generating a location to put the ship
    std::uniform_int_distribution<int> direction_dist(0,3); //0 is up, 1 is down, 2 is right, 3 is left
    char direction;
    int xt;
    int yt;
    int val;
    for (int i=0; i < lengths.size(); i++) {
        do {
            direction = directions[direction_dist(gen)]; 
            xt = coord_dist(gen);
            yt = coord_dist(gen);
            val = validate_ship(g,lengths[i],xt,yt,direction);
            if (val == 0) { //Protection against overlap and out-of-bounds
                g.add_ship(Ship(names[i],lengths[i],xt,yt,direction));
            }
        } while (val != 0);
    }    
    return g;
}

///
/// Generates an attack by the computer. The computer doesn't play smart (i.e. guessing adjacent squares once it hits). 
/// Instead, it attacks randomly 50% of the time and is guaranteed to hit 50% of the time. This is designed to simulate 
/// playing with an opponent. Takes the player's Grid object and calls attack(). 
///
void attack_of_the_computer(Grid& player_grid) { 
    auto hit_coords = player_grid.get_hit_coords();
    int attack_coord = -1; // Attack location
    ///
    /// Determine whether the computer is attacking randomly or guaranteed to hit
    ///
    if (attack_dist(gen) <= 0.5) { //Guess randomly
        std::uniform_int_distribution<int> coord_dist(0, GRID_SIZE*GRID_SIZE-1);
        int g;
        do {
            g = coord_dist(gen);
            if (std::find(hit_coords.cbegin(),hit_coords.cend(),g) == hit_coords.cend()) { //Computer won't repeat an attack
                attack_coord = g;
            }
        } while (attack_coord < 0);
    } else { //Attack a ship coordinate
        auto ship_coords = player_grid.get_ship_coords();
        std::uniform_int_distribution<int> coord_dist(0, ship_coords.size()-1);
        attack_coord = ship_coords[coord_dist(gen)];
    }
    auto attack_grid = grid_to_coord(attack_coord);
    std::cout << " " << attack_grid[0] << " " << attack_grid[1] << std::endl;
    attack(player_grid, attack_coord,true); 
}

/// 
/// Attacks a coordinate on a grid. Takes the grid to be attacked and the location to attack, and calls grid.hit(). 
/// 
void attack(Grid& grid, int attack_coord, bool show_ships) {
    auto ship_coords = grid.get_ship_coords();
    auto hit_coords = grid.get_hit_coords();
    if (std::find(hit_coords.cbegin(), hit_coords.cend(), attack_coord) != hit_coords.cend()) {
        std::cout << "Already hit that coordinate. " << std::endl;
        return; 
    } else if (std::find(ship_coords.cbegin(), ship_coords.cend(), attack_coord) != ship_coords.cend()) {
        for (Ship s : grid.get_ships()) {
            const auto& safe_coords = s.get_safe_coords();
            if (std::find(safe_coords.cbegin(), safe_coords.cend(), attack_coord) != safe_coords.cend()) {
                std::cout << "Hit a " << s.get_name() << "! " << std::endl;
            }
        }
    } else {
        std::cout << "Miss." << std::endl;
    }
    grid.hit(attack_coord);
    if (show_ships) {
        std::cout << grid <<std::endl;
    } else {
        print_hidden(grid);
    }
    
    return;
}

/// 
/// Converts an x, y coordinate pair to an int grid index. 
/// 
int coord_to_grid(int x,int y) {return y*GRID_SIZE+x;}

///
/// Converts a grid index to a coordinate pair. Returns a vector containing (x, y). 
/// 
std::vector<int> grid_to_coord (int c) {return std::vector<int>{c%GRID_SIZE,c/GRID_SIZE};}


#endif