#include <vector>
using namespace std;

class AgentMemory {
	public:
		AgentMemory(int id);
		int get_id(void);
	private:
		int a_id;
		vector<char> pri_mem;
		vector<char> pub_mem;
};

