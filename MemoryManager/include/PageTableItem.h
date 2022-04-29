/*
 * @Date: 2022-04-01 15:07:33
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-28 20:02:27
 * @FilePath: \Operating-System\MemoryManager\include\PageTableItem.h
 */
class PageTableItem
{
private:
    int pageNo;//在进程逻辑空间内的页号
    bool isSwap;
    int accessTime;
    bool isChange;
    int swapAddress;

public:
    PageTableItem *pre, *next;
    PageTableItem();
    ~PageTableItem();
};

PageTableItem::PageTableItem(/* args */)
{
}

PageTableItem::~PageTableItem()
{
}
