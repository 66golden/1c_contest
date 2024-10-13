#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <string>
#include <chrono>
#define state std::pair<int, std::vector<std::stack<int>>>

bool isReducible(std::stack<int> heap) {
  if (heap.size() < 9) return false;
  for (int i = 1; i <= 9; ++i) {
    if (heap.top() != i) return false;
    heap.pop();
  }
  return true;
}

bool isEmpty(const std::vector<std::stack<int>>& decks) {
  for (const std::stack<int>& heap : decks) {
    if (!heap.empty()) {
      return false;
    }
  }
  return true;
}

bool isTimeOut(long timeout_ms, std::chrono::time_point<std::chrono::high_resolution_clock> start_time) {
  std::chrono::time_point<std::chrono::high_resolution_clock> current_time = std::chrono::high_resolution_clock::now();
  long time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
  if (time_elapsed > timeout_ms) {
    return true;
  }
  return false;
}

std::string stateKey(const std::vector<std::stack<int>>& decks) {
  std::string current_key;
  for (std::stack<int> heap : decks) {
    while (!heap.empty()) {
      current_key += static_cast<char>(heap.top());
      heap.pop();
    }
    current_key += "X";
  }
  return current_key;
}

int h(const std::vector<std::stack<int>>& decks) {
  int misplaced_cards_cnt = 0;
  for (std::stack<int> heap: decks) {
    int expected_card = 1;
    while (!heap.empty() && heap.top() == expected_card) {
      --expected_card;
      heap.pop();
    }
    misplaced_cards_cnt += static_cast<int>(heap.size());
  }
  return misplaced_cards_cnt;
}

int aStar(std::vector<std::stack<int>>& start_decks, long timeout_ms) {
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time = std::chrono::high_resolution_clock::now();
  std::priority_queue<state, std::vector<state>, std::greater<>> pq;
  std::set<std::string> visited_states;

  pq.push({h(start_decks), start_decks});
  visited_states.insert(stateKey(start_decks));

  while (!pq.empty()) {
    auto [f, decks] = pq.top();
    pq.pop();
    int g = f - h(decks);

    if (isEmpty(decks)) {
      return g;
    }
    if (isTimeOut(timeout_ms, start_time)) {
      std::cout << "Превышен тайм-аут в 30 секунд\n";
      return -1;
    }

    for (int i = 0; i < 8; ++i) {
      if (decks[i].empty()) continue;
      int card = decks[i].top();
      for (int j = 0; j < 8; ++j) {
        if (i != j && (decks[j].empty() || card < decks[j].top())) {
          std::vector<std::stack<int>> new_decks = decks;
          new_decks[i].pop();
          new_decks[j].push(card);

          if (isReducible(new_decks[j])) {
            for (int k = 0; k < 9; ++k) {
              if (!new_decks[j].empty()) {
                new_decks[j].pop();
              }
            }
          }

          std::string new_state_key = stateKey(new_decks);
          if (visited_states.find(new_state_key) == visited_states.end()) {
            visited_states.insert(new_state_key);
            int h_value = h(new_decks);
            pq.push({g + 1 + h_value, new_decks});
          }
        }
      }
    }
  }
  std::cout << "Тайм-аут в 30 секунд не превышен\n";
  return -1;
}

int main() {
  std::vector<std::stack<int>> decks(8);
  int card;
  long timeout_ms = 30000;

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 9; ++j) {
      std::cin >> card;
      decks[i].push(card);
    }
  }

  int result = aStar(decks, timeout_ms);

  if (result == -1) {
    std::cout << "Решение не найдено";
  } else {
    std::cout << "Решение найдено за " << result << " ходов";
  }
}
