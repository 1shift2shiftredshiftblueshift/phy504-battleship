#include "battleship.H"
#include <cassert>

int main() {
    //Test Ship.hit()
    std::cout << "Testing Ship.hit()" << std::endl;
    Ship ship = Ship("Test 1",5,0,0,'r');
    std::vector<int> safe_coords_before = ship.get_safe_coords();

    ship.hit(0);
    std::vector<int> safe_coords_after = ship.get_safe_coords();
    std::vector<int> hit_coords_after = ship.get_hit_coords();

    assert(hit_coords_after.size() == 1);
    assert(safe_coords_after.size() == safe_coords_before.size() - 1);
    assert(hit_coords_after[0] == 0);

    std::cout << "All tests passed! ";

}