#include <gtest/gtest.h>
#include "buffered_channel.h"

//template<typename T1, typename T2>
//class PipelineManager{
//	std::vector<std::pair<BufferedChannel<T1>, BufferedChannel<T2>>> m_pipeline_vector;
//public:
//	void RegisterPipeline(BufferedChannel<T1>& input, BufferedChannel<T2>& output){
//		m_pipeline_vector.push(input, output);
//	}
//};

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();

	return ret;
}