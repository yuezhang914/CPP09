#include "PmergeMe.hpp"

/*
包含库说明：
cstdlib：使用 std::atoi 把命令行字符串转换成 int。
iomanip：使用 std::fixed 和 std::setprecision 控制耗时输出精度。
iostream：使用 std::cout 输出 Before、After 和两种容器的运行时间。
sys/time.h：使用 gettimeofday 和 timeval 获取微秒级时间。
*/
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sys/time.h>

/*
结构体用途：保存 Ford-Johnson 配对后的两个数字，其中 high 是较大值，low 是较小值。
成员变量：high 和 low 都来自输入序列中相邻两个数字比较后的结果。
实现逻辑：排序时先递归排序所有 high，再按排序后的 high 顺序取回对应 low 作为 pending 序列。
*/
struct IntPair
{
    int high;
    int low;
};

/*
结构体用途：保存一个待插入主链的数字，以及它原来对应的 high 上界。
成员变量：value 是待插入的 low 或落单元素；high 是 value 对应的较大值上界；
hasHigh 表示这个元素是否有对应 high，落单元素没有对应 high。
实现逻辑：严格版 Ford-Johnson 插入 low 时，不在整条主链里查找，而是只查找到它对应的 high 之前；
因此 pending 不能只保存 value，还要保存对应 high。
*/
struct PendingValue
{
    int value;
    int high;
    bool hasHigh;
};

/*
函数用途：在有序 vector 主链中找到第一个不小于 high 的位置，作为插入 low 时的右边界。
参数说明：chain 是当前已经有序的主链；high 是待插入 low 原来对应的较大值。
变量说明：left 和 right 是二分查找的左右边界；middle 是当前检查的中间位置。
实现逻辑：用二分查找找到第一个 chain[middle] >= high 的位置；
严格版插入时只允许在 begin 到这个位置之前查找，因为 low 不可能排在对应 high 后面。
*/
static std::size_t findVectorLimit(std::vector<int> const &chain, int high)
{
    std::size_t left;
    std::size_t right;
    std::size_t middle;

    left = 0;
    right = chain.size();
    while (left < right)
    {
        middle = left + (right - left) / 2;
        if (chain[middle] < high)
            left = middle + 1;
        else
            right = middle;
    }
    return left;
}

/*
函数用途：把一个 pending 数字按有序位置插入 vector 主链，并在有对应 high 时限制查找范围。
参数说明：chain 是当前已经有序的 vector 引用；item 是待插入信息，包含 value、对应 high 以及是否有 high。
变量说明：left 和 right 是二分查找边界；middle 是当前中间位置。
实现逻辑：如果 item 有对应 high，就先把 right 限制到 high 之前；
如果是落单元素，就在整条 chain 中查找。
然后在限定范围内用二分查找找到插入位置并插入。
*/
static void insertVectorValue(std::vector<int> &chain, PendingValue const &item)
{
    std::size_t left;
    std::size_t right;
    std::size_t middle;

    left = 0;
    if (item.hasHigh == true)
        right = findVectorLimit(chain, item.high);
    else
        right = chain.size();
    while (left < right) //循环的目标是找到：第一个 >= item.value 的位置
    {
        middle = left + (right - left) / 2; //当前搜索范围的中间位置。不用middle = (left + right) / 2 是为了避免 left + right 太大时溢出。
        if (chain[middle] < item.value)
            left = middle + 1; //中间这个数字比要插入的值小。那么 item.value 不应该插在 middle 或它左边，而应该去右边找。
        else //不是小于，也就是：chain[middle] >= item.value 说明 item.value 应该插在 middle 这个位置，或者更左边。
            right = middle; //这里不写 middle - 1，因为 middle 本身还有可能就是正确插入位置。
    }//这段 while 结束时，left 就是第一个 >= item.value 的位置。此时left == right 
    // left 左边的元素都 < item.value
    // left 位置及右边的元素都 >= item.value
    // 所以把 item.value 插到 left 这个位置，整个 chain 还是有序的。
    chain.insert(chain.begin() + left, item.value); //在 chain 的第 left 个位置插入 item.value。
    // 比如 left = 2，意思就是插到第 2 个下标的位置，也就是原来的 chain[2] 前面。
// 因为 vector::insert() 要的不是下标数字，而是 iterator 位置，
// 所以不能直接给 left，要用 chain.begin() + left 把“下标”转换成“位置”。
}

/*
函数用途：按 Ford-Johnson 的 Jacobsthal 分组顺序，把 pending 中的较小值插入有序 vector 主链。
参数说明：chain 是已经按 high 排好序的主链；
pending 是待插入序列，每个元素保存 value 和对应 high，最后可能包含没有 high 的落单元素。
变量说明：这 5 个变量都是在控制 pending 的分组边界和组内倒序插入位置。
重点是：这段代码用的是左开右闭的思想：(previous, end]
previous 和 current 表示当前 Jacobsthal 分组边界；next 保存下一组边界；
end 是当前组实际结束位置；index 用来倒序插入当前组。
实现逻辑：这段代码用的不是完整 Jacobsthal 数列 0, 1, 1, 3, 5, 11...，而是用它整理出来的插入边界1, 3, 5, 11, 21...
先插入 pending[0]，之后按 1、3、5、11 这类边界分组，并在每组内从后往前插入；
每次真正插入时由 insertVectorValue 限制到对应 high 前面。
*/
static void insertVectorPending(std::vector<int> &chain, std::vector<PendingValue> const &pending)
{

    std::size_t previous; // 上一组已经处理到的边界；左边界，不包括它本身，例如 previous = 1 表示 b1 已经插过，下一组从 b2 开始。
    std::size_t current;  // 当前 Jacobsthal 分组想处理到的边界；右边界，理论上包括它本身，例如 current = 3 表示这一组处理到 b3。
    std::size_t next;     // 下一组的 Jacobsthal 边界；由 current + previous * 2 推出来，用来更新下一轮 current。
    std::size_t end;      // 当前组实际能处理到的右边界；等于 min(current, pending.size())，防止 current 超过 pending 数量。
    std::size_t index;    // 当前组内正在插入的 pending 编号；从 end 倒着走到 previous + 1。

    if (pending.empty() == true)
        return; // 如果没有待插入元素，直接结束。

    insertVectorValue(chain, pending[0]); // 先插入第一个 pending，也就是 b1；因为 vector 下标从 0 开始，所以 pending[0] 对应 b1。

    previous = 1; // 已经处理到 b1；它是下一组的左边界，不包括 b1。
    current = 3;  // 第一组 Jacobsthal 右边界是 b3；所以第一组范围是 (1, 3]，也就是 b2 到 b3。

    while (previous < pending.size()) // 只要上一组边界还小于 pending 总数量，说明后面还有 pending 没插完。
    {
        end = current; // 先假设这一组可以处理到 current，比如 current = 3 就处理到 b3。
        if (end > pending.size())
            end = pending.size(); // 如果 current 超过 pending 实际数量，就把右边界改成 pending.size()，防止越界。

        index = end;             // 从当前组实际右边界开始倒着插，比如 end = 3 就先插 b3。
        while (index > previous) // 范围是 (previous, end]：不包括 previous，包括 end。
        {
            insertVectorValue(chain, pending[index - 1]); // 插入 b_index 对应的 PendingValue；里面保存 value 和对应 high，用于限制查找范围。
            index--;                                      // 组内倒序移动：b3 -> b2，或者 b5 -> b4。
        }

        next = current + previous * 2; // 计算下一个 Jacobsthal 边界：1,3 -> 5；3,5 -> 11。
        previous = current;            // 当前右边界变成下一轮左边界；表示这一组已经处理完。
        current = next;                // 下一轮使用新的右边界。
    }
}

/*
函数用途：根据已经排好序的 high，找到并取出它原来对应的完整 IntPair。
参数说明：pairs 是还没被取完的临时配对表引用；high 是当前主链中的较大值。
变量说明：i 是遍历 pairs 的下标；pair 保存找到的 high-low 配对。
实现逻辑：从 pairs 中找到第一个 high 相等的配对，
取出整个 pair 后删除这个临时配对记录，避免重复 high 时重复使用同一个 low。
*/
static IntPair takeVectorPair(std::vector<IntPair> &pairs, int high)
{
    std::size_t i;
    IntPair pair;

    i = 0;
    while (i < pairs.size())
    {
        if (pairs[i].high == high)
        {
            pair = pairs[i];
            pairs.erase(pairs.begin() + i);
            return pair;
        }
        i++;
    }
    pair.high = high;
    pair.low = high;
    return pair;
}

/*
函数用途：使用 vector 版本的严格 Ford-Johnson / merge-insert 思路排序。
参数说明：values 是传入的 vector 副本，来源于 sortVector 中装载好的 _vector。
变量说明：pairs 保存每两个数字形成的 high-low 对；
highs 保存所有较大值；
pending 保存待插入的 low 以及其对应 high；
chain 保存递归排序后的主链；
pair 是当前配对或取回的临时变量；
item 是待插入信息；
hasStray 记录是否有落单数字；
stray 保存落单数字；
i 是遍历下标。
实现逻辑：先两两配对，把较大值递归排序成主链；再按主链顺序取回完整 pair，生成带 high 上界的 pending；
落单元素没有 high 上界；最后按 Jacobsthal 顺序把 pending 插回主链并返回。
*/
static std::vector<int> fordJohnsonVector(std::vector<int> values)
{
    std::vector<IntPair> pairs;
    std::vector<int> highs;
    std::vector<PendingValue> pending;
    std::vector<int> chain;
    IntPair pair;
    PendingValue item;
    bool hasStray;
    int stray;
    std::size_t i;

    if (values.size() < 2)
        return values;
    hasStray = false;
    i = 0;
    while (i + 1 < values.size())
    {
        if (values[i] < values[i + 1])
        {
            pair.high = values[i + 1];
            pair.low = values[i];
        }
        else
        {
            pair.high = values[i];
            pair.low = values[i + 1];
        }
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
        pair = takeVectorPair(pairs, chain[i]);
        item.value = pair.low;
        item.high = pair.high;
        item.hasHigh = true;
        pending.push_back(item);
        i++;
    }
    if (hasStray == true)
    {
        item.value = stray;
        item.high = 0;
        item.hasHigh = false;
        pending.push_back(item);
    }
    insertVectorPending(chain, pending);
    return chain;
}

/*
函数用途：把一个 pending 数字按有序位置插入 list 主链，并在有对应 high 时限制查找范围。
参数说明：chain 是当前已经有序的 list 引用；item 是待插入信息，包含 value、对应 high 以及是否有 high。
变量说明：limit 是查找右边界，表示对应 high 的位置；it 是从头寻找插入位置的迭代器。
实现逻辑：如果 item 有对应 high，就先找到第一个不小于 high 的节点作为停止位置；随后只在 begin 到 limit 之间寻找 value 的插入位置。落单元素没有 high 上界，则查找到 end。
*/
static void insertListValue(std::list<int> &chain, PendingValue const &item) // 把 item.value 插入到有序 list 主链 chain 中；如果 item 有对应 high，就只查找到 high 前面。
{
    std::list<int>::iterator limit; // 插入查找的右边界；如果 item 有对应 high，limit 指向第一个不小于 item.high 的位置。
    std::list<int>::iterator it; // 实际寻找 item.value 插入位置的迭代器。

    limit = chain.end(); // 默认右边界是整条链的结尾；用于没有对应 high 的落单元素。
    if (item.hasHigh == true) // 如果这个 pending 元素是某个 pair 里的 low，说明它有对应 high。
    {
        limit = chain.begin(); // 从主链开头开始寻找对应 high 的位置。
        while (limit != chain.end() && *limit < item.high) // 一直向后走，直到找到第一个不小于 item.high 的位置。
            limit++; // 移动到下一个节点。
    }

    it = chain.begin(); // 从主链开头开始寻找 item.value 的插入位置。
    while (it != limit && *it < item.value) // 只在 [begin, limit) 范围内找；找到第一个不小于 item.value 的位置就停止。
        it++; // 当前元素仍然小于 item.value，继续往后走。

    chain.insert(it, item.value); // 在 it 指向的位置前插入 item.value；如果 it == limit，就插在限定范围末尾。
}

/*
函数用途：按下标读取 list 中的一个 PendingValue。
参数说明：values 是待读取的 pending list；index 是调用者传入的目标位置，来源于 Jacobsthal 分组插入顺序。
变量说明：it 是 list 迭代器；i 是当前位置计数器。
实现逻辑：从 begin 开始向后移动 index 次，然后返回当前位置保存的 PendingValue。
*/
static PendingValue getListPending(std::list<PendingValue> const &values, std::size_t index)
{
    std::list<PendingValue>::const_iterator it;
    std::size_t i;

    it = values.begin();
    i = 0;
    while (i < index && it != values.end())
    {
        it++;
        i++;
    }
    return *it;
}

/*
函数用途：按 Ford-Johnson 的 Jacobsthal 分组顺序，把 pending 中的较小值插入有序 list 主链。
参数说明：chain 是已经按 high 排好序的主链；pending 是待插入序列，每个元素保存 value 和对应 high，最后可能包含没有 high 的落单元素。
变量说明：previous 和 current 表示当前 Jacobsthal 分组边界；next 保存下一组边界；end 是当前组实际结束位置；index 用来倒序插入当前组。
实现逻辑：因为 list 没有随机访问，使用 getListPending 按位置取 pending 元素，再调用 insertListValue 在对应 high 前面的限定范围内插入。
*/
static void insertListPending(std::list<int> &chain, std::list<PendingValue> const &pending) // 把 pending 里的待插入元素按 Jacobsthal 分组顺序插入 list 主链 chain。
{
    std::size_t previous; // 上一组已经处理到的边界；左边界，不包括它本身，例如 previous = 1 表示 b1 已经插过。
    std::size_t current; // 当前 Jacobsthal 分组想处理到的边界；右边界，理论上包括它本身，例如 current = 3 表示处理到 b3。
    std::size_t next; // 下一组 Jacobsthal 边界；由 current + previous * 2 计算出来。
    std::size_t end; // 当前组实际能处理到的右边界；等于 min(current, pending.size())，防止超过 pending 数量。
    std::size_t index; // 当前组内正在插入的 pending 编号；从 end 倒着走到 previous + 1。

    if (pending.empty() == true) // 如果没有任何待插入元素，直接结束。
        return; // pending 为空时没有东西可插入。

    insertListValue(chain, getListPending(pending, 0)); // 先插入第一个 pending，也就是 b1；list 不能用下标，所以用 getListPending 取第 0 个元素。

    previous = 1; // b1 已经插入完成，所以上一组边界设为 1；下一组不再包含 b1。
    current = 3; // 第一组 Jacobsthal 右边界设为 3；下一组范围是 (1, 3]，也就是 b2 到 b3。

    while (previous < pending.size()) // 只要上一组边界还小于 pending 总数量，说明还有待插入元素。
    {
        end = current; // 先假设当前组可以处理到 current 这个边界。
        if (end > pending.size()) // 如果 current 超过 pending 的实际数量。
            end = pending.size(); // 把实际右边界改成 pending.size()，防止访问不存在的元素。

        index = end; // 从当前组的实际右边界开始倒序插入。
        while (index > previous) // 当前组范围是 (previous, end]：不包括 previous，包括 end。
        {
            insertListValue(chain, getListPending(pending, index - 1)); // 插入 b_index；因为 pending 下标从 0 开始，所以 b_index 对应 index - 1。
            index--; // 组内倒序移动，例如 b3 -> b2，或者 b5 -> b4。
        }

        next = current + previous * 2; // 计算下一组 Jacobsthal 边界，例如 1,3 推出 5；3,5 推出 11。
        previous = current; // 当前右边界变成下一轮左边界，表示这一组已经处理完。
        current = next; // 下一轮使用新计算出的右边界。
    }
}

/*
函数用途：根据已经排好序的 high，找到并取出它原来对应的完整 IntPair。
参数说明：pairs 是还没被取完的临时配对 list 引用；high 是当前主链中的较大值。
变量说明：it 是遍历 pairs 的迭代器；pair 保存找到的 high-low 配对。
实现逻辑：从 pairs 中找到第一个 high 相等的配对，取出整个 pair 后删除这个临时配对记录，避免重复 high 时重复使用同一个 low。
*/
static IntPair takeListPair(std::list<IntPair> &pairs, int high)
{
    std::list<IntPair>::iterator it;
    IntPair pair;

    it = pairs.begin();
    while (it != pairs.end())
    {
        if (it->high == high)
        {
            pair = *it;
            pairs.erase(it);
            return pair;
        }
        it++;
    }
    pair.high = high;
    pair.low = high;
    return pair;
}

/*
函数用途：使用 list 版本的严格 Ford-Johnson / merge-insert 思路排序。
参数说明：values 是传入的 list 副本，来源于 sortList 中装载好的 _list。
变量说明：pairs 保存每两个数字形成的 high-low 对；highs 保存所有较大值；
pending 保存待插入的 low 以及其对应 high；chain 保存递归排序后的主链；
it 是遍历 list 的迭代器；pair 是当前配对或取回的临时变量；item 是待插入信息；
hasStray 记录是否有落单数字；first 和 second 是当前配对的两个数字；stray 保存落单数字。
实现逻辑：先用迭代器两两配对，把较大值递归排序成主链；
再按主链顺序取回完整 pair，生成带 high 上界的 pending；
落单元素没有 high 上界；最后按 Jacobsthal 顺序把 pending 插回主链并返回。
*/
static std::list<int> fordJohnsonList(std::list<int> values) // list 版本 Ford-Johnson 排序函数；values 是传入的 list 副本，函数会返回排序后的新 list。
{
    std::list<IntPair> pairs; // 临时配对表；每个元素保存一组 {high, low}，用于之后根据排好序的 high 找回对应 low。
    std::list<int> highs; // 保存每一对里的较大值 high；后面会递归排序 highs，形成主链。
    std::list<PendingValue> pending; // 保存等待插入主链的元素；每个元素包含 value、对应 high、是否有 high。
    std::list<int> chain; // 主链；递归排序 highs 后得到的有序链，后面会把 pending 插入进来。
    std::list<int>::iterator it; // list 迭代器；因为 list 不能用下标访问，所以要用 it 一个节点一个节点走。
    IntPair pair; // 当前正在处理的一组 high-low 配对；也用于接收 takeListPair 找回来的配对。
    PendingValue item; // 当前准备放入 pending 的待插入元素。
    bool hasStray; // 标记是否有落单元素；奇数个元素时最后一个没有配对，就是 stray。
    int first; // 当前配对中的第一个数字。
    int second; // 当前配对中的第二个数字。
    int stray; // 保存落单元素；只有 hasStray 为 true 时才有意义。

    if (values.size() < 2) // 递归停止条件：0 个或 1 个元素已经天然有序。
        return values; // 直接返回当前 values，不再继续拆分。

    hasStray = false; // 先假设没有落单元素；后面如果发现最后只剩一个数，再改成 true。
    it = values.begin(); // 从 list 的第一个元素开始遍历。

    while (it != values.end()) // 只要还没走到 list 末尾，就继续取元素配对。
    {
        first = *it; // 取当前节点的值作为这一对的第一个数字。
        it++; // 迭代器向后走一步，准备取第二个数字。

        if (it == values.end()) // 如果取完 first 后已经到末尾，说明没有第二个数字可以配对。
        {
            hasStray = true; // 标记存在落单元素。
            stray = first; // 当前这个 first 就是落单元素。
        }
        else // 如果还有第二个数字，就正常组成一对。
        {
            second = *it; // 取当前节点的值作为这一对的第二个数字。
            it++; // 迭代器再向后走一步，准备进入下一对。

            if (first < second) // 比较这一对里的两个数字，分出较大值和较小值。
            {
                pair.high = second; // second 更大，作为 high。
                pair.low = first; // first 更小，作为 low。
            }
            else // first >= second 时，first 作为 high。
            {
                pair.high = first; // first 更大或相等，作为 high。
                pair.low = second; // second 更小或相等，作为 low。
            }

            pairs.push_back(pair); // 把这一对 {high, low} 保存到临时配对表，之后要靠 high 找回 low。
            highs.push_back(pair.high); // 只把 high 放入 highs，后面递归排序这些 high。
        }
    }

    chain = fordJohnsonList(highs); // 递归排序所有 high；返回的 chain 是已经有序的 high 主链。

    it = chain.begin(); // 从排好序的 high 主链开头开始遍历。
    while (it != chain.end()) // 按照排序后的 high 顺序，逐个找回它原来对应的 low。
    {
        pair = takeListPair(pairs, *it); // 在 pairs 中找到当前 high 对应的完整 {high, low}，并删除这条已用记录。
        item.value = pair.low; // 待插入的值是这组里的 low。
        item.high = pair.high; // 记录这个 low 对应的 high；严格版插入时要限制到这个 high 前面。
        item.hasHigh = true; // 这个 item 是由 high-low pair 来的，所以有对应 high。
        pending.push_back(item); // 把这个 low 作为待插入元素放进 pending。
        it++; // 继续处理 chain 中下一个 high。
    }

    if (hasStray == true) // 如果原始 values 有落单元素。
    {
        item.value = stray; // 落单元素本身也需要插入主链。
        item.high = 0; // 落单元素没有对应 high；这里的 high 值不会用于限制范围。
        item.hasHigh = false; // 标记它没有对应 high，所以插入时要在整条 chain 中找位置。
        pending.push_back(item); // 把落单元素加入 pending，等待插入。
    }

    insertListPending(chain, pending); // 按 Jacobsthal 分组顺序，把 pending 中的 low 和 stray 插入主链。
    return chain; // 返回完整排序后的 list。
}

/*
函数用途：取得当前时间，单位为微秒。
参数说明：本函数没有参数。
变量说明：time 是 gettimeofday 填充的 timeval 结构体，包含秒和微秒。
实现逻辑：调用 gettimeofday 获取当前时间，再把秒转换成微秒并加上 tv_usec。
*/
static double getTimeInMicroseconds(void)
{
    struct timeval time;

    gettimeofday(&time, NULL);
    return static_cast<double>(time.tv_sec) * 1000000.0 + static_cast<double>(time.tv_usec);
}

/*
函数用途：输出排序前的原始命令行数字序列。
参数说明：values 是 main 传给 sort 的字符串数组，来源于 argv + 1；count 是命令行数字参数的数量，来源于 argc - 1；本程序按题目示例要求每个数字作为一个独立参数传入。
变量说明：i 是遍历 values 的下标。
实现逻辑：按原顺序输出每个字符串数字，数字之间用一个空格分隔。
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
参数说明：values 是排序后的 vector 引用，来源于 PmergeMe::_vector。
变量说明：i 是遍历 values 的下标。
实现逻辑：按顺序输出每个整数，数字之间用一个空格分隔。
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
函数用途：默认构造函数，初始化 PmergeMe 的计时结果。
参数说明：本函数没有参数。
变量说明：本函数没有局部变量；_vectorTime 和 _listTime 是对象成员变量。
实现逻辑：把两个耗时成员都设为 0，容器成员由标准库自动构造成空容器。
*/
PmergeMe::PmergeMe(void)
{
    _vectorTime = 0;
    _listTime = 0;
}

/*
函数用途：拷贝构造函数，用另一个 PmergeMe 对象初始化当前对象。
参数说明：other 是传入的已有对象引用，数据来源是调用拷贝构造时的实参。
变量说明：本函数没有局部变量；_vector、_list、_vectorTime、_listTime 是当前对象成员变量。
实现逻辑：复制 other 的两个容器和两个耗时值，让新对象拥有相同状态。
*/
PmergeMe::PmergeMe(PmergeMe const &other)
{
    _vector = other._vector;
    _list = other._list;
    _vectorTime = other._vectorTime;
    _listTime = other._listTime;
}

/*
函数用途：赋值运算符，把另一个 PmergeMe 对象的状态赋给当前对象。
参数说明：other 是等号右边传入的对象引用。
变量说明：本函数没有局部变量；_vector、_list、_vectorTime、_listTime 是当前对象成员变量。
实现逻辑：先避免自己给自己赋值，再复制容器和耗时成员，最后返回当前对象引用。
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
函数用途：析构函数，负责 PmergeMe 对象生命周期结束时的清理入口。
参数说明：本函数没有参数。
变量说明：本函数没有局部变量。
实现逻辑：类中没有手动申请的资源，vector 和 list 会自动释放，所以函数体为空。
*/
PmergeMe::~PmergeMe(void)
{
}

/*
函数用途：把命令行字符串装入 _vector，使用 vector 版本严格 Ford-Johnson 排序，并记录总耗时。
参数说明：values 是从 main 传来的字符串数组，来源于 argv + 1；count 是命令行数字参数的数量，来源于 argc - 1。
变量说明：start 保存开始时间；i 是遍历 values 的下标。
实现逻辑：开始计时后清空 _vector；逐个 atoi 转成 int 并 push_back；调用 fordJohnsonVector 排序；结束时计算装载加排序总耗时。
*/
void PmergeMe::sortVector(char **values, int count)
{
    double start;
    int i;

    start = getTimeInMicroseconds();
    _vector.clear();
    i = 0;
    while (i < count)
    {
        _vector.push_back(std::atoi(values[i]));
        i++;
    }
    _vector = fordJohnsonVector(_vector);
    _vectorTime = getTimeInMicroseconds() - start;
}

/*
函数用途：把命令行字符串装入 _list，使用 list 版本严格 Ford-Johnson 排序，并记录总耗时。
参数说明：values 是从 main 传来的字符串数组，来源于 argv + 1；count 是命令行数字参数的数量，来源于 argc - 1。
变量说明：start 保存开始时间；i 是遍历 values 的下标。
实现逻辑：开始计时后清空 _list；逐个 atoi 转成 int 并 push_back；调用 fordJohnsonList 排序；结束时计算装载加排序总耗时。
*/
void PmergeMe::sortList(char **values, int count)
{
    double start;
    int i;

    start = getTimeInMicroseconds();
    _list.clear();
    i = 0;
    while (i < count)
    {
        _list.push_back(std::atoi(values[i]));
        i++;
    }
    _list = fordJohnsonList(_list);
    _listTime = getTimeInMicroseconds() - start;
}

/*
函数用途：PmergeMe 的公开排序入口，负责输出题目要求的四行信息。
参数说明：values 是从 main 传来的字符串数组，来源于 argv + 1；count 是命令行数字参数的数量，来源于 argc - 1。
变量说明：本函数没有局部变量；_vector、_list、_vectorTime、_listTime 会在 sortVector 和 sortList 中更新。
实现逻辑：先输出原序列；分别用 vector 和 list 装载加排序；用 vector 的结果输出排序后序列；最后输出两个容器的处理时间。
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