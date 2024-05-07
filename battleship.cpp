#include "battleship.H"

///
/// The main function! This handles user input in the setup phase and attack phase, and contains the while loop that runs the game itself. 
///

int main() {
    ///
    /// Setup phase: read in the user ships
    ///
    std::vector<std::string> ship_names{"Carrier","Destroyer","Submarine","Patrol"};
    std::vector<int> ship_lengths{5,4,3,2};
    Grid player_grid = Grid();
    for (int i=0; i<ship_names.size(); i++) {
        read_in_ship(ship_names[i],ship_lengths[i],player_grid);
        std::cout << player_grid;
    }

    Grid computer_grid = make_computer_grid(ship_names,ship_lengths);
    std::cout << std::endl << "Computer grid: " <<std::endl;
    print_hidden(computer_grid);

    bool game_end = false;
    int xt; //For reading in attacks
    int yt;

    ///
    /// Loop to run the attack phase! The user enters a coordinate pair to attack, and the attack() function checks whether it hits anything,
    /// reports which ship it hit, and updates the Ship and Grid objects. Then, attack_of_the_computer generates a location to attack, and
    /// calls attack(). 
    ///
    while (!game_end) {
        std::cout << "Enter the x coordinate of your attack: ";
        std::cin >> xt;
        std::cout << std::endl << "Enter the y coordinate of your attack: ";
        std::cin >> yt;
        std::cout << std::endl;
        int attack_coord = coord_to_grid(xt,yt);
        attack(computer_grid,attack_coord,false);
        
        if (computer_grid.get_ship_coords().size() == 0) {
            game_end = true;
            std::cout << "You win!" << std::endl;
            game_end = true;
            continue;
            ///
            /// The continue statement is here to make sure the computer doesn't get a guess in once the user wins. 
            /// 
        }

        std::cout << std::endl << "Computer attack: " ;
        attack_of_the_computer(player_grid);
        
        if (player_grid.get_ship_coords().size() == 0) {
            std::cout << "Computer wins :(" << std::endl;
            std::cout << "The computer grid was: " <<std::endl;
            std::cout << computer_grid;
            game_end = true;
        }
    }
}