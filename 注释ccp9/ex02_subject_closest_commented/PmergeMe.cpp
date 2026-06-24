#include "PmergeMe.hpp"

/*
包含库说明：
cstdlib：使用 std::atoi，把已经通过 main 检查的命令行字符串转成 int。
iomanip：使用 std::fixed 和 std::setprecision 控制耗时输出格式。
iostream：使用 std::cout 输出 Before、After 和两种容器耗时。
sys/time.h：使用 gettimeofday 获取微秒级时间。
*/
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sys/time.h>

/*
结构体用途：表示一个参与 Ford-Johnson 递归排序的元素。
成员说明：value 是真实数字；id 是这个数字的唯一编号，来源于输入位置。
实现逻辑：id 用来绑定 high 和 low 的对应关系；这样重复数字也能区分，不再靠 high 的值回头找 low。
*/
struct SortItem
{
    int value;
    std::size_t id;
};

/*
结构体用途：保存一次两两配对后的 high-low 关系。
成员说明：high 是这一对中较大的 SortItem；low 是这一对中较小的 SortItem。
实现逻辑：递归排序时只把 high 放入主链递归，但 low 通过 high.id 保持绑定，不靠数值比较找回。
*/
struct ItemPair
{
    SortItem high;
    SortItem low;
};

/*
结构体用途：保存一个等待插入主链的元素，以及它对应的 high 上界。
成员说明：item 是待插入元素；highId 是它对应 high 的唯一编号；hasHigh 表示它是否有对应 high。
实现逻辑：普通 low 有对应 high，只需要插入到对应 high 前面的范围；stray 没有 high，需要在整条链中找位置。
*/
struct PendingItem
{
    SortItem item;
    std::size_t highId;
    bool hasHigh;
};

/*
函数用途：创建一个 SortItem。
参数说明：value 是真实数字；id 是唯一编号，通常由输入位置 i + 1 生成。
返回值：返回填好 value 和 id 的 SortItem。
实现逻辑：把普通 int 包装成带身份编号的元素，方便后面处理重复值和 high-low 绑定关系。
*/
static SortItem makeSortItem(int value, std::size_t id)
{
    SortItem item;

    item.value = value;
    item.id = id;
    return item;
}

/*
函数用途：在 vector 主链中找到某个 highId 对应 high 的当前位置。
参数说明：chain 是当前已经有序的主链；highId 是待查找 high 的唯一编号。
返回值：返回 high 在 chain 中的位置；如果没找到，返回 chain.size()。
实现逻辑：这里用 id 找绑定关系，不用 high 的数值做匹配，所以重复数字不会导致找错 low。
*/
static std::size_t findVectorHighLimit(std::vector<SortItem> const &chain, std::size_t highId)
{
    std::size_t i;

    i = 0;
    while (i < chain.size())
    {
        if (chain[i].id == highId)
            return i;
        i++;
    }
    return chain.size();
}

/*
函数用途：把一个 PendingItem 插入到有序 vector 主链中。
参数说明：chain 是当前有序主链；pending 是待插入元素，里面保存 item 和对应 highId。
变量说明：left/right/middle 是二分查找边界和中点。
实现逻辑：如果 pending 有 high，就先用 highId 找到 high 的位置，把它作为右边界；
然后只在这个范围内二分查找 item.value 的插入位置。stray 没有 high，就在整条链中找。
*/
static void insertVectorItem(std::vector<SortItem> &chain, PendingItem const &pending)
{
    std::size_t left;
    std::size_t right;
    std::size_t middle;

    left = 0;
    if (pending.hasHigh == true)
        right = findVectorHighLimit(chain, pending.highId);
    else
        right = chain.size();
    while (left < right)
    {
        middle = left + (right - left) / 2;
        if (chain[middle].value < pending.item.value)
            left = middle + 1;
        else
            right = middle;
    }
    chain.insert(chain.begin() + left, pending.item);
}

/*
函数用途：根据 high 的唯一编号，从 vector 的 pair 表中取回对应的完整 high-low 配对。
参数说明：pairs 是还没取完的临时配对表；highId 是当前主链元素的唯一编号。
返回值：返回找到的 ItemPair。
实现逻辑：找到后立刻 erase，避免重复数字时同一个 pair 被重复使用。这里匹配的是 id，不是 high 的值。
*/
static ItemPair takeVectorPair(std::vector<ItemPair> &pairs, std::size_t highId)
{
    std::size_t i;
    ItemPair pair;

    i = 0;
    while (i < pairs.size())
    {
        if (pairs[i].high.id == highId)
        {
            pair = pairs[i];
            pairs.erase(pairs.begin() + i);
            return pair;
        }
        i++;
    }
    pair.high = makeSortItem(0, 0);
    pair.low = makeSortItem(0, 0);
    return pair;
}

/*
函数用途：按 Jacobsthal 分组顺序，把 vector pending 元素插入主链。
参数说明：chain 是已经排好的 high 主链；pending 是等待插入的 low 和 stray。
变量说明：previous/current/next 控制 Jacobsthal 边界；end 是当前组实际右边界；index 用于组内倒序插入。
实现逻辑：先插 b1，再按 (1,3]、(3,5]、(5,11] 这类分组倒序插入，使搜索范围更贴近 Ford-Johnson 的比较次数设计。
*/
static void insertVectorPending(std::vector<SortItem> &chain, std::vector<PendingItem> const &pending)
{
    std::size_t previous;
    std::size_t current;
    std::size_t next;
    std::size_t end;
    std::size_t index;

    if (pending.empty() == true)
        return;
    insertVectorItem(chain, pending[0]);
    previous = 1;
    current = 3;
    while (previous < pending.size())
    {
        end = current;
        if (end > pending.size())
            end = pending.size();
        index = end;
        while (index > previous)
        {
            insertVectorItem(chain, pending[index - 1]);
            index--;
        }
        next = current + previous * 2;
        previous = current;
        current = next;
    }
}

/*
函数用途：把两个 SortItem 分成 high 和 low。
参数说明：first/second 是当前相邻的两个元素；pair 是输出参数，用来保存结果。
实现逻辑：只比较一次 value，较大的作为 high，较小的作为 low；两个元素的 id 会一起保留。
*/
static void makeVectorPair(SortItem const &first, SortItem const &second, ItemPair &pair)
{
    if (first.value < second.value)
    {
        pair.high = second;
        pair.low = first;
    }
    else
    {
        pair.high = first;
        pair.low = second;
    }
}

/*
函数用途：向 vector pending 表中加入一个待插入元素。
参数说明：pending 是待插入表；item 是要插入的元素；highId 是对应 high 编号；hasHigh 表示是否有上界。
实现逻辑：把 low 或 stray 包装成 PendingItem，统一交给 insertVectorPending 处理。
*/
static void pushVectorPending(std::vector<PendingItem> &pending, SortItem item, std::size_t highId, bool hasHigh)
{
    PendingItem pendingItem;

    pendingItem.item = item;
    pendingItem.highId = highId;
    pendingItem.hasHigh = hasHigh;
    pending.push_back(pendingItem);
}

/*
函数用途：vector 版本 Ford-Johnson / merge-insert 排序核心。
参数说明：values 是带 id 的 SortItem 序列。
返回值：返回按 value 排好序的 SortItem 序列。
实现逻辑：两两配对得到 pairs 和 highs；递归排序 highs；
再根据 high.id 取回对应 low，生成 pending；最后按 Jacobsthal 顺序插回主链。
这一版不再靠 high 的数值回头找 low，而是用唯一 id 保持 pair 绑定，更贴近 subject 对 Ford-Johnson 结构的要求。
*/
static std::vector<SortItem> fordJohnsonVector(std::vector<SortItem> values)
{
    std::vector<ItemPair> pairs;
    std::vector<SortItem> highs;
    std::vector<PendingItem> pending;
    std::vector<SortItem> chain;
    ItemPair pair;
    bool hasStray;
    SortItem stray;
    std::size_t i;

    if (values.size() < 2)
        return values;
    hasStray = false;
    i = 0;
    while (i + 1 < values.size())
    {
        makeVectorPair(values[i], values[i + 1], pair);
        pairs.push_back(pair);
        highs.push_back(pair.high);
        i += 2;
    }
    if (i < values.size())
    {
        hasStray = true;
        stray = values[i];
    }
    chain = fordJohnsonVector(highs);
    i = 0;
    while (i < chain.size())
    {
        pair = takeVectorPair(pairs, chain[i].id);
        pushVectorPending(pending, pair.low, pair.high.id, true);
        i++;
    }
    if (hasStray == true)
        pushVectorPending(pending, stray, 0, false);
    insertVectorPending(chain, pending);
    return chain;
}

/*
函数用途：在 list 主链中找到某个 highId 对应 high 的当前位置。
参数说明：chain 是当前有序 list 主链；highId 是待查找 high 的唯一编号。
返回值：返回指向 high 的迭代器；如果找不到，返回 chain.end()。
实现逻辑：list 不能下标访问，所以只能从 begin 开始逐个节点查找 id。
*/
static std::list<SortItem>::iterator findListHighLimit(std::list<SortItem> &chain, std::size_t highId)
{
    std::list<SortItem>::iterator it;

    it = chain.begin();
    while (it != chain.end())
    {
        if (it->id == highId)
            return it;
        it++;
    }
    return chain.end();
}

/*
函数用途：把一个 PendingItem 插入到有序 list 主链中。
参数说明：chain 是当前有序主链；pending 是待插入元素。
实现逻辑：如果 pending 有 high，就先找到 highId 对应位置作为 limit；然后只在 begin 到 limit 范围中线性查找插入点。
*/
static void insertListItem(std::list<SortItem> &chain, PendingItem const &pending)
{
    std::list<SortItem>::iterator limit;
    std::list<SortItem>::iterator it;

    if (pending.hasHigh == true)
        limit = findListHighLimit(chain, pending.highId);
    else
        limit = chain.end();
    it = chain.begin();
    while (it != limit && it->value < pending.item.value)
        it++;
    chain.insert(it, pending.item);
}

/*
函数用途：按位置从 list pending 表中取出一个 PendingItem。
参数说明：pending 是待插入元素列表；index 是目标位置。
返回值：返回第 index 个 PendingItem。
实现逻辑：因为 std::list 没有 operator[]，所以从 begin 开始移动 index 次。
*/
static PendingItem getListPending(std::list<PendingItem> const &pending, std::size_t index)
{
    std::list<PendingItem>::const_iterator it;
    std::size_t i;

    it = pending.begin();
    i = 0;
    while (i < index && it != pending.end())
    {
        it++;
        i++;
    }
    return *it;
}

/*
函数用途：根据 high 的唯一编号，从 list 的 pair 表中取回对应的完整 high-low 配对。
参数说明：pairs 是还没取完的临时配对表；highId 是当前主链元素的唯一编号。
返回值：返回找到的 ItemPair。
实现逻辑：使用 id 匹配并 erase 已用 pair，避免重复值导致 low 绑定错误。
*/
static ItemPair takeListPair(std::list<ItemPair> &pairs, std::size_t highId)
{
    std::list<ItemPair>::iterator it;
    ItemPair pair;

    it = pairs.begin();
    while (it != pairs.end())
    {
        if (it->high.id == highId)
        {
            pair = *it;
            pairs.erase(it);
            return pair;
        }
        it++;
    }
    pair.high = makeSortItem(0, 0);
    pair.low = makeSortItem(0, 0);
    return pair;
}

/*
函数用途：按 Jacobsthal 分组顺序，把 list pending 元素插入主链。
参数说明：chain 是已经排好的 high 主链；pending 是等待插入的 low 和 stray。
实现逻辑：逻辑和 vector 版本相同；区别是 list 不能下标访问，所以每次通过 getListPending 取目标 pending。
*/
static void insertListPending(std::list<SortItem> &chain, std::list<PendingItem> const &pending)
{
    std::size_t previous;
    std::size_t current;
    std::size_t next;
    std::size_t end;
    std::size_t index;

    if (pending.empty() == true)
        return;
    insertListItem(chain, getListPending(pending, 0));
    previous = 1;
    current = 3;
    while (previous < pending.size())
    {
        end = current;
        if (end > pending.size())
            end = pending.size();
        index = end;
        while (index > previous)
        {
            insertListItem(chain, getListPending(pending, index - 1));
            index--;
        }
        next = current + previous * 2;
        previous = current;
        current = next;
    }
}

/*
函数用途：把两个 list SortItem 分成 high 和 low。
参数说明：first/second 是当前两个元素；pair 是输出参数。
实现逻辑：比较 value，但移动和保存的是完整 SortItem，因此 id 会跟随元素一起保留。
*/
static void makeListPair(SortItem const &first, SortItem const &second, ItemPair &pair)
{
    if (first.value < second.value)
    {
        pair.high = second;
        pair.low = first;
    }
    else
    {
        pair.high = first;
        pair.low = second;
    }
}

/*
函数用途：向 list pending 表中加入一个待插入元素。
参数说明：pending 是待插入表；item 是要插入的元素；highId 是对应 high 编号；hasHigh 表示是否有上界。
实现逻辑：普通 low 保存对应 highId；stray 标记 hasHigh 为 false。
*/
static void pushListPending(std::list<PendingItem> &pending, SortItem item, std::size_t highId, bool hasHigh)
{
    PendingItem pendingItem;

    pendingItem.item = item;
    pendingItem.highId = highId;
    pendingItem.hasHigh = hasHigh;
    pending.push_back(pendingItem);
}

/*
函数用途：list 版本 Ford-Johnson / merge-insert 排序核心。
参数说明：values 是带 id 的 SortItem 链表。
返回值：返回按 value 排好序的 SortItem 链表。
实现逻辑：用迭代器两两配对；递归排序 highs；根据 high.id 取回 low；
把 low 和 stray 作为 pending，最后按 Jacobsthal 顺序插入主链。
*/
static std::list<SortItem> fordJohnsonList(std::list<SortItem> values)
{
    std::list<ItemPair> pairs;
    std::list<SortItem> highs;
    std::list<PendingItem> pending;
    std::list<SortItem> chain;
    std::list<SortItem>::iterator it;
    ItemPair pair;
    bool hasStray;
    SortItem first;
    SortItem second;
    SortItem stray;

    if (values.size() < 2)
        return values;
    hasStray = false;
    it = values.begin();
    while (it != values.end())
    {
        first = *it;
        it++;
        if (it == values.end())
        {
            hasStray = true;
            stray = first;
        }
        else
        {
            second = *it;
            it++;
            makeListPair(first, second, pair);
            pairs.push_back(pair);
            highs.push_back(pair.high);
        }
    }
    chain = fordJohnsonList(highs);
    it = chain.begin();
    while (it != chain.end())
    {
        pair = takeListPair(pairs, it->id);
        pushListPending(pending, pair.low, pair.high.id, true);
        it++;
    }
    if (hasStray == true)
        pushListPending(pending, stray, 0, false);
    insertListPending(chain, pending);
    return chain;
}

/*
函数用途：获取当前时间，单位是微秒。
参数说明：无参数。
返回值：返回当前时间对应的微秒数。
实现逻辑：gettimeofday 得到秒和微秒，再把秒转换为微秒后相加。
*/
static double getTimeInMicroseconds(void)
{
    struct timeval time;

    gettimeofday(&time, NULL);
    return static_cast<double>(time.tv_sec) * 1000000.0 + static_cast<double>(time.tv_usec);
}

/*
函数用途：输出排序前的原始命令行参数。
参数说明：values 来源于 argv + 1；count 来源于 argc - 1。
实现逻辑：直接输出字符串参数，保持用户输入顺序，用于 Before 行。
*/
static void printValues(char **values, int count)
{
    int i;

    i = 0;
    while (i < count)
    {
        std::cout << values[i];
        if (i + 1 < count)
            std::cout << " ";
        i++;
    }
}

/*
函数用途：输出排序后的 vector 数字序列。
参数说明：values 是排序后的 _vector。
实现逻辑：按顺序输出每个数字，数字之间用一个空格分隔。
*/
static void printVector(std::vector<int> const &values)
{
    std::size_t i;

    i = 0;
    while (i < values.size())
    {
        std::cout << values[i];
        if (i + 1 < values.size())
            std::cout << " ";
        i++;
    }
}

/*
函数用途：默认构造函数，初始化耗时成员。
实现逻辑：两个容器会自动构造成空容器，这里只把时间设置为 0。
*/
PmergeMe::PmergeMe(void)
{
    _vectorTime = 0;
    _listTime = 0;
}

/*
函数用途：拷贝构造函数，用 other 初始化当前对象。
参数说明：other 是已有 PmergeMe 对象。
实现逻辑：复制两个容器和两个耗时成员。
*/
PmergeMe::PmergeMe(PmergeMe const &other)
{
    _vector = other._vector;
    _list = other._list;
    _vectorTime = other._vectorTime;
    _listTime = other._listTime;
}

/*
函数用途：赋值运算符，把 other 的状态复制给当前对象。
参数说明：other 是等号右侧对象。
返回值：返回当前对象引用。
实现逻辑：先避免自赋值，再复制两个容器和两个耗时成员。
*/
PmergeMe &PmergeMe::operator=(PmergeMe const &other)
{
    if (this != &other)
    {
        _vector = other._vector;
        _list = other._list;
        _vectorTime = other._vectorTime;
        _listTime = other._listTime;
    }
    return *this;
}

/*
函数用途：析构函数。
实现逻辑：类中没有手动 new 出来的资源，vector 和 list 会自动释放，所以函数体为空。
*/
PmergeMe::~PmergeMe(void)
{
}

/*
函数用途：vector 版本装载、排序和计时入口。
参数说明：values 是命令行数字字符串数组；count 是数字数量。
实现逻辑：开始计时后，把每个数字包装成带 id 的 SortItem；调用 fordJohnsonVector；
最后把排序后的 SortItem.value 写回 _vector，并记录总耗时。
*/
void PmergeMe::sortVector(char **values, int count)
{
    std::vector<SortItem> items;
    double start;
    int i;

    start = getTimeInMicroseconds();
    _vector.clear();
    i = 0;
    while (i < count)
    {
        items.push_back(makeSortItem(std::atoi(values[i]), static_cast<std::size_t>(i + 1)));
        i++;
    }
    items = fordJohnsonVector(items);
    i = 0;
    while (i < static_cast<int>(items.size()))
    {
        _vector.push_back(items[static_cast<std::size_t>(i)].value);
        i++;
    }
    _vectorTime = getTimeInMicroseconds() - start;
}

/*
函数用途：list 版本装载、排序和计时入口。
参数说明：values 是命令行数字字符串数组；count 是数字数量。
实现逻辑：开始计时后，把每个数字包装成带 id 的 SortItem；调用 fordJohnsonList；
最后把排序后的 SortItem.value 写回 _list，并记录总耗时。
*/
void PmergeMe::sortList(char **values, int count)
{
    std::list<SortItem> items;
    std::list<SortItem>::iterator it;
    double start;
    int i;

    start = getTimeInMicroseconds();
    _list.clear();
    i = 0;
    while (i < count)
    {
        items.push_back(makeSortItem(std::atoi(values[i]), static_cast<std::size_t>(i + 1)));
        i++;
    }
    items = fordJohnsonList(items);
    it = items.begin();
    while (it != items.end())
    {
        _list.push_back(it->value);
        it++;
    }
    _listTime = getTimeInMicroseconds() - start;
}

/*
函数用途：PmergeMe 对外公开的总入口。
参数说明：values 是 argv + 1；count 是 argc - 1。
实现逻辑：输出 Before；分别执行 vector 和 list 的装载、排序、计时；
用 vector 结果输出 After；最后输出两个容器的耗时。
*/
void PmergeMe::sort(char **values, int count)
{
    std::cout << "Before: ";
    printValues(values, count);
    std::cout << std::endl;
    sortVector(values, count);
    sortList(values, count);
    std::cout << "After:  ";
    printVector(_vector);
    std::cout << std::endl;
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Time to process a range of " << count << " elements with std::vector<int>: " << _vectorTime << " us" << std::endl;
    std::cout << "Time to process a range of " << count << " elements with std::list<int>: " << _listTime << " us" << std::endl;
}
