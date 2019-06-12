/**
    This is my solution to the challenge "The last crusade - Episode 02" on Codingame.

    Challenge link : https://www.codingame.com/ide/puzzle/the-last-crusade-episode-2
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <queue>

using namespace std;

/**
 * Direction common enum
 */
enum Direction{TOP = 0, RIGHT = 1 , BOTTOM = 2, LEFT = 3};

/**
 * Actions possibile on the tiles.
 */
enum TileAction{ROTATE_LEFT, ROTATE_RIGHT};


/**
 * Tile types
 */
enum TileType{TYPE0, TYPE1, TYPE2, TYPE3, TYPE4, TYPE5, TYPE6, TYPE7, TYPE8, TYPE9, TYPE10, TYPE11, TYPE12, TYPE13, EXIT};


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
        
        bool operator ==(const Vector2i & obj) const
        {
            if (x == obj.x && y == obj.y)
              return true;
            else
              return false;
        }
};


/**
 *  Hash ID for Vector2i to be used in Hashmaps like unordered_map. 
 */
namespace std
{
template<>
    struct hash<Vector2i>
    {
        size_t operator()(const Vector2i& obj) const
        {
            return hash<string>()(obj.x+"|"+obj.y);
        }
    };
}


class Tile;

/**
 * This class describes a path node, consisting of a tile and a direction to enter it.
 * It's ok to let the fields public here to simplify access, since the only purpose of this class is to store them.
 */
class PathNode
{
    public :
        Tile* tile;
        Direction enter_direction;

        PathNode() : tile(nullptr), enter_direction(Direction::TOP) {};
        PathNode(Tile* c_tile, Direction c_enter_direction) : tile(c_tile), enter_direction(c_enter_direction) {};
        ~PathNode() = default;
};


/**
 * Tile class that describes what a Tile is on the map.
 */
class Tile
{
    public:
        
        // ----------------------------- Constructors -----------------------------
        Tile() : m_position(), m_type(TileType::TYPE0), m_paths(), m_neighborhood(), m_is_locked(false), m_contains_player(false)
        {
            initNeighborhood();
            resetTile();
        }

        explicit Tile(TileType type) : m_position(), m_type(type), m_paths(), m_neighborhood(), m_is_locked(false), m_contains_player(false)
        {
            initNeighborhood();
            resetTile();
        }

        Tile(Vector2i position, TileType type) : m_position(position), m_type(type), m_paths(), m_neighborhood(), m_is_locked(false), m_contains_player(false)
        {
            initNeighborhood();
            resetTile();
        }

        Tile(int x, int y, TileType type) : m_position(x,y), m_type(type), m_paths(), m_neighborhood(), m_is_locked(false), m_contains_player(false)
        {
            initNeighborhood();
            resetTile();
        }

        Tile(const Tile &tile) = default;
        // -----------------------------------------------------------------------

        /** 
            Tile destructor. Nothing special, neighborhood will be destrcuted with map.
        */
        virtual ~Tile() = default;

        /**
            Locks the tile to that it can't rotate anymore.
        **/
        void lock()
        {
            m_is_locked = true;
        }

        /**
            Unlocks the tile to that it can rotate again.
        **/
        void unlock()
        {
            m_is_locked = false;
        }

        /**
            @returns TRUE if the tile is locked, FALSE otherwise.
        **/
        bool isLocked() const
        {
            return m_is_locked;
        }
        
        /**
            @returns TRUE if the tile currently contains the player, FALSE otherwise.
        **/
        bool containsPlayer() const
        {
            return m_contains_player;
        }
        
        /**
            Sets if the tile contains the player or not.

            @param[in] value TRUE for the tile to contain the player, FALSE otherwise.
        **/
        void setContainsPlayer(bool value)
        {
            m_contains_player = value;
        }
        
        /**
            Tells whether the tile is invariant regarding its paths by 2 quarter rotation (i.e 180 degrees rotation).

            @returns TRUE is the tile is invariant by 2 quarters, FALSE otherwise. 
        **/
        bool isInvariantBy2Quarters()
        {
            switch (m_type)
            {
                case TileType::TYPE1 :
                    return true;
                case TileType::TYPE2 :
                    return true;
                case TileType::TYPE3 :
                    return true;
                case TileType::TYPE4 :
                    return true;
                case TileType::TYPE5 :
                    return true;
                case TileType::TYPE6 :
                    return false;
                case TileType::TYPE7 :
                    return false;
                case TileType::TYPE8 :
                    return false;
                case TileType::TYPE9 :
                    return false;
                case TileType::TYPE10 :
                    return false;
                case TileType::TYPE11 :
                    return false;
                case TileType::TYPE12 :
                    return false;
                case TileType::TYPE13 :
                    return false;
                case TileType::EXIT :
                    return true;
                default: // TYPE0 and other => no path
                    return true;
            }
        }
        
        /**
            Tells whether the tile is invariant regarding its paths whatever we come from the left or the right.

            @returns TRUE is the tile is invariant by left to right, FALSE otherwise. 
        **/
        bool isInvariantByLeftToRight()
        {
            switch (m_type)
            {
                case TileType::TYPE1 :
                    return true;
                case TileType::TYPE2 :
                    return true;
                case TileType::TYPE3 :
                    return true;
                case TileType::TYPE4 :
                    return true;
                case TileType::TYPE5 :
                    return true;
                case TileType::TYPE6 :
                    return false;
                case TileType::TYPE7 :
                    return false;
                case TileType::TYPE8 :
                    return false;
                case TileType::TYPE9 :
                    return false;
                case TileType::TYPE10 :
                    return false;
                case TileType::TYPE11 :
                    return false;
                case TileType::TYPE12 :
                    return false;
                case TileType::TYPE13 :
                    return false;
                case TileType::EXIT :
                    return true;
                default: // TYPE0 and other => no path
                    return true;
            }
        }
        
        /**
            Resets the tile to its initial rotation, with its original paths.
        **/
        void resetTile()
        {
            m_paths.clear();
            
            switch (m_type)
            {
                case TileType::TYPE1 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE2 :
                    addPath(Direction::RIGHT, Direction::LEFT);
                    addPath(Direction::LEFT, Direction::RIGHT);
                    break;
                case TileType::TYPE3 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    break;
                case TileType::TYPE4 :
                    addPath(Direction::TOP, Direction::LEFT);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE5 :
                    addPath(Direction::TOP, Direction::RIGHT);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE6 :
                    addPath(Direction::LEFT, Direction::RIGHT);
                    addPath(Direction::RIGHT, Direction::LEFT);
                    break;
                case TileType::TYPE7 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE8 :
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE9 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE10 :
                    addPath(Direction::TOP, Direction::LEFT);
                    break;
                case TileType::TYPE11 :
                    addPath(Direction::TOP, Direction::RIGHT);
                    break;
                case TileType::TYPE12 :
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE13 :
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::EXIT :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                default: // TYPE0 and other => no path
                    break;
            }
        }
        
        /**
            Rotates the tile 2 quarters (i.e 180 degrees).
            This updates tile paths in function of the rotation result.
        **/
        void rotate2Quarters()
        {
            if (m_is_locked)
            {
                return;
            }
            
            m_paths.clear();
            
            switch (m_type)
            {
                case TileType::TYPE1 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE2 :
                    addPath(Direction::RIGHT, Direction::LEFT);
                    addPath(Direction::LEFT, Direction::RIGHT);
                    break;
                case TileType::TYPE3 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    break;
                case TileType::TYPE4 :
                    addPath(Direction::TOP, Direction::LEFT);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE5 :
                    addPath(Direction::TOP, Direction::RIGHT);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE6 :
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE7 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE8 :
                    addPath(Direction::LEFT, Direction::RIGHT);
                    addPath(Direction::RIGHT, Direction::LEFT);
                    break;
                case TileType::TYPE9 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE10 :
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE11 :
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE12 :
                    addPath(Direction::TOP, Direction::LEFT);
                    break;
                case TileType::TYPE13 :
                    addPath(Direction::TOP, Direction::RIGHT);
                    break;
                case TileType::EXIT :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                default: // TYPE0 and other => no path
                    break;
            }
        }

        /**
            Rotates the tile a quarter left (i.e 90 degrees anticlockwise).
            This updates tile paths in function of the rotation result.
        **/
        void rotateAQuarterLeft()
        {
            if (m_is_locked)
            {
                return;
            }

             m_paths.clear();
            
            switch (m_type)
            {
                case TileType::TYPE1 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE2 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    break;
                case TileType::TYPE3 :
                    addPath(Direction::LEFT, Direction::RIGHT);
                    addPath(Direction::RIGHT, Direction::LEFT);
                    break;
                case TileType::TYPE4 :
                    addPath(Direction::TOP, Direction::RIGHT);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE5 :
                    addPath(Direction::TOP, Direction::LEFT);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE6 :
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    addPath(Direction::TOP, Direction::BOTTOM);
                    break;
                case TileType::TYPE7 :
                    addPath(Direction::LEFT, Direction::RIGHT);
                    addPath(Direction::RIGHT, Direction::LEFT);
                    break;
                case TileType::TYPE8 :
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    addPath(Direction::TOP, Direction::BOTTOM);
                    break;
                case TileType::TYPE9 :
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE10 :
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE11 :
                    addPath(Direction::TOP, Direction::LEFT);
                    break;
                case TileType::TYPE12 :
                    addPath(Direction::TOP, Direction::RIGHT);
                    break;                    
                case TileType::TYPE13 :
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                    
                case TileType::EXIT :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                default: // TYPE0 and other => no path
                    break;
            }
        }

         /**
            Rotates the tile a quarter right (i.e 90 degrees clockwise).
            This updates tile paths in function of the rotation result.
        **/
        void rotateAQuarterRight()
        {
            if (m_is_locked)
            {
                return;
            }

             m_paths.clear();
            
            switch (m_type)
            {
                case TileType::TYPE1 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE2 :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    break;
                case TileType::TYPE3 :
                    addPath(Direction::LEFT, Direction::RIGHT);
                    addPath(Direction::RIGHT, Direction::LEFT);
                    break;
                case TileType::TYPE4 :
                    addPath(Direction::TOP, Direction::RIGHT);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;
                case TileType::TYPE5 :
                    addPath(Direction::TOP, Direction::LEFT);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE6 :
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    addPath(Direction::TOP, Direction::BOTTOM);
                    break;
                case TileType::TYPE7 :
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE8 :
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    addPath(Direction::TOP, Direction::BOTTOM);
                    break;
                case TileType::TYPE9 :
                    addPath(Direction::LEFT, Direction::RIGHT);
                    addPath(Direction::RIGHT, Direction::LEFT);
                    break;
                case TileType::TYPE10 :
                    addPath(Direction::TOP, Direction::RIGHT);
                    break;
                case TileType::TYPE11 :
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                case TileType::TYPE12 :
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    break;                    
                case TileType::TYPE13 :
                    addPath(Direction::TOP, Direction::LEFT);
                    break;
                    
                case TileType::EXIT :
                    addPath(Direction::TOP, Direction::BOTTOM);
                    addPath(Direction::LEFT, Direction::BOTTOM);
                    addPath(Direction::RIGHT, Direction::BOTTOM);
                    break;
                default: // TYPE0 and other => no path
                    break;
            }
        }

        /**
            @returns The tile at the top of the current one (can be nullptr).
        **/
        Tile* getTileTop()
        {
            return m_neighborhood[Direction::TOP];
        }

        /**
            @returns The tile at the right of the current one (can be nullptr).
        **/
        Tile* getTileRight()
        {
            return m_neighborhood[Direction::RIGHT];
        }

        /**
            @returns The tile at the bottom of the current one (can be nullptr).
        **/
        Tile* getTileBottom()
        {
            return m_neighborhood[Direction::BOTTOM];
        }

        /**
            @returns The tile at the left of the current one (can be nullptr).
        **/
        Tile* getTileLeft()
        {
            return m_neighborhood[Direction::LEFT];
        }



        /**
            Sets the postion of the tile on the map.

            @param[in] x Row position on the map.
            @param[in] y Column position on the map.
        **/
        void setPosition(int x, int y)
        {
            m_position = Vector2i(x,y);
        }

        /**
            Sets the postion of the tile on the map.

            @param[in] position New position on the map.
        **/
        void setPosition(const Vector2i &position)
        {
            m_position = position;
        }

        /**
            Sets all surrounding tiles for the current one.
            Pointers can be nullptr.

            @param[in] top_tile Tile at (x,y-1).
            @param[in] right_tile Tile at (x+1,y).
            @param[in] bottom_tile Tile at (x,y+1).
            @param[in] left_tile Tile at (x-1,y).
        **/
        void setNeighborhood(Tile* top_tile, Tile* right_tile, Tile* bottom_tile, Tile *left_tile)
        {
            m_neighborhood[Direction::TOP] = top_tile;
            m_neighborhood[Direction::RIGHT] = right_tile;
            m_neighborhood[Direction::BOTTOM] = bottom_tile;
            m_neighborhood[Direction::LEFT] = left_tile;
        }

        /**
            Sets tile at the top.

            @param[in] top_tile New tile at the top (can be nullptr).
        **/
        void setTileTop(Tile* top_tile)
        {
            m_neighborhood[Direction::TOP] = top_tile;
        }


        /**
            Sets tile at the right.

            @param[in] right_tile New tile at the right (can be nullptr).
        **/
        void setTileRight(Tile* right_tile)
        {
            m_neighborhood[Direction::RIGHT] = right_tile;
        }


        /**
            Sets tile at the bottom.

            @param[in] bottom_tile New tile at the bottom (can be nullptr).
        **/
        void setTileBottom(Tile* bottom_tile)
        {
            m_neighborhood[Direction::BOTTOM] = bottom_tile;
        }


        /**
            Sets tile at the left.

            @param[in] left_tile New tile at the left (can be nullptr).
        **/
        void setTileLeft(Tile* left_tile)
        {
            m_neighborhood[Direction::LEFT] = left_tile;
        }


        /**
            Adds a path to the current tile for the player.

            @param[in] input Input of the path.
            @param[in] output Output of the path.
        **/
        void addPath(const Direction input, const Direction output)
        {
            m_paths[input] = output;
        }

        /**
            @returns Current position of the tile.
        */
        Vector2i getPosition() const
        {
            return m_position;
        }

        /**
            @returns Type of the current tile.
        */
        TileType getType() const
        {
            return this->m_type;
        }

        /**
            Tells whether the tile can allow a direction as an input.

            @param[in] direction Direction to test.

            @returns TRUE if the tile can allow the direction as an input, FALSE otherwise.
        */
        bool hasInputOnDirection(const Direction &direction)
        {
            return m_paths.find(direction) != m_paths.end();
        }

        /**
         * Gives the output informations for an input on the current Tile.
         *
         * @param[in] input The direction from which the character enters the Tile.
         *
         * @returns The next path node.
         */
        PathNode getOutput(Direction input)
        {
            
            PathNode result(nullptr, input);
                
            if (m_paths.find(input) != m_paths.end())
            {
                Direction output_direction = m_paths[input], next_tile_input_direction = getReversedEntryDirection(output_direction);

                Tile *next_tile = m_neighborhood[output_direction];
                
                result.tile = next_tile;
                result.enter_direction = next_tile_input_direction;
            }
            
            return result;
        }


    private:
        /**
            Inits tile neighbourhood to nullptr.
        **/
        void initNeighborhood()
        {
            m_neighborhood[Direction::TOP] = nullptr;
            m_neighborhood[Direction::RIGHT] = nullptr;
            m_neighborhood[Direction::BOTTOM] = nullptr;
            m_neighborhood[Direction::LEFT] = nullptr;
        }
       

        /**
            Gives the inverse of a direction.

            @param[in] d Direction to reverse.

            @returns The inverse direction.
        **/
        static Direction getReversedEntryDirection(Direction d)
        {
            switch(d)
            {
                case Direction::TOP :
                    return Direction::BOTTOM; 
                case Direction::LEFT :
                    return Direction::RIGHT; 
                case Direction::RIGHT :
                    return Direction::LEFT; 
                default :
                    return Direction::TOP; 
            }
        }

        Vector2i m_position;
        TileType m_type;
        bool m_is_locked;
        bool m_contains_player;
        map<Direction,Direction> m_paths; // Keys : input, Values : Output
        map<Direction, Tile*> m_neighborhood; // tiles around
};


/**
 * Map storing all the Tiles and their position.
 */
class Map
{
    public:
        /**
            Map constructor.
        **/
        Map() : m_size(0,0), m_mapping() {}

        /**
            Map destructor. Nothing special because tiles will auto destruct there (they are not pointers).
        **/
        virtual ~Map() = default;


        /**
         * Adds a Tile to the map
         *
         * @param[in] x Position in the tiles line.
         * @param[in] y Position in the tiles column.
         * @param[in] tile_type Type of the Tile.
         * @param[in] lockTile TRUE to lock the new tile, FALSE otherwise.
         */
        void addTile(int x, int y, TileType tile_type, bool lockTile = false)
        {
            Tile newtile(x,y,tile_type);

            if (lockTile)
            {
                newtile.lock();
            }

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
         * @param[in] x Current player position on tiles line.
         * @param[in] y Current player position on tiles column.
         * @param[in] input Direction by which the player is coming in the current tile.
         *
         * @returns Position of the next tile the player will ends up in.
         */
        Vector2i getNextPlayerPosition(int x, int y, Direction input)
        {
            PathNode nextTileInfos = m_mapping[Vector2i(x,y)].getOutput(input);

            if (nextTileInfos.tile != nullptr)
            {
                return nextTileInfos.tile->getPosition();
            }
            else
            {
                return Vector2i(x,y); // cannot move
            }
        }

        /**
            Accessor to map tiles.

            @param[in] x Map x position.
            @param[in] y Map y position.

            @returns The tile that corresponds to the requested position, or NULLPTR if the requested tile doesn't exist.
        **/
        Tile* getTileAt(int x, int y)
        {
            Vector2i position(x,y);
            
            if (m_mapping.find(position) != m_mapping.end())
            {
                return &m_mapping[position];
            }
            else
            {
                return nullptr;
            }
        }

    private :

        /**
         * Updates the neighborhood of a new tile and all the tiles touching it currently, so that the grid remains
         * consistent in the mapping.
         *
         * @param[inout] tile The new tile that has just been added to the Map.
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
 * This class describes an instruction to output in the terminal.
 * It's ok to let the fields public here to simplify access, since the only purpose of this class is to store them.
 */
class SolverInstruction
{
    public :
        Tile* tile;
        TileAction action;
        int path_position;

        SolverInstruction() : tile(nullptr), action(TileAction::ROTATE_LEFT){};
        SolverInstruction(Tile* c_tile, TileAction c_action, int tunnel_position) : tile(c_tile), action(c_action), path_position(tunnel_position) {};

};

/**
 * This class performs path finding for the player.
 */
class PathSolver
{
    public:

        /**
            Path solver constructor.

            @param[in] map The map to solve on.
        **/
        explicit PathSolver(Map* map) : m_map(map) {};
        PathSolver() = delete;

        /**
            Path solver destructor. The map is not destructed by this class. 
        **/
        virtual ~PathSolver() = default;

        /**
         * This method resolves player path.
         * 
         * @param[in] player_enter_x X coordinate of the first tile the player enters by.
         * @param[in] player_enter_y Y coordinate of the first tile the player enters by.
         * @param[in] player_enter_direction Player enter direction on the first tile.
         * @param[out] instructions SolverInstructions to follow in order to construct the final path through the map.
         * @param[out] final_path Final player path through the map.
         *
         */
        void resolvePathToExit(int player_enter_x, int player_enter_y, Direction player_enter_direction, std::vector<SolverInstruction> &instructions, std::unordered_map<Vector2i,PathNode> &final_path)
        {
            // Making sure we start from scratch.
            instructions.clear();
            final_path.clear();
            
            // Player start tile
            Tile* enter_tile = m_map->getTileAt(player_enter_x,player_enter_y);
            
            // Path of the player on the first tile
            PathNode entering(enter_tile, player_enter_direction);
                       
            
            // Processing path finding
            bool path_exists = computePathToExit(enter_tile, player_enter_direction, instructions, final_path);
            
            if (!path_exists)
            {
                cerr << "##################################################################" << endl;
                cerr << "############################ NO EXIT #############################" << endl;
                cerr << "##################################################################" << endl;
            }
            
        }

    private:
    
        /**
         * Recursive method that performs path finding for the player to the exit.
         * 
         * @param[inout] start_tile The tile the player enters by.
         * @param[in] entering_direction The direction the palyer enters by on the tile.
         * @param[inout] instructions_to_follow Instructions retained so far to resolve the current path.
         * @param[inout] kept_positions Final path positions retained so far.
         * 
         * @returns TRUE if a path could be computed from the start_tile to the exit, FALSE otherwise.
         **/
        bool computePathToExit(Tile* start_tile, Direction entering_direction, std::vector<SolverInstruction> &instructions_to_follow,  std::unordered_map<Vector2i,PathNode>& kept_positions)
        {
            if (kept_positions.find(start_tile->getPosition()) != kept_positions.end())
            {
                return false;
            }
            
            PathNode output;
            
            if (start_tile->getType() == TileType::EXIT) // Tile is exit
            {
                return true;
            }
            else if (start_tile->getType() == TileType::TYPE0) // Tile has no path through it
            {
                return false;
            }
            else if (start_tile->isLocked()) // Tile can't be moved.
            {
                output = start_tile->getOutput(entering_direction);
                
                if (output.tile == nullptr)
                {
                    return false;
                }
                else
                {
                    if (output.tile->getType() == TileType::EXIT)
                    {
                        return true;
                    }
                    else
                    {
                        kept_positions.insert(make_pair(start_tile->getPosition(), PathNode(start_tile, entering_direction)));
                        return computePathToExit(output.tile, output.enter_direction, instructions_to_follow, kept_positions);
                    }
                }
            }
            else
            {
                bool found_exit = false;
                bool pushed_position = false;
                int path_position = kept_positions.size();
               
               // ============= Try DIRECT ===========
                output = start_tile->getOutput(entering_direction);
                
                if (output.tile != nullptr)
                {
                    //cerr << "---- (" << start_tile->getPosition().x << "," << start_tile->getPosition().y << ") Try DIRECT : ";
                    kept_positions.insert(make_pair(start_tile->getPosition(), PathNode(start_tile, entering_direction)));
                    pushed_position = true;
                    
                    found_exit = computePathToExit(output.tile, output.enter_direction, instructions_to_follow, kept_positions);
                    
                    if (found_exit)
                    {
                        return true;
                    }
                }
                
                
                // ============= Try LEFT ===========                
                if (!found_exit)
                {
                    start_tile->rotateAQuarterLeft();
                    
                    SolverInstruction instruction(start_tile, TileAction::ROTATE_LEFT, path_position);
                    instructions_to_follow.push_back(instruction);
                    
                    
                    output = start_tile->getOutput(entering_direction);
                    
                    if (output.tile != nullptr)
                    {                        
                        if (!pushed_position)
                        {
                            kept_positions.insert(make_pair(start_tile->getPosition(), PathNode(start_tile, entering_direction)));    
                            pushed_position = true;  
                        }  
                        
                        found_exit = computePathToExit(output.tile, output.enter_direction, instructions_to_follow, kept_positions);
                        
                        if (found_exit)
                        {
                            return true;
                        }
                        else
                        {
                            start_tile->resetTile();
                            instructions_to_follow.pop_back();
                        }
                    }
                    else
                    {
                        start_tile->resetTile();
                        instructions_to_follow.pop_back();
                    }
                }
                
                
                // ============= Try RIGHT ===========   
                if (!found_exit && !start_tile->isInvariantByLeftToRight())
                {
                    start_tile->rotateAQuarterRight();
                    
                    SolverInstruction instruction(start_tile, TileAction::ROTATE_RIGHT, path_position);
                    instructions_to_follow.push_back(instruction);
                
                    output = start_tile->getOutput(entering_direction);
                    
                    if (output.tile != nullptr)
                    {
                        if (!pushed_position)
                        {
                            kept_positions.insert(make_pair(start_tile->getPosition(), PathNode(start_tile, entering_direction)));    
                            pushed_position = true;  
                        }                  
                                                
                        found_exit = computePathToExit(output.tile, output.enter_direction, instructions_to_follow, kept_positions);
                        
                        if (found_exit)
                        {
                            return true;
                        }
                        else
                        {
                            start_tile->resetTile();
                            instructions_to_follow.pop_back();
                        }
                    }
                    else
                    {
                        start_tile->resetTile();
                        instructions_to_follow.pop_back();
                        
                    }
                }
                
                
                 // ============= Try LEFT LEFT ===========
                if (!found_exit && !start_tile->isInvariantBy2Quarters())
                {
                    start_tile->rotate2Quarters();
                    
                    SolverInstruction instruction(start_tile, TileAction::ROTATE_LEFT, path_position);
                    instructions_to_follow.push_back(instruction);
                    instructions_to_follow.push_back(instruction);
                    
                    output = start_tile->getOutput(entering_direction);
                    
                    if (output.tile != nullptr)
                    {
                        if (!pushed_position)
                        {
                            kept_positions.insert(make_pair(start_tile->getPosition(), PathNode(start_tile, entering_direction)));    
                            pushed_position = true;  
                        }             
                        
                        found_exit = computePathToExit(output.tile, output.enter_direction, instructions_to_follow, kept_positions);
                        
                        if (found_exit)
                        {
                            return true;
                        }
                        else
                        {
                            start_tile->resetTile();
                            instructions_to_follow.pop_back();
                            instructions_to_follow.pop_back();
                        }
                    }
                    else
                    {
                        start_tile->resetTile();
                        instructions_to_follow.pop_back();
                        instructions_to_follow.pop_back();
                        
                    }
                }
                
                
                if (!found_exit)
                {
                    kept_positions.erase(start_tile->getPosition());
                    
                    return false;
                }
            }
        }
    
        Map* m_map;
};

/**
 * This class describes the manager that will handle rocks to block them. 
 **/
class RockManager
{
    public : 

        /**
            Rock manager constructor.

            @param[in] map Map to work on with rocks.
            @param[in] player_path Path the player will follow, to compute intersections with rocks.
        **/
        RockManager(Map* map, std::unordered_map<Vector2i,PathNode>* player_path) : m_player_path(player_path), m_map(map), m_rocks_safety(), m_rocks_blocking_instructions(), m_rocks_current_node(), m_rocks_priorities()
        {}


        RockManager() = delete;

        /**
            Rock manager destructor. Nothing special, pointers referenced in this class must not be destroyed by it.
        **/
        virtual ~RockManager() = default; 
        
        /**
         * This method makes the known rocks progress one tile ahead (which keeps the model up to date at each iteration).
         */
        void updateRocksAhead()
        {
            for (auto &p : m_rocks_current_node)
            {
                PathNode new_pos = p.second.tile->getOutput(p.second.enter_direction);
                
                if (new_pos.tile != nullptr)
                {
                    p.second = new_pos;
                }
            }
            
            for (auto &p : m_rocks_blocking_instructions)
            {
                if (!m_rocks_safety[p.first])
                {
                    for (auto &v : p.second)
                    {
                        v.first--;
                    }
                }
            }
            
            for (auto &p : m_rocks_priorities)
            {
                p.second--;
            }
        }
        
        /**
         * This method registers a rock if it doesn't already exists.
         * 
         * @param[in] rock_pos_x Rock X coordinate.
         * @param[in] rock_pos_y Rock Y coordinate.
         * @param[in] entering_direction Rock entering direction on the current tile.
         * 
         */
        void processRock(int rock_pos_x, int rock_pos_y, Direction entering_direction)
        {
            bool already_exists = false;
            Vector2i rock_pos(rock_pos_x,rock_pos_y);
            
            // 1- we check if the given position is part of an already existing rock.
            for (auto p : m_rocks_current_node)
            {
                if (p.second.tile != nullptr && p.second.tile->getPosition() == rock_pos
                    && p.second.enter_direction == entering_direction) 
                {
                    already_exists = true;
                    break;
                }
            }
            
            // 2 - If this is a new rock, we compute possible plans to block it, and we store SolverInstructions to be executed later on.
            if (!already_exists)
            {
                int rock_id = m_rocks_safety.size();
                PathNode rock_current_node(m_map->getTileAt(rock_pos_x,rock_pos_y), entering_direction);
                
                m_rocks_safety[rock_id] = false;
                m_rocks_priorities[rock_id] = numeric_limits<int>::max();
                m_rocks_current_node[rock_id] = rock_current_node;
                
                int current_distance_to_rock = 0;
                bool has_full_path = false;
                
                // While we don't arrive to player path.
                while (!has_full_path)
                {
                    rock_current_node = rock_current_node.tile->getOutput(rock_current_node.enter_direction);
                    current_distance_to_rock++;
                    
                    // #### The rock is already safe before reaching player path
                    if (rock_current_node.tile == nullptr)
                    {
                         m_rocks_safety[rock_id] = true;
                         has_full_path = true;
                    }
                    // #### We reached player path => we need to block the rock before (or on) this tile.
                    else if (m_player_path->find(rock_current_node.tile->getPosition()) != m_player_path->end())
                    {
                         has_full_path = true;
                         
                         Direction player_direction = (*m_player_path)[rock_current_node.tile->getPosition()].enter_direction;
                         
                         if (!rock_current_node.tile->isLocked()) // We can act on the tile.
                         {
                            // 1 - ===== TRY BLOCKING BY ROTATING LEFT =====
                            rock_current_node.tile->rotateAQuarterLeft();
                            
                            if (!rock_current_node.tile->hasInputOnDirection(rock_current_node.enter_direction) && rock_current_node.tile->hasInputOnDirection(player_direction))
                            {
                                SolverInstruction instruction(rock_current_node.tile, TileAction::ROTATE_LEFT, current_distance_to_rock);
                                
                                queue<SolverInstruction> solve_instructions;
                                solve_instructions.push(instruction);
                                
                                m_rocks_blocking_instructions[rock_id].push_back(make_pair(current_distance_to_rock, move(solve_instructions)));
                                m_rocks_priorities[rock_id] = current_distance_to_rock;
                            }
                            else
                            {
                                // 2 - ===== TRY BLOCKING BY ROTATING RIGHT =====
                                rock_current_node.tile->resetTile();
                                
                                rock_current_node.tile->rotateAQuarterRight();
                            
                                if (!rock_current_node.tile->hasInputOnDirection(rock_current_node.enter_direction) && rock_current_node.tile->hasInputOnDirection(player_direction))
                                {
                                    SolverInstruction instruction(rock_current_node.tile, TileAction::ROTATE_RIGHT, current_distance_to_rock);
                                
                                    queue<SolverInstruction> solve_instructions;
                                    solve_instructions.push(instruction);
                                    
                                    m_rocks_blocking_instructions[rock_id].push_back(make_pair(current_distance_to_rock, move(solve_instructions)));
                                    m_rocks_priorities[rock_id] = current_distance_to_rock;
                                }
                                else
                                {
                                    // 3 - ===== TRY BLOCKING BY INVERTING TILE =====
                                    rock_current_node.tile->resetTile();
                                    
                                    rock_current_node.tile->rotate2Quarters();
                            
                                    if (!rock_current_node.tile->hasInputOnDirection(rock_current_node.enter_direction) && rock_current_node.tile->hasInputOnDirection(player_direction))
                                    {
                                        SolverInstruction instruction(rock_current_node.tile, TileAction::ROTATE_LEFT, current_distance_to_rock);
                                
                                        queue<SolverInstruction> solve_instructions;
                                        solve_instructions.push(instruction);
                                        solve_instructions.push(instruction);
                                        
                                        m_rocks_blocking_instructions[rock_id].push_back(make_pair(current_distance_to_rock, move(solve_instructions)));
                                        m_rocks_priorities[rock_id] = current_distance_to_rock-1; // Two instructions to execute => we must take care of them one tile before others.
                                    }
                                }
                            }
                        }
                    }
                    // #### We continue to follow rock's path.
                    else
                    {
                        if (!rock_current_node.tile->isLocked()) // We can act on the tile.
                        {
                            // 1 - ===== TRY BLOCKING BY ROTATING LEFT =====
                            rock_current_node.tile->rotateAQuarterLeft();
                            
                            if (!rock_current_node.tile->hasInputOnDirection(rock_current_node.enter_direction))
                            {
                                SolverInstruction instruction(rock_current_node.tile, TileAction::ROTATE_LEFT, current_distance_to_rock);
                                
                                queue<SolverInstruction> solve_instructions;
                                solve_instructions.push(instruction);
                                
                                m_rocks_blocking_instructions[rock_id].push_back(make_pair(current_distance_to_rock, move(solve_instructions)));
                                m_rocks_priorities[rock_id] = current_distance_to_rock;
                            }
                            else
                            {
                                 // 2 - ===== TRY BLOCKING BY ROTATING RIGHT =====
                                rock_current_node.tile->resetTile();
                                
                                rock_current_node.tile->rotateAQuarterRight();
                            
                                if (!rock_current_node.tile->hasInputOnDirection(rock_current_node.enter_direction))
                                {
                                    SolverInstruction instruction(rock_current_node.tile, TileAction::ROTATE_RIGHT, current_distance_to_rock);
                                
                                    queue<SolverInstruction> solve_instructions;
                                    solve_instructions.push(instruction);
                                    
                                    m_rocks_blocking_instructions[rock_id].push_back(make_pair(current_distance_to_rock, move(solve_instructions)));
                                    m_rocks_priorities[rock_id] = current_distance_to_rock;
                                }
                                else
                                {
                                    // 3 - ===== TRY BLOCKING BY INVERTING TILE =====
                                    rock_current_node.tile->resetTile();
                                    
                                    rock_current_node.tile->rotate2Quarters();
                            
                                    if (!rock_current_node.tile->hasInputOnDirection(rock_current_node.enter_direction))
                                    {
                                        SolverInstruction instruction(rock_current_node.tile, TileAction::ROTATE_LEFT, current_distance_to_rock);
                                
                                        queue<SolverInstruction> solve_instructions;
                                        solve_instructions.push(instruction);
                                        solve_instructions.push(instruction);
                                        
                                        m_rocks_blocking_instructions[rock_id].push_back(make_pair(current_distance_to_rock, move(solve_instructions)));
                                        m_rocks_priorities[rock_id] = current_distance_to_rock-1; // Two instructions to execute => we must take care of them one tile before others.
                                    }
                                }
                            }
                            
                        
                            rock_current_node.tile->resetTile();
                        }
                    }
                }
                
            }
        }
        
        /**
         * This method performs rock blocking by executing SolverInstructions that have been stored for each rock path during their registration by method processRock();
         * 
         * @returns TRUE if we found an instruction to execute to block a rock and executed it. FALSE if we did nothing.
         */
        bool processRockInterception()
        {
            int min_priority = numeric_limits<int>::max();
            int rock_id = -1;
            
            // 1 - Let's find the most problematic rock to block.
            for (auto p : m_rocks_priorities)
            {
                if (p.second < min_priority && !m_rocks_safety[p.first])
                {
                    min_priority = p.second;
                    rock_id = p.first;
                }
            }
            
            SolverInstruction* instruction = nullptr;
            string action_str = "";
            bool has_moved_tile = false;
            
            if (rock_id != -1)
            {
                // Let's find an instruction to block the chosen rock.
                for (auto &p : m_rocks_blocking_instructions[rock_id])
                {
                    if (p.first > (int)p.second.size()) // If the instruction is still executable by acting on a tile ahead of the current rock tile.
                    {
                        instruction = &p.second.front();
                        
                        if (!instruction->tile->containsPlayer()) 
                        {
                            bool is_player_path = m_player_path->find(instruction->tile->getPosition()) != m_player_path->end();
                        
                            if (is_player_path)
                            {
                                 instruction->tile->unlock();
                            }
                            
                            switch (instruction->action)
                            {
                                case ROTATE_LEFT:
                                    action_str = "LEFT";
                                    break;
                                case ROTATE_RIGHT:
                                    action_str = "RIGHT";
                                    break;
                            }
                    
                            cout << instruction->tile->getPosition().x << " " << instruction->tile->getPosition().y << " " << action_str << endl;
                            
                            p.second.pop();
                                            
                            if (is_player_path)
                            {
                                 instruction->tile->lock();
                            }
                            
                            if (p.second.size() == 0)
                            {
                                m_rocks_safety[rock_id] = true;
                                m_rocks_blocking_instructions.erase(rock_id);
                                m_rocks_priorities.erase(rock_id);
                            }
                            
                            has_moved_tile = true;         
                            break;
                        }
                    }
                }
                
                if (has_moved_tile)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    
    private :
        Map* m_map; // Level map.
        std::unordered_map<Vector2i,PathNode> *m_player_path; // Pointer to player path.
        
        // Associative container rock_id => priority . Priorities corresponds to the farther tile we can act on to block the rock. 
        //
        // For example a priority of 1 tells that the rock can only be blocked by the tile 1 PathNode away from it, which means that 
        // we must block it right now, otherwise we won't be able to block it.
        // 
        // Priorities can be negative if the last possible blocking tile for a rock has been exceeded, which is bad news and can
        // indicate a bug in the algorithm.
        unordered_map<int, int> m_rocks_priorities; 
        
        // Associative container rock_id => bool. 
        // Contains TRUE for a rock_id if the rock is already blocked.
        // Contains FALSE for a rock_id if the rock can still reach player path.
        unordered_map<int, bool> m_rocks_safety;
        
        // Associative container rock_id => instructions to block it.
        // Instructions are stored for each tile that can block the rock, starting from it starting tile.
        // They are stored in a vector that associates the tile priority (identical in meaning to m_rocks_safety priorities), to a queue
        // that contains instructions to execute on the tile to block the rock.
        unordered_map<int, vector<pair<int, queue<SolverInstruction>>>> m_rocks_blocking_instructions;
        
        // Associative container rock_id => current path node.
        unordered_map<int, PathNode> m_rocks_current_node;
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
    bool lock_tile;

    for (int i = 0; i < H; i++)
    {
        string LINE; // represents a line in the grid and contains W integers. Each integer represents one room of a given type.
        getline(cin, LINE);
        iss.str(LINE); // setting the stream content for the parsing.
        iss.clear(); // to be able to parse from the start on the new line (else remains on the end of the last one).

        j = 0;

        for (token; getline(iss, token, ' '); ) // parsing the line
        {
            lock_tile = false;

            if (token == "-1")
            {
                t = TileType::TYPE1;
                lock_tile = true;
            }
            else if (token ==  "-2")
            {
                t = TileType::TYPE2;
                lock_tile = true;
            }
            else if (token == "-3")
            {
                t = TileType::TYPE3;
                lock_tile = true;
            }
            else if (token == "-4")
            {
                t = TileType::TYPE4;
                lock_tile = true;
            }
            else if (token == "-5")
            {
                t = TileType::TYPE5;
                lock_tile = true;
            }
            else if (token == "-6")
            {
                t = TileType::TYPE6;
                lock_tile = true;
            }
            else if (token == "-7")
            {
                t = TileType::TYPE7;
                lock_tile = true;
            }
            else if (token == "-8")
            {
                t = TileType::TYPE8;
                lock_tile = true;
            }
            else if (token == "-9")
            {
                t = TileType::TYPE9;
                lock_tile = true;
            }
            else if (token == "-10")
            {
                t = TileType::TYPE10;
                lock_tile = true;
            }
            else if (token == "-11")
            {
                t = TileType::TYPE11;
                lock_tile = true;
            }
            else if (token == "-12")
            {
                t = TileType::TYPE12;
                lock_tile = true;
            }
            else if (token == "-13")
            {
                t = TileType::TYPE13;
                lock_tile = true;
            }
            else if (token == "1")
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
            else if (token == "7")
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

            level_map.addTile(j, i, t,lock_tile); // creates a new tile in the map.
            j++;
        }
    }



    // ========================================================================
    //                              EXIT LINE MAPPING
    // ========================================================================

    int EX; // the coordinate along the X axis of the exit (not useful for this first mission, but must be read).
    cin >> EX; cin.ignore();

    for (int k = 0; k < W; k++)
    {
        if (k == EX)
        {
            level_map.addTile(k, H, TileType::EXIT); // creates a new tile in the map.
        }
        else
        {
            level_map.addTile(k, H, TileType::TYPE0); // creates a new tile in the map.
        }
    }
    // ========================================================================
    //                              GAME LOOP
    // ========================================================================


    PathSolver solver(&level_map);
    std::vector<SolverInstruction> instructions;
    std::unordered_map<Vector2i,PathNode> final_player_path;
    std::unordered_set<Vector2i> safe_rock_tiles;
    SolverInstruction loop_instruction;
    bool solved = false;
    Direction player_dir;
    Tile* previous_tile = nullptr;
    Tile* current_tile = nullptr;
    
    RockManager rock_manager(&level_map, &final_player_path);

    int XI;
    int YI;
    string POSI;

    int R; // the number of rocks currently in the grid.

    int XR;
    int YR;
    string POSR;

    int instruction_index = 0;
    string action_str;
    
    std::map<pair<Vector2i,int>, pair<Direction,int>> rocks;
    Direction rock_dir;
    int player_advancement = 0;

    // game loop
    while (1)
    {
        // ======================== PLAYER POS UPDATE ==========================   
        cin >> XI >> YI >> POSI; cin.ignore();
        
        if (previous_tile != nullptr)
        {
            previous_tile->setContainsPlayer(false);
        }
        
        current_tile = level_map.getTileAt(XI,YI);
        
        if (current_tile != nullptr)
        {
            current_tile->setContainsPlayer(true);
        }
        
        player_advancement++;
        // =====================================================================
        
        // ============================ PATH FINDING ===========================  
        // (executed once only)
        if (!solved)
        {
            if (POSI == "TOP")
            {
                player_dir = Direction::TOP;
            }
            else if (POSI == "BOTTOM")
            {
                player_dir = Direction::BOTTOM;
            }
            else if (POSI == "LEFT")
            {
                player_dir = Direction::LEFT;
            }
            else if (POSI == "RIGHT")
            {
                player_dir = Direction::RIGHT;
            }

            solver.resolvePathToExit(XI,YI,player_dir, instructions, final_player_path);

            solved = true;
        }
        // =====================================================================
        
        
        // ============================ ROCKS UPDATE ===========================  
        cin >> R; cin.ignore();
        rocks.clear();
        
        rock_manager.updateRocksAhead();

        bool can_insert = true;
        
        for (int i = 0; i < R; i++) 
        {

            cin >> XR >> YR >> POSR; cin.ignore();
            
             if (POSR == "TOP")
            {
                rock_dir = Direction::TOP;
            }
            else if (POSR == "BOTTOM")
            {
                rock_dir = Direction::BOTTOM;
            }
            else if (POSR == "LEFT")
            {
                rock_dir = Direction::LEFT;
            }
            else if (POSR == "RIGHT")
            {
                rock_dir = Direction::RIGHT;
            }
                      
            Vector2i rock_pos(XR,YR);
            
            if (final_player_path.find(rock_pos) == final_player_path.end())
            {
                rock_manager.processRock(XR, YR, rock_dir);
            }
        }
        // =====================================================================

        // ====================== INSTRUCTION DECISION ========================= 
        
        if (instruction_index < instructions.size())
        {            
            bool has_loop_instruction = false;
            
            // Try to find the next instruction to construct player path
            while (instruction_index < instructions.size() && !has_loop_instruction)
            {
                loop_instruction = instructions[instruction_index];

                // Sometimes player path tiles are locked by rock blocking algorithm when needed.
                // But when it arrives, we are sure that the tile is already well placed.
                if (!loop_instruction.tile->isLocked()) 
                {
                    has_loop_instruction = true;
                }
                else
                {
                    instruction_index++;
                }
            }
            
            
            bool has_write_for_rock = false;
            
            // If we don't have an instruction to execute on player path or the instruction is not urgent.
            // Then we check if we can take the opportunity to block a rock.
            if (!has_loop_instruction || loop_instruction.path_position > player_advancement)
            {
                // Attempt to block a rock on the map
                has_write_for_rock = rock_manager.processRockInterception();
            }
            
            // If we have an instruction to execute on player path that is urgent, or we haven't found 
            // any rock to block.
            if (has_loop_instruction && !has_write_for_rock)
            {
                switch (loop_instruction.action)
                {
                    case ROTATE_LEFT:
                        action_str = "LEFT";
                        break;
                    case ROTATE_RIGHT:
                        action_str = "RIGHT";
                        break;
                }
    
                // Solving player path
                cout << loop_instruction.tile->getPosition().x << " " << loop_instruction.tile->getPosition().y << " " << action_str << endl;
    
                instruction_index++;
            }
            else if (!has_write_for_rock)
            {
                cout << "WAIT" << endl;
            }
        }
        else
        {
            cout << "WAIT" << endl;
        }
        
        // =====================================================================

        previous_tile = current_tile;
    }
}