#include <thread>
#include <atomic>
#include <vector>
#include <bits/stdc++.h>
#include <functional>
//#include "buffered_channel.h"
#include <gtest/gtest.h>

template<typename T, typename U>
class PipelineManager{

    int last_id = 0;
    std::unordered_map<int, std::atomic_bool> pipelines_closed_id;
    std::unordered_map<int, std::vector<std::thread>> pipelines_threads;
    std::unordered_map<int, std::pair<IChannel<T>*, IChannel<U>*>> pipelines_channels;
public:

    int RegisterPipeline(IChannel<T>* input_channel,
                         IChannel<U>* output_channel, 
                         int threads_count, U(*foo)(T) )  // std::function<U(T))> foo
    {
        pipelines_closed_id[this->last_id] = false;
        int current_id = this->last_id;

        for(int thread_index = 0; thread_index < threads_count; ++thread_index){
            pipelines_threads[current_id].emplace_back([input_channel, output_channel, current_id, foo, this](){
                while(!this->pipelines_closed_id[current_id])
                {
                    auto current_value = input_channel->Recv();
                    if(current_value == boost::none){
                        break;
                    }
                    try{
                    output_channel->Send(foo(*current_value));
                    }
                    catch(...){
                        break;
                    }
                }
            });
        }

        pipelines_channels[this->last_id] = std::make_pair(input_channel, output_channel);

        return this->last_id++;
    }

    void ClosePipeline(int id){
        //pipelines_channels[id].first->Close();
        //pipelines_channels[id].second->Close();
        pipelines_closed_id[id] = true;
        for(auto &thread : pipelines_threads[id]){
            thread.join();
        }
    }
};