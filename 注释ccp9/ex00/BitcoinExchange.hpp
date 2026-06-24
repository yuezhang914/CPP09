#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

/*
包含库说明：
map：使用 std::map 保存“日期字符串 -> 比特币汇率”的有序键值表，方便按日期查找 lower_bound。
string：使用 std::string 保存文件名、日期、输入行和错误信息。
*/
#include <map>
#include <string>

/*
类用途：BitcoinExchange 负责读取 data.csv 中的历史汇率，并按输入文件中的“date | value”计算兑换结果。
成员变量：_rates 是类内部保存的汇率表，来源于 loadDatabase 读取 data.csv 后插入的数据。
成员函数逻辑：构造函数加载数据库；convert 读取用户输入文件并逐行计算；getRate 查找指定日期或更早的最近日期；loadDatabase 只在类内部使用，用来填充 _rates。
*/
class BitcoinExchange
{
    private:
        std::map<std::string, double> _rates;

        void loadDatabase(std::string const &file);
        double getRate(std::string const &date) const;

    public:
        BitcoinExchange(void);
        BitcoinExchange(BitcoinExchange const &other);
        BitcoinExchange &operator=(BitcoinExchange const &other);
        ~BitcoinExchange(void);

        void convert(std::string const &file) const;
};

#endif
