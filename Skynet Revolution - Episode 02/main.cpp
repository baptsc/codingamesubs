/**
    This is my solution to the challenge "Skynet revolution - Episode 02" on Codingame.

    Challenge link : https://www.codingame.com/ide/puzzle/skynet-revolution-episode-2
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
         *  @returns One exit node among children, or NULLPTR if this node hasn't got any exit node.
         */
        Node* getDirectExitNode()
        {
            Node* result = nullptr;
            
            for (auto c : m_children)
            {
                if (c.second->isExit())
                {
                    result = c.second;
                    break;
                }
            }
            
            return result;
        }
        
        /**
         * @returns A set containing all direct exits values.
         */
        unordered_set<int> getAllDirectExits() 
        {
            unordered_set<int> result;
            
            for (auto c : m_children)
            {
                if (c.second->isExit())
                {
                   result.insert(c.first);
                }
            }
            
            return move(result);
        }
        
        /**
         * Marks this node as an exit or removes its exit flag.
         * 
         * @param[in] is_exit TRUE to mark the node as an exit, FALSE to mark it as a normal node.
         * 
         */
        void setExit(bool is_exit)
        {
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
         * Optimized BFS algorithm taking into account the new problem constraints.
         * If this node is not directly linked to an exit (in which case we need to cut this link now), the alogrithm acts as follow :
         * 1 - It looks for the most critical "hot node" (i.e node leading to two exits or more) from the current node. 
         *    The most critical hot node is the one that is the closest one in "normal" node count (i.e the one we have less time to cut off).
         * 2 - If no hot node is left, we cut the closest edge leading to an exit.
         * 
         * @returns A pair in which pair.first is the node previous to an exit and pair.second is an exit node. If no exit is left to cut,
         *          the method returns a pair containing two NULLPTR.
         */
        pair<Node*, Node*> getMostUrgentEdge()
        {
            Node* direct_exit = getDirectExitNode();
            
            // Case in which this node is linked to an exit, we need to cut it now.
            if (direct_exit != nullptr)
            {
                return make_pair(this, direct_exit);
            }
            
            // Priority variables to make decisions about hot nodes.
            int lowest_priority = numeric_limits<int>::max();
            unordered_set<int> next_hot_nodes;
            unordered_map<int, int> current_nodes_priorities;
            unordered_map<int, int> next_nodes_priorities;
            
            // Classic BFS layer lists
            unordered_set<int> walked_nodes;
            unordered_map<int, Node*> current_layer;   
            unordered_map<int, Node*> next_layer; 
            
            // First BFS layer is the curent node.
            current_layer[m_value] = this;
            walked_nodes.insert(m_value);
            current_nodes_priorities[m_value] = 1;
            
            // We will reach end when no more node neeads to be investigated, 
            // which means that we don't necessarily go through the whole graph to answer.
            bool has_reached_end = false;
            
            // Answer if no hot node is found.
            Node* default_exit_node = nullptr;
            Node* default_prev_node_to_exit = nullptr;
            
            // Answer if we find a hot node.
            Node* hot_exit_node = nullptr;
            Node* hot_prev_node_to_exit = nullptr;
            
            // BFS LOOP
            while (!has_reached_end)
            {
                // Filling in the next BFS layer
                for (auto p : current_layer)
                {
                    Node::addChildrenToLayer(p.second, walked_nodes, next_layer, current_nodes_priorities[p.first], next_nodes_priorities, next_hot_nodes);
                }
                
                // As soon as we have an exit, we need to set the default node edge to cut,
                // in case we don't find a hot node. 
                if (default_exit_node == nullptr)
                {
                    for (auto p : next_layer)
                    {
                        default_exit_node = p.second->getDirectExitNode();
                        
                        if (default_exit_node != nullptr)
                        {
                            default_prev_node_to_exit = p.second;
                            break;
                        }
                    }
                }
                
                // If the next layer contains a hot node.
                if (next_hot_nodes.size() > 0)
                {
                    // Let's check what hot nodes we found.
                    for (int i : next_hot_nodes)
                    {
                        // If we found a hot node that is better in priority to the previous chosen one, we keep it for now.
                        if (next_nodes_priorities[i] < lowest_priority)
                        {
                            lowest_priority = next_nodes_priorities[i];
                            hot_prev_node_to_exit = next_layer[i];
                            hot_exit_node = next_layer[i]->getDirectExitNode();
                        }
                        
                        // In this algorithm, hot nodes are considered as exits (nodes we loose if reached), so they end 
                        // their current path there.
                        next_layer.erase(i);
                        current_nodes_priorities.erase(i);
                    }
                    
                    // For every other node in the layer that already has a worst priority than our lowest one,
                    // no need to go any further for it, no way it can be chosen in the end, so we remove it
                    // from the next iteration.
                    for (auto p : next_nodes_priorities)
                    {
                        if (p.second > lowest_priority)
                        {
                            next_layer.erase(p.first);
                            current_nodes_priorities.erase(p.first);
                        }
                    }
                }
                
                // If the next BFS layer is empty, we can't go any further.
                if (next_layer.size() == 0)
                {
                    has_reached_end = true;
                }
                
                // From one iteration to the other, the next_layer becomes the current one and it will restart from scratch at the next iteration.
                current_layer = next_layer;
                current_nodes_priorities = next_nodes_priorities;
                next_nodes_priorities.clear();
                next_hot_nodes.clear();
                next_layer.clear();
            }
            
            // In the end, either we can cut off from a hot node, or we cut the default edge we found. 
            if (hot_exit_node != nullptr)
            {
                return make_pair(hot_prev_node_to_exit, hot_exit_node);
            }
            else
            {
                return make_pair(default_prev_node_to_exit, default_exit_node);
            }
        }
        
    private :
        
        
        /**
         *  getMostUrgentEdge() subMethod to add nodes to the next layer.
         *  It adds children from a parent except if :
         *   - They have already been walked through during a previous iteration.
         *   - They are an exit.
         *  All inserted children will be marked as walked through in the walked_nodes array.
         * 
         *  This method also updates priority data for the next layer so that decisions can be made in method getMostUrgentEdge().
         * 
         *  @param[in] node Parent node to add children from.
         *  @param[inout] walked_nodes Already walked nodes during the BFS algorithm.
         *  @param[inout] layer Layer to add children to.
         *  @param[in] node_priority Priority of the parent node.node
         *  @param[inout] node_priorities Priorities list to update with the retained children nodes.
         *  @param[inout] hot_nodes List of hot nodes (i.e nodes leading to at least 2 exits) to update if we find hot nodes among children.
         * 
         */
        static void addChildrenToLayer(const Node* node, unordered_set<int> &walked_nodes, unordered_map<int, Node*> &layer, int node_priority, unordered_map<int, int> &node_priorities, unordered_set<int> &hot_nodes)
        {
            int child_exit_size = 0;
            
            for (auto p : node->m_children)
            {
                if (!p.second->isExit() && walked_nodes.find(p.first) == walked_nodes.end())
                {
                    walked_nodes.insert(p.first);
                    layer[p.first] = p.second;
                    child_exit_size = p.second->getAllDirectExits().size();
                    
                    
                    if (child_exit_size == 0)
                    {
                        node_priorities[p.first] = node_priority + 1;
                    }
                    else
                    {
                        node_priorities[p.first] = node_priority;
                        
                        if (child_exit_size > 1)
                        {
                            hot_nodes.insert(p.first);
                        }
                    }
                }
            }
        }
    
        int m_value;
        bool m_is_exit;
        unordered_map<int, Node*> m_children;
};

/**
 * This class decribes a Network graph.
 */
class Network
{
    public :
        /**
         * Network constructor.
         */
        Network() : m_nodes(){}
        
        
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
         * Removes the most urgent edge that leads to an exit in the graph and returns the resulting command for the virus to cut it.
         * It removes either an edge on a node leading to two exit if the current agent position is not critical, or the closest edge
         * leading to an exit otherwise.
         * 
         * @param[in] agent_position Agent postion to start path finding from.
         * 
         * @returns A command that corresponds to the cut operated by the method, or "" if no cut was done.
         */
        string cutPathToExit(int agent_position)
        {
            string result("");
            
            Node* agent_node = m_nodes[agent_position];
            
            pair<Node*, Node*> edge = agent_node->getMostUrgentEdge();
            
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