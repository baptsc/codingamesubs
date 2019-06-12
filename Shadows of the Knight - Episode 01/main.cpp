/**
    This is my solution to the challenge "Shadows of the knight - Episode 01" on Codingame.

    Challenge link : https://www.codingame.com/training/medium/shadows-of-the-knight-episode-1
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Enum for bomb positions.
enum BOMB_ORIENTATION{U, UR, R, DR, D, DL, L, UL, UNKNOWN};

/**
 * Simple structure to store a position on the grid.
 **/
class vector2i
{
public :
    vector2i() : x(0), y(0) {}
    vector2i(int x0, int y0) : x(x0), y(y0) {}

    int x;
    int y;
};


/**
 * Class that solves the problem.
 * It uses an interval-update pattern with a binary search.
 **/
class BatmanControl
{
public :
    /**
        Batman Control constructor.
        
        @param[in] grid_width Game grid width.
        @param[in] grid_height Game grid height.
        @param[in] x0 Batman intial x position.
        @param[in] y0 Batman intial y position.
    **/
    BatmanControl(int grid_width, int grid_height, int x0, int y0) : m_position(x0, y0), 
                                                                    m_min_x_pos(0),
                                                                    m_min_y_pos(0),
                                                                    m_max_x_pos(grid_width-1),
                                                                    m_max_y_pos(grid_height-1)
    {
    }
    
    /**
     * Main solving method called at each iteration. 
     * It updates batman position according to the current bomb's direction.
     * 
     * @param[in] bomb_orientation Current bomb orientation.
     * 
     **/
    void updateBatmanPosition(BOMB_ORIENTATION bomb_orientation)
    {
        switch (bomb_orientation)
        {
            case BOMB_ORIENTATION::U :
                m_max_y_pos = m_position.y-1;
                
                m_position.y = m_min_y_pos + ( (m_max_y_pos - m_min_y_pos) / 2);
                break;
                
            case BOMB_ORIENTATION::UR :
                m_max_y_pos = m_position.y-1;
                m_min_x_pos = m_position.x+1;
                
                m_position.y = m_min_y_pos + ( (m_max_y_pos - m_min_y_pos) / 2);
                m_position.x = m_min_x_pos + ((m_max_x_pos -  m_min_x_pos) / 2);
                break;
                
            case BOMB_ORIENTATION::R :
                m_min_x_pos = m_position.x+1;
            
                m_position.x = m_min_x_pos + ((m_max_x_pos -  m_min_x_pos) / 2);
                break;
                
            case BOMB_ORIENTATION::DR :
                m_min_y_pos = m_position.y+1;
                m_min_x_pos = m_position.x+1;
                
                m_position.y = m_min_y_pos + ( (m_max_y_pos - m_min_y_pos) / 2);
                m_position.x = m_min_x_pos + ((m_max_x_pos -  m_min_x_pos) / 2);
                break;
                
            case BOMB_ORIENTATION::D :
                m_min_y_pos = m_position.y+1;
            
                m_position.y = m_min_y_pos + ( (m_max_y_pos - m_min_y_pos) / 2);
                break;
                
            case BOMB_ORIENTATION::DL :
                m_max_x_pos = m_position.x-1;
                m_min_y_pos = m_position.y+1;
                
                m_position.y = m_min_y_pos + ( (m_max_y_pos - m_min_y_pos) / 2);
                m_position.x = m_min_x_pos + ((m_max_x_pos -  m_min_x_pos) / 2);
                break;
                
            case BOMB_ORIENTATION::L :
                m_max_x_pos = m_position.x-1;
                
                m_position.x = m_min_x_pos + ((m_max_x_pos -  m_min_x_pos) / 2);
                break;
                
            case BOMB_ORIENTATION::UL :
                m_max_x_pos = m_position.x-1;
                m_max_y_pos = m_position.y-1;
            
                m_position.y = m_min_y_pos + ( (m_max_y_pos - m_min_y_pos) / 2);
                m_position.x = m_min_x_pos + ((m_max_x_pos -  m_min_x_pos) / 2);
                break;
        }
    }

    /**
     * This method is called after updateBatmanPosition() to get the updated position.
     * 
     * @returns Batman position on the grid.
     **/
    vector2i getPosition() const
    {
        return m_position;
    }
        
private : 
    vector2i m_position;
    int m_min_x_pos;
    int m_min_y_pos;
    int m_max_x_pos;
    int m_max_y_pos;
};



/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int W; // width of the building.
    int H; // height of the building.
    cin >> W >> H; cin.ignore();
    int N; // maximum number of turns before game over.
    cin >> N; cin.ignore();
    int X0;
    int Y0;
    cin >> X0 >> Y0; cin.ignore();
    
    BatmanControl batman_control(W,H,X0,Y0);
    BOMB_ORIENTATION current_orientation;
    vector2i current_pos;
    
    // game loop
    while (1) 
    {
        string bombDir; // the direction of the bombs from batman's current location (U, UR, R, DR, D, DL, L or UL)
        cin >> bombDir; cin.ignore();
        
        if (bombDir == "U")
        {
            current_orientation = BOMB_ORIENTATION::U;
        }
        else if (bombDir == "UR")
        {
            current_orientation = BOMB_ORIENTATION::UR;
        }
        else if (bombDir == "R")
        {
            current_orientation = BOMB_ORIENTATION::R;
        }
        else if (bombDir == "DR")
        {
            current_orientation = BOMB_ORIENTATION::DR;
        }
        else if (bombDir == "D")
        {
            current_orientation = BOMB_ORIENTATION::D;
        }
        else if (bombDir == "DL")
        {
            current_orientation = BOMB_ORIENTATION::DL;
        }
        else if (bombDir == "L")
        {
            current_orientation = BOMB_ORIENTATION::L;
        }
        else if (bombDir == "UL")
        {
            current_orientation = BOMB_ORIENTATION::UL;
        }
        else
        {
            cout << "UNKNOWN ENTRY : " << bombDir << endl;
            cerr << "UNKNOWN ENTRY : " << bombDir << endl;
        }
        
        cerr << "BOMB POSITION : " << bombDir << endl;
        batman_control.updateBatmanPosition(current_orientation);
        
        current_pos = batman_control.getPosition();

        // the location of the next window Batman should jump to.
        cout << current_pos.x << " " << current_pos.y << endl;
    }
}