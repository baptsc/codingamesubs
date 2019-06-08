/**
    This is my solution to the challenge "Chuck Norris" on Codingame.

    Challenge link : https://www.codingame.com/ide/puzzle/chuck-norris
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>

using namespace std;

int main()
{
    string MESSAGE;
    getline(cin, MESSAGE);
    
    int i = 0;
    int j = 0;
    
    int pending_size = 0;
    bool current_sequence_value = false;
    
    string result = "";
    
    for(auto c : MESSAGE)
    {
        bitset<7> bits(c);
                
        for (i = 0; i < 7; i++)
        {
            if (bits[6-i] != current_sequence_value)
            {
                if (pending_size > 0)
                {
                    if (result.size() > 0)
                    {
                        result.append(" ");
                    }
                    
                    result.append((current_sequence_value ? "0 " : "00 "));
                    
                    for (j = 0; j < pending_size; j++)
                    {
                        result.append("0");
                    }
                }
                
                pending_size = 1;
                current_sequence_value = bits[6-i];
            }
            else
            {
                pending_size++;
            }
        }
    }
    
    // After the loop, the last sequence hasn't been terminated, so we must do it.
    if (pending_size > 0)
    {
        if (result.size() > 0)
        {
            result.append(" ");
        }
        
        result.append((current_sequence_value ? "0 " : "00 "));
        
        for (j = 0; j < pending_size; j++)
        {
            result.append("0");
        }
    }
    
    
    cout << result << endl;
}