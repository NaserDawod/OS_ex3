//Naser Dawod 323953893
#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <thread>
#include <queue>
#include <vector>
#include <semaphore.h>
#include <unistd.h>
using namespace std;

/**
 * Note: im using the second option for the Configuration file where its build only from numbers,
 * I've added a Configuration file for the example.
 * Also there is a makefile so in order to run it write "make" in the terminal then the ex3.out file well appear
 * */

class BQueue : public queue<string> {
    sem_t empty;
    sem_t full;
    sem_t mutex;
public:
    int news_ = 0, sports_ = 0, weather_ = 0;
    BQueue(int size) {
        sem_init(&mutex, 0, 1);
        sem_init(&full, 0, 0);
        sem_init(&empty, 0, size + 1);
    }

    void insert (string s) {
        sem_wait(&empty);
        sem_wait(&mutex);
        queue::push(s);
        sem_post(&mutex);
        sem_post(&full);
    }

    string remove() {
        sem_wait(&full);
        sem_wait(&mutex);
        string s = queue::front();
        queue::pop();
        sem_post(&mutex);
        sem_post(&empty);
        return s;
    }
};

class UBQueue : public queue<string> {
    sem_t mutex;
    sem_t full;
public:
    UBQueue() {
        sem_init(&mutex, 0, 1);
        sem_init(&full, 0, 0);
    }

    void insert (string s) {
        sem_wait(&mutex);
        queue::push(s);
        sem_post(&mutex);
        sem_post(&full);
    }

    string remove() {
        sem_wait(&full);
        sem_wait(&mutex);
        string s = queue::front();
        queue::pop();
        sem_post(&mutex);
        return s;
    }
};

vector<BQueue*> prods;
vector<int> amounts;
BQueue* coEditor = NULL;
UBQueue* sports;
UBQueue* news;
UBQueue* weather;

void *produce(void * arg) {
    int *index = (int *)arg;
    int i = *index;
    int n = amounts[i];
    BQueue* bq = prods[i];
    for (int j = 0; j < n; j++) {
        if (j % 3 == 0){
            string s = "Producer " + to_string(i + 1) + " NEWS " + to_string(bq->news_);
            bq->news_++;
            bq->insert(s);
        } else if (j % 3 == 1) {
            string s = "Producer " + to_string(i + 1) + " SPORTS " + to_string(bq->sports_);
            bq->sports_++;
            bq->insert(s);
        } else {
            string s = "Producer " + to_string(i + 1) + " WEATHER " + to_string(bq->weather_);
            bq->weather_++;
            bq->insert(s);
        }
    }
    bq->insert("done");
    return NULL;
}

void *dispatch(void* arg) {
    bool stop = false;
    int n = prods.size();
    while (!stop) {
        stop = true;
        for (int i = 0; i < n; i++) {
            if (prods[i] != NULL) {
                string article = prods[i]->remove();
                if (article == "done") {
                    prods[i] = NULL;
                } else if (article.find("SPORTS") != string::npos) {
                    sports->insert(article);
                } else if (article.find("NEWS") != string::npos) {
                    news->insert(article);
                } else {
                    weather->insert(article);
                }
                stop = false;
            }
        }
    }
    sports->insert("done");
    news->insert("done");
    weather->insert("done");
    return NULL;
}

void *coEdit(void * arg) {
    UBQueue* q = (UBQueue *)arg;
    this_thread::sleep_for(std::chrono::milliseconds(100));
    string art = q->remove();
    while (art != "done") {
        this_thread::sleep_for(std::chrono::milliseconds(100));
        coEditor->insert(art);
        art = q->remove();
    }
    coEditor->insert("done");
    return NULL;
}

void *screenManager(void* arg) {
    sleep(1);
    int c = 0;
    while (c != 3) {
        string output = coEditor->remove();
        if (output == "done") {
            c++;
        } else {
            cout << output << endl;
        }
    }
    cout << "DONE" << endl;
    return NULL;
}

int main(int argc, char** argv) {
    sports = new UBQueue();
    news = new UBQueue();
    weather = new UBQueue();
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
        prods.push_back(new BQueue(size));
        N++;
        amounts.push_back(amount);
        getline(conf, line);
    }

    for (int i = 0; i < N; i += 1) {
        pthread_t t;
        int x = i;
        pthread_create(&t, NULL, produce, (void *)&x);
        this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    pthread_t disP, coEdit1, coEdit2, coEdit3, screen;
    pthread_create(&disP, NULL, dispatch, NULL);
    pthread_create(&coEdit1, NULL, coEdit, (void *)sports);
    pthread_create(&coEdit2, NULL, coEdit, (void *)news);
    pthread_create(&coEdit3, NULL, coEdit, (void *)weather);

    pthread_create(&screen, NULL, screenManager, NULL);
    pthread_join(screen, NULL);

    return 0;
}
