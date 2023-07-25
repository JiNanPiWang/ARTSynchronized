#include <iostream>
#include <chrono>
#include "tbb/tbb.h"
#include<unordered_map>
#include<thread>
#include "OptimisticLockCoupling/Tree.h"
#include "ROWEX/Tree.h"
#include "ART/Tree.h"
#include<functional>
#include<omp.h>
#include<algorithm>
#include<map>
#include<vector>
#include<thread>
#include<queue>
#include<math.h>
#include <mutex>
#include <condition_variable>
#include <memory>
#include<fstream>
#include<assert.h>
#include<set>

using namespace std::literals::chrono_literals;
using callback = void(*)(void*);
int NUMBER = 2;
// std::atomic<uint64_t> sum{0};
//extern int count=0;
using namespace std;
void loadKey(TID tid, Key &key) {
    // Store the key of the tuple into the key vector
    // Implementation is database specific
    key.setKeyLen(sizeof(tid));
    reinterpret_cast<uint64_t *>(&key[0])[0] = __builtin_bswap64(tid);
}

ART_OLC::Tree tree(loadKey);

void read_data_from_file(string &path, uint64_t *result, int n){
    ifstream infile;
    infile.open(path.data(), ios::binary | ios::in);
    if(!infile.is_open()){
        cout<<"open file error"<<endl;
        return;
    }
    uint64_t filesize = infile.seekg(0, ios::end).tellg();
    if(filesize < n * sizeof(uint64_t))
        cout << "file size is not enough" << endl;

    infile.seekg(0, ios::beg);
    infile.read((char*)result, filesize);
    //cout << "read " << infile.gcount() << " bytes" << endl;
    infile.close();
}



void singlethreaded(char **argv) {
    std::cout << "single threaded:" << std::endl;

    uint64_t n = std::atoll(argv[1]);
    uint64_t *keys = new uint64_t[n];

    // Generate keys
    for (uint64_t i = 0; i < n; i++)
        // dense, sorted
        keys[i] = i + 1;
    if (atoi(argv[2]) == 1)
        // dense, random
        std::random_shuffle(keys, keys + n);
    if (atoi(argv[2]) == 2)
        // "pseudo-sparse" (the most-significant leaf bit gets lost)
        for (uint64_t i = 0; i < n; i++)
            keys[i] = (static_cast<uint64_t>(rand()) << 32) | static_cast<uint64_t>(rand());

    //printf("operation,n,ops/s\n");
    ART_unsynchronized::Tree tree(loadKey);

    // Build tree
    {
        auto starttime = std::chrono::system_clock::now();
       for (uint64_t i = 0; i != n; i++) {
            Key key;
            loadKey(keys[i], key);
            //key.getall();
            //printf("%d %d\n",keys[i],key.getKeyLen());
            tree.insert(key, keys[i]);
   }
        auto endtime = std::chrono::system_clock::now();
        cout<<"insert, "<<n<<" "<<std::chrono::duration_cast<std::chrono::milliseconds>(endtime-starttime).count()<<"ms"<<endl;
        //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        //        std::chrono::system_clock::now() - starttime);
        //printf("insert,%ld,%f\n", n, (n * 1.0) / duration.count());
    }

    {
        // Lookup
        auto starttime = std::chrono::system_clock::now();
        for (uint64_t i = 0; i != n; i++) {
            Key key;
            loadKey(keys[i], key);
            auto val = tree.lookup(key);
            if (val != keys[i]) {
                std::cout << "wrong key read: " << val << " expected:" << keys[i] << std::endl;
                throw;
            }
        }
        //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        //    std::chrono::system_clock::now() - starttime);
        //printf("lookup,%ld,%f\n", n, (n * 1.0) / duration.count());
        auto endtime = std::chrono::system_clock::now();
        cout<<"lookup, "<<n<<" "<<std::chrono::duration_cast<std::chrono::milliseconds>(endtime-starttime).count()<<"ms"<<endl;
    }

    {
        auto starttime = std::chrono::system_clock::now();

        for (uint64_t i = 0; i != n; i++) {
            Key key;
            loadKey(keys[i], key);
            tree.remove(key, keys[i]);
        }
        auto endtime = std::chrono::system_clock::now();
        cout<<"remove, "<<n<<" "<<std::chrono::duration_cast<std::chrono::milliseconds>(endtime-starttime).count()<<"ms"<<endl;
        // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        //        std::chrono::system_clock::now() - starttime);
        // printf("remove,%ld,%f\n", n, (n * 1.0) / duration.count());
    }
    delete[] keys;

    std::cout << std::endl;
}

void multithreaded_ART_OLC(char **argv) {
    std::cout << "multi threaded ART_OLC:" << std::endl;

    uint64_t n = std::atoll(argv[1]);
    uint64_t *keys = new uint64_t[n];

    //uint64_t *lookkeys = new uint64_t[2*n];
    //std::random_shuffle(lookkeys, lookkeys + 2*n);

    // Generate keys
    for (uint64_t i = 0; i < n; i++)
        // dense, sorted
        keys[i] = i + 1;
    if (atoi(argv[2]) == 1)
        // dense, random
        std::random_shuffle(keys, keys + n);
    if (atoi(argv[2]) == 2)
        // "pseudo-sparse" (the most-significant leaf bit gets lost)
        for (uint64_t i = 0; i < n; i++)
            keys[i] = (static_cast<uint64_t>(rand()) << 32) | static_cast<uint64_t>(rand());

  
    ART_OLC::Tree tree(loadKey);
    tbb::task_scheduler_init init(atoi(argv[3]));
    // for(int i=0;i<(512000/2)*10;i++)
    // {
    //     int index1=rand()%5120000;
    //     int index2=rand()%5120000;
    //     // cout<<index1<<" "<<index2<<endl;
    //     swap(keys[index1],keys[index2]);
    // }
    int blocksize=n/10000;
    {
        int count[16]={0};
        int sum=0;
        auto starttime = std::chrono::system_clock::now();
        for(int j=0;j<10000;j++)
        {
            // int amax=0;
            auto t0 = std::chrono::system_clock::now();
            tbb::parallel_for(tbb::blocked_range<uint64_t>(blocksize*j,blocksize*(j+1)), [&](const tbb::blocked_range<uint64_t> &range) {
                auto t = tree.getThreadInfo();
                for (uint64_t i = range.begin(); i != range.end(); i++) {
                    Key key;
                    loadKey(keys[i], key);
                    tree.insert(key, keys[i], t );
                }
            });
            auto t1 = std::chrono::system_clock::now();
        }
        
        auto endtime = std::chrono::system_clock::now();
        cout<<"insert, "<<n<<" "<<std::chrono::duration_cast<std::chrono::milliseconds>(endtime-starttime).count()<<"us"<<endl;
    }

    {
        // Lookup
        auto starttime = std::chrono::system_clock::now();
        tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
            auto t = tree.getThreadInfo();
            for (uint64_t i = range.begin(); i != range.end(); i++) {
                Key key;
                loadKey(keys[i], key);
                auto val = tree.lookup(key, t);
                //printf("%d",i);
                if (val != keys[i]) {
                    std::cout << "wrong key read: " << val << " expected:" << keys[i] << std::endl;
                    throw;
                }
            }

        });
        auto endtime = std::chrono::system_clock::now();
        cout<<"lookup, "<<n<<" "<<std::chrono::duration_cast<std::chrono::milliseconds>(endtime-starttime).count()<<"ms"<<endl;
    }

    {
        auto starttime = std::chrono::system_clock::now();

        tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
            auto t = tree.getThreadInfo();
            for (uint64_t i = range.begin(); i != range.end(); i++) {
                Key key;
                loadKey(keys[i], key);
                tree.remove(key, keys[i], t);
            }
        });
        auto endtime = std::chrono::system_clock::now();
        cout<<"remove, "<<n<<" "<<std::chrono::duration_cast<std::chrono::milliseconds>(endtime-starttime).count()<<"ms"<<endl;
    }
    delete[] keys;
}




bool flag=true;
int64_t amax=0;
void thread_function(vector<uint64_t> *bucket,int index)
{
    //printf("test\n");
    std::thread::id this_id = std::this_thread::get_id();
    // ofstream outfile;
    // outfile.open("out.txt",ios::out);
    //out << "test";
    while(flag){std::this_thread::sleep_for(std::chrono::nanoseconds(1));};
    
    
    //Cache c;
    //c.cache_node=NULL; 
    int n=bucket->size();
    //int levelcount[8]={0};
    int64_t sum=0;
    // auto t = tree.getThreadInfo();
    auto starttime = std::chrono::system_clock::now();
    
    for(int i=0;i<n;i++)
    {
        // auto t0 = std::chrono::system_clock::now();
        Key key;
        loadKey((*bucket)[i], key);        
        // tree.insert_lockfree(key, (*bucket)[i], t);
        tree.insert_level(key, (*bucket)[i],0);
        // auto t1 = std::chrono::system_clock::now();
        // if(index==10||index==4)
        // {
        //     outfile<<"thread_id: "<<index<<", key: "<<(*bucket)[i]<<", time: "<<std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count()<<"us"<<endl;
        // }
        
        // if(std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count()>10)
        // {
        //     printf("thread_id: %d, key: %lx, time: %dus\n",index,(*bucket)[i],std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count());
        // }
        // sum+=std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
    }
    
    auto endtime = std::chrono::system_clock::now();
    // outfile.close();
    // if((*bucket)[0]==0x643c986966334873)
    // {
    //     for(int i=0;i<n;i++)printf("%lx\n",(*bucket)[i]);
    // }
    // printf("bucket: %d,size: %d,time: %dus\n",index,bucket->size(),std::chrono::duration_cast<std::chrono::microseconds>(endtime-starttime).count());
    
    // amax=max(amax,sum);
    amax=max(amax,std::chrono::duration_cast<std::chrono::microseconds>(endtime-starttime).count());
    // amax+=std::chrono::duration_cast<std::chrono::microseconds>(endtime-starttime).count();
}



struct block
{
    // int index;
    vector<uint64_t> val;
    Cache c;
    // struct block* next;
};
int time0=0,time1=0;
void thread_fun(block* p,int index)
{
    // while(flag){std::this_thread::sleep_for(std::chrono::nanoseconds(1));};
    int n=p->val.size();
    int time=0;
    Key key;
    auto t0 = std::chrono::system_clock::now();
    loadKey(p->val[0], key);
    tree.insert(key, p->val[0],p->c, time);
    loadKey(p->val[1], key);
    tree.insert(key, p->val[1],p->c, time);
    auto t1 = std::chrono::system_clock::now();
    time+=std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count();
    for(int i=2;i<n;i++)
    {
        auto t2 = std::chrono::system_clock::now();
        loadKey(p->val[i], key);
        tree.insert(key, p->val[i],p->c, time);
        auto t3 = std::chrono::system_clock::now();
    }
    if(index==0)
    {
        time0+=time/1000;
    }
    else 
    {
        time1+=time/1000;
    }
}
void test(char **argv)
{
    uint64_t n = std::atoll(argv[1]);
    uint64_t *keys = new uint64_t[n];


    // Generate keys
    for (uint64_t i = 0; i < n; i++)
        // dense, sorted
        keys[i] = i;
    if (atoi(argv[2]) == 1)
        // dense, random
        {
            std::random_shuffle(keys, keys + n);
            for(int i=0;i<n;i++)
            {
                keys[i]=keys[i]<<16;
            }
        }
    if (atoi(argv[2]) == 2)
    {
        for (uint64_t i = 0; i < n; i++)
        {
            keys[i] = (static_cast<uint64_t>(rand()) << 32) | static_cast<uint64_t>(rand());
        }
    }
    
    tbb::task_scheduler_init init(atoi(argv[3]));

    int sumtime=0;
    int64_t sumparttime=0;
    int blocksize=n/10000;

    // 打乱顺序
    for(int i=0;i<(512000/2)*1;i++)
    {
        int index1=rand()%5120000;
        int index2=rand()%5120000;
        swap(keys[index1],keys[index2]);
    }
    {
        auto t = tree.getThreadInfo();
        long long int sum=0;
        auto starttime = std::chrono::system_clock::now();
        thread *mythread[16];
        //  10000批插入
        for(int j=0;j<10000;j++)
        {
            //分桶
            unordered_map<uint64_t,block*>mp;    
            auto t0 = std::chrono::system_clock::now();        
            for(int i=blocksize*j;i<blocksize*(j+1);i++)
            {
                uint64_t prefix=keys[i]&(0xFFFFFFFFFFFFFF00);
                if(mp.find(prefix)==mp.end())
                {
                    block* p=new block();
                    p->c.cache_node=NULL;
                    mp[prefix]=p;
                    p->val.push_back(keys[i]);
                }
                else 
                {
                    mp[prefix]->val.push_back(keys[i]);
                }
            }
            auto t1 = std::chrono::system_clock::now();
            sum+=std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
            int count=0;

            for(auto &p:mp)  //划分给多个线程进行插入
            {
                int n=p.second->val.size();
                if(n>48)//插入在同一个节点中的256个数据  生成找到并生成N256  缓存其地址
                {
                    //大块生成线程插入
                    // mythread[count]=new thread(thread_fun,p.second,count);
                    // count++;
                    
                    //由main函数插入
                    int time=0;
                    Key key;
                    
                    for(int i=0;i<n;i++)
                    {
                        
                        loadKey(p.second->val[i], key);
                        tree.insert(key, p.second->val[i],p.second->c, time);
                    }
                }
                else //让一个线程进行有锁插入
                {
                    for(int i=0;i<n;i++)
                    {
                        Key key;
                        loadKey(p.second->val[i], key);
                        tree.insert(key, p.second->val[i], t );
                    }
                }
            }
            //多线程join            
            // for(int i=0;i<2;i++)
            // {
            //     mythread[i]->join();
            // }

            for(auto p:mp)
            {
                delete p.second;
            }
            mp.clear();

        }
        // cout<<sum/1000<<endl;
        auto endtime = std::chrono::system_clock::now();
        cout<<"insert, "<<n<<" "<<std::chrono::duration_cast<std::chrono::milliseconds>(endtime-starttime).count()<<"ms"<<endl;
    }
    
    {
        // Lookup
        auto starttime = std::chrono::system_clock::now();
        tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
            auto t = tree.getThreadInfo();
            for (uint64_t i = range.begin(); i != range.end(); i++) {
                Key key;
                loadKey(keys[i], key);
                auto val = tree.lookup(key, t);
                //printf("%d",i);
                if (val != keys[i]) {
                    // cout<<keys[i]<<endl;
                    std::cout << "wrong key read: " << val << " expected:" << keys[i] << std::endl;
                    // throw;
                }
            }

        });
        auto endtime = std::chrono::system_clock::now();
        cout<<"lookup, "<<n<<" "<<std::chrono::duration_cast<std::chrono::microseconds>(endtime-starttime).count()<<"us"<<endl;
    }

    {
        auto starttime = std::chrono::system_clock::now();

        tbb::parallel_for(tbb::blocked_range<uint64_t>(0, n), [&](const tbb::blocked_range<uint64_t> &range) {
            auto t = tree.getThreadInfo();
            for (uint64_t i = range.begin(); i != range.end(); i++) {
                Key key;
                loadKey(keys[i], key);
                tree.remove(key, keys[i], t);
            }
        });
        auto endtime = std::chrono::system_clock::now();
        cout<<"remove, "<<n<<" "<<std::chrono::duration_cast<std::chrono::microseconds>(endtime-starttime).count()<<"us"<<endl;
    }

    delete[] keys;
}


int main(int argc, char **argv) {
    if (argc != 4) {
        printf("usage: %s n 0|1|2 线程数 test需要自己修改代码 原版直接输入\nn: number of keys\n0: sorted keys\n1: dense keys\n2: sparse keys\n样例 ./example 5120000 0 1", argv[0]);
        return 1;
    }
    test(argv);   //分桶版本

    //singlethreaded(argv);

    // multithreaded_ART_OLC(argv);   //原版

    return 0;
}
