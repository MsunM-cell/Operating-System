/*
 * @Date: 2022-04-01 15:07:33
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-01 15:28:02
 * @FilePath: \Operating-System\MemoryManager\Manager\pageTableItem.cpp
 */
class PageTableItem
{
private:
    int pageNo;
    bool isSwap;
    int accessTime;
    bool isChange;
    int swapAddress;

public:
    PageTableItem *pre, *next;
    PageTableItem(/* args */);
    ~PageTableItem();
};

PageTableItem::PageTableItem(/* args */)
{
}

PageTableItem::~PageTableItem()
{
}
