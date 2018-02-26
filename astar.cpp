#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <vector>

struct Node {
  std::vector<std::vector<char>> state;
  std::pair<int, int> path;
  int cost;
  std::vector<int> heights;
  std::vector<Node> nextNodes();
  std::vector<int> getHeights(std::vector<std::vector<char>> &s);
  void print_grid();
  int calculateHeuristic(Node *goal_node);
  Node(){};
  Node(std::vector<std::vector<char>> &s, std::vector<int> &h, int c,
       std::pair<int, int> &p)
      : state(s), heights(h), cost(c), path(p){};
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
      // std::cout << "i: " << i << " height: " << curr_height << " \n";
      heights.push_back(curr_height);
    }
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
  // vector where I will add all of my nodes
  std::vector<Node> next_nodes;
  // create vector with heights
  // get number of platforms
  int num_platforms = state[0].size();
  // std::cout << num_platforms << std::endl;
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
        // std::cout << "wut " << i << " " << j << std::endl;
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
      std::cout << tims << std::endl;
      exit(0);
    } else {
      // add curr node to explored
      // get next nodes by expanding current state
      for (auto it : curr_node->nextNodes()) {
        Node *nxt = new Node(it.state, it.heights, it.cost, it.path);
        // std::cout << it.cost << std::endl;
        if (costs.find(nxt) == costs.end() ||
            nxt->cost + costs[curr_node] < costs[nxt]) {
          // std::cout << it.cost + curr_cost + it.calculateHeuristic(goal_node)
          // << std::endl;
          costs[nxt] = nxt->cost + costs[curr_node];
          pq.push(std::make_tuple(
              nxt, costs[nxt] + nxt->calculateHeuristic(goal_node)));
          prev_path[nxt] = curr_node;
          // std::cout << "este NO\n";
        }
      }
    }
  }
}

int Node::calculateHeuristic(Node *goal_node) {
  int heuristic = 0;
  // std::cout << "CALCULATE HEURIRSTIC\n";
  // goal_node->print_grid();
  // std::cout << "";
  // print_grid();
  for (int i = 0; i < state[0].size(); i++) {
    for (int j = 0; j < state.size(); j++) {
      if (goal_node->state[j][i] == 'X') break;
      if (state[j][i] == goal_node->state[j][i]) continue;
      heuristic++;
    }
  }
  for (int i = 0; i < heights.size(); i++) {
    if (heights[i] == goal_node->heights[i]) continue;
    heuristic++;
  }
  // std::cout << "h: " << heuristic << " cost: " << cost << " path: " <<
  // path.first << " " << path.second << std::endl;
  return heuristic;
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

  // goal_node->print_grid();

  // init_node->calculateHeuristic(goal_node);

  Astar(init_node, goal_node);
  std::cout << "No solution found\n";

  return 0;
}
