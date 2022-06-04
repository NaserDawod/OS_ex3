#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <thread>
#include <queue>
#include <vector>
#include <semaphore.h>
using namespace std;

class BQueue : public queue<string> {
    int size;
    sem_t empty;
    sem_t full;
    sem_t mutex;
public:
    BQueue(int size) {
        this->size = size;
        sem_init(&mutex, 0, 1);
        sem_init(&full, 0, 0);
        sem_init(&empty, 0, size);
    }

    void insert (string s) {

        queue::push(s);

    }

    string remove() {

        string s = queue::back();
        queue::pop();

        return s;
    }
};

vector<BQueue> prods;
vector<int> amounts;
BQueue* coEditor = NULL;
queue<string> sports;
queue<string> news;
queue<string> weather;

void produce(int i) {
    int n = amounts[i];
    BQueue bq = prods[i];
    int news_ = 0, sports_ = 0, weather_ = 0;
    for (int j = 0; j < n; j++) {
        if (j % 3 == 0){
            string s = "Producer " + to_string(i) + " NEWS " + to_string(news_++);
            bq.insert(s);
        } else if (j % 3 == 1) {
            string s = "Producer " + to_string(i) + " SPORTS " + to_string(sports_++);
            bq.insert(s);
        } else {
            string s = "Producer " + to_string(i) + " WEATHER " + to_string(weather_++);
            bq.insert(s);
        }
    }
}

void dispatch() {
    for (int i = 0; i < prods.size(); i++) {
        
    }
}

int main(int argc, char** argv) {
    fstream conf;
    conf.open(argv[1]);
    string line, size_S, amount_S;
    int size, amount, N = 0;
    if (!conf.is_open()) {
        perror("Error open");
        exit(EXIT_FAILURE);
    }
    while(getline(conf, line)) {
        if (!getline(conf, amount_S)) {
            size = atoi(const_cast<char*>(line.c_str()));
            coEditor = new BQueue(size);
            break;
        }
        getline(conf, size_S);
        size = atoi(const_cast<char*>(size_S.c_str()));
        amount = atoi(const_cast<char*>(amount_S.c_str()));
        BQueue bQueue(size);
        prods.push_back(bQueue);
        N++;
        amounts.push_back(amount);
        getline(conf, line);
    }




    return 0;
}
