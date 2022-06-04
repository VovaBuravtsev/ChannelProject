#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <mutex>
#include <iostream>
#include <bits/stdc++.h>
#include <condition_variable>
#include <functional>
#include <boost/optional/optional_io.hpp>
#include <boost/none.hpp>
#include "buffered_channel.h"
#include "pipeline_manager.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <boost/regex.hpp>
#include <regex>
#include <boost/algorithm/string.hpp>

const std::regex regex_email("(\\w+)@(\\w+)\\.com.ua");

std::string TextGenerator(int size){
	srand((int) time(0));
	std::string text;
	std::string email;
	for ( int i = 0, j = 0; i < size; i++ ) {
		bool cond1 = (rand() % (1 - 0 + 1)) == 1;;
		bool cond2 = (rand() % (1 - 0 + 1)) == 1;;

		if(cond1 && cond2){
        	j = rand() % 15 + 3;
        
        	if ( j > size - i ){j = size - i;}
        	i += j;
        
        	for ( int k = 0; k < j; k++ ) {email += (char)(rand() % 26 + 97);}
			email += "@";

			j = rand() % 5 + 3;
        
        	if ( j > size - i ){j = size - i;}
        	i += j;

			for ( int k = 0; k < j; k++ ) {email += (char)(rand() % 26 + 97);}
			email += ".com.ua";

			text += email;
			email.clear();
		}
		else{
			j = rand() % 10 + 1;
        
        	if ( j > size - i ){
				j = size - i;
			}
        	i += j;
        
        	for ( int k = 0; k < j; k++ ) {
        	    text += (char)(rand() % 26 + 97);
        	}
		}
        text += ' ';
    }
	return text;
}

std::vector<std::string> EmailParser(std::string input_text){
	std::vector<std::string> strs;
	boost::split(strs, input_text, boost::is_any_of("\t "));
	std::vector<std::string> output_vector;
	for(auto & word : strs)
		{
			if(std::regex_match(word, regex_email)){
				//std::cout << "Regex MATCH with " << word << '\t'; 
				output_vector.emplace_back(word);
			}
			else{
				//std::cout << "Regex NO MATCH with " << word << '\t'; 
			}
		}
	return output_vector;
}

double EmailParsing_func(int characters_count, int texts_count, int threads_count){
	BufferedChannel<std::string> input_channel(texts_count);
	BufferedChannel<std::vector<std::string>> output_channel(texts_count);
	PipelineManager<std::string, std::vector<std::string>> manager;

	for(int i = 0; i < texts_count; ++i){
		input_channel.Send(TextGenerator(characters_count));
	}

	auto start = std::chrono::system_clock::now();

	int id = manager.RegisterPipeline(&input_channel, &output_channel, threads_count, EmailParser);
	input_channel.WaitFree();
	input_channel.Close();
	manager.ClosePipeline(id);

	auto end = std::chrono::system_clock::now();
	
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "TIME FOR EmailParsing in " << threads_count << " threads is: - " << elapsed_seconds.count() <<'\n';
	while(!output_channel.getEmpty()){
		std::vector<std::string> string_vector = *output_channel.Recv();
	}

	return elapsed_seconds.count();
}


TEST(Correctness, UNI_EmailParsing){
	std::vector<std::vector<double>> vec_vec_double;

	for(int i = 0; i < 8; ++i){
		std::vector<double> vec_double;
		vec_double.push_back(EmailParsing_func(32768, 1024, 1));
		vec_double.push_back(EmailParsing_func(32768, 1024, 2));
		vec_double.push_back(EmailParsing_func(32768, 1024, 4));
		vec_double.push_back(EmailParsing_func(32768, 1024, 8));

		vec_vec_double.push_back(vec_double);
		std::cout<<'\n';
	}
	
	double num_1 = 0.0;
	double num_2 = 0.0;
	double num_4 = 0.0;
	double num_8 = 0.0;

	for(auto & vec : vec_vec_double){
		num_1 += vec[0];
		num_2 += vec[1];
		num_4 += vec[2];
		num_8 += vec[3];
	}

	num_1 = num_1/8.0;
	num_2 = num_2/8.0;
	num_4 = num_4/8.0;
	num_8 = num_8/8.0;

	std::cout<<"Avg 1 thread work: " << num_1 << '\n';
	std::cout<<"Avg 2 thread work: " << num_2 << '\n';
	std::cout<<"Avg 4 thread work: " << num_4 << '\n';
	std::cout<<"Avg 8 thread work: " << num_8 << '\n';
}

// std::string demo_EmailParser(std::string input_text){
// 	if(std::regex_match(input_text, regex_email)){
// 		return input_text;
// 	}
// 	return std::string("NO");
// }

// TEST(REGEX, regexp1){
// 	std::string word1 = "no";
// 	std::string word2 = "nono@mail.com.ua";

// 	std::thread t1([&, word1, word2](){
// 		for(int i = 0; i < 1000; ++i){
// 			//EXPECT_EQ(demo_EmailParser(word1), std::string("NO"));
// 			EXPECT_EQ(demo_EmailParser(word2), word2);
// 			auto vec1 = EmailParser(word1);
// 			auto vec2 = EmailParser(word2);
// 		}
// 	});

// 	std::thread t2([&, word1, word2](){
// 		for(int i = 0; i < 1000; ++i){
// 			//EXPECT_EQ(demo_EmailParser(word1), std::string("NO"));
// 			EXPECT_EQ(demo_EmailParser(word2), word2);
// 			auto vec1 = EmailParser(word1);
// 			auto vec2 = EmailParser(word2);
// 		}
// 	});

// 	t1.join();
// 	t2.join();
// }

// TEST(FUNCTIONS, functions1){
// 	std::string text1 = TextGenerator(300);

// 	std::thread t1([&](){
// 		std::vector<std::string> strs;
// 		for(int i = 0; i < 500; ++i){
// 			boost::split(strs, text1, boost::is_any_of("\t "));
// 		}
// 	});

// 	std::thread t2([&](){
// 		std::vector<std::string> strs;
// 		for(int i = 0; i < 500; ++i){
// 			boost::split(strs, text1, boost::is_any_of("\t "));
// 		}
// 	});

// 	t1.join();
// 	t2.join();
// }

// TEST(FUNCTIONS, functions2){
// 	std::string text1 = TextGenerator(300);

// 	std::thread t1([&](){
// 		std::vector<std::string> strs;
// 		std::vector<std::string> strs_words;
// 		for(int i = 0; i < 500; ++i){
// 			boost::split(strs, text1, boost::is_any_of("\t "));
// 			for(auto & word : strs){
// 				strs_words.emplace_back(word);
// 			}
// 		}
// 	});

// 	std::thread t2([&](){
// 		std::vector<std::string> strs;
// 		std::vector<std::string> strs_words;
// 		for(int i = 0; i < 500; ++i){
// 			boost::split(strs, text1, boost::is_any_of("\t "));
// 			for(auto & word : strs){
// 				strs_words.emplace_back(word);
// 			}
// 		}
// 	});

// 	t1.join();
// 	t2.join();
// }


// namespace {

// struct SegmentAndValue{
//   SegmentAndValue(int left_bound, int right_bound, int value)
//     : left_bound(left_bound), right_bound(right_bound), value(value)
//   {}

//   int left_bound{0};
//   int right_bound{0};
//   int value{0};
// };

// std::vector<std::pair<int, int>> Linspace(uint64_t left_bound, uint64_t right_bound, int count_segment) {
//   uint64_t len = (right_bound - left_bound) / count_segment;
//   std::vector<std::pair<int, int>> result;
//   uint64_t current_left = left_bound, current_right = left_bound + len;
//   for (int i = 0; i < count_segment; ++i) {
//       result.push_back({current_left, current_right});
//       current_left = current_right + 1;
//       current_right = current_left + len;
//       current_left = std::min(current_left, right_bound);
//       current_right = std::min(current_right, right_bound);
//   }

//   return result;
// }


// bool checkSegment(SegmentAndValue segment_and_value)
// {
//   bool ok{true};
//   for (int current = segment_and_value.left_bound; current <= segment_and_value.right_bound; ++current)
//   {
//     if (segment_and_value.value % current == 0)
//     {
//       ok = false;
//       break;
//     }
//   }
//   return ok;
// }


// bool isPrime(int x)
// {
//   const auto count_thread = std::thread::hardware_concurrency();

//   if (x <= 1)
//   {
//     return false;
//   }
//   if (x == 2)
//   {
//     return true;
//   }

//     int root = sqrt(x) + 6;
//     root = std::min(root, x - 1);

//     const int count_segment = count_thread;

//     auto segments = Linspace(2, root, count_segment);

//     PipelineManager<SegmentAndValue, bool> manager;
//     BufferedChannel<SegmentAndValue> input(count_thread);
//     BufferedChannel<bool> output(count_thread);
     
//     for (const auto& segment : segments)
//     {
//     	input.Send(SegmentAndValue(segment.first, segment.second, x));
//     }
//     auto id = manager.RegisterPipeline(&input, &output, count_thread, checkSegment);
	 
// 	input.WaitFree();
// 	input.Close();
// 	manager.ClosePipeline(id);

//     bool result = true;
//     for (int idx = 0; idx < count_segment; ++idx)
//     {
//     	auto segment_result = output.Recv();
//     	EXPECT_TRUE(segment_result != boost::none);
//     	result = result && segment_result.get();
//     }
//     return result;
// }

// } // anonymous namespace

// TEST(Correctness, checkPrime)
// {
//   int x = 1000000007;
//   EXPECT_TRUE(isPrime(x));
// }

// void Wait(int time){
// 	std::this_thread::sleep_for(std::chrono::milliseconds(time));
// }

// template<typename T, typename U>
// U ValueConverter(T input_value){
//     return U(input_value);
// }

// void RunTest_BufferedChannel(int senders_count, int receivers_count, int buff_size, int close_limit){
// 	BufferedChannel<int> run_test_channel(buff_size);
// 	std::atomic<int> free_slots(buff_size);
// 	std::vector<std::vector<int>> send_values_vector(senders_count);
// 	std::vector<std::vector<int>> recv_values_vector(receivers_count);
// 	std::vector<int> send_values;
// 	std::vector<int> recv_values;
// 	std::vector<std::thread> threads;
// 	std::atomic<int> value(0);
	
// 	for (int i = 0; i < senders_count; i++)
// 	{
// 		threads.emplace_back([&, i](){
// 			while(!run_test_channel.getClosed()){
// 				try{
// 					int new_value = std::atomic_fetch_add(&value, 1);
// 					run_test_channel.Send(new_value);
// 					int current_free_slots = std::atomic_fetch_sub(&free_slots, 1);
// 					send_values_vector[i].emplace_back(new_value);
// 					EXPECT_TRUE(current_free_slots >= 0);
// 				}
// 				catch(...){
// 					EXPECT_EQ(run_test_channel.getClosed(), true);
// 					break;
// 				}
// 			}
// 		});
// 	}

// 	for (int i = 0; i < receivers_count; i++)
// 	{
// 		threads.emplace_back([&, i](){
// 			while(true){
// 				boost::optional<int> current_value = run_test_channel.Recv();
// 				int current_free_slots = std::atomic_fetch_add(&free_slots, 1);
// 				if(current_value == boost::none){EXPECT_EQ(run_test_channel.getClosed(), true);break;}

// 				recv_values_vector[i].emplace_back(*current_value);
// 			}
// 		});
// 	}
	
// 	std::this_thread::sleep_for(std::chrono::milliseconds(close_limit));
// 	run_test_channel.Close();

// 	for (auto& th : threads)th.join();

// 	for(auto& vec : send_values_vector){
// 		for(auto& elem : vec){
// 			send_values.push_back(elem);
// 		}
// 	}

// 	for(auto& vec : recv_values_vector){
// 		for(auto& elem : vec){
// 			 recv_values.push_back(elem);
// 		}
// 	}
	
// 	//recv_values.emplace_back(777);

// 	std::sort(send_values.begin(), send_values.end());
// 	std::sort(recv_values.begin(), recv_values.end());

// 	EXPECT_EQ(send_values, recv_values);
// }

// TEST(Correctness, RunTest_){
// 	RunTest(10, 10, 20, 300);
// }

// TEST(Correctness, Simple_Buf_RunTest1){
// 	RunTest_BufferedChannel(1, 1, 10, 500);
// }

// TEST(Correctness, Simple_Buf_RunTest2){
// 	RunTest_BufferedChannel(1, 10, 5, 500);
// }

// TEST(Correctness, Simple_Buf_RunTest3){
// 	RunTest_BufferedChannel(10, 1, 5, 500);
// }

// TEST(Correctness, Simple_Buf_RunTest4){
// 	RunTest_BufferedChannel(10, 10, 20, 500);
// }

// TEST(Correctness, Simple_Buf_RunTest_Problemo1){
// 	RunTest_BufferedChannel(10, 10, 1, 1000);
// }

// TEST(Correctness, Simple_Buf_RunTest_Problemo2){
// 	RunTest_BufferedChannel(20, 20, 0, 300);
// }

// TEST(Correctness, Simple_Buf_RunTest_Problemo3){
// 	RunTest_BufferedChannel(0, 20, 0, 300);
// }

// TEST(Correctness, Simple_Buf_RunTest_Problemo4){
// 	RunTest_BufferedChannel(30, 30, 1, 10000);
// }

// TEST(Correctness, Simple_Buf_RunTest_Problemo5){
// 	RunTest_BufferedChannel(20, 0, 0, 300);
// }

// TEST(Correctness, Simple_Buf_RunTest_Problemo6){
// 	RunTest_BufferedChannel(20, 1, 1, 1000);
// }

// TEST(Correctness, Simple_Buf_RunTest_Problemo7){
// 	RunTest_BufferedChannel(0, 20, 1, 300);
// }

// TEST(Correctness, Buf_Simple){
// 	BufferedChannel<int> demo_channel(3);

// 	demo_channel.Send(10);
// 	EXPECT_EQ(demo_channel.Recv(), 10);

// 	demo_channel.Send(5);
// 	EXPECT_EQ(demo_channel.Recv(), 5);

// 	demo_channel.Send(1);
// 	demo_channel.Send(2);
// 	demo_channel.Send(4);
	
// 	EXPECT_EQ(demo_channel.Recv(), 1);
// 	EXPECT_EQ(demo_channel.Recv(), 2);
// }

// TEST(Correctness, Buf_Senders){
// 	BufferedChannel<int> demo_channel(3);

// 	try{
// 		demo_channel.Close();
// 		demo_channel.Send(10);
// 	}
// 	catch(const std::logic_error& excep){
		
// 	}
// }

// TEST(Correctness, Buf_Threads){
// 	BufferedChannel<int> demo_channel(3);

// 	std::thread t([&demo_channel]() {
// 		for(int i = 0; i < 100; i++){
// 			demo_channel.Send(21);
// 		}
// 	});

// 	//std::this_thread::sleep_for(std::chrono::milliseconds(1));

// 	for(int i = 0; i < 100; i++){
// 		EXPECT_EQ(demo_channel.Recv(), 21);
// 	}

// 	t.join();
// }

// TEST(Correctness, Buf_ThreadsClosed){
// 	BufferedChannel<int> demo_channel(3);

// 	std::thread t([&demo_channel](){
// 		for (int i = 0; i < 100; ++i)
// 		{
// 			demo_channel.Send(1);
// 			demo_channel.Send(2);
// 			demo_channel.Send(3);

// 			demo_channel.Recv();
// 			demo_channel.Recv();
// 			demo_channel.Recv();
// 		}
// 	});

// 	std::this_thread::sleep_for(std::chrono::milliseconds(5));

// 	demo_channel.Close();
// 	EXPECT_EQ(demo_channel.Recv(), boost::none);
// 	t.join();
// }

// TEST(Correctness, PipelineManager_Test1){
// 	PipelineManager<int, double> manager;
// 	BufferedChannel<int> input_buf_channel(5);
// 	BufferedChannel<double> output_buf_channel(5);

// 	for(int i = 5; i > 0; --i)input_buf_channel.Send(i);
// 	int current_id;

// 	current_id = manager.RegisterPipeline(&input_buf_channel, &output_buf_channel, 3, ValueConverter);
	
// 	std::this_thread::sleep_for(std::chrono::milliseconds(50));
// 	input_buf_channel.WaitFree();
// 	input_buf_channel.Close();
// 	manager.ClosePipeline(current_id);

// 	EXPECT_EQ(current_id, 0);
// 	for(double i = 5; i > 0; --i)EXPECT_EQ(i, output_buf_channel.Recv());
// }

// TEST(Correctness, PipelineManager_Test2){
// 	PipelineManager<int, double> manager;
// 	BufferedChannel<int> input_buf_channel(10);
// 	BufferedChannel<double> output_buf_channel(10);

// 	for(int i = 10; i > 0; --i)input_buf_channel.Send(i);
// 	int current_id;

// 	current_id = manager.RegisterPipeline(&input_buf_channel, &output_buf_channel, 5, ValueConverter);
	
// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
// 	input_buf_channel.WaitFree();
// 	input_buf_channel.Close();
// 	manager.ClosePipeline(current_id);

// 	EXPECT_EQ(current_id, 0);
// 	for(double i = 10; i > 0; --i)EXPECT_EQ(i, output_buf_channel.Recv());
// }

// TEST(Correctness, PipelineManager_Test3){
// 	PipelineManager<int, double> manager;
// 	BufferedChannel<int> input_buf_channel(10);
// 	BufferedChannel<double> output_buf_channel(10);

// 	for(int i = 10; i > 0; --i)input_buf_channel.Send(i);
// 	int current_id;

// 	current_id = manager.RegisterPipeline(&input_buf_channel, &output_buf_channel, 20, ValueConverter);
	
// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
// 	input_buf_channel.WaitFree();
// 	input_buf_channel.Close();
// 	manager.ClosePipeline(current_id);

// 	EXPECT_EQ(current_id, 0);
// 	for(double i = 10; i > 0; --i)EXPECT_EQ(i, output_buf_channel.Recv());
// }

// TEST(Correctness, PipelineManager_Test4){
// 	PipelineManager<int, double> manager;
// 	BufferedChannel<int> input_buf_channel(10);
// 	BufferedChannel<double> output_buf_channel(10);

// 	for(int i = 5; i > 0; --i)input_buf_channel.Send(i);
// 	int current_id;

// 	current_id = manager.RegisterPipeline(&input_buf_channel, &output_buf_channel, 1, ValueConverter);
	
// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
// 	input_buf_channel.WaitFree();
// 	input_buf_channel.Close();
// 	manager.ClosePipeline(current_id);

// 	EXPECT_EQ(current_id, 0);
// 	for(double i = 5; i > 0; --i)EXPECT_EQ(i, output_buf_channel.Recv());
// }

// TEST(Correctness, PipelineManager_Test5){
// 	PipelineManager<double, int> manager;
// 	BufferedChannel<double> input_buf_channel(10);
// 	BufferedChannel<int> output_buf_channel(5);

// 	for(int i = 5; i > 0; --i)input_buf_channel.Send(double(i));
// 	int current_id;

// 	current_id = manager.RegisterPipeline(&input_buf_channel, &output_buf_channel, 5, ValueConverter);
	
// 	std::this_thread::sleep_for(std::chrono::milliseconds(100));
// 	input_buf_channel.WaitFree();
// 	input_buf_channel.Close();
// 	manager.ClosePipeline(current_id);

// 	EXPECT_EQ(input_buf_channel.getClosed(), true);
// 	EXPECT_EQ(output_buf_channel.getEmpty(), false);
// 	EXPECT_EQ(input_buf_channel.getEmpty(), true);
// 	EXPECT_EQ(current_id, 0);
// 	for(int i = 5; i > 0; --i)EXPECT_EQ(i, output_buf_channel.Recv());
// }