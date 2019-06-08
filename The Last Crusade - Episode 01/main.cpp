/**
    This is my solution to the challenge "The last crusade - Episode 01" on Codingame.

    Challenge link : https://www.codingame.com/ide/puzzle/the-last-crusade-episode-1
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <sstream>

using namespace std;

/**
 * Direction common enum
 */
enum Direction{TOP,RIGHT,BOTTOM,LEFT};

/**
 * Tile types
 */
enum TileType{TYPE0, TYPE1, TYPE2, TYPE3, TYPE4, TYPE5, TYPE6, TYPE7, TYPE8, TYPE9, TYPE10, TYPE11, TYPE12, TYPE13};


/**
 * This class stores a standard integer vector2.
 * Can be used as a key for maps.
 */
class Vector2i
{
    public:
        int x;
        int y;

        Vector2i() : x(0), y(0) {}
        Vector2i(int ox, int oy) : x(ox), y(oy) {}
        Vector2i(const Vector2i &vector) = default;

        bool operator <(const Vector2i& v) const
        {
            /*
             * Sorts so that
             * 0 1 2 3
             * 4 5 6 7
             * 8 9 10 11
             * ....
             */
            return (y < v.y) || (x < v.x && y == v.y) ;
        }
};

/**
 * Tile class that describes what a Tile is on the map.
 */
class Tile
{
    public:
        Tile() : m_position(), m_type(TileType::TYPE0), m_paths(), m_neighborhood()
        {
            initNeighborhood();
        }

        explicit Tile(TileType type) : m_position(), m_type(type), m_paths(), m_neighborhood()
        {
            initNeighborhood();
        }

        Tile(Vector2i position, TileType type) : m_position(position), m_type(type), m_paths(), m_neighborhood()
        {
            initNeighborhood();
        }

        Tile(int x, int y, TileType type) : m_position(x,y), m_type(type), m_paths(), m_neighborhood()
        {
            initNeighborhood();
        }

        ~Tile() = default;
        Tile(const Tile &tile) = default;

        void setPosition(int x, int y)
        {
            m_position = Vector2i(x,y);
        }

        void setPosition(const Vector2i &position)
        {
            m_position = position;
        }

        void setNeighborhood(Tile* top_tile, Tile* right_tile, Tile* bottom_tile, Tile *left_tile)
        {
            m_neighborhood[Direction::TOP] = top_tile;
            m_neighborhood[Direction::RIGHT] = right_tile;
            m_neighborhood[Direction::BOTTOM] = bottom_tile;
            m_neighborhood[Direction::LEFT] = left_tile;
        }

        void setTileTop(Tile* top_tile)
        {
            m_neighborhood[Direction::TOP] = top_tile;
        }

        void setTileRight(Tile* right_tile)
        {
            m_neighborhood[Direction::RIGHT] = right_tile;
        }

        void setTileBottom(Tile* bottom_tile)
        {
            m_neighborhood[Direction::BOTTOM] = bottom_tile;
        }

        void setTileLeft(Tile* left_tile)
        {
            m_neighborhood[Direction::LEFT] = left_tile;
        }

        void addPath(const Direction input, const Direction output)
        {
            m_paths[input] = output;
        }

        Vector2i getPosition() const
        {
            return m_position;
        }

        TileType getType() const
        {
            return this->m_type;
        }

        /**
         * Gives the output Tile for an input on the current Tile.
         *
         * @param Direction input The direction from which the character enters the Tile.
         *
         * @return Tile* The next Tile the character will enter.
         */
        Tile* getOutputTile(Direction input)
        {
            if (m_paths.find(input) != m_paths.end())
            {
                return m_neighborhood[m_paths[input]];
            }
            else // Can't enter this tile from this direction
            {
                return nullptr;
            }
        }

    private:
        void initNeighborhood()
        {
            m_neighborhood[Direction::TOP] = nullptr;
            m_neighborhood[Direction::RIGHT] = nullptr;
            m_neighborhood[Direction::BOTTOM] = nullptr;
            m_neighborhood[Direction::LEFT] = nullptr;
        }

        Vector2i m_position;
        TileType m_type;
        map<Direction,Direction> m_paths; // Keys : input, Values : Output
        map<Direction, Tile*> m_neighborhood;
};

/**
 * Factory that creates correct Tile instances for the map.
 */
class TileFactory
{
    public :
        /**
         * Creates an object Tile for the map with all its paths settled.
         * The output tile will need further settings to be usable like its position in the map (default 0,0) and its 
         * neighborhood (these settings must be set by the Map object).
         * 
         * @param Tiletype type Type of the new tile. 
         * 
         * @return Tile the new tile.
         */
        static Tile createTile(TileType type)
        {
            Tile tile(type);

            switch (type)
            {
                case TileType::TYPE1 :
                    tile.addPath(Direction::TOP, Direction::BOTTOM);
                    tile.addPath(Direction::RIGHT, Direction::BOTTOM);
                    tile.addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE2 :
                    tile.addPath(Direction::RIGHT, Direction::LEFT);
                    tile.addPath(Direction::LEFT, Direction::RIGHT);
                    break;
                case TileType::TYPE3 :
                    tile.addPath(Direction::TOP, Direction::BOTTOM);
                    break;
                case TileType::TYPE4 :
                    tile.addPath(Direction::TOP, Direction::LEFT);
                    tile.addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE5 :
                    tile.addPath(Direction::TOP, Direction::RIGHT);
                    tile.addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE6 :
                    tile.addPath(Direction::LEFT, Direction::RIGHT);
                    tile.addPath(Direction::RIGHT, Direction::LEFT);
                    break;
                case TileType::TYPE7 :
                    tile.addPath(Direction::TOP, Direction::BOTTOM);
                    tile.addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE8 :
                    tile.addPath(Direction::LEFT, Direction::BOTTOM);
                    tile.addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE9 :
                    tile.addPath(Direction::TOP, Direction::BOTTOM);
                    tile.addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE10 :
                    tile.addPath(Direction::TOP, Direction::LEFT);
                    break;
                case TileType::TYPE11 :
                    tile.addPath(Direction::TOP, Direction::RIGHT);
                    break;
                case TileType::TYPE12 :
                    tile.addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE13 :
                    tile.addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                default: // TYPE0 and unknown => no path
                    return tile;
            }
        }
};

/**
 * Map storing all the Tiles and their position.
 */
class Map
{
    public:
        Map() : m_size(0,0), m_mapping() {}
        ~Map() = default;

        /**
         * Adds a Tile to the map
         *
         * @param int x Position in the tiles line.
         * @param int y Position in the tiles column.
         * @param TileType tile_type Type of the Tile.
         */
        void addTile(int x, int y, TileType tile_type)
        {
            Tile newtile = TileFactory::createTile(tile_type);
            newtile.setPosition(x,y);
            Vector2i position(x,y);

            m_mapping[position] = newtile;
            updateTileNeighborhood(&m_mapping[position]);

            m_size.x = max(m_size.x, x+1);
            m_size.y = max(m_size.y, y+1);
        }

        /**
         * Returns next player position for a player falling through the map.
         *
         * @param int x Current player position on tiles line.
         * @param int y Current player position on tiles column.
         * @param Direction input Direction by which the player is coming in the current tile.
         *
         * @return Vector2i Position of the next tile the player will ends up in.
         */
        Vector2i getNextPlayerPosition(int x, int y, Direction input)
        {
            Tile* nextTile = m_mapping[Vector2i(x,y)].getOutputTile(input);

            if (nextTile != nullptr)
            {
                return nextTile->getPosition();
            }
            else
            {
                cerr << "Cell (" << x << "," << y << ") - Type:" << m_mapping[Vector2i(x,y)].getType() << " - Enter:" << input << " >> NO NEXT" << endl;
                return Vector2i(x,y); // cannot move
            }
        }

    private :

        /**
         * Updates the neighborhood of a new tile and all the tiles touching it currently, so that the grid remains 
         * consistent in the mapping.
         *
         * @param Tile* The new tile that has just been added to the Map.
         */
        void updateTileNeighborhood(Tile *tile)
        {
            Vector2i tile_position = tile->getPosition(),
                    top_position(tile_position.x, tile_position.y-1),
                    right_position(tile_position.x+1, tile_position.y),
                    bottom_position(tile_position.x, tile_position.y+1),
                    left_position(tile_position.x-1, tile_position.y)
            ;

            auto it_tile_top = m_mapping.find(top_position),
                    it_tile_right = m_mapping.find(right_position),
                    it_tile_bottom = m_mapping.find(bottom_position),
                    it_tile_left = m_mapping.find(left_position); // iterators on the finding of the neighbours

            /*
             *  -|x|-
             *  -| |-
             *  -|-|-
             */
            if (it_tile_top != m_mapping.end()) // There is a tile above
            {
                it_tile_top->second.setTileBottom(tile);
                tile->setTileTop(&it_tile_top->second);
            }

            /*
             *  -|-|-
             *  -| |x
             *  -|-|-
             */
            if (it_tile_right != m_mapping.end()) // There is a tile on the right
            {
                it_tile_right->second.setTileLeft(tile);
                tile->setTileRight(&it_tile_right->second);
            }

            /*
             *  -|-|-
             *  -| |-
             *  -|x|-
             */
            if (it_tile_bottom != m_mapping.end()) // There is a tile underneath
            {
                it_tile_bottom->second.setTileTop(tile);
                tile->setTileBottom(&it_tile_bottom->second);
            }

            /*
             *  -|-|-
             *  x| |-
             *  -|-|-
             */
            if (it_tile_left != m_mapping.end()) // There is a tile on the left
            {
                it_tile_left->second.setTileRight(tile);
                tile->setTileLeft(&it_tile_left->second);
            }
        }

        Vector2i m_size;
        map<Vector2i, Tile> m_mapping;
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    // ========================================================================
    //                              MAP INIT
    // ========================================================================

    int W; // number of columns.
    int H; // number of rows.
    int currentType;
    cin >> W >> H; cin.ignore();
    Map level_map;
    TileType t;
    string token; // token in a parsed line.
    istringstream iss; // string stream to parse every line
    int j;

    for (int i = 0; i < H; i++)
    {
        string LINE; // represents a line in the grid and contains W integers. Each integer represents one room of a given type.
        getline(cin, LINE);

        iss.str(LINE); // setting the stream content for the parsing.
        iss.clear(); // to be able to parse from the start on the new line (else remains on the end of the last one).

        j = 0;

        for (token; getline(iss, token, ' '); ) // parsing the line
        {
            if (token == "1")
                t = TileType::TYPE1;
            else if (token ==  "2")
                t = TileType::TYPE2;
            else if (token == "3")
                t = TileType::TYPE3;
            else if (token == "4")
                t = TileType::TYPE4;
            else if (token == "5")
                t = TileType::TYPE5;
            else if (token == "6")
                t = TileType::TYPE6;
            else if (token ==  "7")
                t = TileType::TYPE7;
            else if (token == "8")
                t = TileType::TYPE8;
            else if (token == "9")
                t = TileType::TYPE9;
            else if (token == "10")
                t = TileType::TYPE10;
            else if (token == "11")
                t = TileType::TYPE11;
            else if (token == "12")
                t = TileType::TYPE12;
            else if (token == "13")
                t = TileType::TYPE13;
            else
                t = TileType::TYPE0;

            level_map.addTile(j, i, t); // creates a new tile in the map.
            j++;
        }
    }


    // ========================================================================
    //                              GAME LOOP
    // ========================================================================

    int EX; // the coordinate along the X axis of the exit (not useful for this first mission, but must be read).
    cin >> EX; cin.ignore();

    int XI;
    int YI;
    string POS;
    Direction d;
    Vector2i next_pos;

    // game loop
    while (1)
    {
        cin >> XI >> YI >> POS; cin.ignore();

        if (POS == "TOP")
        {
            d = Direction::TOP;
        }
        else if (POS == "RIGHT")
        {
            d = Direction::RIGHT;
        }
        else if (POS == "BOTTOM")
        {
            d = Direction::BOTTOM;
        }
        else if (POS == "LEFT")
        {
            d = Direction::LEFT;
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;
        next_pos = level_map.getNextPlayerPosition(XI,YI,d);

        // One line containing the X Y coordinates of the room in which you believe Indy will be on the next turn.
        cout << next_pos.x << " " << next_pos.y << endl;
    }
}