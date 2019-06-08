/**
    This is my solution to the challenge "Skynet revolution - Episode 01" on Codingame.

    Challenge link : https://www.codingame.com/ide/puzzle/skynet-revolution-episode-1
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

using namespace std;


/**
 * This class describes a Network node.
 */
class Node
{
    public :
        /**
         * Node constructor.
         * 
         * @param[in] Node number.
         */
        Node(int value) : m_value(value), m_is_exit(false), m_children() {}
        Node() = delete;
        
        
        /**
         * Adds a direct children to the node (if it doesn't exist yet, otherwise it does nothing).
         * 
         * @param[in] node The new children.
         */
        void addDirectChildren(Node* node)
        {
            m_children[node->m_value] = node;
        }
        
        /**
         * Cuts the edge between the node and one of its children (if the children exists, otherwise it does nothing).
         * 
         * @param[in] node The children to remove.
         */
        void cutEdge(Node* node)
        {
            m_children.erase(node->m_value);
        }
        
        /**
         * Tells whether this node has a specific child or not.
         * 
         * @param[in] child_value The node value to look for in the child list.
         * 
         * @returns TRUE if the requested child exists, FALSE otherwise.
         */
        bool hasChild(int child_value) const
        {
            return m_children.find(child_value) != m_children.end();
        }
        
        /**
         * Marks this node as an exit or removes its exit flag.
         * 
         * @param[in] is_exit TRUE to mark the node as an exit, FALSE to mark it as a normal node.
         * 
         */
        void setExit(bool is_exit)
        {
            if (m_is_exit && !is_exit)
            {
                Node::sm_exits.erase(m_value);
            }
            else if (!m_is_exit && is_exit)
            {
                Node::sm_exits.insert(m_value);
            }
            
            m_is_exit = is_exit;
        }
        
        /**
         * @returns TRUE if this node is an exit, FALSE otherwise.
         */
        bool isExit() const
        {
            return m_is_exit;
        }
        
        /**
          * @returns Node's number.
         */
        int getValue() const
        {
            return m_value;
        }
        
        /**
         * Method based on the BFS algorithm to find the shortest path from this node to an exit.
         * It returns the last edge that leads to the found exit.
         * 
         * @returns Node pair in which pair.first corresponds to the node previous to the found exit and  pair.second corresponds to the exit node.
         *          If NO exit was found, pair.first == nullptr and pair.second == nullptr.
         */
        pair<Node*, Node*> getClosestExitEdge()
        {
            unordered_set<int> walked_nodes;
            unordered_map<int, Node*> current_layer;   
            unordered_map<int, Node*> next_layer; 
            
            // First BFS layer is the curent node.
            current_layer[m_value] = this;
            walked_nodes.insert(m_value);
            
            bool has_reached_end = false;
            bool has_found_exit = false;
            Node* found_exit = nullptr;
            Node* prev_node_to_exit = nullptr;
            
            while (!has_reached_end && !has_found_exit)
            {
                // Filling in the next BFS layer
                for (auto p : current_layer)
                {
                    Node::addChildrenToLayer(p.second, walked_nodes, next_layer);
                }
                
                // If the next BFS layer is empty, we can't go any further.
                if (next_layer.size() == 0)
                {
                    has_reached_end = true;
                }
                else // We walked some nodes after the current ones.
                {
                    // Checking if an exit was reached in the current walked_nodes (updated while filling in the next BFS layer).
                    for (int ex : Node::sm_exits)
                    {
                        if (walked_nodes.find(ex) != walked_nodes.end()) // An exit has been reached.
                        {
                            found_exit = next_layer[ex];
                            
                            // Looking for the node that leads to the reached exit
                            for (auto np : current_layer) 
                            {
                                if (np.second->hasChild(ex))
                                {
                                    prev_node_to_exit = np.second;
                                    break;
                                }
                            }
                            
                            if (found_exit != nullptr && prev_node_to_exit != nullptr) // We found everything we need to return
                            {
                                
                                has_found_exit = true;
                                break;
                            }
                            else // Strange case : reached an exit that is no longer linked to a node in the current layer.
                            {
                                found_exit = nullptr;
                                prev_node_to_exit = nullptr;
                                continue;
                            }
                        }
                    }
                }
                
                // From one iteration to the other, the next_layer becomes the current one and it will restart from scratch at the next iteration.
                current_layer = next_layer;
                next_layer.clear();
            }
            
            return make_pair(prev_node_to_exit, found_exit);
        }
    
    private :
        
        /**
         *  getClosestExitEdge() subMethod to add nodes to the next layer.
         *  It adds children from a parent except if they have already been walked through during a previous iteration.
         *  All inserted children will be marked as walked through in the walked_nodes array.
         * 
         *  @param[in] node Parent node to add children from.
         *  @param[inout] walked_nodes Already walked nodes during the BFS algorithm.
         *  @param[inout] layer Layer to add children to.
         */
        static void addChildrenToLayer(const Node* node, unordered_set<int> &walked_nodes, unordered_map<int, Node*> &layer)
        {
            for (auto p : node->m_children)
            {
                if (walked_nodes.find(p.first) == walked_nodes.end())
                {
                    walked_nodes.insert(p.first);
                    layer[p.first] = p.second;
                }
            }
        }
        
        // Static member that stores exit nodes numbers.
        static unordered_set<int> sm_exits;
    
        int m_value;
        bool m_is_exit;
        unordered_map<int, Node*> m_children;
};

// Static member initialization.
unordered_set<int> Node::sm_exits(0);


/**
 * This class decribes a Network graph.
 */
class Network
{
    public :
        /**
         * Network constructor.
         */
        Network() : m_nodes() {}
        
        
        /**
         * Network destructor.
         */
        ~Network()
        {
            for(auto p : m_nodes)
            {
                delete p.second;
            }
        }
        
        /**
         * Adds an edge to the graph, adding also nodes along the way if they don't already exist.
         * 
         * @param[in] node_1_val First edge node number.
         * @param[in] node_2_val Second edge node number.
         */
        void addEdge(int node_1_val, int node_2_val)
        {            
            if (m_nodes.find(node_1_val) == m_nodes.end())
            {
                m_nodes[node_1_val] = new Node(node_1_val);
            }       
            
            if (m_nodes.find(node_2_val) == m_nodes.end())
            {
                m_nodes[node_2_val] = new Node(node_2_val);
            }
            
             m_nodes[node_1_val]->addDirectChildren(m_nodes[node_2_val]);
             m_nodes[node_2_val]->addDirectChildren(m_nodes[node_1_val]);
        }
        
        /**
         * Marks a node as an exit in the network's graph.
         * 
         * @param[in] node_val Node number to mark as an exit.
         */
        void setExit(int node_val)
        {
            unordered_map<int, Node*>::iterator search_res;
            
            if ((search_res = m_nodes.find(node_val)) != m_nodes.end())
            {
                search_res->second->setExit(true);
            } 
        }
        
        
        /**
         * Removes an edge that leads to an exit in the graph and returns the resulting command for the virus.
         * It removes the closest edge to the closest exit from agent position.
         * 
         * @param[in] agent_position Agent postion to start path finding from.
         * 
         * @returns A command that corresponds to the cur operated by the method, or "" if no cut was done.
         */
        string cutPathToExit(int agent_position)
        {
            string result("");
            pair<Node*, Node*> edge = m_nodes[agent_position]->getClosestExitEdge();
            
            if (edge.first != nullptr && edge.second != nullptr) // We found an edge to cut.
            {
                result = to_string(edge.first->getValue())+" "+ to_string(edge.second->getValue());
                edge.first->cutEdge(edge.second);
                edge.second->cutEdge(edge.first);
            }
            
            return result;
        }
        
    private :
        unordered_map<int, Node*> m_nodes;     
};


/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int N; // the total number of nodes in the level, including the gateways
    int L; // the number of links
    int E; // the number of exit gateways
    cin >> N >> L >> E; cin.ignore();
    Network game_network;
    
    for (int i = 0; i < L; i++) 
    {
        int N1; // N1 and N2 defines a link between these nodes
        int N2;
        cin >> N1 >> N2; cin.ignore();
        
        game_network.addEdge(N1, N2);
    }
    
    for (int i = 0; i < E; i++) 
    {
        int EI; // the index of a gateway node
        cin >> EI; cin.ignore();
        game_network.setExit(EI);
    }

    // game loop
    while (1) 
    {
        int SI; // The index of the node on which the Skynet agent is positioned this turn
        cin >> SI; cin.ignore();

        cout << game_network.cutPathToExit(SI) << endl;
    }
}