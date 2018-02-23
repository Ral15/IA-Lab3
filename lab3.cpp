#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <vector>

struct Node {
  std::vector<std::vector<char>> state;
  int cost;
  std::vector<int> heights;
  std::vector<Node> nextNodes();
  std::vector<int> getHeights(std::vector<std::vector<char>> &s);
  void print_grid();
  Node(){};
  Node(std::vector<std::vector<char>> &s, std::vector<int> &h, int c)
      : state(s), heights(h), cost(c){};
  Node(std::vector<std::vector<char>> &s) : state(s) {
    // print_grid();
    // std::cout << "heights: " << std::endl;
    for (int i = 0; i < s[0].size(); i++) {
      int curr_height = 0;
      for (int j = 0; j < s.size(); j++) {
        // empty
        if (s[j][i] == '*')
          continue;
        else
          curr_height++;
      }
      // std::cout << curr_height << " ";
      heights.push_back(curr_height);
    }
    // std::cout << std::endl;
    cost = 0;
  }
  bool is_goal_node(Node *goal_node);

  bool operator>(const Node &n) const { return cost > n.cost; }
};

void Node::print_grid() {
  for (auto it : state) {
    for (auto c : it) {
      std::cout << c << " ";
    }
    std::cout << std::endl;
  }
}

bool Node::is_goal_node(Node *g_node) {
  auto &g = g_node->state;
  for (int i = 0; i < state[0].size(); i++) {
    for (int j = 0; j < state.size(); j++) {
      // see if i have x
      if (g[0][i] == 'X') break;
      if (g[j][i] == state[j][i])
        continue;
      else
        return false;
    }
  }
  return true;
}

// how will i compare my nodes cost
class PqCompare {
 public:
  bool operator()(std::tuple<Node*, int> n1, std::tuple<Node*, int> n2) const { 
  	Node *a, *b; 
  	int cost_a, cost_b;
  	std::tie(a, cost_a) = n1;
  	std::tie(b, cost_b) = n2;
  	return (cost_a > cost_b); 
  }
};

std::vector<Node> Node::nextNodes() {
  // vector where I will add all of my nodes
  std::vector<Node> next_nodes;
  // create vector with heights
  // get number of platforms
  int num_platforms = state[0].size();
  // go through platforms & expand
  for (int i = 0; i < num_platforms; i++) {
    if (!heights[i]) {
      continue;
    }
    for (int j = 0; j < num_platforms; j++) {
      // same platform, skip
      if (j == i) {
        continue;
      }
      // you can move
      // std::cout << "i: " << i << " " << j << std::endl;
      if (heights[j] < state.size()) {
        // std::cout << "from" << std::endl;
        // curr_node->print_grid();

        std::swap(state[heights[j]][j], state[heights[i] - 1][i]);
        heights[j]++, heights[i]--;
        int cost = 1 + abs(i - j);
        // std::cout << "cost: " << cost << std::endl;
        next_nodes.push_back(Node(state, heights, cost));

        // std::cout << "to" << std::endl;
        // next_nodes.back().print_grid();

        heights[j]--, heights[i]++;
        std::swap(state[heights[j]][j], state[heights[i] - 1][i]);
        //  std::cout << "from2" << std::endl;
        // curr_node->print_grid();
        // std::cout << "----------------------" << std::endl;

      } else {
        std::cout << "wut " << i << " " << j << std::endl;
        break;
      }
    }
  }
  return next_nodes;
}

std::vector<std::vector<char>> parse_state(std::string &in, int max_height,
                                           int platforms) {
  // create matrix filled with *
  std::vector<std::vector<char>> s(max_height,
                                   std::vector<char>(platforms, '*'));
  int i = 0, j = 0;
  for (int k = 0; k < in.length(); k++) {
    // platform starts
    if (in[k] == '(') {
      while (in[k++] != ')') {
        if (isalpha(in[k])) {
          s[i][j] = in[k];
          i++;
        }
      }
      j++;
    } else if (in[k] == 'X') {
      s[i][j] = 'X';
      j++;
    }
    i = 0;
  }
  return s;
}

int count_platforms(std::string &in) {
  int p = 0;
  for (int i = 0; i < in.length(); i++) {
    if (in[i] == ')') {
      p++;
    }
  }
  return p;
}

void UCS(Node *init_node, Node *goal_node) {
  // all my costs, this can help to see if i find the same node with lower cost
  std::map<Node*, int> costs;
  // in here i can have the path
  // std::map<Node, Node> prev_nodes;
  std::priority_queue<std::tuple<Node *, int>, std::vector<std::tuple<Node *, int>>, PqCompare> pq;

  // cost of inital node
  costs[init_node] = 0;
  // add init node to the priority_queue
  pq.push(std::make_tuple(init_node, 0));
  // int x;
  while (!pq.empty()) {
    Node *curr_node;
    int curr_cost;
    std::tie(curr_node, curr_cost) = pq.top();
    std::cout << "curr_node: " << std::endl;
    curr_node->print_grid();
    std::cout << "***********************" << std::endl;
    pq.pop();
    if (curr_cost > costs[curr_node]) {
    	continue;
    }

    if (curr_node->is_goal_node(goal_node)) {
      std::cout << "si se armo" << std::endl;
      std::cout << costs[curr_node] << std::endl;
      exit(0);
      // get path and costs
    } else {
      // add curr node to explored
      // get next nodes by expanding current state
      for (auto it : curr_node->nextNodes()) {
      	Node *nxt = new Node(it.state, it.heights, it.cost);
        std::cout << it.cost << std::endl;
        if (costs.find(nxt) == costs.end() || nxt->cost + curr_cost < costs[nxt]) {
        	costs[nxt] = it.cost + curr_cost;
        	pq.push(std::make_tuple(nxt, costs[nxt]));
        }
        // Node *t = new Node(it.state, it.heights, it.cost);
        // t->cost += costs[t];
        // pq.push(t);
        it.print_grid();
        // std::cin >> x;
        // std::cout << "***********************" << std::endl;
      }
    }
  }
}

int main() {
  std::string height, i_state, g_state;
  getline(std::cin, height);
  getline(std::cin, i_state);
  getline(std::cin, g_state);

  int max_height = stoi(height);

  int platforms = count_platforms(i_state);

  auto a = parse_state(i_state, max_height, platforms);
  auto b = parse_state(g_state, max_height, platforms);

  Node *init_node = new Node(a);
  Node *goal_node = new Node(b);

  for (int i = 0; i < a.size(); i++) {
    for (int j = 0; j < a[i].size(); j++) {
      std::cout << a[i][j] << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << "***************" << std::endl;

  // auto nxt_nodes = init_node->nextNodes(init_node);
  std::cout << "goal node: \n";
  goal_node->print_grid();
  std::cout << std::endl;

  // std::cout << init_node->is_goal_node(goal_node) << std::endl;

  UCS(init_node, goal_node);

  // for (auto it: nxt_nodes) std::cout << it.state[2][0] << std::endl;

  // for (int i = 0; i < init_state->heights.size(); i++) {
  //   std::cout << init_state->heights[i] << std::endl;
  // }

  return 0;
}
