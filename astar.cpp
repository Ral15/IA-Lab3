/*
Raúl Mar
A00512318
https://www.alphagrader.com/courses/13/assignments/99
*/
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <vector>

struct Node {
  // vector with curr state
  std::vector<std::vector<char>> state;
  // the path that will tell from which platform i come from
  std::pair<int, int> path;
  // the cost of moving
  int path_cost;
  //
  std::vector<int> heights;
  std::vector<Node> nextNodes();
  void print_grid();
  int calculateHeuristic(Node *goal_node);
  Node(){};
  Node(std::vector<std::vector<char>> &s, std::vector<int> &h, int c,
       std::pair<int, int> &p)
      : state(s), heights(h), path_cost(c), path(p){};
  Node(std::vector<std::vector<char>> &s) : state(s) {
    for (int i = 0; i < s[0].size(); i++) {
      int curr_height = 0;
      for (int j = 0; j < s.size(); j++) {
        // empty
        if (s[j][i] == '*')
          continue;
        else
          curr_height++;
      }
      heights.push_back(curr_height);
    }
    path_cost = 0;
  }
  bool is_goal_node(Node *goal_node);

  bool operator>(const Node &n) const { return path_cost > n.path_cost; }
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
  bool operator()(std::tuple<Node *, int> n1,
                  std::tuple<Node *, int> n2) const {
    Node *a, *b;
    int cost_a, cost_b;
    std::tie(a, cost_a) = n1;
    std::tie(b, cost_b) = n2;
    return (cost_a > cost_b);
  }
};

std::vector<Node> Node::nextNodes() {
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
      if (heights[j] < state.size()) {
        std::swap(state[heights[j]][j], state[heights[i] - 1][i]);
        heights[j]++, heights[i]--;
        int cost = 1 + abs(i - j);
        std::pair<int, int> p = std::make_pair(i, j);
        next_nodes.push_back(Node(state, heights, cost, p));
        heights[j]--, heights[i]++;
        std::swap(state[heights[j]][j], state[heights[i] - 1][i]);

      } else {
        continue;
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
          if (i == max_height) {
            std::cout << "No solution found\n";
            exit(0);
          }
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

void Astar(Node *init_node, Node *goal_node) {
  // int start_s=clock();
  std::map<Node *, int> costs;
  std::map<Node *, Node *> prev_path;
  std::vector<std::string> paths;
  // in here i can have the path
  std::priority_queue<std::tuple<Node *, int>,
                      std::vector<std::tuple<Node *, int>>, PqCompare>
      pq;
  // cost of inital node
  costs[init_node] = 0;
  // add init node to the priority_queue
  pq.push(std::make_tuple(init_node, 0));

  int tims = 0;
  while (!pq.empty()) {
    Node *curr_node;
    int estimated_cost;
    tims++;
    std::tie(curr_node, estimated_cost) = pq.top();
    pq.pop();
    if (estimated_cost - curr_node->calculateHeuristic(goal_node) >
        costs[curr_node]) {
      continue;
    }

    if (curr_node->is_goal_node(goal_node)) {
      // std::cout << "nodes serched: " << tims << std::endl;
      // print total cost
      std::cout << costs[curr_node] << std::endl;
      // loop to the prev path to create the output path
      auto it = curr_node;
      while (prev_path.find(it) != prev_path.end()) {
        paths.push_back("(" + std::to_string(it->path.first) + ", " +
                        std::to_string(it->path.second) + ")");
        it = prev_path[it];
      }
      for (int i = paths.size() - 1; i >= 1; i--) {
        std::cout << paths[i] << "; ";
      }
      std::cout << paths[0] << std::endl;
      // int stop_s=clock();
      // std::cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;
      exit(0);
    } else {
      // add curr node to explored
      // get next nodes by expanding current state
      for (auto it : curr_node->nextNodes()) {
        Node *nxt = new Node(it.state, it.heights, it.path_cost, it.path);
        if (costs.find(nxt) == costs.end() ||
            nxt->path_cost + costs[curr_node] < costs[nxt]) {
          costs[nxt] = nxt->path_cost + costs[curr_node];
          pq.push(std::make_tuple(
              nxt, costs[nxt] + nxt->calculateHeuristic(goal_node)));
          prev_path[nxt] = curr_node;
        }
      }
    }
  }
}

// calculates the heuristic value of the current node
int Node::calculateHeuristic(Node *goal_node) {
  int heuristic = 0;
  // iterate through all the state
  for (int i = 0; i < state[0].size(); i++) {
    for (int j = 0; j < state.size(); j++) {
      // if goal_node has X , skip
      if (goal_node->state[j][i] == 'X') break;
      if (state[j][i] == goal_node->state[j][i]) continue;
      heuristic++;
    }
  }
  for (int i = 0; i < heights.size(); i++) {
    if (heights[i] == goal_node->heights[i]) continue;
    heuristic++;
  }
  return heuristic;
}

int main() {
  // get max height of platforms, initial state & goal state
  std::string height, i_state, g_state;
  getline(std::cin, height);
  getline(std::cin, i_state);
  getline(std::cin, g_state);

  int max_height = stoi(height);

  // count number of platforms
  int platforms = count_platforms(i_state);
  // parse the init state & goal state
  auto parsed_i_state = parse_state(i_state, max_height, platforms);
  auto parsed_g_state = parse_state(g_state, max_height, platforms);

  // create goal & init state
  Node *init_node = new Node(parsed_i_state);
  Node *goal_node = new Node(parsed_g_state);
  // do A* search, exits program if solution found
  Astar(init_node, goal_node);
  // if no solution found
  std::cout << "No solution found\n";

  return 0;
}
