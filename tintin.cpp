#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define ld long double
#define mp make_pair
#define nl "\n"

#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);

class WordGenerator{
    set<string>generatedWords;
public:
    WordGenerator(){
        srand(time(0));
    }

    string generateWord(int len){
        string a="";
        for(int i=0;i<len;i++){
            char c='a'+(rand()%26);
            a+=c;
        }
        return a;
    }

    string generateUniqueWord(int len){
        string a;
        do{
            a=generateWord(len);
        }while(generatedWords.find(a)!=generatedWords.end());
        generatedWords.insert(a);
        return a;
    }

    void reset(){
        generatedWords.clear();
    }
};

template<typename K,typename V>
class ChainNode{
public:
    K key;
    V value;
    ChainNode* next;

    ChainNode(K k, V v){
        key=k;
        value=v;
        next=nullptr;
    }
};

template<typename K,typename V>
class HashEntry{
public:
    K key;
    V value;
    bool isOccupied;
    bool isDeleted;

    HashEntry(){
        isOccupied=false;
        isDeleted=false;
    }

    HashEntry(K k,V v){
        key=k;
        value=v;
        isOccupied=true;
        isDeleted=false;
    }
};

template<typename K,typename V>
class HashTable{
public:
    int tablesize;
    int total;
    int initsize;
    double load_factor;
    int insertcount;
    int deletecount;
    int lastsize;
    int collisioncount;

    bool isPrime(int n){
        if(n<=1){
            return false;
        }
        for(int i=2;i*i<=n;i++){
            if(n%i==0){
                return false;
            }
        }
        return true;
    }

    int nextPrime(int n){
        if(n<=1){
            return 2;
        }
        int prime=n;
        while(!isPrime(prime)){
            prime++;
        }
        return prime;
    }

    int previousPrime(int n){
        if(n<=2){
            return 2;
        }
        int prime=n;
        while(!isPrime(prime)){
            prime--;
            if(prime<2){
                return 2;
            }
        }
        return prime;
    }

    HashTable(int init=13,double loadfactor=0.5){
        initsize=nextPrime(init);
        tablesize=initsize;
        load_factor=loadfactor;
        total=0;
        insertcount=0;
        deletecount=0;
        lastsize=0;
        collisioncount=0;
    }

    virtual ~HashTable(){}

    int hash1(const string& key){
        unsigned long hashvalue=0;
        int prime=31;
        for(char c:key){
            hashvalue=(hashvalue*prime+c)%tablesize;
        }
        return hashvalue;
    }

    int hash2(const string& key){
        unsigned long hashvalue=0;
        for(char c:key){
            hashvalue=c+(hashvalue<<6)+(hashvalue<<16)-hashvalue;
        }
        return (int)((hashvalue%tablesize+tablesize)%tablesize);
    }

    int auxHash(const string& key){
        unsigned long hashvalue=0;
        for(char c:key){
            hashvalue=hashvalue*29+c;
        }
        return (hashvalue%(tablesize-1))+1;
    }

    double loadfactor(){
        return (double)total/tablesize;
    }

    int hashInt(int key) {
        unsigned int x = (unsigned int)key;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = (x >> 16) ^ x;
        return x % tablesize;
    }

    int hashChar(char key) {
        // We cast to unsigned char to avoid negative values from extended ASCII
        unsigned int hashvalue = (unsigned char)key;
        // Apply a simple transformation to spread it out
        hashvalue = (hashvalue * 31) % tablesize;
        return (int)hashvalue;
    }

    int auxHashInt(int key) {
        int R = previousPrime(tablesize - 1);
        return R - (key % R);
    }

    virtual void insert(const K& key,const V& value)=0;
    virtual bool search(const K& key,V& value,int& hits)=0;
    virtual bool remove(const K& key)=0;
};

template<typename K,typename V>
class Chaining: public HashTable<K,V>{
    ChainNode<K,V>** table;
    int choice;
    int getHash(const string& key){
        if(choice==1){
            return this->hash1(key);
        }
        else{
            return this->hash2(key);
        }
    }

    void rehash(int newSize){
        ChainNode<K,V>** oldtable=table;
        int oldsize=this->tablesize;
        this->tablesize=newSize;
        table=new ChainNode<K,V>*[this->tablesize];
        for(int i=0;i<this->tablesize;i++){
            table[i]=nullptr;
        }
        this->total=0;
        for(int i=0;i<oldsize;i++){
            ChainNode<K,V>*curr=oldtable[i];
            while(curr!=nullptr){
                ChainNode<K,V>*next=curr->next;
                int idx=getHash(curr->key);
                curr->next=table[idx];
                table[idx]=curr;
                this->total++;
                curr=next;
            }
        }
        delete[] oldtable;
        this->lastsize=this->total;
    }
public:
    Chaining(int choice=1,int initsize=13,double loadfactor=0.5):HashTable<K,V>(initsize,loadfactor){
        this->choice=choice;
        table=new ChainNode<K,V>*[this->tablesize];
        for(int i=0;i<this->tablesize;i++){
            table[i]=nullptr;
        }
    }

    ~Chaining(){
        for(int i=0;i<this->tablesize;i++){
            ChainNode<K,V>* curr=table[i];
            while(curr!=nullptr){
                ChainNode<K,V>* temp=curr;
                curr=curr->next;
                delete temp;
            }
        }
        delete[] table;
    }

    bool search(const K& key,V& value,int& hits) override{
        hits=0;
        int idx=getHash(key);
        ChainNode<K,V>* curr=table[idx];
        while(curr!=nullptr){
            hits++;
            if(curr->key==key){
                value=curr->value;
                return true;
            }
            curr=curr->next;
        }
        return false;
    }

    void insert(const K& key,const V& value) override{
        V temp;
        int hits;
        if(search(key,temp,hits)){
            return;
        }
        int idx=getHash(key);
        if(table[idx]!=nullptr){
            this->collisioncount++;
        }
        ChainNode<K,V>* newNode=new ChainNode<K,V>(key,value);
        newNode->next=table[idx];
        table[idx]=newNode;
        this->total++;
        this->insertcount++;
        if(this->loadfactor()>0.5 && this->insertcount>=this->lastsize/2){
            int newsize=this->nextPrime(2*this->tablesize);
            rehash(newsize);
            this->insertcount=0;
        }
    }

    bool remove(const K& key)override{
        int idx=getHash(key);
        ChainNode<K,V>* curr=table[idx];
        ChainNode<K,V>* prev=nullptr;
        while(curr!=nullptr){
            if(curr->key==key){
                if(prev==nullptr){
                    table[idx]=curr->next;
                }
                else{
                    prev->next=curr->next;
                }
                delete curr;
                this->total--;
                this->deletecount++;
                if(this->tablesize>this->initsize && this->loadfactor()<0.25 && this->deletecount>=this->lastsize/2){
                    int newSize=this->previousPrime(this->tablesize/2);
                    if(newSize>=this->initsize){
                        rehash(newSize);
                        this->deletecount=0;
                    }
                }
                return true;
            }
            prev=curr;
            curr=curr->next;
        }
        return false;
    }    
};

template<typename K,typename V>
class DoubleHashingTable:public HashTable<K,V>{
    HashEntry<K,V>*table;
    int choice;
    int getHash(const string& key){
        if(choice==1){
            return this->hash1(key);
        }
        else{
            return this->hash2(key);
        }
    }

    int doubleHash(const string& key,int i){
        return (getHash(key)+i*this->auxHash(key))%this->tablesize;
    }

    void rehash(int newSize){
        HashEntry<K,V>*oldtable=table;
        int oldsize=this->tablesize;
        this->tablesize=newSize;
        table=new HashEntry<K,V>[this->tablesize];
        this->total=0;
        for(int i=0;i<oldsize;i++){
            if(oldtable[i].isOccupied && !oldtable[i].isDeleted){
                int j=0;
                int idx;
                while(j<this->tablesize){
                    idx=doubleHash(oldtable[i].key,j);
                    if(!table[idx].isOccupied){
                        table[idx]=HashEntry<K,V>(oldtable[i].key,oldtable[i].value);
                        this->total++;
                        break;
                    }
                    j++;
                }
            }
        }
        delete[] oldtable;
        this->lastsize=this->total;
    }
public:
    DoubleHashingTable(int choice=1,int initsize=13,double loadfactor=0.5):HashTable<K,V>(initsize,loadfactor){
        this->choice=choice;
        table=new HashEntry<K,V>[this->tablesize];
    }
    ~DoubleHashingTable(){
        delete[] table;
    }

    bool search(const K& key,V& value,int& hits)override{
        hits=0;
        int i=0;
        int idx;
        while(i<this->tablesize){
            idx=doubleHash(key,i);
            hits++;
            if(!table[idx].isOccupied){
                return false;
            }
            if(table[idx].isOccupied && !table[idx].isDeleted && table[idx].key==key){
                value=table[idx].value;
                return true;
            }
            i++;
        }
        return false;
    }

    void insert(const K& key,const V& value)override{
        V temp;
        int hits;
        if(search(key,temp,hits)){
            return;
        }
        int i=0;
        int idx;
        while(i<this->tablesize){
            idx=doubleHash(key,i);
            if(!table[idx].isOccupied || table[idx].isDeleted){
                if(i>0){
                    this->collisioncount++;
                }
                table[idx]=HashEntry<K,V>(key,value);
                this->total++;
                this->insertcount++;
                if(this->loadfactor()>0.5 && this->insertcount>=this->lastsize/2){
                    int newSize=this->nextPrime(2*this->tablesize);
                    rehash(newSize);
                    this->insertcount=0;
                }
                return;
            }
            i++;
        }
    }

    bool remove(const K& key)override{
        int i=0;
        int idx;
        while(i<this->tablesize){
            idx=doubleHash(key,i);
            if(!table[idx].isOccupied){
                return false;
            }
            if(table[idx].isOccupied && !table[idx].isDeleted &&table[idx].key==key){
                table[idx].isDeleted=true;
                this->total--;
                this->deletecount++;
                if(this->tablesize>this->initsize && this->loadfactor()<0.25 && this->deletecount>=this->lastsize/2){
                    int newSize=this->previousPrime(this->tablesize/2);
                    if(newSize>=this->initsize){
                        rehash(newSize);
                        this->deletecount=0;
                    }
                }
                return true;
            }
            i++;
        }
        return false;
    }

    bool update(const K& key, const V& newValue) {
        int i = 0;
        while (i < this->tablesize) {
            int idx = doubleHash(key, i);
            
            // If we hit an empty slot, the key isn't in the table
            if (!table[idx].isOccupied) {
                return false; 
            }

            // If we find the key (and it's not a 'deleted' tombstone)
            if (!table[idx].isDeleted && table[idx].key == key) {
                table[idx].value = newValue; // Update the value
                return true;
            }
            i++;
        }
        return false;
    }
};

template<typename K,typename V>
class CustomProbingTable:public HashTable<K,V>{
    HashEntry<K,V>*table;
    int choice;
    int C1,C2;
    int gethash(const string& key){
        if(choice==1){
            return this->hash1(key);
        }
        else{
            return this->hash2(key);
        }
    }

    int customHash(const string& key,int i){
        ll h1=gethash(key);
        ll aux=this->auxHash(key);
        ll probe=(h1+C1*i*aux+C2*i*i)%this->tablesize;
        if(probe<0){
            probe+=this->tablesize;
        }
        return probe;
    }

    void rehash(int newSize){
        HashEntry<K,V>*oldtable=table;
        int oldsize=this->tablesize;
        this->tablesize=newSize;
        table=new HashEntry<K,V>[this->tablesize];
        this->total=0;
        for(int i=0;i<oldsize;i++){
            if(oldtable[i].isOccupied && !oldtable[i].isDeleted){
                int j=0;
                int idx;
                while(j<this->tablesize){
                    idx=customHash(oldtable[i].key,j);
                    if(!table[idx].isOccupied){
                        table[idx]=HashEntry<K,V>(oldtable[i].key,oldtable[i].value);
                        this->total++;
                        break;
                    }
                    j++;
                }
            }
        }
        delete[] oldtable;
        this->lastsize=this->total;
    }
public:
    CustomProbingTable(int choice=1,int C1=1,int C2=3,int initsize=13,double loadfactor=0.5):HashTable<K,V>(initsize,loadfactor){
        this->choice=choice;
        this->C1=C1;
        this->C2=C2;
        table=new HashEntry<K,V>[this->tablesize];
    }

    ~CustomProbingTable(){
        delete[] table;
    }

    bool search(const K& key,V& value,int& hits)override{
        hits=0;
        int i=0;
        int idx;
        while(i<this->tablesize){
            idx=customHash(key,i);
            hits++;
            if(!table[idx].isOccupied){
                return false;
            }
            if(table[idx].isOccupied && !table[idx].isDeleted && table[idx].key==key){
                value=table[idx].value;
                return true;
            }
            i++;
        }
        return false;
    }

    void insert(const K& key,const V& value)override{
        V temp;
        int hits;
        if(search(key,temp,hits)){
            return;
        }
        int i=0;
        int idx;
        while(i<this->tablesize){
            idx=customHash(key,i);
            if(!table[idx].isOccupied ||table[idx].isDeleted){
                if(i>0){
                    this->collisioncount++;
                }
                table[idx]=HashEntry<K,V>(key,value);
                this->total++;
                this->insertcount++;
                if(this->loadfactor()>0.5 && this->insertcount>=this->lastsize/2){
                    int newSize=this->nextPrime(2*this->tablesize);
                    rehash(newSize);
                    this->insertcount=0;
                }
                return;
            }
            i++;
        }
    }

    bool remove(const K& key)override{
        int i=0;
        int idx;
        while(i<this->tablesize){
            idx=customHash(key,i);
            if(!table[idx].isOccupied){
                return false;
            }
            if(table[idx].isOccupied && !table[idx].isDeleted && table[idx].key==key){
                table[idx].isDeleted=true;
                this->total--;
                this->deletecount++;
                if(this->tablesize>this->initsize && this->loadfactor()<0.25 && this->deletecount>=this->lastsize/2){
                    int newSize=this->previousPrime(this->tablesize/2);
                    if(newSize>=this->initsize){
                        rehash(newSize);
                        this->deletecount=0;
                    }
                }
                return true;
            }
            i++;
        }
        return false;
    }
};

// InnerTable uses Quadratic Probing as per requirements
template<typename K, typename V>
class InnerTable : public HashTable<K, V> {
    HashEntry<K, V>* table;
public:
    InnerTable(int size) : HashTable<K, V>(size) {
        table = new HashEntry<K, V>[this->tablesize];
    }

    ~InnerTable() { delete[] table; }

    void insert(const K& key, const V& value) override {
        int i = 0;
        while (i < this->tablesize) {
            // Quadratic Probing: (h + i^2) % N
            int idx = (this->hash2(key) + (i * i)) % this->tablesize;
            if (!table[idx].isOccupied || table[idx].isDeleted) {
                table[idx] = HashEntry<K, V>(key, value);
                this->total++;
                return;
            }
            if (table[idx].key == key && !table[idx].isDeleted) {
                table[idx].value = value; // Update if exists
                return;
            }
            i++;
        }
    }

    bool search(const K& key, V& value, int& hits) override {
        hits = 0;
        int i = 0;
        while (i < this->tablesize) {
            hits++;
            int idx = (this->hash2(key) + (i * i)) % this->tablesize;
            if (!table[idx].isOccupied) return false;
            if (table[idx].key == key && !table[idx].isDeleted) {
                value = table[idx].value;
                return true;
            }
            i++;
        }
        return false;
    }

    // Helper to return all pairs for the SEARCH(group_id) operation
    vector<pair<K, V>> getAllPairs() {
        vector<pair<K, V>> pairs;
        for (int i = 0; i < this->tablesize; i++) {
            if (table[i].isOccupied && !table[i].isDeleted) {
                pairs.push_back({table[i].key, table[i].value});
            }
        }
        return pairs;
    }
};

class PermissionDatabase {
    struct OuterEntry {
        string group_id;
        InnerTable<string, string>* innerTable;
        bool isOccupied = false;
    };

    OuterEntry* outerTable;
    int N;

public:
    PermissionDatabase(int size = 101) : N(size) {
        outerTable = new OuterEntry[N];
    }

    ~PermissionDatabase() {
        for (int i = 0; i < N; i++) {
            if (outerTable[i].isOccupied) delete outerTable[i].innerTable;
        }
        delete[] outerTable;
    }

    // INSERT(group_id, user_id, permission)
    void insert(string g_id, string u_id, string perm) {
        int i = 0;
        while (i < N) {
            // Linear Probing: (h + i) % N
            int idx = (hash1_logic(g_id) + i) % N;
            if (!outerTable[idx].isOccupied) {
                outerTable[idx].group_id = g_id;
                outerTable[idx].innerTable = new InnerTable<string, string>(N);
                outerTable[idx].isOccupied = true;
                outerTable[idx].innerTable->insert(u_id, perm);
                return;
            } else if (outerTable[idx].group_id == g_id) {
                outerTable[idx].innerTable->insert(u_id, perm);
                return;
            }
            i++;
        }
    }

    // SEARCH(group_id, user_id)
    void search(string g_id, string u_id) {
        int i = 0;
        while (i < N) {
            int idx = (hash1_logic(g_id) + i) % N;
            if (!outerTable[idx].isOccupied) break;
            if (outerTable[idx].group_id == g_id) {
                string perm;
                int hits;
                if (outerTable[idx].innerTable->search(u_id, perm, hits)) 
                    cout << "Found: " << perm << " (Hits: " << hits << ")" << endl;
                else cout << "User not found in group." << endl;
                return;
            }
            i++;
        }
        cout << "Group not found." << endl;
    }

    // SEARCH(group_id)
    void searchGroup(string g_id) {
        int i = 0;
        while (i < N) {
            int idx = (hash1_logic(g_id) + i) % N;
            if (!outerTable[idx].isOccupied) break;
            if (outerTable[idx].group_id == g_id) {
                auto pairs = outerTable[idx].innerTable->getAllPairs();
                for (auto& p : pairs) cout << "(" << p.first << ", " << p.second << ") ";
                cout << endl;
                return;
            }
            i++;
        }
        cout << "Group not found." << endl;
    }

private:
    int hash1_logic(string key) {
        unsigned long h = 0;
        for (char c : key) h = (h * 31 + c) % N;
        return h;
    }
};

vector<vector<int>> solveThreeSum(vector<int>& nums) {
    int n = nums.size();
    if (n < 3) return {};

    // Sort to easily handle duplicates
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;

    for (int i = 0; i < n - 2; i++) {
        // Skip duplicate anchors to ensure unique triplets
        if (i > 0 && nums[i] == nums[i - 1]) continue;

        // We use your Chaining table to find the pair for the current anchor
        // Initializing with a prime size roughly proportional to N
        Chaining<int, int> table(1, 101); 
        
        int target = -nums[i];
        set<int> usedInThisLoop; // To avoid duplicate pairs for the same anchor

        for (int j = i + 1; j < n; j++) {
            int complement = target - nums[j];
            int dummyValue, hits;

            // Search in your HashTable
            if (table.search(complement, dummyValue, hits)) {
                // Check if we've already used this specific pair for this anchor
                if (usedInThisLoop.find(nums[j]) == usedInThisLoop.end()) {
                    result.push_back({nums[i], complement, nums[j]});
                    usedInThisLoop.insert(nums[j]);
                }
            }
            // Insert current number into the table for future matches
            table.insert(nums[j], j);
        }
    }
    return result;
}

// Logic to find indices of two numbers adding to target
vector<int> solveTwoSum(vector<int>& nums, int target, int N) {
    // K = int (the number), V = int (its index)
    Chaining<int, int> table(1, N); // Using hash1 and initial size N
    
    for (int i = 0; i < nums.size(); i++) {
        int complement = target - nums[i];
        int complementIndex;
        int hits;

        // Using your search override: bool search(const K& key, V& value, int& hits)
        if (table.search(complement, complementIndex, hits)) {
            return {complementIndex, i};
        }

        // Store current number and its index
        table.insert(nums[i], i);
    }
    return {}; // No solution found
}

void runEvaluation(){
    const int NUM_WORDS=10000;
    const int WORD_LENGTH=10;
    const int NUM_SEARCH=1000;
    
    cout<<"Generating "<<NUM_WORDS<<" unique words of length "<<WORD_LENGTH<<"..."<<endl;
    
    WordGenerator wordGen;
    vector<string>words;
    
    for(int i=0;i<NUM_WORDS;i++) {
        words.push_back(wordGen.generateUniqueWord(WORD_LENGTH));
    }
    
    cout<<"Words generated successfully!"<<endl<<endl;
    
    struct Result{
        string method;
        int collisionsHash1;
        double avgHitsHash1;
        int collisionsHash2;
        double avgHitsHash2;
    };
    
    vector<Result>results;
    cout<<"Testing Chaining Method..."<<endl;
    
    Chaining<string,int>chainHash1(1);
    for(int i=0;i<NUM_WORDS;i++){
        chainHash1.insert(words[i],i+1);
    }
    
    vector<string>searchWords;
    for(int i=0;i<NUM_SEARCH;i++){
        int randomIndex=rand()%NUM_WORDS;
        searchWords.push_back(words[randomIndex]);
    }
    
    int totalHits1=0;
    for(const string& word:searchWords){
        int value,hits;
        chainHash1.search(word,value,hits);
        totalHits1+=hits;
    }
    
    Chaining<string,int>chainHash2(2);
    for(int i=0;i<NUM_WORDS;i++) {
        chainHash2.insert(words[i],i+1);
    }
    
    int totalHits2=0;
    for(const string& word:searchWords){
        int value,hits;
        chainHash2.search(word,value,hits);
        totalHits2+=hits;
    }
    
    results.push_back({
        "Chaining Method",
        chainHash1.collisioncount,
        (double)totalHits1/NUM_SEARCH,
        chainHash2.collisioncount,
        (double)totalHits2/NUM_SEARCH
    });
    
    cout<<"  Hash1 - Collisions: "<<chainHash1.collisioncount<<", Avg Hits: "<<fixed<<setprecision(2)<<(double)totalHits1/NUM_SEARCH<<endl;
    cout<<"  Hash2 - Collisions: "<<chainHash2.collisioncount<<", Avg Hits: "<<fixed<<setprecision(2)<<(double)totalHits2/NUM_SEARCH<<endl<<endl;
    cout<<"Testing Double Hashing..."<<endl;
    
    DoubleHashingTable<string,int>doubleHash1(1);
    for(int i=0;i<NUM_WORDS;i++){
        doubleHash1.insert(words[i],i+1);
    }
    
    totalHits1=0;
    for(const string& word:searchWords){
        int value,hits;
        doubleHash1.search(word,value,hits);
        totalHits1+=hits;
    }
    
    DoubleHashingTable<string,int>doubleHash2(2);
    for(int i=0;i<NUM_WORDS;i++){
        doubleHash2.insert(words[i],i+1);
    }
    
    totalHits2=0;
    for(const string& word:searchWords){
        int value,hits;
        doubleHash2.search(word,value,hits);
        totalHits2+=hits;
    }
    
    results.push_back({
        "Double Hashing",
        doubleHash1.collisioncount,
        (double)totalHits1/NUM_SEARCH,
        doubleHash2.collisioncount,
        (double)totalHits2/NUM_SEARCH
    });
    
    cout<<"  Hash1 - Collisions: "<<doubleHash1.collisioncount<<", Avg Hits: "<<fixed<<setprecision(2)<<(double)totalHits1/NUM_SEARCH<<endl;
    cout<<"  Hash2 - Collisions: "<<doubleHash2.collisioncount<<", Avg Hits: "<<fixed<<setprecision(2)<<(double)totalHits2/NUM_SEARCH<<endl<<endl;
    cout<<"Testing Custom Probing..."<<endl;
    
    CustomProbingTable<string,int>customHash1(1);
    for(int i=0;i<NUM_WORDS;i++) {
        customHash1.insert(words[i],i+1);
    }
    
    totalHits1=0;
    for(const string& word:searchWords){
        int value,hits;
        customHash1.search(word,value,hits);
        totalHits1+=hits;
    }
    
    CustomProbingTable<string,int>customHash2(2);
    for(int i=0;i<NUM_WORDS;i++) {
        customHash2.insert(words[i],i+1);
    }
    
    totalHits2=0;
    for(const string& word:searchWords) {
        int value,hits;
        customHash2.search(word,value,hits);
        totalHits2+=hits;
    }
    
    results.push_back({
        "Custom Probing",
        customHash1.collisioncount,
        (double)totalHits1/NUM_SEARCH,
        customHash2.collisioncount,
        (double)totalHits2/NUM_SEARCH
    });
    
    cout<<"  Hash1 - Collisions: "<<customHash1.collisioncount<<", Avg Hits: "<<fixed<<setprecision(2)<<(double)totalHits1/NUM_SEARCH<<endl;
    cout<<"  Hash2 - Collisions: "<<customHash2.collisioncount<<", Avg Hits: "<<fixed<<setprecision(2)<<(double)totalHits2/NUM_SEARCH<<endl<<endl;
    cout << "\n========================================" << endl;
    cout << "         PERFORMANCE REPORT" << endl;
    cout << "========================================" << endl;
    cout << "Total Words: " << NUM_WORDS << endl;
    cout << "Word Length: " << WORD_LENGTH << endl;
    cout << "Search Words: " << NUM_SEARCH << endl;
    cout << "========================================\n" << endl;
    
    cout << setw(20) << left << "" 
         << setw(35) << "Hash1" 
         << setw(35) << "Hash2" << endl;
    cout << setw(20) << left << "" 
         << setw(17) << "Collisions" 
         << setw(18) << "Avg Hits"
         << setw(17) << "Collisions" 
         << setw(18) << "Avg Hits" << endl;
    cout << string(90, '-') << endl;
    
    for (const Result& r : results) {
        cout << setw(20) << left << r.method
             << setw(17) << r.collisionsHash1
             << setw(18) << fixed << setprecision(2) << r.avgHitsHash1
             << setw(17) << r.collisionsHash2
             << setw(18) << fixed << setprecision(2) << r.avgHitsHash2
             << endl;
    }
    cout << string(90, '-') << endl;
}

int main() {
    FAST_IO;
    int choice;
    
    while (true) {
        cout <<"\nMenu:" << endl;
        cout <<"1. Run full evaluation (10,000 words)" << endl;
        cout <<"2. Test with custom number of words" << endl;
        cout <<"3. Exit" << endl;
        cin>>choice;
        
        if (choice == 1) {
            runEvaluation();
        }
        else if (choice == 2) {
            int numWords, wordLength;
            cout << "Enter number of words: ";
            cin >> numWords;
            cout << "Enter word length: ";
            cin >> wordLength;
            
            WordGenerator wordGen;
            vector<string>words;
            
            cout << "Generating words..." << endl;
            for (int i = 0; i < numWords; i++) {
                words.push_back(wordGen.generateUniqueWord(wordLength));
            }
            
            Chaining<string, int> table(1);
            for (int i = 0; i < numWords; i++) {
                table.insert(words[i], i + 1);
            }
            
            cout << "Inserted " << numWords << " words" << endl;
            cout << "Total collisions: " << table.collisioncount << endl;
            cout << "Load factor: " << fixed << setprecision(2) << table.loadfactor() << endl;
            cout << "Table size: " << table.tablesize << endl;
        }
        else if(choice==3) {
            cout << "Exiting..." << endl;
            break;
        }
        else {
            cout << "Invalid choice!" << endl;
        }
    }
    return 0;
}


bool search(const K& key, V& value, int& hits) override {
    hits = 0;
    int i = 0;
    int idx;
    cout << "Probe sequence for key [" << key << "]: "; // New print
    while (i < this->tablesize) {
        idx = doubleHash(key, i);
        cout << idx << (i == this->tablesize - 1 ? "" : " -> "); // Print each step
        hits++;
        if (!table[idx].isOccupied) {
            cout << " (Empty slot found - Stop)" << endl;
            return false;
        }
        if (table[idx].isOccupied && !table[idx].isDeleted && table[idx].key == key) {
            value = table[idx].value;
            cout << " (Key found!)" << endl;
            return true;
        }
        i++;
    }
    cout << " (Table exhausted)" << endl;
    return false;
}


void insert(const K& key, const V& value) override {
    V temp;
    int hits;
    if (search(key, temp, hits)) return;

    int i = 0;
    int idx;
    cout << "Insertion probe sequence for [" << key << "]: ";
    while (i < this->tablesize) {
        idx = customHash(key, i);
        cout << idx << " "; // Print the index being checked
        
        if (!table[idx].isOccupied || table[idx].isDeleted) {
            if (i > 0) this->collisioncount++;
            table[idx] = HashEntry<K, V>(key, value);
            this->total++;
            this->insertcount++;
            cout << "[Inserted at " << idx << "]" << endl;
            
            // Rehash check...
            if (this->loadfactor() > 0.5 && this->insertcount >= this->lastsize / 2) {
                int newSize = this->nextPrime(2 * this->tablesize);
                rehash(newSize);
                this->insertcount = 0;
            }
            return;
        }
        i++;
    }
}



int solveSubarraySum(const vector<int>& nums, int k) {
    int n = nums.size();
    // K = prefixSum, V = frequency
    // Using a prime size for the table roughly equal to input size
    Chaining<int, int> freqTable(1, 10007); 
    
    int currentPrefixSum = 0;
    int count = 0;
    int hits;

    // Base Case: A prefix sum of 0 has occurred once 
    // (This handles subarrays starting from index 0)
    freqTable.insert(0, 1);

    for (int num : nums) {
        currentPrefixSum += num;
        
        // Calculate the value we need to find in the table
        int complement = currentPrefixSum - k;
        int freqOfComplement = 0;

        // Search if (PrefixSum - K) exists in our frequency table
        if (freqTable.search(complement, freqOfComplement, hits)) {
            count += freqOfComplement;
        }

        // Update the frequency of the current prefix sum in the table
        int currentFreq = 0;
        if (freqTable.search(currentPrefixSum, currentFreq, hits)) {
            // If it exists, update it (using the Update logic we discussed)
            freqTable.update(currentPrefixSum, currentFreq + 1);
        } else {
            // Otherwise, insert it for the first time
            freqTable.insert(currentPrefixSum, 1);
        }
    }

    return count;
}