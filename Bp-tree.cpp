#include <bits/stdc++.h>
using std::ceil;
using std::cout;
using std::endl;
using std::make_pair;
using std::map;
using std::pair;
using std::queue;
using std::string;
using std::vector;
#define INF ((1ll) << 60)
#define rep(i, n) for (long long i = 0; i < n; i++)
bool flag = false;
template <class T>
class Node
{
public:
    int n;
    bool is_leaf;
    vector<T> key;
    vector<Node<T> *> c;
    Node<T> *next, *prev; // only leaf
    bool is_next = false, is_prev = false;
    Node()
    {
        n = 0;
        is_leaf = true;
    }
    Node(int n_, bool is_leaf_)
    {
        n = n_;
        is_leaf = is_leaf_;
    }
    ~Node()
    {
        for (int i = 0; i < c.size(); i++)
            delete c[i];
    }
};

template <class T>
class Btree
{
public:
    int M;
    Node<T> *root;
    map<Node<T> *, int> NUMBER;
    map<Node<T> *, Node<T> *> Parent;
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
        delete root;
    }
    vector<T> Search_Range(T left, T right)
    {
        auto l = Search_Node(left, root, root);
        auto r = Search_Node(right, root, root);
        vector<T> res;
        while (1)
        {
            if (l == r)
            {
                for (auto key : l->key)
                {
                    if (left <= key && key <= right)
                    {
                        res.push_back(key);
                    }
                }
                break;
            }
            for (auto key : l->key)
            {
                if (left <= key)
                {
                    res.push_back(key);
                }
            }
            l = l->next;
        }
        return res;
    }
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
        Search(k, x->c[i]);
    }
    Node<T> *Search_Node(T k, Node<T> *x, Node<T> *p)
    {
        if (x != p)
        {
            Parent[x] = p;
        }
        if (x->is_leaf)
        {
            bool f = false;
            for (auto j : x->key)
                if (j == k)
                    flag = true;
            // assert(flag);
            return x;
        }
        int i = 0;
        for (; i < x->n && x->key[i] <= k; i++)
        {
        }
        Search_Node(k, x->c[i], x);
    }
    void Insert(T k)
    {
        Parent = map<Node<T> *, Node<T> *>();
        auto leaf = Search_Node(k, root, root);
        Insert_Leaf(k, leaf);
    }
    void Insert_Leaf(T k, Node<T> *x)
    {
        int i = 0;
        for (; i < x->n && x->key[i] < k; i++)
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
        if (x->n == M)
        {
            SplitLeaf(x);
        }
    }

    T SplitLeaf(Node<T> *leaf)
    {
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

        left->next = right;
        right->next = leaf->next;
        if (leaf->is_prev)
        {
            leaf->prev->next = left;
            left->prev = leaf->prev;
            left->is_prev = true;
        }
        left->next = right;
        right->prev = left;
        left->is_next = true;
        right->is_prev = true;
        if (leaf->is_next)
        {
            leaf->next->prev = right;
            right->next = leaf->next;
            right->is_next = true;
        }
        if (Parent.find(leaf) != Parent.end())
        {
            Insert_Non_Leaf(key, Parent[leaf], left, right);
        }
        else
        {
            root = new Node<T>(1, false);
            root->key.push_back(key);
            root->c.push_back(left);
            root->c.push_back(right);
        }
    }
    void Insert_Non_Leaf(T k, Node<T> *x, Node<T> *left, Node<T> *right)
    {
        int i = 0;
        for (; i < x->n && x->key[i] < k; i++)
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
            Split_Non_Leaf(x);
        }
    }
    void Split_Non_Leaf(Node<T> *x)
    {
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
            Insert_Non_Leaf(key, Parent[x], left, right);
        }
        else
        {
            root = new Node<T>(1, false);
            root->c.push_back(left);
            root->c.push_back(right);
            root->key.push_back(key);
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

    void Delete(T k, Node<T> *x, int pointer = -1)

    {
        // cout<<"k : "<<k<<endl;
        // cout<<"x->key : ";for(auto j:x->key)cout<<j<<" ";cout<<endl;
        for (int i = 0; i < x->key.size(); i++)
        {
            if (x->key[i] == k)
            {
                x->key.erase(x->key.begin() + i);
                break;
            }
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
                root = new Node<T>(0, true);
                return;
            }
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
                    if (p->c[i]->is_leaf)
                    {
                        if (p->c[i]->is_next)
                        {
                            node->is_next = true;
                            node->next = p->c[i]->next;
                        }
                        else
                        {
                            node->is_next = false;
                        }
                    }
                    Delete(K, p, i);
                }
                else
                {
                    Node<T> *new_node = new Node<T>(0, x->is_leaf);
                    if (x->is_leaf == false)
                    {
                        new_node->key.push_back(K);
                        p->key[i - 1] = node->key[node->key.size() - 1];
                    }
                    else
                    {
                        new_node->key.push_back(node->key[node->key.size() - 1]);
                        p->key[i - 1] = node->key[node->key.size() - 1];
                    }
                    new_node->n++;
                    node->key.erase(node->key.begin() + node->key.size() - 1);
                    node->n--;
                    for (auto j : x->key)
                    {
                        new_node->key.push_back(j);
                        new_node->n++;
                    }
                    if (x->is_leaf == false)
                    {
                        new_node->c.push_back(node->c[node->c.size() - 1]);
                        node->c.erase(node->c.begin() + node->c.size() - 1);
                        for (auto j : x->c)
                        {
                            new_node->c.push_back(j);
                        }
                    }
                    p->c[i] = new_node;
                }
            }
            else
            {
                // from right
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
                    if (p->c[i]->is_leaf)
                    {
                        if (p->c[i + 1]->is_next)
                        {
                            x->is_next = true;
                            x->next = p->c[i + 1]->next;
                        }
                        else
                        {
                            x->is_next = false;
                        }
                    }
                    Delete(K, p, i + 1);
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
            }
        }
        else
        {
            return;
        }
    }
    void Delete(T k)
    {
        auto L = Search_Node(k, root, root);
        Delete(k, L);
    }
};

int main()
{
    {
        cout << "HELLO" << endl;
        int mi = 1000000;
        Btree<int> *tree = new Btree<int>(3);
        std::random_device seed_gen;
        std::mt19937 engine(seed_gen());
        std::uniform_int_distribution<> dis(0, 3000);
        vector<int> v(1000000 + 10);
        vector<int> nums;
        for (int i = 0; i < 10000; i++)
        {
            int n = dis(engine);
            if (v[n])
                continue;
            v[n] = 1;
            tree->Insert(n);
            nums.push_back(n);
        }
        sort(nums.begin(), nums.end());

        for (int i = 1; i < nums.size(); i += 2)
        {
            tree->Delete(nums[i]);
            nums.erase(nums.begin() + i);
        }
        for (int l = 0; l < nums.size(); l+=10)
        {
            for (int r = l + 1; r < nums.size(); r+=10)
            {
                auto res = tree->Search_Range(nums[l], nums[r]);
                vector<int> check;
                for (int i = l; i <= r; i++)
                {
                    check.push_back(nums[i]);
                }
                assert(res == check);
            }
            if (l % 10 == 0)
                cout << l << endl;
        }
        cout << "END" << endl;
        delete tree;
    }
    {
        int mi = 1000000;
        Btree<int> *tree = new Btree<int>(50);
        std::random_device seed_gen;
        std::mt19937 engine(seed_gen());
        std::uniform_int_distribution<> dis(0, 300000);
        vector<int> v(1000000 + 10);
        map<int, int> m;
        int i = 0;
        Btree<int> *res;
        for (;; i++)
        {
            int n = dis(engine);
            // cout << "NUM : " << n << endl;
            if (i % 1000000 == 0)
                cout << i << endl;
            if (v[n])
            {
                if (tree->Search(n) == false)
                {
                    cout << "a" << endl;
                    tree->make_number();
                    tree->print();
                    break;
                }
                tree->Delete(n);
                if (tree->Search(n) == true)
                {
                    cout << "b" << endl;
                    tree->make_number();
                    // tree->print();
                    break;
                }
                v[n] = 0;
            }
            else
            {
                if (tree->Search(n) == true)
                {
                    cout << "c" << endl;
                    tree->make_number();
                    // tree->print();
                    break;
                }
                tree->Insert(n);
                if (tree->Search(n) == false)
                {
                    cout << "d" << endl;
                    tree->make_number();
                    // tree->print();
                    break;
                }
                v[n] = 1;
            }
        }
    }
}