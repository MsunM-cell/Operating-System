#pragma once
#include "mem.h"

#define head_addr first
#define len second

//鐎瑰綊鍣洪柅鎺戭杻閹烘帒绨弬瑙勭《
bool cmp1(PII x, PII y) { return x.second == y.second ? x.first < y.first : x.second < y.second; }
//閸︽澘娼冮柅鎺戭杻閹烘帒绨弬瑙勭《
bool cmp2(PII x, PII y) { return x.first < y.first; }

//閸斻劍鈧礁鍨庨崠鍝勫瀻闁板秶顓搁敓锟�?
class BlockMemoryManager : public MemoryManager
{
private:
    //缁屾椽妫介崚鍡楀隘闁炬崘銆�
    vector<PII> free_block_table;
    //鏉╂稓鈻奸崪宀勵浕閸︽澘娼冪€电懓绨查敓锟�?
    map<int, PII> pid2addr;
    map<int, int> addr2pid;
    //鏉╂稓鈻奸幐鍥︽姢闂€鍨
    map<int,int> ins_sum_len;
    //鐠囪褰囬柊宥囩枂閺傚洣娆㈤敍灞兼叏閺€鐟板瀻闁板秶鐡ラ敓锟�?
    void modify_tactic(int new_mode) { mem_config.BLOCK_ALGORITHM = new_mode; }
    //閸掓繂顫愰崠鏍嚉閸愬懎鐡ㄧ粻锛勬倞缁崵绮�
    void init_manager();

public:
    //閺嬪嫰鈧姴鍤遍敓锟�?
    BlockMemoryManager() { init_manager(); }
    //閺嬫劖鐎崙鑺ユ殶
    ~BlockMemoryManager() {}
    //缁屾椽妫介崚鍡楀隘闁炬崘銆冮幒鎺戠碍,type=0娑撳搫婀撮崸鈧柅鎺戭杻閹烘帒绨敍瀹紋pe=1娑撳搫顔愰柌蹇涒偓鎺戭杻閹烘帒绨�
    void adjust_list(int type);
    //閹兼粎鍌ㄥ⿰陇鍐婚弶鈥叉閻ㄥ嫮鈹栭梻鎻掓健閿涘苯鑻熸潻鏂挎礀缁屾椽妫介崸妤咁浕閸︽澘娼�
    int createProcess(PCB& p);
    //闁插﹥鏂侀崘鍛摠閿涘奔绱堕崗銉ュ棘閺佺殐cb
    int freeProcess(PCB& p);
    //鐠佸潡妫堕崘鍛摠閹恒儱褰�
    char accessMemory(int pid, int address);
    //閸愭瑥鍞寸€涙ɑ甯撮敓锟�?
    int writeMemory(int logicalAddress, char src, unsigned int pid);
    //閸樺缂�
    int compress_mem();
    //閸旂姾娴囬幐鍥︽姢
    int load_ins(int addr, int length,string path);
    //閹垫挸宓冪粚娲＝閸掑棗灏敓锟�?
    void print_list();

    string getMode(){return "block";}
    void dms_command();
};
