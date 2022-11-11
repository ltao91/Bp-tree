#include <bits/stdc++.h>
#include <deque>
#include <exception>
#include <mutex>
#include <chrono>
#include <functional>
#include <atomic>
// #define DEBUG
using std::ceil;
using std::cout;
using std::deque;
using std::endl;
using std::make_pair;
using std::map;
using std::mutex;
using std::pair;
using std::queue;
using std::string;
using std::thread;
using std::vector;
#define INF ((1ll) << 60)
#define rep(i, n) for (long long i = 0; i < n; i++)
mutex print_lock;
bool flag = false;
template <class T>
class Node
{
public:
    int n;
    bool is_leaf;
    vector<T> key;
    vector<Node<T> *> c;
    // Node<T> *next, *prev; // only leaf
    mutex nlock;
    std::thread::id owner;
    // bool is_next = false, is_prev = false;
    Node() : nlock()
    {
        n = 0;
        is_leaf = true;
    }
    Node(int n_, bool is_leaf_) : nlock()
    {
        n = n_;
        is_leaf = is_leaf_;
    }
    ~Node()
    {
        if (is_leaf == false)
        {
            for (auto i : c)
            {
                delete i;
            }
        }
    }
};

template <class T>
class Btree
{
public:
    bool is_empty = true;
    mutex is_empty_lock;
    int M;
    Node<T> *root;
    map<Node<T> *, int> NUMBER;
    Btree()
    {
        Node<T> *x = new Node<T>(0, true);
        root = x;
    }
    // we have cail(M/2)<=|children|<=M;
    // |children|<=n+1 n==|key|
    Btree(int M_)
    {
        Node<T> *x = new Node<T>(0, true);
        root = x;
        M = M_;
    }
    ~Btree()
    {
        if (!is_empty)
            delete root;
    }
    // vector<T> Search_Range(T left, T right)
    // {
    //     auto l = Search_Node(left, root, root);
    //     auto r = Search_Node(right, root, root);
    //     vector<T> res;
    //     while (1)
    //     {
    //         if (l == r)
    //         {
    //             for (auto key : l->key)
    //             {
    //                 if (left <= key && key <= right)
    //                 {
    //                     res.push_back(key);
    //                 }
    //             }
    //             break;
    //         }
    //         for (auto key : l->key)
    //         {
    //             if (left <= key)
    //             {
    //                 res.push_back(key);
    //             }
    //         }
    //         l = l->next;
    //     }
    //     return res;
    // }
    bool Search(T k)
    {
        if (root->n == 0)
            return false;
        return Search(k, root);
    }
    bool Search(T k, Node<T> *x)
    {
        if (x->is_leaf)
        {
            for (auto i : x->key)
                if (k == i)
                    return true;
            return false;
        }
        int i = 0;
        for (; i < x->n && x->key[i] <= k; i++)
        {
        }
        return Search(k, x->c[i]);
    }
    Node<T> *Search_Node(T k, Node<T> *x, Node<T> *p, map<Node<T> *, Node<T> *> &Parent, deque<Node<T> *> &locked)
    {
        if (x->owner != std::this_thread::get_id())
        {
            print_lock.lock();
            cout << "OWNER" << endl;
            cout << x->owner << endl;
            cout << endl;
            cout << "this" << endl;
            cout << std::this_thread::get_id() << endl;
            cout << (root == x) << endl;
            print_lock.unlock();
            exit(0);
        }
        if (x != p)
        {
            Parent[x] = p;
        }
        if (x->is_leaf)
        {
            return x;
        }
        int i = 0;
        for (; i < x->n && x->key[i] <= k; i++)
        {
        }
        while (!x->c.at(i)->nlock.try_lock())
        {
        }
        auto next = x->c[i];
        x->c[i]->owner = std::this_thread::get_id();
        if (x->c[i]->n < M - 1)
        {
            while (!locked.empty())
            {
                locked.front()->nlock.unlock();
                locked.pop_front();
            }
        }
        locked.push_back(next);
        return Search_Node(k, next, x, Parent, locked);
    }
    void Insert(T k)
    {
        if (is_empty)
        {
            is_empty_lock.lock();
            if (is_empty)
            {
                // cout << "CREATE TREE" << endl;
                auto node = new Node<T>(1, true);
                node->key.push_back(k);
                root = node;
                is_empty = false;
                is_empty_lock.unlock();
                return;
            }
            is_empty_lock.unlock();
        }

        map<Node<T> *, Node<T> *> Parent = map<Node<T> *, Node<T> *>();
        deque<Node<T> *> locked;

        while (!root->nlock.try_lock())
        {
            // print_lock.lock();
            // cout<<std::this_thread::get_id()<<" is waiting for "<<root->owner<<endl;
            // print_lock.unlock();
        }
        root->owner = std::this_thread::get_id();

        locked.push_back(root);
        auto leaf = Search_Node(k, root, root, Parent, locked);
        for (auto key : leaf->key)
            if (key == k)
            {
                return;
            }
        Insert_Leaf(k, leaf, Parent, locked);
        while (!locked.empty())
        {
            locked.front()->nlock.unlock();
            locked.pop_front();
        }
    }
    void Insert_Leaf(T k, Node<T> *x, map<Node<T> *, Node<T> *> &Parent, deque<Node<T> *> &locked)
    {
        assert(x->key.size() == x->n);
        int i = 0;
        for (; i < x->n && x->key.at(i) <= k; i++)
        {
        }
        T HOGE;
        x->key.push_back(HOGE);
        for (int j = x->n; j > i; j--)
        {
            x->key.at(j) = x->key.at(j - 1);
        }
        x->key.at(i) = k;
        x->n++;
        if (x->n == M)
        {
            SplitLeaf(x, Parent, locked);
        }
    }

    void SplitLeaf(Node<T> *leaf, map<Node<T> *, Node<T> *> &Parent, deque<Node<T> *> &locked)
    {
        assert(leaf->key.size() == leaf->n);
        Node<T> *left = new Node<T>(0, true);
        Node<T> *right = new Node<T>(0, true);
        int up = (M - 1 + 2 - 1) / 2;
        for (int i = 0; i < up; i++)
        {
            left->key.push_back(leaf->key[i]);
            left->n++;
        }
        T key = leaf->key[up];
        for (int i = up; i < leaf->key.size(); i++)
        {
            right->key.push_back(leaf->key[i]);
            right->n++;
        }

        // left->next = right;
        // right->next = leaf->next;
        // if (leaf->is_prev)
        // {
        //     leaf->prev->next = left;
        //     left->prev = leaf->prev;
        //     left->is_prev = true;
        // }
        // left->next = right;
        // right->prev = left;
        // left->is_next = true;
        // right->is_prev = true;
        // if (leaf->is_next)
        // {
        //     leaf->next->prev = right;
        //     right->next = leaf->next;
        //     right->is_next = true;
        // }
        if (Parent.find(leaf) != Parent.end())
        {
            Insert_Non_Leaf(key, Parent[leaf], left, right, Parent, locked);
        }
        else
        {
            auto node = new Node<T>(1, false);
            node->key.push_back(key);
            node->c.push_back(left);
            node->c.push_back(right);
            locked.pop_front();
            root = node;
        }
    }
    void Insert_Non_Leaf(T k, Node<T> *x, Node<T> *left, Node<T> *right, map<Node<T> *, Node<T> *> &Parent, deque<Node<T> *> &locked)
    {
        assert(x->key.size() == x->n);
        assert(x->c.size() == x->n + 1);
        int i = 0;
        for (; i < x->n && x->key[i] <= k; i++)
        {
        }
        T HOGE;
        x->key.push_back(HOGE);
        for (int j = x->n; j > i; j--)
        {
            x->key[j] = x->key[j - 1];
        }
        x->key[i] = k;
        x->n++;

        x->c[i] = left;
        x->c.push_back(new Node<T>());
        for (int j = x->c.size() - 1; j > i + 1; j--)
        {
            x->c[j] = x->c[j - 1];
        }
        x->c[i + 1] = right;

        if (x->n == M)
        {
            Split_Non_Leaf(x, Parent, locked);
        }
    }
    void Split_Non_Leaf(Node<T> *x, map<Node<T> *, Node<T> *> &Parent, deque<Node<T> *> &locked)
    {
        assert(x->key.size() == x->n);
        assert(x->c.size() == x->n + 1);
        Node<T> *left = new Node<T>(0, false);
        Node<T> *right = new Node<T>(0, false);
        int up = (M + 2 - 1) / 2 - 1;
        for (int i = 0; i < up; i++)
        {
            left->key.push_back(x->key[i]);
            left->c.push_back(x->c[i]);
            left->n++;
        }
        left->c.push_back(x->c[up]);
        T key = x->key[up];
        for (int i = up + 1; i < x->key.size(); i++)
        {
            right->key.push_back(x->key[i]);
            right->c.push_back(x->c[i]);
            right->n++;
        }
        right->c.push_back(x->c[x->n]);
        if (Parent.find(x) != Parent.end())
        {
            Insert_Non_Leaf(key, Parent[x], left, right, Parent, locked);
        }
        else
        {
            auto node = new Node<T>(1, false);
            node->c.push_back(left);
            node->c.push_back(right);
            node->key.push_back(key);
            locked.pop_front();
            root = node;
        }
    }
    void make_number()
    {
        NUMBER = map<Node<T> *, int>();
        queue<Node<T> *> q;
        q.push(root);
        int c = 0;
        while (!q.empty())
        {
            auto now = q.front();
            q.pop();
            if (NUMBER.find(now) != NUMBER.end())
            {
                continue;
            }
            NUMBER[now] = c;
            c++;
            for (auto i : now->c)
            {
                q.push(i);
            }
        }
    }
    void print()
    {
        queue<Node<T> *> q;
        map<Node<T> *, int> done;
        q.push(root);
        while (!q.empty())
        {
            auto now = q.front();
            q.pop();
            if (done.find(now) != done.end())
                continue;
            cout << "Node : " << NUMBER[now] << endl;
            cout << "   "
                 << "Key : ";
            for (auto i : now->key)
            {
                cout << i << " ";
            }
            cout << endl;
            cout << "   "
                 << "Child : ";
            for (auto i : now->c)
            {
                cout << NUMBER[i] << " ";
            }
            cout << endl;
            for (auto i : now->c)
            {
                q.push(i);
            }
            done[now] = 1;
        }
    }
    bool debug()
    {
        queue<Node<T> *> q;
        map<Node<T> *, int> done;
        q.push(root);
        bool res = false;
        while (!q.empty())
        {
            auto now = q.front();
            q.pop();
            if (now == root)
            {
                continue;
            }

            if (done.find(now) != done.end())
                continue;

            if (now->n != now->key.size())
            {
                cout << NUMBER[now] << " : "
                     << "ERROR_N" << now->n << endl;
                res = true;
            }
            if (now->is_leaf == true && now->c.size() != 0)
            {
                cout << NUMBER[now] << " : "
                     << "ERROR_IS_LEAF_TRUE" << endl;
                res = true;
            }
            if (now->is_leaf == false && now->c.size() == 0)
            {
                cout << NUMBER[now] << " : "
                     << "ERROR_IS_LEAF_FALSE" << endl;
                res = true;
            }
            if (now->n + 1 != now->c.size() && now->is_leaf == false)
            {
                cout << NUMBER[now] << " : "
                     << "ERROR_CHILD_SIZE" << endl;
                res = true;
            }
            if ((now->is_leaf && now->n < M / 2) || (now->is_leaf == false && now->n < (M + 1) / 2 - 1))
            {
                cout << NUMBER[now] << " : "
                     << "ERROR_SIZE_TOO_SMALL" << endl;
                res = true;
            }
            if (now->n > M - 1)
            {
                cout << NUMBER[now] << " : "
                     << "ERROR_SIZE_TOO_LARGE" << endl;
                res = true;
            }
            for (auto i : now->c)
            {
                q.push(i);
            }
            done[now] = 1;
        }
        return res;
    }

    void Delete(T k, Node<T> *x, map<Node<T> *, Node<T> *> &Parent, deque<Node<T> *> &locked, int pointer = -1)
    {

        // print_lock.lock();
        // cout << std::this_thread::get_id() << "entry delete" << endl;
        // print_lock.unlock();
        bool f = false;
        for (int i = 0; i < x->key.size(); i++)
        {
            if (x->key[i] == k)
            {
                f = true;
                x->key.erase(x->key.begin() + i);
                break;
            }
        }
        if (!f)
        {
            print_lock.lock();
            cout << "NO DELETE KEY" << endl;
            cout << x->n << " " << x->key.size() << " " << x->c.size() << endl;
            // cout << k << endl;
            // cout << (x == root) << endl;
            // cout << (x->owner == std::this_thread::get_id()) << endl;
            cout << x->is_leaf << endl;

            // // exit(0);
            print_lock.unlock();
            return;
        }
        if (pointer != -1)
        {
            x->c.erase(x->c.begin() + pointer);
        }
        x->n--;
        if (x == root && x->n == 0)
        {
            if (x->c.size() == 0)
            {
                locked.pop_front();
                root = new Node<T>(0, true);
                return;
            }
            locked.pop_front();
            root = x->c[0];
            return;
        }
        if (x == root)
            return;
        // cout<<"TOO FEW? : "<<(x->is_leaf && x->n < (M / 2))<<" "<<(x->is_leaf == false && x->n < (M + 1) / 2 - 1)<<endl;
        if ((x->is_leaf && x->n < (M / 2)) || (x->is_leaf == false && x->n < (M + 1) / 2 - 1))
        {
            // too few
            Node<T> *p = Parent[x];
            int i = 0;
            for (;; i++)
            {
                if (p->c[i] == x)
                    break;
            }
            bool l = false, r = false;
            if (i != 0)
            {
                // from left
                while (!p->c[i - 1]->nlock.try_lock())
                {
                }
                Node<T> *node = p->c[i - 1];
                T K = p->key[i - 1];
                // cout<<"CAN LEFT MERGE? : "<<(x->is_leaf && x->n + node->n <= M - 1)<<" "<<(x->is_leaf == false && x->n + node->n + 1 <= M - 1)<<endl;
                if ((x->is_leaf && x->n + node->n <= M - 1) || (x->is_leaf == false && x->n + node->n + 1 <= M - 1))
                {
                    if (x->is_leaf == false)
                    {
                        node->key.push_back(K);
                        node->n++;
                    }
                    for (auto j : x->key)
                    {
                        node->key.push_back(j);
                        node->n++;
                    }
                    if (x->is_leaf == false)
                        for (auto j : x->c)
                        {
                            node->c.push_back(j);
                        }
                    Delete(K, p, Parent, locked, i);
                }
                else
                {
                    T HOGE;
                    x->key.push_back(HOGE);
                    for (int i = x->key.size() - 1; i > 0; i--)
                        x->key[i] = x->key[i - 1];
                    if (x->is_leaf == false)
                    {
                        x->key[0] = K;
                        p->key[i - 1] = node->key[node->key.size() - 1];
                    }
                    else
                    {
                        x->key[0] = node->key[node->key.size() - 1];
                        p->key[i - 1] = node->key[node->key.size() - 1];
                    }
                    x->n++;
                    node->key.erase(node->key.begin() + node->key.size() - 1);
                    node->n--;
                    if (x->is_leaf == false)
                    {
                        Node<T> *NP;
                        x->c.push_back(NP);
                        for (int i = x->c.size() - 1; i > 0; i--)
                            x->c[i] = x->c[i - 1];

                        x->c[0] = node->c[node->c.size() - 1];
                        node->c.erase(node->c.begin() + node->c.size() - 1);
                    }
                }
                node->nlock.unlock();
            }
            else
            {
                // from right
                while (!p->c[i + 1]->nlock.try_lock())
                {
                }
                Node<T> *node = p->c[i + 1];
                T K = p->key[i];
                // cout<<"CAN RIGHT MERGE? : "<<(x->is_leaf && x->n + node->n <= M - 1) << " "<<(x->is_leaf == false && x->n + node->n <= M - 1)<<endl;
                if ((x->is_leaf && x->n + node->n <= M - 1) || (x->is_leaf == false && x->n + node->n + 1 <= M - 1))
                {
                    if (x->is_leaf == false)
                    {
                        x->key.push_back(K);
                        x->n++;
                    }
                    for (auto j : node->key)
                    {
                        x->key.push_back(j);
                        x->n++;
                    }
                    if (x->is_leaf == false)
                        for (auto j : node->c)
                        {
                            x->c.push_back(j);
                        }

                    Delete(K, p, Parent, locked, i + 1);
                }
                else
                {
                    if (x->is_leaf == false)
                    {
                        x->key.push_back(K);
                        p->key[i] = node->key[0];
                        node->key.erase(node->key.begin());
                        x->n++;
                        node->n--;
                        x->c.push_back(node->c[0]);
                        node->c.erase(node->c.begin());
                    }
                    else
                    {
                        x->key.push_back(node->key[0]);
                        p->key[i] = node->key[1];
                        node->key.erase(node->key.begin());
                        x->n++;
                        node->n--;
                    }
                }
                node->nlock.unlock();
            }
        }
        else
        {
            return;
        }
    }
    Node<T> *Search_Node_d(T k, Node<T> *x, Node<T> *p, map<Node<T> *, Node<T> *> &Parent, deque<Node<T> *> &locked)
    {
        // print_lock.lock();
        // cout << std::this_thread::get_id() << " start search node d" << endl;
        // print_lock.unlock();
        if (x->owner != std::this_thread::get_id())
        {
            print_lock.lock();
            cout << "OWNER" << endl;
            cout << x->owner << endl;
            cout << endl;
            cout << "this" << endl;
            cout << std::this_thread::get_id() << endl;
            cout << (root == x) << endl;
            print_lock.unlock();
            exit(0);
        }
        if (x != p)
        {
            Parent[x] = p;
        }
        if (x->is_leaf)
        {
            // print_lock.lock();
            // cout << std::this_thread::get_id() << " end search node d" << endl;
            // print_lock.unlock();
            return x;
        }
        int i = 0;
        for (; i < x->n && x->key[i] <= k; i++)
        {
        }
        if (x->owner != std::this_thread::get_id())
        {
            print_lock.lock();
            cout << "W" << endl;
            cout << x->owner << endl;
            cout << endl;
            cout << "this" << endl;
            cout << std::this_thread::get_id() << endl;
            cout << (root == x) << endl;
            print_lock.unlock();
            exit(0);
        }
        try
        {
            while (!x->c.at(i)->nlock.try_lock())
            {
            }
        }
        catch (std::exception e)
        {
            cout << "HELLO" << endl;
            cout << (x == root) << endl;
            cout << (x->owner == std::this_thread::get_id()) << endl;
            cout << x->n << " " << x->c.size() << " " << x->key.size() << endl;
            cout << e.what() << endl;
            exit(0);
        }

        // print_lock.lock();
        // cout << std::this_thread::get_id() << " get child lock" << endl;
        // print_lock.unlock();
        if (x->owner != std::this_thread::get_id())
        {
            print_lock.lock();
            cout << "CHILD" << endl;
            cout << "OWNER" << endl;
            cout << x->owner << endl;
            cout << endl;
            cout << "this" << endl;
            cout << std::this_thread::get_id() << endl;
            cout << (root == x) << endl;
            print_lock.unlock();
            exit(0);
        }
        auto next = x->c.at(i);
        x->c.at(i)->owner = std::this_thread::get_id();
        if ((next->is_leaf && next->n > (M / 2)) || (next->is_leaf == false && next->n > (M + 1) / 2 - 1))
        {
            while (!locked.empty())
            {
                locked.front()->nlock.unlock();
                locked.pop_front();
            }
        }
        locked.push_back(next);
        return Search_Node_d(k, next, x, Parent, locked);
    }
    void Delete(T k)
    {
        map<Node<T> *, Node<T> *> Parent;
        deque<Node<T> *> locked;
        while (!root->nlock.try_lock())
        {
        }
        root->owner = std::this_thread::get_id();
        locked.push_back(root);
        auto L = Search_Node_d(k, root, root, Parent, locked);
        Delete(k, L, Parent, locked);
        while (!locked.empty())
        {
            locked.front()->nlock.unlock();
            locked.pop_front();
        }
    }
};
using ll = long long;
Btree<ll> *tree;
ll N = 100000;
ll A = 33797, B = 1;
void f(pair<ll, ll> arg)
{
    ll nn = arg.first;
    ll seed = arg.second;
    ll M = 1;
    for (int k = 0; k < 32; k++)
        M *= 2;

    ll X = seed % M;
    while (nn--)
    {
        // assert(m.find(X) != m.end());
        tree->Delete(X);
        X = (A * X + B) % M;
    }
}
int main()
{
    vector<ll> sums(9);
    for (int count = 0; count < 100; count++)
    {
        cout << "NOW :" << count << endl;
        for (int THREAD_NUM = 1; THREAD_NUM < 9; THREAD_NUM++)
        {
            // m = map<ll, ll>();
            tree = new Btree<ll>(5);
            vector<thread> threads;
            vector<ll> seeds;
            auto h = std::hash<std::thread::id>{}(std::this_thread::get_id());
            ll M = 1;
            for (int k = 0; k < 32; k++)
                M *= 2;
            ll X = h % M;
            ll seed = X;
            ll length = (N / THREAD_NUM - 1);
            map<ll, ll> m;
            for (ll i = 0; i < N; i++)
            {
                if (i % (length) == 0 && seeds.size() != THREAD_NUM)
                {
                    seeds.push_back(X);
                }
                // cout<<X<<endl;
                m[X]++;
                tree->Insert(X);
                X = (A * X + B) % M;
            }
            bool IS_CONFLICT = false;
            for (auto i : m)
                if (i.second >= 2)
                {
                    cout << "CONFLICT" << endl;
                    IS_CONFLICT = true;
                }
            if (IS_CONFLICT)
                continue;
            tree->make_number();
            if (tree->debug())
            {
                exit(0);
            }
            assert(seeds.size() == THREAD_NUM);
            ll nn = N / THREAD_NUM - 3;
            auto b = std::chrono::system_clock::now();
            for (int i = 0; i < THREAD_NUM; i++)
            {
                threads.emplace_back(f, make_pair(nn, seeds[i]));
            }
            for (int i = 0; i < THREAD_NUM; i++)
            {
                threads[i].join();
            }
            auto e = std::chrono::system_clock::now();
            cout << (std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count()) << endl;
            sums[THREAD_NUM] += std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count();
            tree->make_number();
            if (tree->debug())
            {
                exit(0);
            }

            assert(nn <= length);

            {
                // check
                for (int i = 0; i < THREAD_NUM; i++)
                {
                    ll n = seeds[i];
                    for (int i = 0; i < nn; i++)
                    {
                        // assert(m.find(n) != m.end());
                        if (tree->Search(n) == 1)
                        {
                            tree->make_number();
                            tree->print();
                            cout << n << endl;
                            exit(0);
                        }
                        n = (A * n + B) % M;
                    }
                    // for (int i = nn; i < length; i++)
                    // {
                    //     if (tree->Search(n) == 0)
                    //     {
                    //         tree->make_number();
                    //         tree->print();
                    //         cout << n << endl;
                    //         exit(0);
                    //     }
                    //     n = (A * n + B) % M;
                    // }
                }
            }
            delete tree;
        }
        system("clear");
    }
    for (auto i : sums)
    {
        cout << i / 100 << endl;
    }
}